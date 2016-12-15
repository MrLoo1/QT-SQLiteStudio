#include "sqlitestudio.h"
#include "common.h"
#include "ECBCipher.h"
#include "CBCCipher.h"
#include "widgetcreatedb.h"
#include "widgetseedb.h"
#include <QSettings>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QRegExpValidator>
#include <QRegExp>

SQLiteStudio::SQLiteStudio(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setupUi();
	setupSignalSlots();
}

SQLiteStudio::~SQLiteStudio()
{
}

/* ��ʼ������ */
void SQLiteStudio::setupUi()
{
	// ���ô���ͼ��
	setWindowIcon(QIcon(RS_ICON_WINDOW));
	setWindowTitle(QSL("SQLiteStudio"));

	//���ü��ܽ��ܲ���
	m_pCripher = new ECBCipher(this);
	//m_pCripher = new CBCCipher(this);

	QFile styleFile(QSS_MAINWINDOW);
	styleFile.open(QIODevice::ReadOnly);
	QString setStyleSheet(styleFile.readAll());;
	this->setStyleSheet(setStyleSheet);

	// ������������
	createIni();

	m_pwCreateDB = new widgetCreateDB(this, m_pConfigIni,ui.edPassword);
	m_pwSeeDB = new widgetSeeDB(this, m_pConfigIni,ui.edPassword);

	//stackedWidger����
	ui.stackedWidget->addWidget(m_pwSeeDB);
	ui.stackedWidget->addWidget(m_pwCreateDB);
	ui.stackedWidget->setCurrentWidget(m_pwSeeDB);
	ui.btnSeeDB->setChecked(true);
	//�������������
	initEdPW();
}

void SQLiteStudio::setupSignalSlots()
{	
	connect(ui.btnSeeDB, SIGNAL(clicked()), this, SLOT(doBtnSeeDBClicked()));
	connect(ui.btnCreateDB, SIGNAL(clicked()), this, SLOT(doBtnCreateDBClicked()));
}

/*��ȡ�����ļ�*/
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

void SQLiteStudio::doBtnSeeDBClicked()
{
	ui.stackedWidget->setCurrentWidget(m_pwSeeDB);
	ui.btnCreateDB->setChecked(false);
	ui.btnSeeDB->setChecked(true);
}

void SQLiteStudio::doBtnCreateDBClicked()
{	
	ui.stackedWidget->setCurrentWidget(m_pwCreateDB);
	ui.btnSeeDB->setChecked(false);
	ui.btnCreateDB->setChecked(true);
}

void SQLiteStudio::closeEvent(QCloseEvent *event)
{
	/*������ܺ�д�����ļ�*/
	QString encod = "";
	if (!ui.edPassword->text().isEmpty())
	{
		encod = encodePW(ui.edPassword->text());
	}
	m_pConfigIni->setValue(SETTING_NAME_PASSWORD_PATH, encod);
	m_pwCreateDB->close();
	m_pwSeeDB->close();
}

QString SQLiteStudio::encodePW(QString clearpw)
{
	QString encoPW = "";
	if (m_pCripher->DES_Encrypt(clearpw, PW_KEY, encoPW) != ENCRYPT_SUCCESS)
	{
		QMessageBox::critical(this,QSL("����ʧ��"),QSL("�����û�����ʧ��."));
	}
	return encoPW;
}

QString SQLiteStudio::decodePW(QString cryptpw)
{
	QString clePW = "";
	if (m_pCripher->DES_Decrypt(cryptpw, PW_KEY, clePW) != DECRYPT_SUCCESS)
	{
		QMessageBox::critical(this, QSL("����ʧ��"), QSL("�����û�����ʧ��."));
	}
	return clePW;
}

void SQLiteStudio::initEdPW()
{
	/*���������������������*/
	QRegExp rx("\\S{0,32}");
	QValidator* vaPassword = new QRegExpValidator(rx, this);
	ui.edPassword->setValidator(vaPassword);
	ui.edPassword->setEchoMode(QLineEdit::Password);

	/*��ȡ�����ļ��е�����*/
	QString decod = "";
	QString encoPassword = m_pConfigIni->value(SETTING_NAME_PASSWORD_PATH).toString();
	if (!encoPassword.isEmpty())
	{
		decod = decodePW(encoPassword);
	}
	ui.edPassword->setText(decod);
}

