#include "sqlitestudio.h"
#include "queryseedb.h"
#include "tabbutton.h"
#include "sqlTableModel.h"
#include "sqlconfig.h"
#include "common.h"
#include "msgdlg.h"
#include <QSqlDriver>
#include <QFileDialog>
#include <QSettings>
#include <QMenu>
#include <QTableView>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

#define  ROWHEIGHT_DEF			28

SQLiteStudio::SQLiteStudio(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setupUi();
	setupSignalSlots();
}

SQLiteStudio::~SQLiteStudio()
{
	if (m_pTextDocument)
		delete m_pTextDocument;
}

/* 初始化界面 */
void SQLiteStudio::setupUi()
{
	// 设置窗口图标
	setWindowIcon(QIcon(RS_ICON_WINDOW));

	// sql语句编辑框
	ui.edSql->setWordWrapMode(QTextOption::NoWrap);

	// 创建程序配置
	createIni();

	// 初始化文件路径
	m_slCreateDBFileName = m_pConfigIni->value(SETTING_NAME_CREATEDB_FIELPATH)
		.toString().split(SETTING_VALUE_SPLIT, QString::SkipEmptyParts);
	createFileDropList(m_slCreateDBFileName, ui.cbCreateDB);
	
	m_slSqlFileName = m_pConfigIni->value(SETTING_NAME_SQL_FIELPATH)
		.toString().split(SETTING_VALUE_SPLIT, QString::SkipEmptyParts);
	createFileDropList(m_slSqlFileName, ui.cbSqlFile);

	m_slSeeDBFileName = m_pConfigIni->value(SETTING_NAME_SEEDB_FIEL_PATH)
		.toString().split(SETTING_VALUE_SPLIT, QString::SkipEmptyParts);
	createFileDropList(m_slSeeDBFileName, ui.cbSeeDB);

	// 数据库连接
	m_sdbSeeDB = QSqlDatabase::addDatabase("QSQLITE", "SQLStudio");

	// 创建DB保存按钮下拉菜单
	m_menu = new QMenu(this);
	m_actSave = new QAction(KSL("另存为"), m_menu);
	m_menu->addAction(m_actSave);
	ui.btnSave->setMenu(m_menu);

	// 选择生成插页
	ui.tabwSQLiteStudio->setCurrentIndex(1);

	// 设置表名列表
	ui.lwDBStructure->setContextMenuPolicy(Qt::ActionsContextMenu);
	QAction* act = new QAction(this);
	act->setText(KSL("删除"));
	act->setEnabled(true);
	ui.lwDBStructure->addAction(act);
	connect(act, SIGNAL(triggered(bool)), this, SLOT(doActDropTable()));

	ui.swTableData->setContextMenuPolicy(Qt::ActionsContextMenu);
	QAction* actQry = new QAction(this);
	actQry->setText(KSL("查找"));
	actQry->setEnabled(true);
	ui.swTableData->addAction(actQry);
	connect(actQry, SIGNAL(triggered(bool)), this, SLOT(doActQryTable()));

	// 设置表格行高
	m_nTableDataRowHeight = ROWHEIGHT_DEF;

	// 清空下拉列表
	ui.btnClearSqlFile->setIcon(QIcon(RS_ICON_Clear));
	ui.btnClearCreateDB->setIcon(QIcon(RS_ICON_Clear));
	ui.btnClearSeeDB->setIcon(QIcon(RS_ICON_Clear));

	ui.swTableData->installEventFilter(this);
}

/* 创建文件下拉列表 */
void SQLiteStudio::createFileDropList(
	const QStringList& slFileList,
	QComboBox* cb)
{
	cb->blockSignals(true);
	cb->clear();
	for (int i = 0; i < slFileList.size(); ++i)
	{
		cb->addItem(slFileList[i], slFileList[i]);
	}
	cb->setCurrentIndex(-1);
	cb->blockSignals(false);
}

/* 添加文件名到下拉列表 */
void SQLiteStudio::addFileName(
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
		cb->addItem(sFileName, sFileName);
}

