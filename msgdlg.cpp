#include "msgdlg.h"
#include <QLayout>
#include <QTimer>
#include <QScrollBar>
#include <QApplication>
#include <QDesktopWidget>

/*
    显示提示信息提示框
@param
    sMsg			显示的消息
	sTitle			提示标题
	eWinModality	窗口模式形态
	nShowTime		对话框显示时间,超时自动关闭
@return
    单击确定按钮时返回true,否则返回false
*/
bool MsgDlg::info(const QString& sMsg, const QString& sTitle, 
	Qt::WindowModality eWinModality, int nShowTime)
{
	QWidget* pParentForm = qApp->activeWindow();
	MsgDlg* dlg = new MsgDlg(pParentForm, mtInfo, eWinModality);
	dlg->setDialog(sTitle, sMsg, nullptr, mbtOkCancel, mdbOkYes, nShowTime, false);
	if (eWinModality != Qt::NonModal)
	{
		return dlg->exec();
	}
	else
	{
		dlg->show();
		return true;
	}
}

/*
    显示提示信息Ok提示框
@param
    sMsg			显示的消息
	sTitle			提示标题
	eWinModality	窗口模式形态
	nShowTime		对话框显示时间,超时自动关闭
@return
    单击确定按钮时返回true,否则返回false
*/
bool MsgDlg::finish(const QString& sMsg, const QString& sTitle, 
	Qt::WindowModality eWinModality, int nShowTime)
{
	QWidget* pParentForm = qApp->activeWindow();
	MsgDlg* dlg = new MsgDlg(pParentForm, mtFinished, eWinModality);
	dlg->setDialog(sTitle, sMsg, nullptr, mbtOkCancel, mdbOkYes, nShowTime, false);
	if (eWinModality != Qt::NonModal)
	{
		return dlg->exec();
	}
	else
	{
		dlg->show();
		return true;
	}
}

/*
    显示警告提示框
@param
    sMsg			显示的消息
	sTitle			提示标题
	eWinModality	窗口模式形态
	nShowTime		对话框显示时间,超时自动关闭
@return
    单击确定按钮时返回true,否则返回false
*/
bool MsgDlg::warning(const QString& sMsg, const QString& sTitle, 
	Qt::WindowModality eWinModality, int nShowTime)
{
	QWidget* pParentForm = qApp->activeWindow();
	MsgDlg* dlg = new MsgDlg(pParentForm, mtWarning, eWinModality);
	dlg->setDialog(sTitle, sMsg, nullptr, mbtOkCancel, mdbOkYes, nShowTime, false);
	if (eWinModality != Qt::NonModal)
	{
		return dlg->exec();
	}
	else
	{
		dlg->show();
		return true;
	}
}

/*
    显示错误信息提示框
@param
    sMsg			显示的消息
	sTitle			提示标题
	eWinModality	窗口模式形态
	nShowTime		对话框显示时间,超时自动关闭
@return
    单击确定按钮时返回true,否则返回false
*/
bool MsgDlg::error(const QString& sMsg, const QString& sTitle, 
	Qt::WindowModality eWinModality, int nShowTime)
{
	QWidget* pParentForm = qApp->activeWindow();
	MsgDlg* dlg = new MsgDlg(pParentForm, mtError, eWinModality);
	dlg->setDialog(sTitle, sMsg, nullptr, mbtOkCancel, mdbOkYes, nShowTime, false);
	if (eWinModality != Qt::NonModal)
	{
		return dlg->exec();
	}
	else
	{
		dlg->show();
		return true;
	}
}

/*
    显示询问信息提示框
@param
    sMsg			显示的消息
	sTitle			提示标题
	eBtnText		按钮文字
	eDefaultBtn		默认获得焦点的按钮
@return
    单击确定按钮时返回true,否则返回false
*/
bool MsgDlg::confirm(const QString& sMsg, const QString& sTitle, 
	ButtonText eBtnText, DefaultButton eDefaultBtn)
{
	QWidget* pParentForm = qApp->activeWindow();
	MsgDlg* dlg = new MsgDlg(pParentForm, mtConfirm, Qt::ApplicationModal);
	dlg->setDialog(sTitle, sMsg, nullptr, eBtnText, eDefaultBtn, 0, false);
	return dlg->exec();
}

