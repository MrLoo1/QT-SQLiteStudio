﻿#ifndef TABBUTTON_H
#define TABBUTTON_H

#include "common.h"
#include <QToolButton>
#include <QPaintEvent>

namespace tabPos
{
	/* 布局内部按钮 */
	enum AlignPositon
	{
		apVLeftCenter = 0,
		apVLeftTop,
		apVLeftBottom,
		apVRightCenter,
		apVRightTop,
		apVRightBottom,
		apHTopCenter,
		apHBottomCenter,
	};
}

/*-----------------------------------------------------------------------------
* TabButton
* 标签按钮
-----------------------------------------------------------------------------*/
class InternalButton;
class TabButtonPrivate;
class TabButton : public QToolButton
{
    Q_OBJECT
public:
    explicit TabButton(QWidget *parent = 0);
	~TabButton();

	/* 添加内部按钮 */
	void addInternalButton(const QString& sBtnId, tabPos::AlignPositon alignPos,
		const QPoint& offset, const QIcon& icon, const QIcon& hoverIcon, 
		bool bEnabled = true, bool bVisible = true);

	/* 移除内部按钮 */
	void removeInternalButton(const QString& sBtnId);
	void removeInternalButtons();

	/* 隐藏/显示内部按钮 */
	void setInternalButtonVisible(const QString& sBtnId, bool bVisible);

	/* 启用/禁止内部按钮 */
	void setInternalButtonEnabled(const QString& sBtnId, bool bEnabled);

	/* 检查内部按钮是否存在 */
	bool checkInternalButtonExisted(const QString& sBtnId);
	
	/* 设置内部按钮大小 */
	QSize internalButtonSize() const;
	void setInternalButtonSize(const QSize& size);

	/* 文字省略方式 */
	Qt::TextElideMode textElideMode() const;
	void setTextElideMode(const Qt::TextElideMode value);

	/* 标识 */
	QVariant tabId() const;
	void setTabId(const QVariant& id);

	/* 用户对象指针，该对象指针不会自动释放 */
	QObject* userObject() const;
	void setUserObject(QObject* obj);

protected:
    void mouseMoveEvent(QMouseEvent* evt);
    void mousePressEvent(QMouseEvent* evt);
    void mouseReleaseEvent(QMouseEvent* evt);
	void leaveEvent(QEvent * evt);
    void resizeEvent(QResizeEvent* evt);
	void paintEvent(QPaintEvent* evt);

protected:
	/* 检查鼠标是否在内部按钮上 */
	void checkMouseOnButton(const QPoint& pos);

	/* 检查鼠标是否在内部按钮上按下 */
	bool checkMousePressOnButton(const QPoint& pos);

signals:
    /* 发送关闭信号 */
	void onInternalButtonClicked(const QString sButtonId);

private:
	TabButtonPrivate* d;
};

#endif // TABBUTTON_H
