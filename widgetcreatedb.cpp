#include "widgetcreatedb.h"
#include <QSettings>
#include "common.h"
#include <QSqlDriver>
#include <QFileDialog>
#include <QMenu>
#include <QTableView>
#include <QMessageBox>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCloseEvent>
#include <QMimeData>
#include <QPushButton>
widgetCreateDB::widgetCreateDB(QWidget *parent, QSettings* set, QLineEdit* edPw)
	: QWidget(parent), m_pConfigIni(set), m_pedPw(edPw)
{
	ui.setupUi(this);
	setupUi();
	setupSignalSlots();
}

widgetCreateDB::~widgetCreateDB()
{
	if (m_pTextDocument)
		delete m_pTextDocument;
}

/* 初始化界面 */
void widgetCreateDB::setupUi()
{
	// sql语句编辑框
	ui.edSql->setWordWrapMode(QTextOption::NoWrap);

	// 初始化文件路径
	m_slCreateDBFileName = m_pConfigIni->value(SETTING_NAME_CREATEDB_FIELPATH)
		.toString().split(SETTING_VALUE_SPLIT, QString::SkipEmptyParts);
	createFileDropList(m_slCreateDBFileName, ui.cbCreateDB);

	m_slSqlFileName = m_pConfigIni->value(SETTING_NAME_SQL_FIELPATH)
		.toString().split(SETTING_VALUE_SPLIT, QString::SkipEmptyParts);
	createFileDropList(m_slSqlFileName, ui.cbSqlFile);

	// 创建DB保存按钮下拉菜单
	m_menu = new QMenu(this);
	m_actSave = new QAction(QSL("另存为"), m_menu);
	m_menu->addAction(m_actSave);
	ui.btnSave->setMenu(m_menu);
	
	// 设置表格行高
	m_nTableDataRowHeight = ROWHEIGHT_DEF;

	ui.btnSelectCreateDB->setIcon(QIcon(RS_ICON_OPENFILE));
	ui.btnSelectSqlFile->setIcon(QIcon(RS_ICON_OPENFILE));

	ui.cbCreateDB->installEventFilter(this);
	ui.cbSqlFile->installEventFilter(this);
}

void widgetCreateDB::setupSignalSlots()
{
	// 生成db插页
	connect(m_actSave, SIGNAL(triggered()), this, SLOT(doActSaveAs()));

	connect(ui.cbSqlFile->lineEdit(), SIGNAL(editingFinished())
		, this, SLOT(doSqlFileNameEditingFinished()));
	connect(ui.cbSqlFile, SIGNAL(currentIndexChanged(int)),
		this, SLOT(doSqlFileCurrentChanged()));

	connect(ui.cbCreateDB->lineEdit(), SIGNAL(editingFinished())
		, this, SLOT(doCreateDBFileNameEditingFinished()));
	connect(ui.cbCreateDB, SIGNAL(currentIndexChanged(int)),
		this, SLOT(doCreateDBCurrentChanged()));

	connect(ui.btnSelectSqlFile, SIGNAL(clicked()), this, SLOT(doBtnSelectSqlFileClicked()));
	connect(ui.btnSelectCreateDB, SIGNAL(clicked()), this, SLOT(doBtnSelectCreatDbClicked()));
	connect(ui.btnExec, SIGNAL(clicked()), this, SLOT(doBtnExecClicked()));
	connect(ui.btnRefresh, SIGNAL(clicked()), this, SLOT(doBtnRefreshClicked()));
	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(doBtnSaveClicked()));

	//sql文件监视器
	connect(&m_sqlFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(doAfterSqlFileChange(QString)));
}

/* 创建文件下拉列表 */
void widgetCreateDB::createFileDropList(
	const QStringList& slFileList,
	QComboBox* cb)
{
	cb->blockSignals(true);
	cb->clear();
	for (int i = 0; i < slFileList.size(); ++i)
	{
		cb->insertItem(0, slFileList[i], slFileList[i]);
	}
	cb->addItem(QIcon(RS_ICON_Clear), QSL("清楚下拉框"), QSL("清楚下拉框"));
	cb->setCurrentIndex(-1);
	cb->blockSignals(false);
}

void widgetCreateDB::doActSaveAs()
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
		QMessageBox::critical(this, QSL("打开文件失败"), QSL("请检验您所输入的文件名是否正确。"));
		return;
	}
}