/*
	显示提示信息提示框
@param
	parent			父窗口
	eMsgType		消息类型
	sMsg            显示的消息
	sTitle			提示标题
	eWinModality	窗口模式形态
	nShowTime		对话框显示时间,超时自动关闭
	eBtnText		按钮文字
	eDefaultBtn		默认获得焦点的按钮
	sDetail         显示详细信息
	bDetailIsOpen   true为打开详细,false为隐藏
@return
	单击确定按钮时返回true,否则返回false
*/
bool MsgDlg::msgBox(QWidget* parent, MessageType eMsgType,
	const QString& sMsg, const QString& sTitle,  
	Qt::WindowModality eWinModality, int nShowTime, ButtonText eBtnText,
	DefaultButton eDefaultBtn, const QString& sDetail, bool bDetailIsOpen)
{
	MsgDlg* dlg = new MsgDlg(parent, eMsgType, eWinModality, !sDetail.isEmpty());
	dlg->setDialog(sTitle, sMsg, sDetail, eBtnText, eDefaultBtn, nShowTime, bDetailIsOpen);
	if (eWinModality != Qt::NonModal)
		return dlg->exec();
	else
	{
		dlg->show();
		return true;
	}
}

#define CWin_Min_Height		40
#define CWin_Min_Width		300
#define CWin_Icon_Size		40
#define CWin_Border_Spacing	9

/* 
	构造一个对话框 
*/
MsgDlg::MsgDlg(QWidget * parent, MessageType eMsgType, 
	Qt::WindowModality eWinModality, bool bShowDetail)
{
	// 设置为顶层窗口
	if (nullptr == parent)
	{
		setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	}
	setWindowIcon(QIcon(RS_ICON_WINDOW));
	initControl(bShowDetail, mtConfirm == eMsgType);
	setTitleIcon(eMsgType);
	setWindowModality(eWinModality);
	setAttribute(Qt::WA_DeleteOnClose);
}

MsgDlg::~MsgDlg()
{
	if (m_nShowTimer > 0)
	{
		killTimer(m_nShowTimer);
	}
}

