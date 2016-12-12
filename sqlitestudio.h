#ifndef SQLITESTUDIO_H
#define SQLITESTUDIO_H

#include <qmainwindow.h>
#include "ui_sqlitestudio.h"

class QSettings;
class widgetCreateDB;
class widgetSeeDB;

class SQLiteStudio : public QMainWindow
{
	Q_OBJECT

public:
	SQLiteStudio(QWidget *parent = 0);
	~SQLiteStudio();

private slots:
	void doBtnSeeDBClicked();
	
	void doBtnCreateDBClicked();

protected:
	void closeEvent(QCloseEvent *event);

private:
	/* 初始化界面 */
	void setupUi();

	/*所有的信号槽链接*/
	void setupSignalSlots();

	/*创建ini文件*/
	void createIni();

private:
	QSettings *				m_pConfigIni;				// ini文件，用来保存DB路径

	Ui::sqlitestudio		ui;

	widgetCreateDB*			m_pwCreateDB;
	widgetSeeDB*			m_pwSeeDB;
};

#endif // SQLITESTUDIO_H