void SQLiteStudio::setupSignalSlots()
{
	// 生成db插页
	connect(m_actSave, SIGNAL(triggered()), this, SLOT(doActSaveOther()));

	connect(ui.cbSqlFile->lineEdit(), SIGNAL(editingFinished())
		, this, SLOT(doSqlFileNameEditingFinished()));
	connect(ui.cbSqlFile, SIGNAL(currentIndexChanged(int)),
		this, SLOT(doSqlFileCurrentChanged()));

	connect(ui.cbCreateDB->lineEdit(), SIGNAL(editingFinished())
		, this, SLOT(doCreateDBFileNameEditingFinished()));
	connect(ui.cbCreateDB, SIGNAL(currentIndexChanged(int)),
		this, SLOT(doCreateDBCurrentChanged()));
	
	connect(ui.cbSeeDB->lineEdit(), SIGNAL(editingFinished())
		, this, SLOT(doSeeDBFileNameEditingFinished()));
	connect(ui.cbSeeDB, SIGNAL(currentIndexChanged(int)),
		this, SLOT(doSeeDBCurrentChanged()));
	
	connect(ui.btnSelectSqlFile, SIGNAL(clicked()), this, SLOT(doBtnSelectSqlFileClicked()));
	connect(ui.btnSelectCreateDB, SIGNAL(clicked()), this, SLOT(doBtnSelectCreatDbClicked()));
	connect(ui.btnExec, SIGNAL(clicked()), this, SLOT(doBtnExecClicked()));
	connect(ui.btnRefresh, SIGNAL(clicked()), this, SLOT(doBtnRefreshClicked()));
	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(doBtnSaveClicked()));

	// 查看db插页
	connect(ui.lwDBStructure, SIGNAL(currentRowChanged(int)), this, SLOT(doLwDBTableChanged(int)));

	connect(ui.btnSelectSeeDB, SIGNAL(clicked()), this, SLOT(doBtnSelectSeeDBClicked()));
	connect(ui.btnRefreshSeeDB, SIGNAL(clicked()), this, SLOT(doBtnRefreshSeeDBClicked()));
	connect(ui.btnInsert, SIGNAL(clicked()), this, SLOT(doBtnInsertClicked()));
	connect(ui.btnDelete, SIGNAL(clicked()), this, SLOT(doBtnDeleteClicked()));
	connect(ui.btnCommit, SIGNAL(clicked()), this, SLOT(doBtnCommitClicked()));
	connect(ui.btnRevert, SIGNAL(clicked()), this, SLOT(doBtnRevertClicked()));

	// 清空文件列表
	connect(ui.btnClearSqlFile, SIGNAL(clicked()), this, SLOT(doClearFileNameList()));
	connect(ui.btnClearCreateDB, SIGNAL(clicked()), this, SLOT(doClearFileNameList()));
	connect(ui.btnClearSeeDB, SIGNAL(clicked()), this, SLOT(doClearFileNameList()));

	//sql文件监视器
	connect(&m_sqlFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(doAfterSqlFileChange(QString)));

	//文本被修改
	connect(m_pTextDocument, SIGNAL(contentsChanged()),	this, SLOT(documentWasModified()));
}

void SQLiteStudio::createIni()
{
	QDir dir(QCoreApplication::applicationDirPath());
	QString  sCurrentAppExeDirPath = dir.absolutePath();
	if (!sCurrentAppExeDirPath.endsWith(QDir::separator()))
	{
		sCurrentAppExeDirPath += QDir::separator();
	}
	QString strPath = sCurrentAppExeDirPath + "config.ini";
	m_pConfigIni = new QSettings(strPath, QSettings::IniFormat, this);
}

void SQLiteStudio::doBtnExecClicked()
{
	createDatabase(ui.cbCreateDB->currentText(), ui.cbSqlFile->currentText());
}

void SQLiteStudio::doBtnSelectSqlFileClicked()
{	
	QString sSelectSqlFileName = QFileDialog::getOpenFileName(
		this, KSL("选择文件"), m_sSqlFileName, "*.sql");
	if (!sSelectSqlFileName.isEmpty() && m_sSqlFileName != sSelectSqlFileName)
	{
		ui.cbSqlFile->setCurrentText(sSelectSqlFileName);
		doSqlFileCurrentChanged();
	}
}

