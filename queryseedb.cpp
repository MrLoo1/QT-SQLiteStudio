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

/*列名combox槽*/
void QuerySeeDB::doAfterFieldNameChange()
{
	ui.lbQryTip->setText(QSL("请输入要查找的[%1]列内容:").arg(ui.cbFieldName->currentText()));
}

/*查询combox槽*/
void QuerySeeDB::doAfterQryChange()
{
	*m_sQryText = ui.edQry->text();
	*m_sFieldname = ui.cbFieldName->currentText();
}

void QuerySeeDB::doAfterQryEdited()
{
	doAfterQryChange();
}

/*查询按钮槽*/
void QuerySeeDB::doBtnQryClicked()
{
	if (m_sFieldname->isEmpty())
	{
		QMessageBox::critical(this, QSL("查询错误"), QSL("请选择列名"));
		return;
	}
	accept();
}