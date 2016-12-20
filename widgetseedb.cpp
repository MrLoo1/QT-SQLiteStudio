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
#include <QMimeData>
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

/* 初始化界面 */
void widgetSeeDB::setupUi()
{
	// 数据库连接
	m_sdbSeeDB = QSqlDatabase::addDatabase("SQLITECIPHER", "SQLStudio");

	m_slSeeDBFileName = m_pConfigIni->value(SETTING_NAME_SEEDB_FIEL_PATH)
		.toString().split(SETTING_VALUE_SPLIT, QString::SkipEmptyParts);
	createFileDropList(m_slSeeDBFileName, ui.cbSeeDB);

	// 设置表名列表
	ui.lwDBStructure->setContextMenuPolicy(Qt::ActionsContextMenu);
	QAction* act = new QAction(this);
	act->setText(QSL("删除"));
	act->setEnabled(true);
	ui.lwDBStructure->addAction(act);
	connect(act, SIGNAL(triggered(bool)), this, SLOT(doActDropTable()));

	ui.swTableData->setContextMenuPolicy(Qt::ActionsContextMenu);
	QAction* actQry = new QAction(this);
	actQry->setText(QSL("查找"));
	actQry->setEnabled(true);
	ui.swTableData->addAction(actQry);
	connect(actQry, SIGNAL(triggered(bool)), this, SLOT(doActQryTable()));

	// 设置表格行高
	m_nTableDataRowHeight = ROWHEIGHT_DEF;

	ui.btnSelectSeeDB->setIcon(QIcon(RS_ICON_OPENFILE));

	ui.swTableData->installEventFilter(this);
	ui.cbSeeDB->installEventFilter(this);

	setSeeDBBtn(false);
}

void widgetSeeDB::setupSignalSlots()
{
	connect(ui.cbSeeDB->lineEdit(), SIGNAL(editingFinished())
		, this, SLOT(doSeeDBFileNameEditingFinished()));
	connect(ui.cbSeeDB, SIGNAL(currentIndexChanged(int)),
		this, SLOT(doSeeDBCurrentChanged()));

	// 查看db插页
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

/* 创建文件下拉列表 */
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
	cb->addItem(QIcon(RS_ICON_Clear), QSL("清楚下拉框"), QSL("清楚下拉框"));
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
		QMessageBox box(QMessageBox::Information, QSL("删除表"), QSL("确定要删除[%1]表吗").arg(sTableName));
		box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		box.setButtonText(QMessageBox::Yes, QSL("确 定"));
		box.setButtonText(QMessageBox::No, QSL("取 消"));
		if (box.exec() == QMessageBox::Yes)
		{
			QSqlQuery oSqlQuery(m_sdbSeeDB);
			QString sSQL = QSL("drop table %1;").arg(sTableName);

			// 解除模型绑定
			if (pStm) pStm->clear();

			if (!oSqlQuery.exec(sSQL))
			{
				QMessageBox::information(this, QSL("drop table"), QSL("%1").arg(oSqlQuery.lastError().text()));
				// 重新绑定模型
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

		// 设置行高
		pTBV->verticalHeader()->setDefaultSectionSize(m_nTableDataRowHeight);
		int nCount = getSqlTableModel(m_sCurSelectTableName)->rowCount();
		pBtn->setText(sTableName + QSL("(%1行)").arg(nCount));
	}
	setSeeDBBtn(true);
	doChbLockChecked(ui.chbLock->isChecked());
}

/* 取sql表格模型 */
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

/* 清楚表格数据table页 */
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

/* 删除一个插页 */
bool widgetSeeDB::deleteTableData(QTableView* pTbv, KTabButton* pBtn)
{
	if (!pTbv && !pBtn) return false;
	// 清楚按钮
	ui.hloTableNameBtn->removeWidget(pBtn);
	m_bgTableNameBtnFroup.removeButton(pBtn);
	delete pBtn;
	// 清楚表格
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

/* 查看DB路径修改 */
void widgetSeeDB::doSeeDBCurrentChanged()
{
	if (ui.cbSeeDB->currentIndex() == ui.cbSeeDB->count() - 1)
	{
		doClearFileNameList();
		return;
	}

	QString sSeeDBFileName = ui.cbSeeDB->currentText();
	if (sSeeDBFileName.isEmpty() 
		|| m_sSeeDBFileName == sSeeDBFileName
		|| (sSeeDBFileName.right(3) != ".db")) 
		return;
	m_sSeeDBFileName = sSeeDBFileName;
	if(selectedSeeDB())
		addFileName(ui.cbSeeDB, m_slSeeDBFileName
			, sSeeDBFileName, SETTING_NAME_SEEDB_FIEL_PATH);
}

/* 添加文件名到下拉列表 */
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
		ui.cbSeeDB->blockSignals(true);
		cb->insertItem(0, sFileName, sFileName);
		ui.cbSeeDB->blockSignals(false);
	}
}

/* 清空文件列表 */
void widgetSeeDB::doClearFileNameList()
{
	ui.cbSeeDB->blockSignals(true);
	QMessageBox box(QMessageBox::Information, QSL("清空文件列表"), QSL("确定清空[%1]文件列表吗").arg(QSL("查看DB")));
	box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	box.setButtonText(QMessageBox::Yes, QSL("确 定"));
	box.setButtonText(QMessageBox::No, QSL("取 消"));
	if (box.exec() == QMessageBox::Yes)
	{
		m_slSeeDBFileName.clear();
		ui.cbSeeDB->clear();
		ui.cbSeeDB->addItem(QIcon(RS_ICON_Clear), QSL("清楚下拉框"), QSL("清楚下拉框"));
		m_pConfigIni->setValue(SETTING_NAME_SEEDB_FIEL_PATH, "");
	}
	
	ui.cbSeeDB->setCurrentIndex(ui.cbSeeDB->findData(m_sSeeDBFileName));
	ui.cbSeeDB->blockSignals(false);
}

