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

	/*添加新的子版本*/
	void addDB(QList<QMap<int, Qt::CheckState> > list, const QString& strName = QString());

	/*修改子版本*/
	bool updateDB(QList<QMap<int, Qt::CheckState> > list, const QString& strName = QString());

	/*事件过滤器*/
	bool eventFilter(QObject *, QEvent *);

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

	/* 查看DB路径修改 */
	void doSeeDBFileNameEditingFinished();
	void doSeeDBCurrentChanged();

	/* 另存为 */
	void doActSaveOther();

	/*查看DB插页按钮槽*/
	void doBtnSelectSeeDBClicked();
	void doBtnRefreshSeeDBClicked();
	void doBtnInsertClicked();
	void doBtnDeleteClicked();
	void doBtnCommitClicked();
	void doBtnRevertClicked();
	void doSwitchShowTable();
	void doActDropTable();
	void doActQryTable();

	/*保存数据库修改*/
	void doCommitDBSlot();

	/*取出数据库中所有的表*/
	void doLwDBTableChanged(int row);

	/* 清空文件列表 */
	void doClearFileNameList();

	/*sql文件改变槽*/
	void doAfterSqlFileChange(const QString& path);

	/*显示文本被修改*/
	void documentWasModified();

	/*删除seedb标签*/
	void doInternalButtonClicked(const QString sButtonId);

protected:
	void closeEvent(QCloseEvent *event); // 关闭事件

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

	/* 加载数据库数据表 */
	void loadSeeDBTable();

	/*所有的信号槽链接*/
	void setupSignalSlots();

	/*选择DB文件*/
	void selectedSeeDB();

	/*设置按钮是否可用*/
	void controlEnable(bool bEnable);

	/*创建ini文件*/
	void createIni();

	/*显示表中所有的数据*/
	void showTableData(const QString& sTableName);

	/* 将文件内容加载到文本编辑框 */
	bool loadTextEditor(const QString& sFilePath);

	/* 生成DB文件 */
	bool createDatabase(const QString& sDBFileName, const QString& sSqlFileName);

	/* 获取文件夹路径 */
	QString getFileDirPath(const QString& sFilePath);

	/* 获取文件名 */
	QString getFileName(const QString& sFilePath);

	/* 取sql表格模型 */
	QSqlTableModel* getSqlTableModel(const QString&);

	/* 清楚表格数据table页 */
	bool clearTableData();

	/* 删除一个插页 */
	bool deleteTableData(QTableView*, TabButton*);

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

	QMenu*							m_menu;
	QAction*						m_actSave;
	QString							m_sCreateDBFileName;		// db文件名，包含全路径
	QString							m_sSqlFileName;				// sql文件路径
	QStringList						m_slCreateDBFileName;		// 最近使用的db文件名(包含全路径)
	QStringList						m_slSqlFileName;			// 最近使用的sql文件名(包含全路径)
	
	int								m_nTableDataRowHeight = 0;	// 表格行高
	QButtonGroup					m_bgTableNameBtnFroup;		// 表格名按钮组

	QString							m_sSeeDBFileName;			// db文件名，包含全路径
	QStringList						m_slSeeDBFileName;			// 最近使用的db文件名(包含全路径)
	QSqlDatabase					m_sdbSeeDB;					// 数据库连接
	QString							m_sCurSelectTableName;		// 当前选择的表名
	QHash<QString, TabButton*>		m_hashTableNameBtn;			// 表名对应的按钮

	QTextDocument*					m_pTextDocument = nullptr;	// 文本编辑框文本
	QHash<QString, QHash<int, Qt::SortOrder>> m_hashSortIndexList;		// 排序索引

	Ui::SQLiteStudio ui;

	QFileSystemWatcher				m_sqlFileWatcher;              //sql文件监视器

	int								key1;				
	int								key2;
};

#endif // SQLITESTUDIO_H
