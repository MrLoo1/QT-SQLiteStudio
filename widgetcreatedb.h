#ifndef WIDGETCREATEDB_H
#define WIDGETCREATEDB_H

#include "ui_wCreateDB.h"
#include <QWidget>
#include <QFileSystemWatcher>
#include <QSqlDatabase>

class QSettings;

class widgetCreateDB : public QWidget
{
	Q_OBJECT

public:
	widgetCreateDB(QWidget *parent, QSettings* set, QLineEdit* edPw);
	~widgetCreateDB();

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

	/* ���Ϊ */
	void doActSaveAs();

	/* ����ļ��б� */
	void doClearFileNameList();

	/*sql�ļ��ı��*/
	void doAfterSqlFileChange(const QString& path);

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

	/*���е��źŲ�����*/
	void setupSignalSlots();

	/* ���ļ����ݼ��ص��ı��༭�� */
	bool loadTextEditor(const QString& sFilePath);

	/* ����DB�ļ� */
	bool createDatabase(const QString& sDBFileName, const QString& sSqlFileName);

	/* ��ȡ�ļ���·�� */
	QString getFileDirPath(const QString& sFilePath);

	/* ��ȡ�ļ��� */
	QString getFileName(const QString& sFilePath);

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
	QLineEdit*						m_pedPw;					//���������

	QMenu*							m_menu;
	QAction*						m_actSave;
	QString							m_sCreateDBFileName;		// db�ļ���������ȫ·��
	QString							m_sSqlFileName;				// sql�ļ�·��
	QStringList						m_slCreateDBFileName;		// ���ʹ�õ�db�ļ���(����ȫ·��)
	QStringList						m_slSqlFileName;			// ���ʹ�õ�sql�ļ���(����ȫ·��)

	int								m_nTableDataRowHeight = 0;	// ����и�
	QButtonGroup					m_bgTableNameBtnFroup;		// �������ť��

	QTextDocument*					m_pTextDocument = nullptr;	// �ı��༭���ı�

	Ui::wCreateDB ui;

	QFileSystemWatcher				m_sqlFileWatcher;              //sql�ļ�������
};

#endif // WIDGETCREATEDB_H