void SQLiteStudio::doBtnSelectCreatDbClicked()
{
	QString sSelectCreateDBFileName = QFileDialog::getSaveFileName(this,
		tr("Select Save Database File"), "/home/" + ui.cbCreateDB->currentData().toString(), tr("Database Files (*.db)"));
	if (!sSelectCreateDBFileName.isEmpty() && m_sCreateDBFileName != sSelectCreateDBFileName)
	{
		ui.cbCreateDB->setCurrentText(m_sCreateDBFileName);
		doCreateDBCurrentChanged();
	}
}

void SQLiteStudio::doBtnSelectSeeDBClicked()
{
	QString sSelectSeeDBFileName = QFileDialog::getOpenFileName(
		this, KSL("选择数据库"), m_sSeeDBFileName, tr("sql (*.db)"));
	if (!sSelectSeeDBFileName.isEmpty() && m_sSeeDBFileName != sSelectSeeDBFileName)
	{
		ui.cbSeeDB->setCurrentText(m_sSeeDBFileName);
		doSeeDBCurrentChanged();
	}
}

void SQLiteStudio::doBtnRefreshSeeDBClicked()
{
	selectedSeeDB();
}

void SQLiteStudio::doBtnSaveClicked()
{
	QString newSqlName = ui.cbSqlFile->currentText();
	if (newSqlName == "")
	{
		return;
	}
	QFile file(newSqlName);
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream t(&file);
		QString sqlContext = ui.edSql->document()->toPlainText();
		t << sqlContext;
		MsgDlg::info(KSL("保存成功"), KSL("保存成功"));
		file.close();
	}
	else
	{
		MsgDlg::error(KSL("请检验您所输入的文件名是否正确。"), KSL("打不开文件"));
		return;
	}
}

void SQLiteStudio::doBtnRefreshClicked()
{
	if (!loadTextEditor(m_sSqlFileName))
	{
		// TODO::文件加载失败
	}
}

void SQLiteStudio::doActSaveOther()
{
	QString newSqlName = QFileDialog::getSaveFileName(this,
		tr("Select Save Edited InitSql File"), " / home / " + ui.cbSqlFile->currentText(), tr("InitSql Files (*.txt *.sql)"));
	if (newSqlName == "")
	{
		return;
	}
	QFile file(newSqlName);
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream t(&file);
		QString sqlContext = ui.edSql->document()->toPlainText();
		t << sqlContext;
		file.close();
	}
	else
	{
		MsgDlg::error(KSL("请检验您所输入的文件名是否正确。"), KSL("打开文件失败"));
		return;
	}
}

void SQLiteStudio::doSqlFileNameEditingFinished()
{
	doSqlFileCurrentChanged();
}

void SQLiteStudio::doCreateDBFileNameEditingFinished()
{
	doCreateDBCurrentChanged();
}

void SQLiteStudio::doSeeDBFileNameEditingFinished()
{
	doSeeDBCurrentChanged();
}

void SQLiteStudio::doSqlFileCurrentChanged()
{
	QString sCurFile = ui.cbSqlFile->currentText();
	if (sCurFile.isEmpty() || sCurFile == m_sSqlFileName) return;
	if (sCurFile.right(4) != ".sql") return;
	addFileName(ui.cbSqlFile, m_slSqlFileName
		, sCurFile, SETTING_NAME_SQL_FIELPATH);
	m_sSqlFileName = sCurFile;
	if (!loadTextEditor(sCurFile))
	{
		MsgDlg::error(KSL("SQL文件[%1]加载失败!").arg(sCurFile), KSL("打开文件失败"));
	}

	// 重置生成db的文件路径
	QString sCreateDBFileName;
	sCreateDBFileName = m_sSqlFileName.left(m_sSqlFileName.length() - 4) + ".db";
	addFileName(ui.cbCreateDB, m_slCreateDBFileName
		, sCreateDBFileName, SETTING_NAME_CREATEDB_FIELPATH);
	ui.cbCreateDB->setCurrentText(sCreateDBFileName);
}

