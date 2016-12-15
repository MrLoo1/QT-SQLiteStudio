#ifndef SQLITESTUDIO_H
#define SQLITESTUDIO_H

#include <qdialog.h>
#include "ui_sqlitestudio.h"

class QSettings;
class widgetCreateDB;
class widgetSeeDB;
class Cipher;

class SQLiteStudio : public QDialog
{
	Q_OBJECT

public:
	SQLiteStudio(QWidget *parent = 0);
	~SQLiteStudio();

private slots:
	void doBtnSeeDBClicked();
	
	void doBtnCreateDBClicked();

protected:
	void closeEvent(QCloseEvent *event); // 关闭事件

private:
	/* 初始化界面 */
	void setupUi();

	/*所有的信号槽链接*/
	void setupSignalSlots();

	/*创建ini文件*/
	void createIni();

	/*密码加密*/
	QString encodePW(QString clearpw);

	/*密码解密*/
	QString decodePW(QString cryptpw);

	/*初始化密码输入框*/
	void initEdPW();

private:
	QSettings *				m_pConfigIni;				// ini文件，用来保存DB路径
	Cipher*					m_pCripher;					//加密类,策略模式

	Ui::sqlitestudio		ui;

	widgetCreateDB*			m_pwCreateDB;
	widgetSeeDB*			m_pwSeeDB;
};

#endif // SQLITESTUDIO_H
