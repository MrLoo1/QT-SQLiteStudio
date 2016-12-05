#ifndef SQLITESTUDIO_H
#define SQLITESTUDIO_H

#include <QtWidgets/QWidget>
#include "ui_sqlitestudio.h"
#include <QSqlDatabase>
#include <QFileSystemWatcher>

class SQLConfig;
class SqlTableModel;
class AddSqlDialog;
class QSettings;
class QSqlTableModel;
class TabButton;

class SQLiteStudio : public QWidget
{
	Q_OBJECT

public:
	SQLiteStudio(QWidget *parent = 0);
	~SQLiteStudio();

	/*�¼�������*/
	bool eventFilter(QObject *, QEvent *);

private slots:
	/* ����DB��ҳ��ť�� */
	void doBtnSelectSqlFileClicked();
	void doBtnSelectCreatDbClicked();
	void doBtnExecClicked();
	void doBtnSaveClicked();
	void doBtnRefreshClicked();

	/* sql����޸� */
	void doSqlFileNameEditingFinished();
	void doSqlFileCurrentChanged();

	/* ����DB·���޸� */
	void doCreateDBFileNameEditingFinished();
	void doCreateDBCurrentChanged();

	/* �鿴DB·���޸� */
	void doSeeDBFileNameEditingFinished();
	void doSeeDBCurrentChanged();

	/* ���Ϊ */
	void doActSaveOther();

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

	/*�������ݿ��޸�*/
	void doCommitDBSlot();

	/*ȡ�����ݿ������еı�*/
	void doLwDBTableChanged(int row);

	/* ����ļ��б� */
	void doClearFileNameList();

	/*sql�ļ��ı��*/
	void doAfterSqlFileChange(const QString& path);

	/*��ʾ�ı����޸�*/
	void documentWasModified();

	/*ɾ��seedb��ǩ*/
	void doInternalButtonClicked(const QString sButtonId);

protected:
	void closeEvent(QCloseEvent *event); // �ر��¼�

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

	/*���ð�ť�Ƿ����*/
	void controlEnable(bool bEnable);

	/*����ini�ļ�*/
	void createIni();

	/*��ʾ�������е�����*/
	void showTableData(const QString& sTableName);

	/* ���ļ����ݼ��ص��ı��༭�� */
	bool loadTextEditor(const QString& sFilePath);

	/* ����DB�ļ� */
	bool createDatabase(const QString& sDBFileName, const QString& sSqlFileName);

	/* ��ȡ�ļ���·�� */
	QString getFileDirPath(const QString& sFilePath);

	/* ��ȡ�ļ��� */
	QString getFileName(const QString& sFilePath);

	/* ȡsql���ģ�� */
	QSqlTableModel* getSqlTableModel(const QString&);

	/* ����������tableҳ */
	bool clearTableData();

	/* ɾ��һ����ҳ */
	bool deleteTableData(QTableView*, TabButton*);

	/* ɾ���ļ��� */
	void clearFileNameList(
		const QString& sClearFileListName,
		QComboBox* cb,
		QStringList& slFileName,
		const QString& sCIParamName);

	/*�Ƿ���Ҫ����*/
	bool maybeSave();

private:
	QSettings *						m_pConfigIni;				// ini�ļ�����������DB·��

	QMenu*							m_menu;
	QAction*						m_actSave;
	QString							m_sCreateDBFileName;		// db�ļ���������ȫ·��
	QString							m_sSqlFileName;				// sql�ļ�·��
	QStringList						m_slCreateDBFileName;		// ���ʹ�õ�db�ļ���(����ȫ·��)
	QStringList						m_slSqlFileName;			// ���ʹ�õ�sql�ļ���(����ȫ·��)
	
	int								m_nTableDataRowHeight = 0;	// ����и�
	QButtonGroup					m_bgTableNameBtnFroup;		// �������ť��

	QString							m_sSeeDBFileName;			// db�ļ���������ȫ·��
	QStringList						m_slSeeDBFileName;			// ���ʹ�õ�db�ļ���(����ȫ·��)
	QSqlDatabase					m_sdbSeeDB;					// ���ݿ�����
	QString							m_sCurSelectTableName;		// ��ǰѡ��ı���
	QHash<QString, TabButton*>		m_hashTableNameBtn;			// ������Ӧ�İ�ť

	QTextDocument*					m_pTextDocument = nullptr;	// �ı��༭���ı�
	QHash<QString, QHash<int, Qt::SortOrder>> m_hashSortIndexList;		// ��������

	Ui::SQLiteStudio ui;

	QFileSystemWatcher				m_sqlFileWatcher;              //sql�ļ�������

	int								key1;				
	int								key2;
};

#endif // SQLITESTUDIO_H
