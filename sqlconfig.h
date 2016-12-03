#ifndef SQLCONFIG_H
#define SQLCONFIG_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

class SQLConfig : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(SQLConfig)
public:
	SQLConfig(const QString& sDbPath = "", QObject* parent = nullptr);
	~SQLConfig();

	/* 获取查询的记录数 */
	static int getRecordCount(QSqlQuery* query);

public:
	/*
		设置数据库库文件路径,相对与当前工作目录,如:
		设置为data,则数据库文件全路径为:当前目录/data/DBfile
	*/
    virtual void setDbPath(const QString& sDBPath);
	QString dbPath() const;

	/*
		打开数据库
	*/
    virtual bool openDatabase(const QString& sDbName);
	virtual bool openDatabaseEx(const QString& sDbFile);

	/*
		关闭数据库
	*/
    virtual void closeDataset();

	/* 开始事务 */
	virtual bool beginTransaction();

	/* 提交事务 */
	virtual bool commitTransaction();

	/* 回滚事务 */
	virtual bool rollbackTransaction();

	/*
		打开指定查询(select),如果有数据,则记录指针指向第一行记录,
		通过recordCount()获取查询到的记录数
	@param
		sTableName	表名称
		sCondition	查询条件,不带where关键字
	@return
		执行成功返回true,否则返回false
	*/
	bool openQuery(const QString& sTableName, const QString& sCondition = QString(""));

	/*
		执行指定的查询语句:select,如果有数据,则记录指针指向第一行记录,
		通过recordCount()获取查询到的记录数
		注意:不能执行非查询语句
	@param
		sSql	标准的SQL查询语句
	@return
		执行成功返回true,否则返回false
	*/
	bool execQuery(const QString& sSql);

	/*
		执行非查询SQL,如:insert/update/delete
	@param
		sSQL	待执行SQL
	@return
		执行失败返回-1,成功返回影响的行数
	*/
	int execSQL(const QString& sSQL);
	
	/*
		返回当前查询的总记录数		
	*/
	int recordCount() const;

	/*
		向下移动当前查询一行记录
	@return
		移动记录失败返回false,否则返回true
	*/
	bool next();

	/*
		向上移动当前查询一行记录
	@return
		移动记录失败返回false,否则返回true
	*/
	bool previous();

	/*
		移动到记录集第一行记录
	@return
		移动记录失败返回false,否则返回true
	*/
	bool first();

	/*
		移动到记录集最后一行记录
	@return
		移动记录失败返回false,否则返回true
	*/
	bool last();

	/*
		返回当前记录所在行索引,从0开始
	@return
		当前记录所在行索引,如果当前记录无效则返回QSql::BeforeFirstRow或QSql::AfterLastRow
	*/
	int at();

	/*
		取当前查询中当前行指定列数据
	@param
		sFieldName	表名称
	*/
	QVariant getValue(const QString& sFieldName);
	QVariant getValue(const int nCol);

	/*
		返回当前表名称
	*/
	const QString& databaseName() const;

	/*
		返回当前表名称
	*/
	const QString& tableName() const;
	void setTableName(const QString& sTableName);

	/*
		检查表是否存在
	*/
	bool checkTableExist(const QString& sTableName, const bool bThrowExecpt = false);

	/*
		返回数据库连接
	*/
	QSqlDatabase& database()
	{
		return m_oDb;
	}

	/* 返回执行后的错误信息 */
	QString lastError();

private:
	bool              m_bDbOpened;		// 当前是否打开数据库
	uint              m_nRecordCount;	// 当前查询的记录数
	QString           m_sDbPath;		// 当前操作的数据库相对应当前目录的路径
	QString           m_sDbName;		// 当前操作的数据库名称
	QString           m_sTableName;	    // 当前操作的表名称
	QSqlDatabase      m_oDb;		    // 数据库连接
	QSqlQuery*        m_pQuery;	        // 当前查询,用于执行Select语句
	QSqlQuery*        m_pExec;		    // 用于执行insert/delete/update
	QString           m_sLastErrorMsg;  // 最后错误信息
};

#endif // SQLCONFIG_H
