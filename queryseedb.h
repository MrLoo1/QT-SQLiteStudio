#ifndef QUERYSEEDB_H
#define QUERYSEEDB_H

#include <QDialog>
#include "ui_queryseedb.h"

class QuerySeeDB : public QDialog
{
	Q_OBJECT

public:
	QuerySeeDB(const QStringList& fieldnames, QString* qrytext, QString* fieldname, QWidget *parent = 0);
	~QuerySeeDB();

private:
	void setupUI();

	void setupSignalSlots();

private slots:
	/*查询combox槽*/
	void doAfterQryChange();
	void doAfterQryEdited();

	/*查询按钮槽*/
	void doBtnQryClicked();

	/*列名combox槽*/
	void doAfterFieldNameChange();

private:
	Ui::QuerySeeDB ui;

	QString* m_sQryText = nullptr;
	const QStringList m_vFieldnames;
	QString* m_sFieldname = nullptr;
};

#endif // QUERYSEEDB_H
