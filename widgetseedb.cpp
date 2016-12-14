#include "widgetseedb.h"
#include "queryseedb.h"
#include "ktabbutton.h"
#include "sqlTableModel.h"
#include "common.h"
#include <QSqlDriver>
#include <QFileDialog>
#include <QMenu>
#include <QTableView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>
#include <QDebug>
#include <QCheckBox>

widgetSeeDB::widgetSeeDB(QWidget *parent, QSettings* set, QLineEdit* edPw)
	: QWidget(parent), m_pConfigIni(set), m_pedPw(edPw)
{
	ui.setupUi(this);
	setupUi();
	setupSignalSlots();
}

widgetSeeDB::~widgetSeeDB()
{

}

/* ��ʼ������ */
void widgetSeeDB::setupUi()
{
	// ���ݿ�����
	m_sdbSeeDB = QSqlDatabase::addDatabase("SQLITECIPHER", "SQLStudio");

	m_slSeeDBFileName = m_pConfigIni->value(SETTING_NAME_SEEDB_FIEL_PATH)
		.toString().split(SETTING_VALUE_SPLIT, QString::SkipEmptyParts);
	createFileDropList(m_slSeeDBFileName, ui.cbSeeDB);

	// ���ñ����б�
	ui.lwDBStructure->setContextMenuPolicy(Qt::ActionsContextMenu);
	QAction* act = new QAction(this);
	act->setText(QSL("ɾ��"));
	act->setEnabled(true);
	ui.lwDBStructure->addAction(act);
	connect(act, SIGNAL(triggered(bool)), this, SLOT(doActDropTable()));

	ui.swTableData->setContextMenuPolicy(Qt::ActionsContextMenu);
	QAction* actQry = new QAction(this);
	actQry->setText(QSL("����"));
	actQry->setEnabled(true);
	ui.swTableData->addAction(actQry);
	connect(actQry, SIGNAL(triggered(bool)), this, SLOT(doActQryTable()));

	// ���ñ���и�
	m_nTableDataRowHeight = ROWHEIGHT_DEF;

	ui.btnSelectSeeDB->setIcon(QIcon(RS_ICON_OPENFILE));

	ui.swTableData->installEventFilter(this);

	setSeeDBBtn(false);
}

void widgetSeeDB::setupSignalSlots()
{
	connect(ui.cbSeeDB->lineEdit(), SIGNAL(editingFinished())
		, this, SLOT(doSeeDBFileNameEditingFinished()));
	connect(ui.cbSeeDB, SIGNAL(currentIndexChanged(int)),
		this, SLOT(doSeeDBCurrentChanged()));

	// �鿴db��ҳ
	connect(ui.lwDBStructure, SIGNAL(currentRowChanged(int)), this, SLOT(doLwDBTableChanged(int)));

	connect(ui.btnSelectSeeDB, SIGNAL(clicked()), this, SLOT(doBtnSelectSeeDBClicked()));
	connect(ui.btnRefreshSeeDB, SIGNAL(clicked()), this, SLOT(doBtnRefreshSeeDBClicked()));
	connect(ui.btnInsert, SIGNAL(clicked()), this, SLOT(doBtnInsertClicked()));
	connect(ui.btnDelete, SIGNAL(clicked()), this, SLOT(doBtnDeleteClicked()));
	connect(ui.btnCommit, SIGNAL(clicked()), this, SLOT(doBtnCommitClicked()));
	connect(ui.btnRevert, SIGNAL(clicked()), this, SLOT(doBtnRevertClicked()));
	connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(doBtnCloseClicked()));

	connect(ui.chbLock, SIGNAL(clicked(bool)), this, SLOT(doChbLockChecked(bool)));
}

/* �����ļ������б� */
void widgetSeeDB::createFileDropList(
	const QStringList& slFileList,
	QComboBox* cb)
{
	cb->blockSignals(true);
	cb->clear();
	for (int i = 0; i < slFileList.size(); ++i)
	{
		cb->insertItem(0,slFileList[i], slFileList[i]);
	}
	cb->addItem(QIcon(RS_ICON_Clear), QSL("���������"), QSL("���������"));
	cb->setCurrentIndex(-1);
	cb->blockSignals(false);
}

