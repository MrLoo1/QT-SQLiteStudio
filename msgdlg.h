#ifndef MSGDLG_H
#define MSGDLG_H

#include "common.h"
#include <QLabel>
#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollArea>

/* 对话框默认自动关闭时间, 单位:毫秒 */
#define  CDEFAULT_SHOW_TIMES  5000

class MsgDlg : public QDialog
{
    Q_OBJECT
public:
    /* 枚举变量:定义消息对话框的类型 */
    enum MessageType{
        mtInfo,			// 信息
        mtFinished,		// 完成
        mtWarning,		// 警告
        mtError,		// 错误
        mtConfirm		// 询问
    };

    /* 按钮类型 */
    enum ButtonText
    {
        mbtOkCancel,	// 确定/取消
        mbtYesNo		// 是/否
    };

	/* 默认按钮 */
	enum DefaultButton
	{
		mdbOkYes,		// 确定/是
		mdbCancelNo		// 取消/否
	};

public:
    /*
        显示提示信息提示框
    @param
        sMsg			显示的消息
		sTitle			提示标题
		eWinModality	窗口模式形态
		sShowTime		对话框显示时间,超时自动关闭
    @return
        单击确定按钮时返回true,否则返回false
    */
	static bool info(const QString& sMsg, const QString& sTitle = nullptr, 
		Qt::WindowModality eWinModality = Qt::WindowModal, int nShowTime = 0);

    /*
        显示提示信息Ok提示框
    @param
        sMsg			显示的消息
		sTitle			提示标题
		eWinModality	窗口模式形态
		sShowTime		对话框显示时间,超时自动关闭
    @return
        单击确定按钮时返回true,否则返回false
    */
	static bool finish(const QString& sMsg, const QString& sTitle = nullptr, 
		Qt::WindowModality eWinModality = Qt::WindowModal, int nShowTime = 0);

    /*
        显示警告提示框
    @param
        sMsg			显示的消息
		sTitle			提示标题
		eWinModality	窗口模式形态
		sShowTime		对话框显示时间,超时自动关闭
    @return
        单击确定按钮时返回true,否则返回false
    */
	static bool warning(const QString& sMsg, const QString& sTitle = nullptr, 
		Qt::WindowModality eWinModality = Qt::WindowModal, int nShowTime = 0);

    /*
        显示错误信息提示框
    @param
        sMsg			显示的消息
		sTitle			提示标题
		eWinModality	窗口模式形态
		sShowTime		对话框显示时间,超时自动关闭
    @return
        单击确定按钮时返回true,否则返回false
    */
	static bool error(const QString& sMsg, const QString& sTitle = nullptr, 
		Qt::WindowModality eWinModality = Qt::ApplicationModal, int nShowTime = 0);

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
	static bool confirm(const QString& sMsg, const QString& sTitle = nullptr, 
		ButtonText eBtnType = mbtOkCancel, DefaultButton eDefaultBtn = mdbOkYes);

	/*
		显示提示信息提示框
	@param
		parent			父窗口
		eMsgType		消息类型
		sMsg            显示的消息
		sTitle			提示标题
		eWinModality	窗口模式形态
		nShowTime       对话框显示时间,超时自动关闭
		eBtnText		按钮文字
		eDefaultBtn		默认获得焦点的按钮
		sDetail         显示详细信息
		bDetailIsOpen   true为打开详细,false为隐藏
	@return
		单击确定按钮时返回true,否则返回false
	*/
	static bool msgBox(QWidget* parent, MessageType eMsgType,
		const QString& sMsg, const QString& sTitle = nullptr, 
		Qt::WindowModality eWinModality = Qt::WindowModal, int nShowTime = 0,
		ButtonText eBtnText = mbtOkCancel, DefaultButton eDefaultBtn = mdbOkYes,
		const QString& sDetail = NULL, bool bDetailIsOpen = false);

public:
    /* 
		构造一个消息对话框 
	*/
	explicit MsgDlg(QWidget *parent = nullptr, 
					 MessageType eMsgType = mtInfo, 
					 Qt::WindowModality eWinModality = Qt::WindowModal,
					 bool bShowDetail = false);
    ~MsgDlg();

	/* 设置对话框 */
	void setDialog(
				const QString &sTitle,		// 提示信息
				const QString &sMsg,		// 提示信息
				const QString &sDetail,		// 显示详细信息
				ButtonText eBtnText,		// 按钮文字
				DefaultButton eDefaultBtn,	// 默认获得焦点的按钮
				int nShowTime,				// 显示时间，超过自动关闭
				bool bDetailIsOpen			// 打开明细
		);

protected:
	/* 初始化控件 */
	void initControl(bool bShowDetail, bool bIsConfirm);

	/* 设置对话框标题和图标 */
    void setTitleIcon(MessageType eMsgType);

	/* 设置显示信息 */
	void setMsgInfo(const QString & sMsg);

	/* 显示定时处理 */
    void timerEvent(QTimerEvent* event); 

private slots:
	/* 确定按钮的槽 */
	void doOkButtonClick();
	/* 取消按钮的槽 */
	void doCancelButtonClick();
	/* 明细按钮的槽 */
	void doShowDetailButtonClick();

private:
	QFrame* m_msgPanel = nullptr;			// 包括imagelabel和scrollArea
	QLabel* m_imgLabel = nullptr;			// 显示图片的QLabel控件
	QScrollArea* m_msgScroll = nullptr;		// 消息的滚动条
	QLabel* m_msgLabel = nullptr;			// 显示消息文字的QLabel控件
	QFrame* m_buttonBar = nullptr;			// 包括btn_ok,btn_cancel,btn_detail
	QPushButton* m_btnOk = nullptr;			// 确定按钮
	QPushButton* m_btnCancel = nullptr;		// 取消按钮
	QPushButton* m_btnDetail = nullptr;		// 明细按钮
	QTextEdit* m_detailInfo = nullptr;		// 显示明细文字的QTextEdit控件
    
	int m_nShowTimer = 0;					// 显示定时器
    int m_nShowTimes = 0;					// 显示时间
};

#endif // MSGDLG_H
