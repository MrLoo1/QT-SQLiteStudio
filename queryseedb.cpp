#include "queryseedb.h"
#include "common.h"
#include <QComboBox>
#include <QMessageBox>
QuerySeeDB::QuerySeeDB(const QStringList& fieldnames, QString* qrytext, QString* fieldname, QWidget *parent)
	: QDialog(parent), m_vFieldnames(fieldnames), m_sQryText(qrytext), m_sFieldname(fieldname)
{
	ui.setupUi(this);
	setupUI();
	setupSignalSlots();
}

QuerySeeDB::~QuerySeeDB()
{

}

void QuerySeeDB::setupUI()
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
	ui.cbFieldName->addItems(m_vFieldnames);
	adjustSize();
}

void QuerySeeDB::setupSignalSlots()
{
	connect(ui.btnQry, SIGNAL(clicked()), this, SLOT(doBtnQryClicked()));
	
	connect(ui.edQry, SIGNAL(editingFinished()), this, SLOT(doAfterQryEdited()));

	connect(ui.cbFieldName, SIGNAL(currentIndexChanged(int)), this, SLOT(doAfterFieldNameChange()));
}

/*����combox��*/
void QuerySeeDB::doAfterFieldNameChange()
{
	ui.lbQryTip->setText(QSL("������Ҫ���ҵ�[%1]������:").arg(ui.cbFieldName->currentText()));
}

void QuerySeeDB::doAfterQryEdited()
{
	*m_sQryText = ui.edQry->text();
	*m_sFieldname = ui.cbFieldName->currentText();
}

/*��ѯ��ť��*/
void QuerySeeDB::doBtnQryClicked()
{
	if (m_sFieldname->isEmpty())
	{
		QMessageBox::critical(this, QSL("��ѯ����"), QSL("��ѡ������"));
		return;
	}
	accept();
}