void widgetSeeDB::doActDropTable()
{
	QString sTableName;
	QListWidgetItem* pItem = ui.lwDBStructure->currentItem();
	if (pItem)
	{
		sTableName = pItem->data(Qt::DisplayRole).toString();
		if (sTableName.isEmpty()) return;

		QTableView* pTbv = nullptr;
		QSqlTableModel* pStm = nullptr;
		KTabButton* pBtn = m_hashTableNameBtn.value(sTableName);
		if (pBtn) pTbv = qobject_cast<QTableView*>(pBtn->userObject());
		if (pTbv) pStm = qobject_cast<QSqlTableModel*>(pTbv->model());
		if (QMessageBox::information(this, QSL("drop table"), QSL("Are you sure to drop table [%1]?").arg(sTableName)
			, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		{
			QSqlQuery oSqlQuery(m_sdbSeeDB);
			QString sSQL = QSL("drop table %1;").arg(sTableName);

			// ���ģ�Ͱ�
			if (pStm) pStm->clear();

			if (!oSqlQuery.exec(sSQL))
			{
				QMessageBox::information(this, QSL("drop table"), QSL("%1").arg(oSqlQuery.lastError().text()));
				// ���°�ģ��
				if (pStm)
				{
					pStm->setTable(sTableName);
					pStm->setEditStrategy(QSqlTableModel::OnManualSubmit);
					pStm->select();
					pTbv->setModel(pStm);
				}

			}
			else
			{
				doBtnRefreshSeeDBClicked();
				QMessageBox::information(this, QSL("drop table"), QSL("Drop table [%1] succes!").arg(sTableName));
			}
		}
	}
}

void widgetSeeDB::doActQryTable()
{
	QString qrytext;
	QStringList fieldnames;
	QString fieldname;
	if (m_sCurSelectTableName.isEmpty()) return;
	QSqlTableModel* sqlTableModel = getSqlTableModel(m_sCurSelectTableName);
	if (sqlTableModel)
		QTableView* pTBV = qobject_cast<QTableView*>(m_hashTableNameBtn[m_sCurSelectTableName]->userObject());
	if (sqlTableModel)
	{
		for (int i = 0; i < sqlTableModel->columnCount(); i++)
		{
			fieldnames.push_back(sqlTableModel->headerData(i, Qt::Horizontal).toString());
		}
	}
	QuerySeeDB* qryDlg = new QuerySeeDB(fieldnames, &qrytext, &fieldname, this);
	if (qryDlg->exec() == QDialog::Accepted)
	{
		sqlTableModel->setFilter(QSL("%1 = \'%2\'").arg(fieldname).arg(qrytext));
		sqlTableModel->select();
	}
}

void widgetSeeDB::doBtnCloseClicked()
{
	if (m_sdbSeeDB.isOpen())
	{
		m_sdbSeeDB.close();
	}
	ui.cbSeeDB->setCurrentIndex(-1);
	ui.lwDBStructure->clear();
	clearTableData();
	m_sSeeDBFileName = "";
	m_sCurSelectTableName = "";
	setSeeDBBtn(false);
}

void widgetSeeDB::doLwDBTableChanged(int row)
{
	if (ui.lwDBStructure->count() > 0 && row >= 0)
		showTableData(ui.lwDBStructure->item(row)->data(Qt::DisplayRole).toString());
}

void widgetSeeDB::showTableData(const QString& sTableName)
{
	QList<QListWidgetItem*> oSelectItem = ui.lwDBStructure->findItems(sTableName, Qt::MatchExactly);
	if (oSelectItem.isEmpty()) return;
	else
	{
		ui.lwDBStructure->setCurrentItem(oSelectItem.back());
	}

	m_sCurSelectTableName = sTableName;
	if (m_hashTableNameBtn.contains(sTableName))
	{
		KTabButton* pBtn = m_hashTableNameBtn[sTableName];
		pBtn->setChecked(true);
		QTableView* pTBV = qobject_cast<QTableView*>(pBtn->userObject());
		ui.swTableData->setCurrentWidget(pTBV);
	}
	else
	{
		KTabButton* pBtn = new KTabButton(this);
		m_hashTableNameBtn[sTableName] = pBtn;
		m_bgTableNameBtnFroup.addButton(pBtn);
		pBtn->setMinimumWidth(35);
		pBtn->setMinimumHeight(24);
		pBtn->setAutoRaise(true);
		pBtn->setCheckable(true);
		pBtn->setChecked(true);
		pBtn->setTabId(sTableName);
		pBtn->setText(sTableName);
		pBtn->setToolTip(sTableName);
		pBtn->addInternalButton(sTableName, kd::AlignPositon::apVRightCenter, QPoint(6, 0), QIcon(RS_Tab_Close), QIcon(RS_Tab_Close_H));
		connect(pBtn, SIGNAL(onInternalButtonClicked(const QString)), this, SLOT(doInternalButtonClicked(const QString)));
		ui.hloTableNameBtn->insertWidget(0, pBtn, 0, Qt::AlignLeft);
		connect(pBtn, SIGNAL(clicked()), this, SLOT(doSwitchShowTable()));

		QTableView* pTBV = new QTableView(this);
		pTBV->setAlternatingRowColors(true);
		pTBV->setSelectionBehavior(QAbstractItemView::SelectRows);
		pTBV->setSelectionMode(QAbstractItemView::SingleSelection);
		pTBV->verticalHeader()->setHighlightSections(false);
		pTBV->horizontalHeader()->setHighlightSections(false);
		pBtn->setUserObject(pTBV);
		ui.swTableData->addWidget(pTBV);
		ui.swTableData->setCurrentWidget(pTBV);
		QSqlTableModel* pTableModel = new QSqlTableModel(pTBV, m_sdbSeeDB);
		pTableModel->setTable(sTableName);
		pTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
		pTableModel->setSort(0, Qt::AscendingOrder);
		pTableModel->select();
		pTBV->setModel(pTableModel);

		pTBV->resizeColumnsToContents();
		pTBV->resizeRowsToContents();

		// �����и�
		pTBV->verticalHeader()->setDefaultSectionSize(m_nTableDataRowHeight);
		int nCount = getSqlTableModel(m_sCurSelectTableName)->rowCount();
		pBtn->setText(sTableName + QSL("(%1��)").arg(nCount));
	}
	setSeeDBBtn(true);
	doChbLockChecked(ui.chbLock->isChecked());
}

/* ȡsql���ģ�� */
QSqlTableModel* widgetSeeDB::getSqlTableModel(const QString& sTableName)
{
	if (m_hashTableNameBtn.contains(sTableName))
	{
		QTableView* pTbv = qobject_cast<QTableView*>(m_hashTableNameBtn[sTableName]->userObject());
		QSqlTableModel* pSqlTM = qobject_cast<QSqlTableModel*>(pTbv->model());
		return pSqlTM;
	}
	else
		return nullptr;
}

/* ����������tableҳ */
bool widgetSeeDB::clearTableData()
{
	KTabButton* pBtn = nullptr;
	QTableView* pTbv = nullptr;
	QHashIterator<QString, KTabButton*> itTableNameBtn(m_hashTableNameBtn);
	while (itTableNameBtn.hasNext())
	{
		itTableNameBtn.next();
		pBtn = itTableNameBtn.value();
		pTbv = qobject_cast<QTableView*>(pBtn->userObject());
		deleteTableData(pTbv, pBtn);
	}
	m_hashTableNameBtn.clear();
	return true;
}

/* ɾ��һ����ҳ */
bool widgetSeeDB::deleteTableData(QTableView* pTbv, KTabButton* pBtn)
{
	if (!pTbv && !pBtn) return false;
	// �����ť
	ui.hloTableNameBtn->removeWidget(pBtn);
	m_bgTableNameBtnFroup.removeButton(pBtn);
	delete pBtn;
	// ������
	ui.swTableData->removeWidget(pTbv);
	delete pTbv;
	return true;
}

void widgetSeeDB::doSeeDBFileNameEditingFinished()
{
	if (ui.cbSeeDB->currentIndex() == ui.cbSeeDB->count() - 1)
	{
		return;
	}
	doSeeDBCurrentChanged();
}

/* �鿴DB·���޸� */
void widgetSeeDB::doSeeDBCurrentChanged()
{
	if (ui.cbSeeDB->currentIndex() == ui.cbSeeDB->count() - 1)
	{
		doClearFileNameList();
		return;
	}
	QString sSeeDBFileName = ui.cbSeeDB->currentText();
	if (sSeeDBFileName.isEmpty() || m_sSeeDBFileName == sSeeDBFileName) return;
	if (sSeeDBFileName.right(3) != ".db") return;
	addFileName(ui.cbSeeDB, m_slSeeDBFileName
		, sSeeDBFileName, SETTING_NAME_SEEDB_FIEL_PATH);
	m_sSeeDBFileName = sSeeDBFileName;
	selectedSeeDB();
}

/* ����ļ����������б� */
void widgetSeeDB::addFileName(
	QComboBox* cb,
	QStringList& slFileName,
	const QString& sFileName,
	const QString& sCIParamName)
{
	if (!slFileName.contains(sFileName))
	{
		slFileName << sFileName;
		QString sFileNames = slFileName.join(SETTING_VALUE_SPLIT);
		m_pConfigIni->setValue(sCIParamName, sFileNames);
	}

	if (cb->findData(sFileName) < 0)
	{
		cb->blockSignals(true);
		cb->insertItem(0, sFileName, sFileName);
		cb->blockSignals(false);
	}
}

/* ����ļ��б� */
void widgetSeeDB::doClearFileNameList()
{
	ui.cbSeeDB->blockSignals(true);
	if (QMessageBox::information(this, QSL("����ļ��б�"), QSL("ȷ�����[%1]�ļ��б���").arg(QSL("�鿴DB")),
		QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		m_slSeeDBFileName.clear();
		ui.cbSeeDB->clear();
		ui.cbSeeDB->addItem(QIcon(RS_ICON_Clear), QSL("���������"), QSL("���������"));
		m_pConfigIni->setValue(SETTING_NAME_SEEDB_FIEL_PATH, "");
	}
	
	ui.cbSeeDB->setCurrentIndex(-1);
	ui.cbSeeDB->blockSignals(false);
}

void widgetSeeDB::selectedSeeDB()
{
	if (!m_sSeeDBFileName.isEmpty())
	{
		ui.cbSeeDB->setCurrentText(m_sSeeDBFileName);
		if (!QFile::exists(m_sSeeDBFileName))
		{
			QMessageBox::critical(this, QSL("���ش���"), QSL("���ݿ�[%1]������.").arg(m_sSeeDBFileName));
			return;
		}

		// �����ݿ��������
		if (m_sdbSeeDB.isOpen())
			m_sdbSeeDB.close();
		m_sdbSeeDB.setDatabaseName(m_sSeeDBFileName);

		m_sdbSeeDB.setPassword(m_pedPw->text());
		m_sdbSeeDB.setConnectOptions(SQLITECIPHER_OPEN_URI);
		qDebug() << m_sdbSeeDB.databaseName() << "  " << m_sdbSeeDB.password();
		if (!m_sdbSeeDB.open())
		{
			QMessageBox::critical(this, QSL("��ʧ��"), QSL("�����ݿ�[%1]ʧ��!\nʧ��ԭ��:%2")
				.arg(m_sSeeDBFileName)
				.arg(m_sdbSeeDB.lastError().text()));
		}
		else{
			loadSeeDBTable();
		}

	}
}

void widgetSeeDB::loadSeeDBTable()
{
	// ����֮ǰѡ���
	QString sCurSelectTableName;
	QListWidgetItem* pCurItem = ui.lwDBStructure->currentItem();
	if (pCurItem) sCurSelectTableName = pCurItem->data(Qt::DisplayRole).toString();
	// ����DB
	pCurItem = nullptr;
	ui.lwDBStructure->clear();
	clearTableData();
	QSqlQuery oSqlQuery(m_sdbSeeDB);
	QString strList = QSL("select name from sqlite_master where type = 'table'");
	if (!oSqlQuery.exec(strList))
	{
		QString sErrorInfo = QSL("�������ݿ���б�ִ��SQL������!");
		QString sErrorMsg = QSL("\n����ԭ��") + oSqlQuery.lastError().text();
		QString sSQLTitle = QSL("\nSQL��䣺");
		QString sSqlBegin = QSL("\n<----------------------------------------------------------------------------------------->\n");
		QString sSql = strList;
		QStringList slSql = sSql.split("\n", QString::SkipEmptyParts);
		for (int j = 0; j < slSql.size(); ++j)
		{
			QString sSQL = slSql[j];
			sSQL.replace(" ", "");
			sSQL.replace("\t", "");
			if (sSQL.isEmpty()) slSql.removeAt(j);
		}
		sSql = slSql.join("\n");
		QString sSqlEnd = QSL("\n<----------------------------------------------------------------------------------------->");
		sSql = sSqlBegin + sSql + sSqlEnd;
		QMessageBox::critical(this, QSL("load db"), sErrorInfo + sErrorMsg + sSQLTitle + sSql);
		return;
	}

	while (oSqlQuery.next())
	{
		QString strName = oSqlQuery.value(0).toString();
		QListWidgetItem*  pListWidgetItem = new QListWidgetItem(strName);
		pListWidgetItem->setData(Qt::DisplayRole, strName);
		ui.lwDBStructure->addItem(pListWidgetItem);

		if (!sCurSelectTableName.isEmpty() && sCurSelectTableName == strName)
		{
			pCurItem = pListWidgetItem;
		}
	}

	if (pCurItem) ui.lwDBStructure->setCurrentItem(pCurItem);
}

void widgetSeeDB::doSwitchShowTable()
{
	KTabButton* pTBN = qobject_cast<KTabButton*> (sender());
	QStringList slFile = pTBN->text().split("(", QString::SkipEmptyParts);
	showTableData(slFile.front());
}

void widgetSeeDB::doChbLockChecked(bool checked)
{
	if (m_sCurSelectTableName.isEmpty()) return;

	if (checked == true)
	{
		dynamic_cast<QTableView*>(ui.swTableData->currentWidget())->setEditTriggers(QAbstractItemView::NoEditTriggers);
		setSeeDBBtn(!checked,true);
	}
	else
	{
		dynamic_cast<QTableView*>(ui.swTableData->currentWidget())->setEditTriggers(QAbstractItemView::CurrentChanged);
		setSeeDBBtn(!checked, true);
	}
}

void widgetSeeDB::doCommitDBSlot()
{
	QSqlTableModel* pSqlTM = getSqlTableModel(m_sCurSelectTableName);
	if (pSqlTM)
		if (!pSqlTM->submit())
			QMessageBox::information(this, QSL("�ύ"), QSL("%1").arg(pSqlTM->lastError().databaseText()));
}

void widgetSeeDB::doBtnSelectSeeDBClicked()
{
	QString sSelectSeeDBFileName = QFileDialog::getOpenFileName(
		this, QSL("ѡ�����ݿ�"), m_sSeeDBFileName, tr("sql (*.db)"));
	if (!sSelectSeeDBFileName.isEmpty() && m_sSeeDBFileName != sSelectSeeDBFileName)
	{
		ui.cbSeeDB->setCurrentText(sSelectSeeDBFileName);
		doSeeDBCurrentChanged();
	}
}

void widgetSeeDB::doBtnRefreshSeeDBClicked()
{
	selectedSeeDB();
}

void widgetSeeDB::doBtnInsertClicked()
{
	QSqlTableModel* pSqlTableModel = getSqlTableModel(m_sCurSelectTableName);
	if (pSqlTableModel)
	{
		int nRowCount = pSqlTableModel->rowCount();

		pSqlTableModel->insertRow(nRowCount);
	}
}

void widgetSeeDB::doBtnDeleteClicked()
{
	QSqlTableModel* sqlTableModel = getSqlTableModel(m_sCurSelectTableName);
	QTableView* pTBV = qobject_cast<QTableView*>(m_hashTableNameBtn[m_sCurSelectTableName]->userObject());
	if (sqlTableModel)
	{
		QItemSelectionModel *selection = pTBV->selectionModel();
		QModelIndexList list = selection->selectedRows();
		foreach(QModelIndex index, list)
		{
			sqlTableModel->removeRow(index.row());
		}
	}
}

void widgetSeeDB::doBtnCommitClicked()
{
	QSqlTableModel* sqlTableModel = getSqlTableModel(m_sCurSelectTableName);
	if (sqlTableModel != nullptr && (QMessageBox::information(this, QSL("�ύ�޸�"), QSL("ȷ�������[%1]����޸���").arg(m_sCurSelectTableName),
		QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes))
	{
		if (!sqlTableModel->submitAll())
		{
			QMessageBox::information(this, QSL("�ύ"), QSL("%1").arg(sqlTableModel->lastError().text()));
		}
		doCommitDBSlot();
	}
}

void widgetSeeDB::doBtnRevertClicked()
{
	QSqlTableModel* sqlTableModel = getSqlTableModel(m_sCurSelectTableName);
	if (sqlTableModel != nullptr && (QMessageBox::information(this, QSL("�����޸�"), QSL("ȷ��������[%1]����޸���").arg(m_sCurSelectTableName),
		QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes))
	{
		sqlTableModel->revertAll();
	}
}

bool widgetSeeDB::eventFilter(QObject *obj, QEvent * evt)
{
	if (obj == ui.swTableData)
	{
		if (evt->type() == QEvent::KeyPress)
		{
			QKeyEvent* ev = static_cast<QKeyEvent*>(evt);
			if (key1 == 0)
			{
				key1 = ev->key();
			}
			else
			{
				key2 = ev->key();
			}
			if ((key1 == Qt::Key_Control && key2 == Qt::Key_F) || (key2 == Qt::Key_Control && key1 == Qt::Key_F))
			{
				doActQryTable();
			}
		}
		if (evt->type() == QEvent::KeyRelease)
		{
			QKeyEvent* ev = static_cast<QKeyEvent*>(evt);
			if (ev->key() == key1)
			{
				key1 = 0;
				if (key2 != 0)
				{
					key1 = key2;
					key2 = 0;
				}
			}
			else if (ev->key() == key2)
			{
				key2 = 0;
			}
		}
	}

	return false;
}

//ɾ������
void widgetSeeDB::doInternalButtonClicked(const QString sButtonId)
{
	KTabButton* pBtn = qobject_cast<KTabButton*>(sender());
	QTableView* pTbv = nullptr;
	pTbv = qobject_cast<QTableView*>(pBtn->userObject());
	deleteTableData(pTbv, pBtn);
	m_hashTableNameBtn.remove(sButtonId);
	if (m_hashTableNameBtn.isEmpty())
	{
		m_sCurSelectTableName = "";
		setSeeDBBtn(false,true);
	}
}

/*����seedbҳ��ť*/
void widgetSeeDB::setSeeDBBtn(bool enable, bool isCheckClick)
{
	if (!isCheckClick)
	{
		ui.btnClose->setEnabled(enable);
		ui.chbLock->setEnabled(enable);
	}
	ui.btnInsert->setEnabled(enable);
	ui.btnDelete->setEnabled(enable);
	ui.btnCommit->setEnabled(enable);
	ui.btnRevert->setEnabled(enable);
}