void widgetCreateDB::doSqlFileNameEditingFinished()
{
	if (ui.cbSqlFile->currentIndex() == ui.cbSqlFile->count() - 1)
	{
		return;
	}
	doSqlFileCurrentChanged();
}

void widgetCreateDB::doCreateDBFileNameEditingFinished()
{
	if (ui.cbCreateDB->currentIndex() == ui.cbCreateDB->count() - 1)
	{
		return;
	}
	doCreateDBCurrentChanged();
}

void widgetCreateDB::doSqlFileCurrentChanged()
{
	if (ui.cbSqlFile->currentIndex() == ui.cbSqlFile->count() - 1)
	{
		doClearFileNameList(SETTING_NAME_SQL_FIELPATH);
		return;
	}
	QString sCurFile = ui.cbSqlFile->currentText();
	if (sCurFile.isEmpty() || sCurFile == m_sSqlFileName) return;
	if (sCurFile.right(4) != ".sql") return;
	m_sSqlFileName = sCurFile;
	if (!loadTextEditor(sCurFile))
	{
		ui.cbCreateDB->blockSignals(true);
		ui.cbCreateDB->setCurrentIndex(-1);
		ui.cbCreateDB->blockSignals(false);
		return;
	}
	addFileName(ui.cbSqlFile, m_slSqlFileName
		, sCurFile, SETTING_NAME_SQL_FIELPATH);
	// 重置生成db的文件路径
	QString sCreateDBFileName;
	sCreateDBFileName = m_sSqlFileName.left(m_sSqlFileName.length() - 4) + ".db";
	addFileName(ui.cbCreateDB, m_slCreateDBFileName
		, sCreateDBFileName, SETTING_NAME_CREATEDB_FIELPATH);
	ui.cbCreateDB->setCurrentText(sCreateDBFileName);
	m_sCreateDBFileName = sCreateDBFileName;
}

void widgetCreateDB::doCreateDBCurrentChanged()
{
	if (ui.cbCreateDB->currentIndex() == ui.cbCreateDB->count() - 1)
	{
		doClearFileNameList(SETTING_NAME_CREATEDB_FIELPATH);
		return;
	}
	QString sCurFile = ui.cbCreateDB->currentText();
	if (sCurFile.isEmpty() 
		|| m_sCreateDBFileName == sCurFile
		|| sCurFile.right(3) != ".db") 
		return;
	addFileName(ui.cbCreateDB, m_slCreateDBFileName
		, sCurFile, SETTING_NAME_CREATEDB_FIELPATH);
	m_sCreateDBFileName = sCurFile;
}

/* 添加文件名到下拉列表 */
void widgetCreateDB::addFileName(
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
		cb->insertItem(0,sFileName, sFileName);
		cb->blockSignals(false);
	}
}

void widgetCreateDB::doBtnExecClicked()
{
	createDatabase(ui.cbCreateDB->currentText(), ui.cbSqlFile->currentText());
}

