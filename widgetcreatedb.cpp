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

/* ��ʼ������ */
void widgetCreateDB::setupUi()
{
	// sql���༭��
	ui.edSql->setWordWrapMode(QTextOption::NoWrap);

	// ��ʼ���ļ�·��
	m_slCreateDBFileName = m_pConfigIni->value(SETTING_NAME_CREATEDB_FIELPATH)
		.toString().split(SETTING_VALUE_SPLIT, QString::SkipEmptyParts);
	createFileDropList(m_slCreateDBFileName, ui.cbCreateDB);

	m_slSqlFileName = m_pConfigIni->value(SETTING_NAME_SQL_FIELPATH)
		.toString().split(SETTING_VALUE_SPLIT, QString::SkipEmptyParts);
	createFileDropList(m_slSqlFileName, ui.cbSqlFile);

	// ����DB���水ť�����˵�
	m_menu = new QMenu(this);
	m_actSave = new QAction(KSL("���Ϊ"), m_menu);
	m_menu->addAction(m_actSave);
	ui.btnSave->setMenu(m_menu);

	// ���ñ���и�
	m_nTableDataRowHeight = ROWHEIGHT_DEF;

	// ��������б�
	ui.btnClearSqlFile->setIcon(QIcon(RS_ICON_Clear));
	ui.btnClearCreateDB->setIcon(QIcon(RS_ICON_Clear));
}

void widgetCreateDB::setupSignalSlots()
{
	// ����db��ҳ
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

	// ����ļ��б�
	connect(ui.btnClearSqlFile, SIGNAL(clicked()), this, SLOT(doClearFileNameList()));
	connect(ui.btnClearCreateDB, SIGNAL(clicked()), this, SLOT(doClearFileNameList()));

	//sql�ļ�������
	connect(&m_sqlFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(doAfterqlFileChange(QString)));
}

/* �����ļ������б� */
void widgetCreateDB::createFileDropList(
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
		QMessageBox::critical(this, KSL("���ļ�ʧ��"), KSL("���������������ļ����Ƿ���ȷ��"));
		return;
	}
}

void widgetCreateDB::doSqlFileNameEditingFinished()
{
	doSqlFileCurrentChanged();
}

void widgetCreateDB::doCreateDBFileNameEditingFinished()
{
	doCreateDBCurrentChanged();
}

void widgetCreateDB::doSqlFileCurrentChanged()
{
	QString sCurFile = ui.cbSqlFile->currentText();
	if (sCurFile.isEmpty() || sCurFile == m_sSqlFileName) return;
	if (sCurFile.right(4) != ".sql") return;
	addFileName(ui.cbSqlFile, m_slSqlFileName
		, sCurFile, SETTING_NAME_SQL_FIELPATH);
	m_sSqlFileName = sCurFile;
	if (!loadTextEditor(sCurFile))
	{
		QMessageBox::critical(this, KSL("���ļ�ʧ��"), KSL("SQL�ļ�[%1]����ʧ��!").arg(sCurFile));
	}

	// ��������db���ļ�·��
	QString sCreateDBFileName;
	sCreateDBFileName = m_sSqlFileName.left(m_sSqlFileName.length() - 4) + ".db";
	addFileName(ui.cbCreateDB, m_slCreateDBFileName
		, sCreateDBFileName, SETTING_NAME_CREATEDB_FIELPATH);
	ui.cbCreateDB->setCurrentText(sCreateDBFileName);
	m_sCreateDBFileName = sCreateDBFileName;
}

void widgetCreateDB::doCreateDBCurrentChanged()
{
	QString sCurFile = ui.cbCreateDB->currentText();
	if (sCurFile.isEmpty() || m_sCreateDBFileName == sCurFile) return;
	if (sCurFile.right(3) != ".db") return;
	addFileName(ui.cbCreateDB, m_slCreateDBFileName
		, sCurFile, SETTING_NAME_CREATEDB_FIELPATH);
	m_sCreateDBFileName = sCurFile;
}

/* ����ļ����������б� */
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
		cb->addItem(sFileName, sFileName);
}

void widgetCreateDB::doBtnExecClicked()
{
	createDatabase(ui.cbCreateDB->currentText(), ui.cbSqlFile->currentText());
}