void SQLiteStudio::doCreateDBCurrentChanged()
{
	QString sCurFile = ui.cbCreateDB->currentText();
	if (sCurFile.isEmpty() || m_sCreateDBFileName == sCurFile) return;
	if (sCurFile.right(3) != ".db") return;
	addFileName(ui.cbCreateDB, m_slCreateDBFileName
		, sCurFile, SETTING_NAME_CREATEDB_FIELPATH);
	m_sCreateDBFileName = sCurFile;
}

/* 查看DB路径修改 */
void SQLiteStudio::doSeeDBCurrentChanged()
{
	QString sSeeDBFileName = ui.cbSeeDB->currentText();
	if (sSeeDBFileName.isEmpty() || m_sSeeDBFileName == sSeeDBFileName) return;
	if (sSeeDBFileName.right(3) != ".db") return;
	addFileName(ui.cbSeeDB, m_slSeeDBFileName
		, sSeeDBFileName, SETTING_NAME_SEEDB_FIEL_PATH);
	m_sSeeDBFileName = sSeeDBFileName;
	selectedSeeDB();
}

/*sql文件改变槽*/
void SQLiteStudio::doAfterSqlFileChange(const QString& path)
{
	if (MsgDlg::confirm(KSL("The sql file [%1] has been changed !\nDo you want to reload it?").arg(path)))
	{
		doBtnRefreshClicked();
	}
}

/*显示文本被修改*/
void SQLiteStudio::documentWasModified()
{
	ui.wCreateDb->setWindowModified(m_pTextDocument->isModified());
}

/*添加新的子版本*/
void SQLiteStudio::addDB(QList<QMap<int, Qt::CheckState> > list, const QString& strName)
{

}

bool SQLiteStudio::updateDB(QList<QMap<int, Qt::CheckState> > list, const QString& strName)
{
	/*QSqlQuery* oSQLQuery = new QSqlQuery(m_sdbSeeDB);
	
	QString strTemp = KSL("DELETE FROM VersionSelect WHERE versionName='%1'").arg(strName);
	bool bIsSuc = oSQLQuery->exec(strTemp);

	int iID = 0;
	QHashIterator<QString, QSqlTableModel*> itTableModel(m_hashTableDataModel);
	QListIterator<QMap<int, Qt::CheckState>> i(list);
	while (i.hasNext())
	{
		itTableModel.next();
		QSqlTableModel* currentModel = itTableModel.value();
		QMap<int, Qt::CheckState> map = i.next();

		QMap<int, Qt::CheckState>::iterator it;
		for (it = map.begin(); it != map.end(); ++it)
		{
			if (it.value() == Qt::Checked)
			{
				strTemp = QObject::tr("INSERT INTO VersionSelect VALUES('%1', '%2',%3)").arg(strName).arg(currentModel->tableName()).arg(it.key());
				bIsSuc = oSQLQuery->exec(strTemp);
				iID++;
			}
		}

		strTemp = QObject::tr("EXISTS (SELECT *  FROM VersionSelect  WHERE %1.id = VersionSelect.indexID  AND name = '%2'  AND versionName = '%3') ")
			.arg(currentModel->tableName()).arg(currentModel->tableName()).arg(strName);
		currentModel->setFilter(strTemp);
		currentModel->select();
	}
	commitDBSlot();

	if (oSQLQuery)
	{
		delete oSQLQuery;
		oSQLQuery = nullptr;
	}
*/
	return true;
}

void SQLiteStudio::selectedSeeDB()
{
	if (!m_sSeeDBFileName.isEmpty())
	{
		ui.cbSeeDB->setCurrentText(m_sSeeDBFileName);
		if (!QFile::exists(m_sSeeDBFileName))
		{
			MsgDlg::error(KSL("数据库[%1]不存在.").arg(m_sSeeDBFileName));
			return;
		}

		// 打开数据库加载数据
		if (m_sdbSeeDB.isOpen())
			m_sdbSeeDB.close();
		m_sdbSeeDB.setDatabaseName(m_sSeeDBFileName);
		if (!m_sdbSeeDB.open())
		{
			MsgDlg::error(KSL("打开数据库[%1]失败!\n失败原因:%2")
				.arg(m_sSeeDBFileName)
				.arg(m_sdbSeeDB.lastError().text()));
		}
		else
			loadSeeDBTable();
	}
}