/* 初始化控件 */
void MsgDlg::initControl(bool bShowDetail, bool bIsConfirm)
{
	// 主布局器
	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainLayout->setSpacing(0);

	// 主布局器--信息面板
	m_msgPanel = new QFrame(this);
	m_msgPanel->setObjectName(KSL("msg-panel"));
	m_msgPanel->setFrameStyle(QFrame::NoFrame);
	m_msgPanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	pMainLayout->addWidget(m_msgPanel);
	// 主布局器--信息面板--水平布局器
	QHBoxLayout* pHLayout = new QHBoxLayout(m_msgPanel);
	pHLayout->setContentsMargins(CWin_Border_Spacing, CWin_Border_Spacing, 
								 CWin_Border_Spacing, CWin_Border_Spacing);
	pHLayout->setSpacing(0);
	// 主布局器--信息面板--水平布局器--图标组件
	m_imgLabel = new QLabel(m_msgPanel);
	m_imgLabel->setObjectName(KSL("msg-img"));
	m_imgLabel->setMargin(0);
	m_imgLabel->setFixedWidth(CWin_Icon_Size);
	m_imgLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	pHLayout->addWidget(m_imgLabel);
	// 主布局器--信息面板--水平布局器--信息滚动区
	m_msgScroll = new QScrollArea(m_msgPanel);
	m_msgScroll->setObjectName(KSL("msg-scroll"));
	m_msgScroll->setFocusPolicy(Qt::NoFocus);
	m_msgScroll->setFrameStyle(QFrame::NoFrame);
	m_msgScroll->setContentsMargins(0, 0, 0, 0);
	m_msgScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_msgScroll->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	// 主布局器--信息面板--水平布局器--信息滚动区--信息标签
	m_msgLabel = new QLabel(m_msgScroll);
	m_msgLabel->setObjectName(KSL("msg-label"));
	m_msgLabel->setAlignment(Qt::AlignTop);
	m_msgLabel->setContentsMargins(6, 6, 6, 6);
	m_msgScroll->setWidget(m_msgLabel);
	pHLayout->addWidget(m_msgScroll);
	
	// 主布局器--按钮栏
	m_buttonBar = new QFrame(this);
	m_buttonBar->setObjectName(KSL("button-bar"));
	m_buttonBar->setFrameStyle(QFrame::NoFrame);
	m_buttonBar->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum));
	pMainLayout->addWidget(m_buttonBar);
	// 主布局器--按钮栏--水平布局器
	pHLayout = new QHBoxLayout(m_buttonBar);
	pHLayout->setContentsMargins(8, 8, 8, 8);
	pHLayout->setSpacing(8);
	
	// 主布局器--按钮栏--水平布局器--明细按钮
	if (bShowDetail)
	{
		m_btnDetail = new QPushButton(KSL("查看明细"), m_buttonBar);
		m_btnDetail->setObjectName(KSL("msg-detail-btn"));
		m_btnDetail->setFocusPolicy(Qt::NoFocus);
		m_btnDetail->setFixedWidth(75);
		m_btnDetail->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));
		pHLayout->addWidget(m_btnDetail);
		connect(m_btnDetail, SIGNAL(clicked()), this, SLOT(doShowDetailButtonClick()));
	}
	// 主布局器--按钮栏--水平布局器--弹簧
	QSpacerItem* pHLeftSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	pHLayout->addItem(pHLeftSpacer);
	// 主布局器--按钮栏--水平布局器--确定按钮
	m_btnOk = new QPushButton(KSL("确定"), m_buttonBar);
	m_btnOk->setObjectName(KSL("msg-ok-btn")); 
	m_btnOk->setFixedWidth(75);
	m_btnOk->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));
	m_btnOk->setDefault(true);
	pHLayout->addWidget(m_btnOk);
	connect(m_btnOk, SIGNAL(clicked()), this, SLOT(doOkButtonClick()));
	// 主布局器--按钮栏--水平布局器--取消按钮
	if (bIsConfirm)
	{
		m_btnCancel = new QPushButton(KSL("取消"), m_buttonBar);
		m_btnCancel->setObjectName(KSL("msg-cancel-btn"));
		m_btnCancel->setFixedWidth(75);
		m_btnCancel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));
		pHLayout->addWidget(m_btnCancel);
		connect(m_btnCancel, SIGNAL(clicked()), this, SLOT(doCancelButtonClick()));
	}
	if (!bShowDetail)
	{
		// 主布局器--按钮栏--水平布局器--弹簧
		pHLeftSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		pHLayout->addItem(pHLeftSpacer);
	}
	else
	{
		// 主布局器--明细
		m_detailInfo = new QTextEdit(this);
		m_detailInfo->setObjectName(KSL("msg-detail"));
		m_detailInfo->setFocusPolicy(Qt::NoFocus);
		m_detailInfo->setFixedHeight(100);
		m_detailInfo->setLineWrapMode(QTextEdit::NoWrap);
		m_detailInfo->setReadOnly(true);
		m_detailInfo->hide();
		pMainLayout->addWidget(m_detailInfo);
	}
}

/* 设置图标 */
void MsgDlg::setTitleIcon(MessageType eMsgType)
{
	m_imgLabel->setAttribute(Qt::WA_TranslucentBackground);
	switch (eMsgType)
	{
	case mtInfo:
		m_imgLabel->setPixmap(QPixmap(RS_Dlg_Info));
		setWindowTitle(KSL("信息"));
		break;
	case mtFinished:
		m_imgLabel->setPixmap(QPixmap(RS_Dlg_Finish));
		setWindowTitle(KSL("提示"));
		break;
	case mtWarning:
		m_imgLabel->setPixmap(QPixmap(RS_Dlg_Warning));
		setWindowTitle(KSL("警告"));
		break;
	case mtError:
		m_imgLabel->setPixmap(QPixmap(RS_Dlg_Error));
		setWindowTitle(KSL("错误"));
		break;
	case mtConfirm:
		m_imgLabel->setPixmap(QPixmap(RS_Dlg_Confirm));
		setWindowTitle(KSL("问询"));
		break;
	default:
		break;
	}
}

