#ifndef WIDGETSEEDB_H
#define WIDGETSEEDB_H
#include "ui_wSeeDB.h"
#include <QWidget>
#include <QSqlDatabase>

class SqlTableModel;
class AddSqlDialog;
class QSettings;
class QSqlTableModel;
class KTabButton;
class QLineEdit;

class widgetSeeDB : public QWidget
{
	Q_OBJECT

public:
	widgetSeeDB(QWidget *parent, QSettings* set,QLineEdit* edPw);
	~widgetSeeDB();

	/*�¼�������*/
	bool eventFilter(QObject *, QEvent *);

private slots:
	/* �鿴DB·���޸� */
	void doSeeDBFileNameEditingFinished();
	void doSeeDBCurrentChanged();

	/*�鿴DB��ҳ��ť��*/
	void doBtnSelectSeeDBClicked();
	void doBtnRefreshSeeDBClicked();
	void doBtnInsertClicked();
	void doBtnDeleteClicked();
	void doBtnCommitClicked();
	void doBtnRevertClicked();
	void doSwitchShowTable();
	void doActDropTable();
	void doActQryTable();
	void doBtnCloseClicked();

	/*�������ݿ��޸�*/
	void doCommitDBSlot();

	/*ȡ�����ݿ������еı�*/
	void doLwDBTableChanged(int row);

	/* ����ļ��б� */
	void doClearFileNameList();

	/*ɾ��seedb��ǩ*/
	void doInternalButtonClicked(const QString sButtonId);

private:
	/* ��ʼ������ */
	void setupUi();

	/* �����ļ������б� */
	void createFileDropList(const QStringList& slFileList, QComboBox* cb);

	/* ����ļ����������б� */
	void addFileName(
		QComboBox* cb,
		QStringList& slFileName,
		const QString& sFileName,
		const QString& sCIParamName);

	/* �������ݿ����ݱ� */
	void loadSeeDBTable();

	/*���е��źŲ�����*/
	void setupSignalSlots();

	/*ѡ��DB�ļ�*/
	void selectedSeeDB();

	/*��ʾ�������е�����*/
	void showTableData(const QString& sTableName);

	/* ȡsql���ģ�� */
	QSqlTableModel* getSqlTableModel(const QString&);

	/* ����������tableҳ */
	bool clearTableData();

	/* ɾ��һ����ҳ */
	bool deleteTableData(QTableView*, KTabButton*);

	/*����seedbҳ��ť*/
	void setSeeDBBtn(bool enable);

private:
	QSettings *						m_pConfigIni;				// ini�ļ�����������DB·��
	QLineEdit*						m_pedPw;					//���������

	int								m_nTableDataRowHeight = 0;	// ����и�
	QButtonGroup					m_bgTableNameBtnFroup;		// �������ť��

	QString							m_sSeeDBFileName;			// db�ļ���������ȫ·��
	QStringList						m_slSeeDBFileName;			// ���ʹ�õ�db�ļ���(����ȫ·��)
	QSqlDatabase					m_sdbSeeDB;					// ���ݿ�����
	QString							m_sCurSelectTableName;		// ��ǰѡ��ı���
	QHash<QString, KTabButton*>		m_hashTableNameBtn;			// ������Ӧ�İ�ť

	int								key1;
	int								key2;

	Ui::wSeeDB ui;		
};

#endif // WIDGETSEEDB_H