bool widgetCreateDB::createDatabase(const QString &sDBFileName, const QString &sSqlFileName)
{
	if (sSqlFileName.isEmpty() || sDBFileName.isEmpty()) return false;

	// 删除已有的文件，新建一个db文件
	QDir oDir;
	QFile oFile(m_sCreateDBFileName);
	if (oFile.exists()
		&& !oFile.remove())
	{
		QMessageBox::critical(this, QSL("删除db文件"), QSL("删除文件[%1]失败：\n%2")
			.arg(m_sCreateDBFileName)
			.arg(oFile.errorString()));
		return false;
	}
	else
	{
		oDir.setPath(getFileDirPath(m_sCreateDBFileName));
		if (!oDir.exists() && !oDir.mkpath(oDir.path()))
		{
			QMessageBox::critical(this, QSL("路径不存在"), QSL("路径[%1]不存在,且创建失败!").arg(oDir.path()));
			return false;
		}

		if (!oFile.open(QIODevice::ReadWrite))
		{
			QMessageBox::critical(this, QSL("打开文件失败"), QSL("打开文件[%1]失败：\n%2")
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
	QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER", "connection");
	db.setDatabaseName(m_sCreateDBFileName);
	db.setPassword(m_pedPw->text());
	db.setConnectOptions(SQLITECIPHER_CREATE_KEY);
	qDebug() << db.databaseName() << " " << db.password();

	if (!db.open())
	{
		QMessageBox::critical(this, QSL("打开db失败"), QSL("打开数据库[%1]失败:\n%2")
			.arg(m_sCreateDBFileName)
			.arg(db.lastError().text())
			, QSL("打开数据库失败"));
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
				QMessageBox::critical(this, QSL("读取文件出错"), QSL("文件不能为空且文件内要有分隔符！"));
				db.close();
				return false;
			}
		}
		else
		{
			QMessageBox::critical(this, QSL("打不开文件"), QSL("请检验您所输入的文件名是否正确。"));
			db.close();
			return false;
		}
	}


	// 启动数据库事务
	if (!db.driver()->beginTransaction())
	{
		QString sInfo = QSL("失败：启动数据事务失败!\n");
		QString sErrorMsg = QSL("错误原因:") + db.driver()->lastError().text();
		QMessageBox::critical(this, QSL("启动事务失败"), sInfo + sErrorMsg, QSL("提示"));
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
				QString sInfo = QSL("失败：执行第") + sI + QSL("次SQL语句失败！\n");
				QString sErrorMsg = QSL("错误原因:") + oSQLQuery.lastError().text();
				QString sErrorSqlTitle = QSL("\n失败语句:");
				QString sSqlBegin = QSL("\n<----------------------------------------------------------------------------------------->\n");
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
				QString sSqlEnd = QSL("\n<----------------------------------------------------------------------------------------->");
				QMessageBox::critical(this, QSL("提示"), sInfo + sErrorMsg + sErrorSqlTitle + sSqlBegin + sSql + sSqlEnd);
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
		QString sInfo = QSL("失败：提交数据事务失败!\n");
		QString sErrorMsg = QSL("错误原因:") + db.driver()->lastError().text();
		QMessageBox::critical(this, QSL("提交事务"), sInfo + sErrorMsg);
	}

	db.close();
	QMessageBox::information(this, QSL("成功"), QSL("生成数据库[%1]成功!").arg(sDBFileName));
	ui.pbExecSql->setValue(0);

	return true;
}

void widgetCreateDB::doBtnSelectSqlFileClicked()
{
	QString sSelectSqlFileName = QFileDialog::getOpenFileName(
		this, QSL("选择文件"), m_sSqlFileName, "*.sql");
	if (!sSelectSqlFileName.isEmpty() && m_sSqlFileName != sSelectSqlFileName)
	{
		ui.cbSqlFile->setCurrentText(sSelectSqlFileName);
		doSqlFileCurrentChanged();
	}
}

void widgetCreateDB::doBtnSelectCreatDbClicked()
{
	QString sSelectCreateDBFileName = QFileDialog::getSaveFileName(this,
		tr("Select Save Database File"), "/home/" + ui.cbCreateDB->currentData().toString(), tr("Database Files (*.db)"));
	if (!sSelectCreateDBFileName.isEmpty() && m_sCreateDBFileName != sSelectCreateDBFileName)
	{
		ui.cbCreateDB->setCurrentText(sSelectCreateDBFileName);
		doCreateDBCurrentChanged();
	}
}

void widgetCreateDB::doBtnRefreshClicked()
{
	if (!loadTextEditor(m_sSqlFileName))
	{
		// TODO::文件加载失败
	}
}

/* 将文件内容加载到文本编辑框 */
bool widgetCreateDB::loadTextEditor(const QString& sFilePath)
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
		m_pTextDocument->setModified(false);
		ui.edSql->setDocument(m_pTextDocument);
		file.close();
		m_sqlFileWatcher.removePaths(m_sqlFileWatcher.files());
		m_sqlFileWatcher.addPath(m_sSqlFileName);
		return true;
	}
	else
	{
		QMessageBox::critical(this, QSL("打开文件失败"), QSL("打开文件失败:%1").arg(file.errorString()));
		return false;
	}
}

void widgetCreateDB::doBtnSaveClicked()
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
		QMessageBox::information(0, QSL("保存成功"), QSL("保存成功"));
		file.close();
	}
	else
	{
		QMessageBox::critical(this, QSL("打不开文件"), QSL("请检验您所输入的文件名是否正确。"));
		return;
	}
}