bool widgetCreateDB::createDatabase(const QString &sDBFileName, const QString &sSqlFileName)
{
	if (sSqlFileName.isEmpty() || sDBFileName.isEmpty()) return false;

	// ɾ�����е��ļ����½�һ��db�ļ�
	QDir oDir;
	QFile oFile(m_sCreateDBFileName);
	if (oFile.exists()
		&& !oFile.remove())
	{
		QMessageBox::critical(this, KSL("ɾ��db�ļ�"), KSL("ɾ���ļ�[%1]ʧ�ܣ�\n%2")
			.arg(m_sCreateDBFileName)
			.arg(oFile.errorString()));
		return false;
	}
	else
	{
		oDir.setPath(getFileDirPath(m_sCreateDBFileName));
		if (!oDir.exists() && !oDir.mkpath(oDir.path()))
		{
			QMessageBox::critical(this, KSL("·��������"), KSL("·��[%1]������,�Ҵ���ʧ��!").arg(oDir.path()));
			return false;
		}

		if (!oFile.open(QIODevice::ReadWrite))
		{
			QMessageBox::critical(this, KSL("���ļ�ʧ��"), KSL("���ļ�[%1]ʧ�ܣ�\n%2")
				.arg(m_sCreateDBFileName)
				.arg(oFile.errorString()));
			return false;
		}
		else
		{
			oFile.close();
		}
	}

	// �����ݿ��ļ�
	QSqlDatabase db = QSqlDatabase::addDatabase("SQLITECIPHER", "connection");
	db.setDatabaseName(m_sCreateDBFileName);
	db.setPassword(m_pedPw->text());
	db.setConnectOptions(SQLITECIPHER_CREATE_KEY);
	qDebug() << db.databaseName() << " " << db.password();

	if (!db.open())
	{
		QMessageBox::critical(this, KSL("��dbʧ��"), KSL("�����ݿ�[%1]ʧ��:\n%2")
			.arg(m_sCreateDBFileName)
			.arg(db.lastError().text())
			, KSL("�����ݿ�ʧ��"));
		return false;
	}

	// ��ȡSQL���
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
				QMessageBox::critical(this, KSL("��ȡ�ļ�����"), KSL("�ļ�����Ϊ�����ļ���Ҫ�зָ�����"));
				db.close();
				return false;
			}
		}
		else
		{
			QMessageBox::critical(this, KSL("�򲻿��ļ�"), KSL("���������������ļ����Ƿ���ȷ��"));
			db.close();
			return false;
		}
	}


	// �������ݿ�����
	if (!db.driver()->beginTransaction())
	{
		QString sInfo = KSL("ʧ�ܣ�������������ʧ��!\n");
		QString sErrorMsg = KSL("����ԭ��:") + db.driver()->lastError().text();
		QMessageBox::critical(this, KSL("��������ʧ��"), sInfo + sErrorMsg, KSL("��ʾ"));
	}

	// ִ��SQL���
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
				QString sInfo = KSL("ʧ�ܣ�ִ�е�") + sI + KSL("��SQL���ʧ�ܣ�\n");
				QString sErrorMsg = KSL("����ԭ��:") + oSQLQuery.lastError().text();
				QString sErrorSqlTitle = KSL("\nʧ�����:");
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
				QMessageBox::critical(this, KSL("��ʾ"), sInfo + sErrorMsg + sErrorSqlTitle + sSqlBegin + sSql + sSqlEnd);
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
		QString sInfo = KSL("ʧ�ܣ��ύ��������ʧ��!\n");
		QString sErrorMsg = KSL("����ԭ��:") + db.driver()->lastError().text();
		QMessageBox::critical(this, KSL("�ύ����"), sInfo + sErrorMsg);
	}

	db.close();
	QMessageBox::information(this, KSL("�ɹ�"), KSL("�������ݿ�[%1]�ɹ�!").arg(sDBFileName));
	ui.pbExecSql->setValue(0);

	return true;
}

void widgetCreateDB::doBtnSelectSqlFileClicked()
{
	QString sSelectSqlFileName = QFileDialog::getOpenFileName(
		this, KSL("ѡ���ļ�"), m_sSqlFileName, "*.sql");
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
		ui.cbCreateDB->setCurrentText(m_sCreateDBFileName);
		doCreateDBCurrentChanged();
	}
}

void widgetCreateDB::doBtnRefreshClicked()
{
	if (!loadTextEditor(m_sSqlFileName))
	{
		// TODO::�ļ�����ʧ��
	}
}

