#include "sqlTableModel.h"
#include <QSqlQuery>
#include <QSqlRecord>

SqlTableModel::SqlTableModel(QObject * parent, QSqlDatabase db) :QSqlQueryModel(parent)
{
	m_rowCheckStateMap.clear();
}

QMap<int, Qt::CheckState> SqlTableModel::getChecked()
{
	return m_rowCheckStateMap;
}

void SqlTableModel::setChecked(QMap<int, Qt::CheckState> map)
{
	m_rowCheckStateMap = map;
}

QString SqlTableModel::tableName()
{
	return m_strTableName;
}

void SqlTableModel::setTable(const QString& strTableName)
{
	m_strTableName = strTableName;
}

QString SqlTableModel::orderBy()
{
	return m_strOrderBy;
}

void SqlTableModel::setOrderBy(const QString& strOrderBy)
{
	m_strOrderBy = strOrderBy;
}

bool SqlTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;

	if (role == Qt::CheckStateRole && index.column() == 0)
	{
		//m_rowCheckStateMap[index.row()] = (value == Qt::Checked ? Qt::Checked : Qt::Unchecked);
		int iData = QSqlQueryModel::data(index, Qt::DisplayRole).toInt();
		m_rowCheckStateMap[iData] = (value == Qt::Checked ? Qt::Checked : Qt::Unchecked);
	}

	if (role == Qt::EditRole)
	{
		QSqlQueryModel::setData(index, value, role);
	}

	return true;
}

QVariant SqlTableModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	switch (role)
	{
	case Qt::DisplayRole:
		return QSqlQueryModel::data(index, role);

	case Qt::CheckStateRole:
		if (index.column() == 0)
		{
			/*if (m_rowCheckStateMap.contains(index.row()))
				return m_rowCheckStateMap[index.row()] == Qt::Checked ? Qt::Checked : Qt::Unchecked;*/
			int iData = QSqlQueryModel::data(index, Qt::DisplayRole).toInt();
			if (m_rowCheckStateMap.contains(iData))
			return m_rowCheckStateMap[iData] == Qt::Checked ? Qt::Checked : Qt::Unchecked;

			return Qt::Unchecked;
		}
	default:
		return QVariant();
	}

	return QVariant();
}

Qt::ItemFlags SqlTableModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	Qt::ItemFlags theFlags = Qt::NoItemFlags;

	if (index.column() == 0)
		theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
	else
		theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	return theFlags;
}