void SQLiteStudio::doCommitDBSlot()
{
	QSqlTableModel* pSqlTM = getSqlTableModel(m_sCurSelectTableName);
	if (pSqlTM)
		if (!pSqlTM->submit())
			MsgDlg::info(KSL("%1").arg(pSqlTM->lastError().databaseText()));
}

void SQLiteStudio::doBtnInsertClicked()
{
	QSqlTableModel* pSqlTableModel = getSqlTableModel(m_sCurSelectTableName);
	if (pSqlTableModel)
	{
		int nRowCount = pSqlTableModel->rowCount();

		pSqlTableModel->insertRow(nRowCount);
	}
}

void SQLiteStudio::doBtnDeleteClicked()
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

void SQLiteStudio::doBtnCommitClicked()
{
	QSqlTableModel* sqlTableModel = getSqlTableModel(m_sCurSelectTableName);
	if (sqlTableModel != nullptr)
	{
		if (!sqlTableModel->submitAll())
		{
			MsgDlg::info(KSL("%1").arg(sqlTableModel->lastError().text()));
		}
		doCommitDBSlot();
	}
}

void SQLiteStudio::doBtnRevertClicked()
{
	QSqlTableModel* sqlTableModel = getSqlTableModel(m_sCurSelectTableName);
	if (sqlTableModel != nullptr)
	{
		sqlTableModel->revertAll();
	}
}

void SQLiteStudio::doSwitchShowTable()
{
	TabButton* pTBN = qobject_cast<TabButton*> (sender());
	showTableData(pTBN->text());
}

void SQLiteStudio::doActQryTable()
{
	QString qrytext;
	QStringList fieldnames;
	QString fieldname;
	QSqlTableModel* sqlTableModel = getSqlTableModel(m_sCurSelectTableName);
	QTableView* pTBV = qobject_cast<QTableView*>(m_hashTableNameBtn[m_sCurSelectTableName]->userObject());
	if (sqlTableModel)
	{
		for (int i = 0; i < sqlTableModel->columnCount(); i++)
		{
			fieldnames.push_back(sqlTableModel->headerData(i, Qt::Horizontal).toString());
		}
	}
	QuerySeeDB* qryDlg = new QuerySeeDB(fieldnames, &qrytext, &fieldname,this);
	if (qryDlg->exec() == QDialog::Accepted)
	{
		qDebug() << KSL("%1 = \'%2\'").arg(fieldname).arg(qrytext);
		sqlTableModel->setFilter(KSL("%1 = \'%2\'").arg(fieldname).arg(qrytext));
		sqlTableModel->select();
	}
}

void SQLiteStudio::doActDropTable()
{
	QString sTableName;
	QListWidgetItem* pItem = ui.lwDBStructure->currentItem();
	if (pItem)
	{
		sTableName = pItem->data(Qt::DisplayRole).toString();
		if (sTableName.isEmpty()) return;

		QTableView* pTbv = nullptr;
		QSqlTableModel* pStm = nullptr;
		TabButton* pBtn = m_hashTableNameBtn.value(sTableName);
		if (pBtn) pTbv = qobject_cast<QTableView*>(pBtn->userObject());
		if (pTbv) pStm = qobject_cast<QSqlTableModel*>(pTbv->model());
		
		if (MsgDlg::confirm(KSL("Are you sure want to drop table [%1]?").arg(sTableName)))
		{
			QSqlQuery oSqlQuery(m_sdbSeeDB);
			QString sSQL = KSL("drop table %1;").arg(sTableName);

			// 解除模型绑定
			if (pStm) pStm->clear();

			if (!oSqlQuery.exec(sSQL))
			{
				MsgDlg::info(KSL("%1").arg(oSqlQuery.lastError().text()));

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
				MsgDlg::info(KSL("Drop table [%1] succes!").arg(sTableName));
			}
		}
	}
}