/* ���ļ����ݼ��ص��ı��༭�� */
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
		QMessageBox::critical(this, KSL("���ļ�ʧ��"), KSL("���ļ�ʧ��:%1").arg(file.errorString()));
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
		QMessageBox::information(0, KSL("����ɹ�"), KSL("����ɹ�"));
		file.close();
	}
	else
	{
		QMessageBox::critical(this, KSL("�򲻿��ļ�"), KSL("���������������ļ����Ƿ���ȷ��"));
		return;
	}
}

/* ����ļ��б� */
void widgetCreateDB::doClearFileNameList()
{
	QToolButton* btn = qobject_cast<QToolButton*>(sender());
	if (btn == ui.btnClearSqlFile)
	{
		clearFileNameList(KSL("����DB��SQL�ű�"), ui.cbSqlFile,
			m_slSqlFileName, SETTING_NAME_SQL_FIELPATH);
	}
	else if (btn == ui.btnClearCreateDB)
	{
		clearFileNameList(KSL("���ɵ�DB"), ui.cbCreateDB,
			m_slCreateDBFileName, SETTING_NAME_CREATEDB_FIELPATH);
	}
}

/* ��ȡ�ļ���·�� */
QString widgetCreateDB::getFileDirPath(const QString& sFilePath)
{
	QStringList slFile = sFilePath.split("/", QString::SkipEmptyParts);
	return sFilePath.left(sFilePath.length() - slFile.back().length() - 1);
}

/* ��ȡ�ļ��� */
QString widgetCreateDB::getFileName(const QString& sFilePath)
{
	QStringList slFile = sFilePath.split("/", QString::SkipEmptyParts);
	return slFile.back();
}

/* ɾ���ļ��� */
void widgetCreateDB::clearFileNameList(
	const QString& sClearFileListName,
	QComboBox* cb,
	QStringList& slFileName,
	const QString& sCIParamName)
{
	if (QMessageBox::information(this, KSL("����ļ��б�"), KSL("ȷ�����[%1]�ļ��б���").arg(sClearFileListName),
		QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		slFileName.clear();
		cb->clear();

		m_pConfigIni->setValue(sCIParamName, "");
	}
}

/*sql�ļ��ı��*/
void widgetCreateDB::doAfterSqlFileChange(const QString& path)
{
	if (QMessageBox::information(this, KSL("Infomation")
		, KSL("The sql file [%1] has been changed !\nDo you want to reload it?").arg(path),
		QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		doBtnRefreshClicked();
	}
}

void widgetCreateDB::closeEvent(QCloseEvent *event)
{
	if (maybeSave()) { // ���maybeSave()��������true����رմ���
		event->accept();
	}
	else {   // ������Ը��¼�
		event->ignore();
	}
}

bool widgetCreateDB::maybeSave()  // �Ƿ���Ҫ����
{
	if (m_pTextDocument && m_pTextDocument->isModified()) { // ����ĵ������Ĺ�
		QMessageBox box;
		box.setWindowTitle(QStringLiteral("SQL�ļ�"));
		box.setText(QStringLiteral("�Ƿ񱣴�ԡ�%1���ĸ��ģ�")
			.arg(m_sSqlFileName));
		box.setIcon(QMessageBox::Warning);

		// ��Ӱ�ť��QMessageBox::YesRole���Ա��������ť����Ϊ
		QPushButton *yesBtn = box.addButton(QStringLiteral("��(&Y)"), QMessageBox::YesRole);

		box.addButton(QStringLiteral("��(&N)"), QMessageBox::NoRole);
		QPushButton *cancelBtn = box.addButton(QStringLiteral("ȡ��"),
			QMessageBox::RejectRole);
		box.exec(); // �����Ի������û�ѡ���Ƿ񱣴��޸ģ�����ȡ���رղ���
		if (box.clickedButton() == yesBtn)// ����û�ѡ���ǣ��򷵻ر�������Ľ��
		{
			doBtnSaveClicked();
			disconnect(&m_sqlFileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(doAfterSqlFileChange(QString)));
		}
		else if (box.clickedButton() == cancelBtn) // ���ѡ��ȡ�����򷵻�false
			return false;
	}
	return true; // ����ĵ�û�и��Ĺ�����ֱ�ӷ���true
}