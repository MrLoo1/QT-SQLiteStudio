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

	/*事件过滤器*/
	bool eventFilter(QObject *, QEvent *);

private slots:
	/* 查看DB路径修改 */
	void doSeeDBFileNameEditingFinished();
	void doSeeDBCurrentChanged();

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
	void doBtnCloseClicked();

	/*保存数据库修改*/
	void doCommitDBSlot();

	/*取出数据库中所有的表*/
	void doLwDBTableChanged(int row);

	/* 清空文件列表 */
	void doClearFileNameList();

	/*删除seedb标签*/
	void doInternalButtonClicked(const QString sButtonId);

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

	/*显示表中所有的数据*/
	void showTableData(const QString& sTableName);

	/* 取sql表格模型 */
	QSqlTableModel* getSqlTableModel(const QString&);

	/* 清楚表格数据table页 */
	bool clearTableData();

	/* 删除一个插页 */
	bool deleteTableData(QTableView*, KTabButton*);

	/*设置seedb页按钮*/
	void setSeeDBBtn(bool enable);

private:
	QSettings *						m_pConfigIni;				// ini文件，用来保存DB路径
	QLineEdit*						m_pedPw;					//密码输入框

	int								m_nTableDataRowHeight = 0;	// 表格行高
	QButtonGroup					m_bgTableNameBtnFroup;		// 表格名按钮组

	QString							m_sSeeDBFileName;			// db文件名，包含全路径
	QStringList						m_slSeeDBFileName;			// 最近使用的db文件名(包含全路径)
	QSqlDatabase					m_sdbSeeDB;					// 数据库连接
	QString							m_sCurSelectTableName;		// 当前选择的表名
	QHash<QString, KTabButton*>		m_hashTableNameBtn;			// 表名对应的按钮

	int								key1;
	int								key2;

	Ui::wSeeDB ui;		
};

#endif // WIDGETSEEDB_H
