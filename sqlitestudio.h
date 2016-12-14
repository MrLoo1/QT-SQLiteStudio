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
	void closeEvent(QCloseEvent *event); // �ر��¼�

private:
	/* ��ʼ������ */
	void setupUi();

	/*���е��źŲ�����*/
	void setupSignalSlots();

	/*����ini�ļ�*/
	void createIni();

	/*�������*/
	QString encodePW(QString clearpw);

	/*�������*/
	QString decodePW(QString cryptpw);

	/*��ʼ�����������*/
	void initEdPW();

private:
	QSettings *				m_pConfigIni;				// ini�ļ�����������DB·��
	Cipher*					m_pCripher;					//������

	Ui::sqlitestudio		ui;

	widgetCreateDB*			m_pwCreateDB;
	widgetSeeDB*			m_pwSeeDB;
};

#endif // SQLITESTUDIO_H