void SQLiteStudio::loadSeeDBTable()
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
	QString strList = KSL("select name from sqlite_master where type = 'table'");
	if (!oSqlQuery.exec(strList))
	{
		QString sErrorInfo = KSL("加载数据库表列表：执行SQL语句错误!");
		QString sErrorMsg = KSL("\n错误原因：") + oSqlQuery.lastError().text();
		QString sSQLTitle = KSL("\nSQL语句：");
		QString sSqlBegin = KSL("\n<----------------------------------------------------------------------------------------->\n");
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
		QString sSqlEnd = KSL("\n<----------------------------------------------------------------------------------------->");
		sSql = sSqlBegin + sSql + sSqlEnd;

		MsgDlg::error(sErrorInfo + sErrorMsg + sSQLTitle + sSql);
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
	//ui.lwDBStructure->setCurrentRow(0);
}
void SQLiteStudio::controlEnable(bool bEnable)
{
	ui.btnInsert->setEnabled(bEnable);
	ui.btnDelete->setEnabled(bEnable);
	ui.btnCommit->setEnabled(bEnable);
	ui.btnRevert->setEnabled(bEnable);
	
}

void SQLiteStudio::showTableData(const QString& sTableName)
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
		TabButton* pBtn = m_hashTableNameBtn[sTableName];
		pBtn->setChecked(true);
		QTableView* pTBV = qobject_cast<QTableView*>(pBtn->userObject());
		ui.swTableData->setCurrentWidget(pTBV);
	}
	else
	{
		TabButton* pBtn = new TabButton(this);
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
		pBtn->addInternalButton(sTableName, tabPos::AlignPositon::apVRightCenter, QPoint(6, 0), QIcon(RS_Tab_Close), QIcon(RS_Tab_Close_H));
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
	}

	int nCount = getSqlTableModel(m_sCurSelectTableName)->rowCount();
	ui.lbTableStatus->setText(KSL("记录数：") + QString("%1").arg(nCount));
}

/* 将文件内容加载到文本编辑框 */
bool SQLiteStudio::loadTextEditor(const QString& sFilePath)
{
	if (sFilePath.isEmpty()) return false;
	ui.cbSqlFile->setCurrentText(sFilePath);
	QFile file(sFilePath);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		QString sText;
		QString sLine;
		do 
		{
			sLine = in.read(128);
			sText += sLine;
			QApplication::processEvents();
		} while (!sLine.isEmpty());
		
		if (!m_pTextDocument)
		{
			m_pTextDocument = new QTextDocument;
		}
		m_pTextDocument->setPlainText(sText);
		ui.edSql->setDocument(m_pTextDocument);
		file.close();
		m_sqlFileWatcher.removePaths(m_sqlFileWatcher.files());
		m_sqlFileWatcher.addPath(m_sSqlFileName);
		return true;
	}
	else
	{
		MsgDlg::error(KSL("打开文件失败:%1").arg(file.errorString()));
		return false;
	}
}

