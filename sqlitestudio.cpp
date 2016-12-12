#include "sqlitestudio.h"
#include "common.h"
#include "widgetcreatedb.h"
#include "widgetseedb.h"
#include <QSettings>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QRegExpValidator>
#include <QRegExp>

SQLiteStudio::SQLiteStudio(QWidget *parent)
	: QMainWindow(parent)
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

	// ������������
	createIni();

	m_pwCreateDB = new widgetCreateDB(this, m_pConfigIni,ui.edPassword);
	m_pwSeeDB = new widgetSeeDB(this, m_pConfigIni,ui.edPassword);

	//stackedWidger����
	ui.stackedWidget->addWidget(m_pwSeeDB);
	ui.stackedWidget->addWidget(m_pwCreateDB);
	ui.stackedWidget->setCurrentWidget(m_pwSeeDB);

	//�������������
	QRegExp rx("^\\S{0,6}$");
	QValidator* vaPassword = new QRegExpValidator(rx,this);
	ui.edPassword->setValidator(vaPassword);
	ui.edPassword->setEchoMode(QLineEdit::Password);
}

void SQLiteStudio::setupSignalSlots()
{	
	connect(ui.btnSeeDB, SIGNAL(clicked()), this, SLOT(doBtnSeeDBClicked()));
	connect(ui.btnCreateDB, SIGNAL(clicked()), this, SLOT(doBtnCreateDBClicked()));	
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

void SQLiteStudio::doBtnSeeDBClicked()
{
	ui.stackedWidget->setCurrentWidget(m_pwSeeDB);
}

void SQLiteStudio::doBtnCreateDBClicked()
{	
	ui.stackedWidget->setCurrentWidget(m_pwCreateDB);
}

void SQLiteStudio::closeEvent(QCloseEvent *event)
{
	m_pwCreateDB->close();
	m_pwSeeDB->close();
}