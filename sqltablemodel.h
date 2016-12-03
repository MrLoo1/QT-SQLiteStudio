#ifndef QSLTABLEMODEL_H
#define QSLTABLEMODEL_H

#include <QSqlTableModel>

class SqlTableModel : public QSqlQueryModel
{
	Q_OBJECT
public:
	SqlTableModel(QObject * parent = 0, QSqlDatabase db = QSqlDatabase());

public:
	/*重写父类的方法，变更显示，这里是添加勾选框*/
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	/*重写父类的方法，添加可勾选*/
	Qt::ItemFlags flags(const QModelIndex &index) const;

	/*重写父类的方法，设置数据，主要就是勾选行记录*/
	bool setData(const QModelIndex &index, const QVariant &value, int role);

	/*设置和获取那些行勾选了*/
	QMap<int, Qt::CheckState> getChecked();
	void setChecked(QMap<int, Qt::CheckState> map);

	/*设置和获取表名*/
	QString tableName();
	void setTable(const QString& strTableName);

	/*设置和获取排序条件*/
	QString orderBy();
	void setOrderBy(const QString& strOrderBy);

private:
	QMap<int, Qt::CheckState> m_rowCheckStateMap;   //记录那些行勾选了
	QString m_strTableName;    //表名
	QString m_strOrderBy;    //排序条件
};

#endif