bool widgetSeeDB::selectedSeeDB()
{
	if (!m_sSeeDBFileName.isEmpty())
	{
		ui.cbSeeDB->setCurrentText(m_sSeeDBFileName);
		if (!QFile::exists(m_sSeeDBFileName))
		{
			QMessageBox::critical(this, QSL("加载错误"), QSL("数据库[%1]不存在.").arg(m_sSeeDBFileName));
			return false;
		}

		// 打开数据库加载数据
		if (m_sdbSeeDB.isOpen())
			m_sdbSeeDB.close();
		m_sdbSeeDB.setDatabaseName(m_sSeeDBFileName);

		m_sdbSeeDB.setPassword(m_pedPw->text());
		m_sdbSeeDB.setConnectOptions(SQLITECIPHER_OPEN_URI);
		qDebug() << m_sdbSeeDB.databaseName() << "  " << m_sdbSeeDB.password();
		if (!m_sdbSeeDB.open())
		{
			QMessageBox::critical(this, QSL("打开失败"), QSL("打开数据库[%1]失败!\n失败原因:%2")
				.arg(m_sSeeDBFileName)
				.arg(m_sdbSeeDB.lastError().text()));
			return false;
		}
		else{
			loadSeeDBTable();
		}
		return true;
	}
	return false;
}

void widgetSeeDB::loadSeeDBTable()
{
	// 保存之前选择的
	QString sCurSelectTableName;
	QListWidgetItem* pCurItem = ui.lwDBStructure->currentItem();
	if (pCurItem) sCurSelectTableName = pCurItem->data(Qt::DisplayRole).toString();
	// 加载DB
	pCurItem = nullptr;
	ui.lwDBStructure->clear();
	clearTableData();
	QSqlQuery oSqlQuery(m_sdbSeeDB);
	QString strList = QSL("select name from sqlite_master where type = 'table'");
	if (!oSqlQuery.exec(strList))
	{
		QString sErrorInfo = QSL("加载数据库表列表：执行SQL语句错误!");
		QString sErrorMsg = QSL("\n错误原因：") + oSqlQuery.lastError().text();
		QString sSQLTitle = QSL("\nSQL语句：");
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
			QMessageBox::information(this, QSL("提交"), QSL("%1").arg(pSqlTM->lastError().databaseText()));
}

void widgetSeeDB::doBtnSelectSeeDBClicked()
{
	QString sSelectSeeDBFileName = QFileDialog::getOpenFileName(
		this, QSL("选择数据库"), m_sSeeDBFileName, tr("sql (*.db)"));
	if (!sSelectSeeDBFileName.isEmpty() && m_sSeeDBFileName != sSelectSeeDBFileName)
	{
		ui.cbSeeDB->setCurrentText(sSelectSeeDBFileName);
		doSeeDBCurrentChanged();
	}
}

void widgetSeeDB::doBtnRefreshSeeDBClicked()
{
	int row = ui.lwDBStructure->currentRow();
	QString sTableName = ui.lwDBStructure->item(row)->data(Qt::DisplayRole).toString();
	QTableView* pTbv = nullptr;
	pTbv = qobject_cast<QTableView*>(m_hashTableNameBtn[sTableName]->userObject());
	deleteTableData(pTbv, m_hashTableNameBtn[sTableName]);
	m_hashTableNameBtn.remove(sTableName);
	showTableData(sTableName);
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
	QMessageBox box(QMessageBox::Information, QSL("提交修改"), QSL("确定保存对[%1]表的修改吗？").arg(m_sCurSelectTableName));
	box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	box.setButtonText(QMessageBox::Yes, QSL("确 定"));
	box.setButtonText(QMessageBox::No, QSL("取 消"));
	if (sqlTableModel != nullptr && (box.exec() == QMessageBox::Yes))
	{
		if (!sqlTableModel->submitAll())
		{
			QMessageBox::information(this, QSL("提交"), QSL("%1").arg(sqlTableModel->lastError().text()));
		}
		doCommitDBSlot();
	}
}

void widgetSeeDB::doBtnRevertClicked()
{
	QSqlTableModel* sqlTableModel = getSqlTableModel(m_sCurSelectTableName);
	QMessageBox box(QMessageBox::Information, QSL("撤销修改"), QSL("确定撤销对[%1]表的修改吗？").arg(m_sCurSelectTableName));
	box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	box.setButtonText(QMessageBox::Yes, QSL("确 定"));
	box.setButtonText(QMessageBox::No, QSL("取 消"));
	if (sqlTableModel != nullptr && (box.exec() == QMessageBox::Yes))
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
	else if (obj == ui.cbSeeDB)
	{
		if (evt->type() == QEvent::KeyPress)
		{
			QKeyEvent* ev = static_cast<QKeyEvent*>(evt);
			if (ev->key() == Qt::Key_Return)
			{
				doSeeDBFileNameEditingFinished();
				return true;
			}
		}
	}

	return false;
}

//删除分组
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

/*设置seedb页按钮*/
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
	ui.btnRefreshSeeDB->setEnabled(enable);
}

void widgetSeeDB::dragEnterEvent(QDragEnterEvent *event)
{
	//如果为文件，则支持拖放
	if (event->mimeData()->hasFormat("text/uri-list"))
		event->acceptProposedAction();
}

void widgetSeeDB::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty())
		return;

	//往文本框中追加文件名
	QString file_name = urls[0].toLocalFile();
	ui.cbSeeDB->setCurrentText(file_name);
	doSeeDBFileNameEditingFinished();
}