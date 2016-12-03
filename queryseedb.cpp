#include "queryseedb.h"
#include "msgdlg.h"
#include "common.h"
#include <QComboBox>
QuerySeeDB::QuerySeeDB(const QStringList& fieldnames, QString* qrytext, QString* fieldname, QWidget *parent)
	: QDialog(parent), m_vFieldnames(fieldnames), m_sQryText(qrytext), m_sFieldname(fieldname)
{
	ui.setupUi(this);
	setupUI();
	setupSignalSlots();
}

QuerySeeDB::~QuerySeeDB()
{
	if (m_sQryText)
	{
		delete m_sQryText;
	}
	if (m_sFieldname)
	{
		delete m_sFieldname;
	}
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
	
	connect(ui.edQry, SIGNAL(currentIndexChanged(int)), this, SLOT(doAfterQryChange()));
	connect(ui.edQry, SIGNAL(editingFinished()), this, SLOT(doAfterQryEdited()));

	connect(ui.cbFieldName, SIGNAL(currentIndexChanged(int)), this, SLOT(doAfterFieldNameChange()));
}

/*����combox��*/
void QuerySeeDB::doAfterFieldNameChange()
{
	ui.lbQryTip->setText(KSL("������Ҫ���ҵ�[%1]������:").arg(ui.cbFieldName->currentText()));
}

/*��ѯcombox��*/
void QuerySeeDB::doAfterQryChange()
{
	*m_sQryText = ui.edQry->text();
	*m_sFieldname = ui.cbFieldName->currentText();
}

void QuerySeeDB::doAfterQryEdited()
{
	doAfterQryChange();
}

/*��ѯ��ť��*/
void QuerySeeDB::doBtnQryClicked()
{
	if (m_sFieldname->isEmpty())
	{
		MsgDlg::error(KSL("��ѡ������"), KSL("��ѯ����"));
		return;
	}
	accept();
}