bool SQLiteStudio::createDatabase(const QString &sDBFileName, const QString &sSqlFileName)
{
	if (sSqlFileName.isEmpty() || sDBFileName.isEmpty()) return false;

	// 删除已有的文件，新建一个db文件
	QDir oDir;
	QFile oFile(m_sCreateDBFileName);
	if (oFile.exists()
		&& !oFile.remove())
	{
		MsgDlg::error(KSL("删除文件[%1]失败：\n%2")
			.arg(m_sCreateDBFileName)
			.arg(oFile.errorString()));
		return false;
	}
	else
	{
		oDir.setPath(getFileDirPath(m_sCreateDBFileName));
		if (!oDir.exists() && !oDir.mkpath(oDir.path()))
		{
			MsgDlg::error(KSL("路径[%1]不存在,且创建失败!").arg(oDir.path()));
			return false;
		}

		if (!oFile.open(QIODevice::ReadWrite))
		{
			MsgDlg::error(KSL("打开文件[%1]失败：\n%2")
				.arg(m_sCreateDBFileName)
				.arg(oFile.errorString()));
			return false;
		}
		else
		{
			oFile.close();
		}
	}

	// 打开数据库文件
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "connection");
	db.setDatabaseName(m_sCreateDBFileName);
	if (!db.open())
	{
		MsgDlg::error(KSL("打开数据库[%1]失败:\n%2")
			.arg(m_sCreateDBFileName)
			.arg(db.lastError().text())
			, KSL("打开数据库失败"));
		return false;
	}

	// 获取SQL语句
	QStringList szSQL;
	if (ui.edSql->document()->toPlainText() != "")
	{
		szSQL = ui.edSql->document()->toPlainText().split(ui.edSplitChar->text());
	}
	else
	{
		QFile file(sSqlFileName);
		if (file.open(QIODevice::ReadOnly))
		{
			QTextStream t(&file);
			szSQL = t.readAll().split(ui.edSplitChar->text());

			file.close();
			if (szSQL.size() == 0)
			{
				MsgDlg::error(KSL("文件不能为空且文件内要有分隔符！"), KSL("读取文件出错"));
				db.close();
				return false;
			}
		}
		else
		{
			MsgDlg::error(KSL("请检验您所输入的文件名是否正确。"), KSL("打不开文件"));
			db.close();
			return false;
		}
	}

	
	// 启动数据库事务
	if (!db.driver()->beginTransaction())
	{
		QString sInfo = KSL("失败：启动数据事务失败!\n");
		QString sErrorMsg = KSL("错误原因:") + db.driver()->lastError().text();
		MsgDlg::error(sInfo + sErrorMsg, KSL("提示"));
	}

	// 执行SQL语句
	QSqlQuery oSQLQuery(db);
	ui.pbExecSql->setMaximum(szSQL.size());
	int n = szSQL.size();
	for (int i = 0; i < szSQL.size(); ++i)
	{
		if (!szSQL[i].contains("/*") && szSQL[i].trimmed() != "")
		{
			ui.pbExecSql->setValue(i);
			if (!oSQLQuery.exec(szSQL[i]))
			{
				QString sI = QString::number(i);
				QString sInfo = KSL("失败：执行第") + sI + KSL("次SQL语句失败！\n");
				QString sErrorMsg = KSL("错误原因:") + oSQLQuery.lastError().text();
				QString sErrorSqlTitle = KSL("\n失败语句:");
				QString sSqlBegin = KSL("\n<----------------------------------------------------------------------------------------->\n");
				QString sSql = (szSQL[i]);
				QStringList slSql = sSql.split("\n", QString::SkipEmptyParts);
				for (int j = 0; j < slSql.size(); ++j)
				{
					QString sSQL = slSql[j];
					sSQL.replace(" ", "");
					sSQL.replace("\t", "");
					if (sSQL.isEmpty()) slSql.removeAt(j);
				}
				sSql = slSql.join("\n");
				QString sSqlEnd = KSL("\n<----------------------------------------------------------------------------------------->");
				MsgDlg::error(sInfo + sErrorMsg + sErrorSqlTitle + sSqlBegin + sSql + sSqlEnd, KSL("提示"));
				n = i;
				break;
			}
		}
	}

	if (n == szSQL.size())
	{
		ui.pbExecSql->setValue(n);
	}
	else
	{
		ui.pbExecSql->setValue(0);
		db.driver()->rollbackTransaction();
		db.close();
		return false;
	}
	
	if (!db.driver()->commitTransaction())
	{
		QString sInfo = KSL("失败：提交数据事务失败!\n");
		QString sErrorMsg = KSL("错误原因:") + db.driver()->lastError().text();
		MsgDlg::error(sInfo + sErrorMsg, KSL("提示"));
	}

	db.close();
	MsgDlg::info(KSL("生成数据库[%1]成功!").arg(sDBFileName), KSL("成功"));
	ui.pbExecSql->setValue(0);

	return true;
}

/* 获取文件夹路径 */
QString SQLiteStudio::getFileDirPath(const QString& sFilePath)
{
	QStringList slFile = sFilePath.split("/", QString::SkipEmptyParts);
	return sFilePath.left(sFilePath.length() - slFile.back().length() - 1);
}

/* 获取文件名 */
QString SQLiteStudio::getFileName(const QString& sFilePath)
{
	QStringList slFile = sFilePath.split("/", QString::SkipEmptyParts);
	return slFile.back();
}

/* 取sql表格模型 */
QSqlTableModel* SQLiteStudio::getSqlTableModel(const QString& sTableName)
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
bool SQLiteStudio::clearTableData()
{
	TabButton* pBtn = nullptr;
	QTableView* pTbv = nullptr;
	QHashIterator<QString, TabButton*> itTableNameBtn(m_hashTableNameBtn);
	while (itTableNameBtn.hasNext())
	{
		itTableNameBtn.next();
		pBtn = itTableNameBtn.value();
		pTbv = qobject_cast<QTableView*>(pBtn->userObject());
		deleteTableData(pTbv, pBtn);
	}
	ui.lbTableStatus->clear();
	m_hashTableNameBtn.clear();
	return true;
}