/* 清空文件列表 */
void widgetCreateDB::doClearFileNameList(QString set)
{
	if (set == SETTING_NAME_SQL_FIELPATH)
	{
		clearFileNameList(QSL("生成DB的SQL脚本"), ui.cbSqlFile,
			m_slSqlFileName, SETTING_NAME_SQL_FIELPATH);
	}
	else if (set == SETTING_NAME_CREATEDB_FIELPATH)
	{
		clearFileNameList(QSL("生成的DB"), ui.cbCreateDB,
			m_slCreateDBFileName, SETTING_NAME_CREATEDB_FIELPATH);
	}
}

/* 获取文件夹路径 */
QString widgetCreateDB::getFileDirPath(const QString& sFilePath)
{
	QStringList slFile = sFilePath.split("/", QString::SkipEmptyParts);
	return sFilePath.left(sFilePath.length() - slFile.back().length() - 1);
}

/* 获取文件名 */
QString widgetCreateDB::getFileName(const QString& sFilePath)
{
	QStringList slFile = sFilePath.split("/", QString::SkipEmptyParts);
	return slFile.back();
}

/* 删除文件名 */
void widgetCreateDB::clearFileNameList(
	const QString& sClearFileListName,
	QComboBox* cb,
	QStringList& slFileName,
	const QString& sCIParamName)
{
	cb->blockSignals(true);
	QMessageBox box(QMessageBox::Information, QSL("清空文件列表"), QSL("确定清空[%1]文件列表吗？").arg(sClearFileListName));
	box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	box.setButtonText(QMessageBox::Yes, QSL("确 定"));
	box.setButtonText(QMessageBox::No, QSL("取 消"));
	if (box.exec() == QMessageBox::Yes)
	{
		slFileName.clear();
		cb->clear();
		cb->addItem(QIcon(RS_ICON_Clear), QSL("清楚下拉框"), QSL("清楚下拉框"));
		m_pConfigIni->setValue(sCIParamName, "");
	}
	if (cb == ui.cbCreateDB)
	{
		cb->setCurrentIndex(cb->findData(m_sCreateDBFileName));
	}
	else if (cb == ui.cbSqlFile)
	{
		cb->setCurrentIndex(cb->findData(m_sSqlFileName));
	}
	cb->blockSignals(false);
}

/*sql文件改变槽*/
void widgetCreateDB::doAfterSqlFileChange(const QString& path)
{
	QMessageBox box(QMessageBox::Information, QSL("文件修改"), QSL("SQL文件[%1]已被修改。\n请问是否需要重新加载？").arg(path));
	box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	box.setButtonText(QMessageBox::Yes, QSL("确 定"));
	box.setButtonText(QMessageBox::No, QSL("取 消"));
	if (box.exec() == QMessageBox::Yes)
	{
		doBtnRefreshClicked();
	}
}

void widgetCreateDB::closeEvent(QCloseEvent *event)
{
	if (maybeSave()) { // 如果maybeSave()函数返回true，则关闭窗口
		event->accept();
	}
	else {   // 否则忽略该事件
		event->ignore();
	}
}

bool widgetCreateDB::maybeSave()  // 是否需要保存
{
	if (m_pTextDocument && m_pTextDocument->isModified()) { // 如果文档被更改过
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

bool widgetCreateDB::eventFilter(QObject *obj, QEvent * evt)
{
	if (evt->type() == QEvent::KeyPress)
	{
		QKeyEvent* ev = static_cast<QKeyEvent*>(evt);
		if (ev->key() == Qt::Key_Return)
		{
			if (obj == ui.cbCreateDB)
			{
				doCreateDBFileNameEditingFinished();
				return true;
			}
			else if (obj == ui.cbSqlFile)
			{
				doSqlFileNameEditingFinished();
				return true;
			}
		}
	}
	return false;
}

void widgetCreateDB::dragEnterEvent(QDragEnterEvent *event)
{
	//如果为文件，则支持拖放
	if (event->mimeData()->hasFormat("text/uri-list"))
		event->acceptProposedAction();
}

void widgetCreateDB::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty())
		return;

	//往文本框中追加文件名
	QString file_name = urls[0].toLocalFile();
	ui.cbSqlFile->setCurrentText(file_name);
	doSqlFileNameEditingFinished();
}