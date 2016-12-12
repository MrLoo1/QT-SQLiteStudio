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
	/* ��ʼ������ */
	void setupUi();

	/*���е��źŲ�����*/
	void setupSignalSlots();

	/*����ini�ļ�*/
	void createIni();

private:
	QSettings *				m_pConfigIni;				// ini�ļ�����������DB·��

	Ui::sqlitestudio		ui;

	widgetCreateDB*			m_pwCreateDB;
	widgetSeeDB*			m_pwSeeDB;
};

#endif // SQLITESTUDIO_H