/* 删除一个插页 */
bool SQLiteStudio::deleteTableData(QTableView* pTbv, TabButton* pBtn)
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

void SQLiteStudio::doLwDBTableChanged(int row)
{
	if (ui.lwDBStructure->count() > 0 && row >= 0)
		showTableData(ui.lwDBStructure->item(row)->data(Qt::DisplayRole).toString());
}

/* 清空文件列表 */
void SQLiteStudio::doClearFileNameList()
{
	QToolButton* btn = qobject_cast<QToolButton*>(sender());
	if (btn == ui.btnClearSqlFile)
	{
		clearFileNameList(KSL("生成DB的SQL脚本"), ui.cbSqlFile, 
			m_slSqlFileName, SETTING_NAME_SQL_FIELPATH);
	}
	else if (btn == ui.btnClearCreateDB)
	{
		clearFileNameList(KSL("生成的DB"), ui.cbCreateDB,
			m_slCreateDBFileName, SETTING_NAME_CREATEDB_FIELPATH);
	}
	else if (btn == ui.btnClearSeeDB)
	{
		clearFileNameList(KSL("查看DB"), ui.cbSeeDB,
			m_slSeeDBFileName, SETTING_NAME_SEEDB_FIEL_PATH);
	}
}

/* 删除文件名 */
void SQLiteStudio::clearFileNameList(
	const QString& sClearFileListName,
	QComboBox* cb,
	QStringList& slFileName,
	const QString& sCIParamName)
{
	if (MsgDlg::confirm(KSL("确定清空[%1]文件列表吗？").arg(sClearFileListName)))
	{
		slFileName.clear();
		cb->clear();

		m_pConfigIni->setValue(sCIParamName, "");
	}
}

bool SQLiteStudio::eventFilter(QObject *obj, QEvent * evt)
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
			if (key1 == Qt::Key_Control && key2 == Qt::Key_F)
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

void SQLiteStudio::closeEvent(QCloseEvent *event)
{
	if (maybeSave()) { // 如果maybeSave()函数返回true，则关闭窗口
		event->accept();
	}
	else {   // 否则忽略该事件
		event->ignore();
	}
}

bool SQLiteStudio::maybeSave()  // 是否需要保存
{
	if (m_pTextDocument->isModified()) { // 如果文档被更改过
		QMessageBox box;
		box.setWindowTitle(QStringLiteral("SQL文件"));
		box.setText(QStringLiteral("是否保存对“%1”的更改？")
			.arg(m_sSqlFileName));
		box.setIcon(QMessageBox::Warning);

		// 添加按钮，QMessageBox::YesRole可以表明这个按钮的行为
		QPushButton *yesBtn = box.addButton(QStringLiteral("是(&Y)"), QMessageBox::YesRole);

		box.addButton(QStringLiteral("否(&N)"), QMessageBox::NoRole);
		QPushButton *cancelBtn = box.addButton(QStringLiteral("取消"),
			QMessageBox::RejectRole);
		box.exec(); // 弹出对话框，让用户选择是否保存修改，或者取消关闭操作
		if (box.clickedButton() == yesBtn)// 如果用户选择是，则返回保存操作的结果
		{
			doBtnSaveClicked();
			disconnect(&m_sqlFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(doAfterSqlFileChange(QString)));
		}
		else if (box.clickedButton() == cancelBtn) // 如果选择取消，则返回false
			return false;

	}
	return true; // 如果文档没有更改过，则直接返回true
}

//删除分组
void SQLiteStudio::doInternalButtonClicked(const QString sButtonId)
{
	TabButton* pBtn = qobject_cast<TabButton*>(sender());
	QTableView* pTbv = nullptr;
	pTbv = qobject_cast<QTableView*>(pBtn->userObject());
	deleteTableData(pTbv, pBtn);
	m_hashTableNameBtn.remove(sButtonId);
}