/* 设置对话框 */
void MsgDlg::setDialog(const QString &sTitle, const QString &sMsg, const QString &sDetail,
	ButtonText eBtnText, DefaultButton eDefaultBtn, int nShowTime, bool bDetailIsOpen)
{
	// 设置标题
	if (!sTitle.isEmpty())
	{
		setWindowTitle(sTitle);
	}

	// 设置显示信息
	setMsgInfo(sMsg);

	// 更新按钮文字
	if (mbtYesNo == eBtnText)
	{
		m_btnOk->setText(KSL("是"));
		if (m_btnCancel)
			m_btnCancel->setText(KSL("否"));
	}
	else if (nShowTime > 0)
	{
		m_btnOk->setText(KSL("确定(%1)").arg(nShowTime / 1000));
	}

	// 设置默认按钮
	if (mdbCancelNo == eDefaultBtn)
	{
		m_btnCancel->setDefault(true);
	}
	
	// 设置明细信息
	if (!sDetail.isNull() && !sDetail.isEmpty())
	{
		m_detailInfo->setHtml(sDetail);
		if (bDetailIsOpen)
		{
			doShowDetailButtonClick();
		}
	}

	// 设置自动关闭
	if (nShowTime > 0)
	{
		m_nShowTimes = nShowTime / 1000;
		m_nShowTimer = startTimer(1000);
	}
}

/* 设置显示信息 */
void MsgDlg::setMsgInfo(const QString & sMsg)
{
	// 设置显示信息
	m_msgLabel->setText(sMsg);
	m_msgLabel->adjustSize();
	
	int nMaxMW = QApplication::desktop()->width() / 3 * 2 - m_imgLabel->width() - CWin_Border_Spacing * 2;
	int nMaxMH = QApplication::desktop()->height() / 3 * 2 - m_buttonBar->height() - 20;
	
	int nW = m_msgLabel->width();
	int nH = m_msgLabel->height();

	if (nW > nMaxMW)
	{
		nW = nMaxMW;
		m_msgLabel->setWordWrap(true);
		m_msgLabel->setFixedWidth(nW);
		m_msgLabel->adjustSize();
		nH = m_msgLabel->height();
	}
	else if (nW < CWin_Min_Width)
	{
		nW = CWin_Min_Width;
	}
	if (nH > nMaxMH)
	{
		nH = nMaxMH;
	}
	else if (nH < CWin_Min_Height)
	{
		nH = CWin_Min_Height;
	}

	m_msgScroll->setFixedSize(QSize(nW, nH));
	adjustSize();
}

/* 计时器 */
void MsgDlg::timerEvent(QTimerEvent * evt)
{
	if (evt->timerId() == m_nShowTimer)
    {
		--m_nShowTimes;
		if (m_nShowTimes <= 0)
		{
			doOkButtonClick();
		}
		else
		{
			m_btnOk->setText(KSL("确定(%1)").arg(m_nShowTimes));
		}
    }
}

/* 确定按钮曹 */
void MsgDlg::doOkButtonClick()
{
	done(QDialog::Accepted);
	close();
}

/* 取消按钮曹 */
void MsgDlg::doCancelButtonClick()
{
	done(QDialog::Rejected);
	close();
}

/* 详细按钮曹 */
void MsgDlg::doShowDetailButtonClick()
{
    if (m_detailInfo->isHidden())
    {
        m_detailInfo->show();
    }
    else
    {
        m_detailInfo->hide();
    }
	adjustSize();
}
