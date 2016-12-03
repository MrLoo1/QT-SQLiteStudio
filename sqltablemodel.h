#ifndef QSLTABLEMODEL_H
#define QSLTABLEMODEL_H

#include <QSqlTableModel>

class SqlTableModel : public QSqlQueryModel
{
	Q_OBJECT
public:
	SqlTableModel(QObject * parent = 0, QSqlDatabase db = QSqlDatabase());

public:
	/*��д����ķ����������ʾ����������ӹ�ѡ��*/
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	/*��д����ķ�������ӿɹ�ѡ*/
	Qt::ItemFlags flags(const QModelIndex &index) const;

	/*��д����ķ������������ݣ���Ҫ���ǹ�ѡ�м�¼*/
	bool setData(const QModelIndex &index, const QVariant &value, int role);

	/*���úͻ�ȡ��Щ�й�ѡ��*/
	QMap<int, Qt::CheckState> getChecked();
	void setChecked(QMap<int, Qt::CheckState> map);

	/*���úͻ�ȡ����*/
	QString tableName();
	void setTable(const QString& strTableName);

	/*���úͻ�ȡ��������*/
	QString orderBy();
	void setOrderBy(const QString& strOrderBy);

private:
	QMap<int, Qt::CheckState> m_rowCheckStateMap;   //��¼��Щ�й�ѡ��
	QString m_strTableName;    //����
	QString m_strOrderBy;    //��������
};

#endif
