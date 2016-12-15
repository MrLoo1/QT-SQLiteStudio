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
	/* 创建DB插页按钮槽 */
	void doBtnSelectSqlFileClicked();
	void doBtnSelectCreatDbClicked();
	void doBtnExecClicked();
	void doBtnSaveClicked();
	void doBtnRefreshClicked();

	/* sql语句修改 */
	void doSqlFileNameEditingFinished();
	void doSqlFileCurrentChanged();

	/* 创建DB路径修改 */
	void doCreateDBFileNameEditingFinished();
	void doCreateDBCurrentChanged();

	/* 另存为 */
	void doActSaveAs();

	/* 清空文件列表 */
	void doClearFileNameList(QString);

	/*sql文件改变槽*/
	void doAfterSqlFileChange(const QString& path);

protected:
	void closeEvent(QCloseEvent *event); // 关闭事件

	bool eventFilter(QObject *obj, QEvent * evt);

private:
	/* 初始化界面 */
	void setupUi();

	/* 创建文件下拉列表 */
	void createFileDropList(const QStringList& slFileList, QComboBox* cb);

	/* 添加文件名到下拉列表 */
	void addFileName(
		QComboBox* cb,
		QStringList& slFileName,
		const QString& sFileName,
		const QString& sCIParamName);

	/*所有的信号槽链接*/
	void setupSignalSlots();

	/* 将文件内容加载到文本编辑框 */
	bool loadTextEditor(const QString& sFilePath);

	/* 生成DB文件 */
	bool createDatabase(const QString& sDBFileName, const QString& sSqlFileName);

	/* 获取文件夹路径 */
	QString getFileDirPath(const QString& sFilePath);

	/* 获取文件名 */
	QString getFileName(const QString& sFilePath);

	/* 删除文件名 */
	void clearFileNameList(
		const QString& sClearFileListName,
		QComboBox* cb,
		QStringList& slFileName,
		const QString& sCIParamName);

	/*是否需要保存*/
	bool maybeSave();

private:
	QSettings *						m_pConfigIni;				// ini文件，用来保存DB路径
	QLineEdit*						m_pedPw;					//密码输入框

	QMenu*							m_menu;
	QAction*						m_actSave;
	QString							m_sCreateDBFileName;		// db文件名，包含全路径
	QString							m_sSqlFileName;				// sql文件路径
	QStringList						m_slCreateDBFileName;		// 最近使用的db文件名(包含全路径)
	QStringList						m_slSqlFileName;			// 最近使用的sql文件名(包含全路径)

	int								m_nTableDataRowHeight = 0;	// 表格行高
	QButtonGroup					m_bgTableNameBtnFroup;		// 表格名按钮组

	QTextDocument*					m_pTextDocument = nullptr;	// 文本编辑框文本

	Ui::wCreateDB ui;

	QFileSystemWatcher				m_sqlFileWatcher;              //sql文件监视器
};

#endif // WIDGETCREATEDB_H
