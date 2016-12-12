#include "ktabbutton.h"
#include <QPainter>
#include <QStylePainter>
#include <QPainterPath>
#include <QStyleOptionButton>

/*-----------------------------------------------------------------------------
* InternalButton
* 定义一个内部按钮
-----------------------------------------------------------------------------*/
class InternalButton
{
public:
	QString id;
	bool enabled;
	bool visible;
	kd::AlignPositon alignPos;
	QPoint offset;
	QIcon icon;
	QIcon hoverIcon;

	bool mouseOver = false;
	bool mousePress = false;
	QRect position;
};

/*-----------------------------------------------------------------------------
* KTabButtonPrivate
* KTabButton的私有类，保存KTabButton的成员变量
-----------------------------------------------------------------------------*/
class KTabButtonPrivate
{
public:
	KTabButtonPrivate(KTabButton* pWidget = nullptr)
		: m_pWidget(pWidget)
	{

	}

	~KTabButtonPrivate()
	{
		m_oInterBtns.clear();
	}

	/* 计算内部按钮位置 */
	void calcInternalBtnPositions()
	{
		QMap<QString, InternalButton>::Iterator iter = m_oInterBtns.begin();
		while (iter != m_oInterBtns.end())
		{
			iter.value().position = calcInternalBtnPosition(iter.value());
			++iter;
		}
	}

	QRect calcInternalBtnPosition(InternalButton& btn)
	{
		int x, y;
		switch (btn.alignPos)
		{
		case kd::apVRightCenter:
			x = m_pWidget->width() - m_oInterBtnSize.width() - btn.offset.x();
			y = (m_pWidget->height() - m_oInterBtnSize.height()) / 2 - btn.offset.y();
			break;
		case kd::apVRightTop:
			x = m_pWidget->width() - m_oInterBtnSize.width() - btn.offset.x();
			y = btn.offset.y();
			break;
		case kd::apVRightBottom:
			x = m_pWidget->width() - m_oInterBtnSize.width() - btn.offset.x();
			y = m_pWidget->height() - m_oInterBtnSize.height() - btn.offset.y();
			break;
		case kd::apVLeftTop:
			x = btn.offset.x();
			y = btn.offset.y();
			break;
		case kd::apVLeftCenter:
			x = btn.offset.x();
			y = (m_pWidget->height() - m_oInterBtnSize.height()) / 2 - btn.offset.y();
			break;
		case kd::apVLeftBottom:
			x = btn.offset.x();
			y = m_pWidget->height() - m_oInterBtnSize.height() - btn.offset.y();
			break;
		case kd::apHTopCenter:
			x = (m_pWidget->width() - m_oInterBtnSize.width()) / 2 - btn.offset.x();
			y = btn.offset.y();
			break;
		case kd::apHBottomCenter:
			x = (m_pWidget->width() - m_oInterBtnSize.width()) / 2 - btn.offset.x();
			y = m_pWidget->height() - m_oInterBtnSize.height() - btn.offset.y();
			break;
		default:
			return btn.position;
		}
		return QRect(x, y, m_oInterBtnSize.width(), m_oInterBtnSize.height());
	}

	/* 复位内部按钮鼠标Over状态 */
	void resetInternalButtonOverStatus()
	{
		QMap<QString, InternalButton>::Iterator iter = m_oInterBtns.begin();
		while (iter != m_oInterBtns.end())
		{
			if (iter.value().mouseOver)
			{
				iter.value().mouseOver = false;
				m_pWidget->update(QRegion(iter.value().position));
			}
			++iter;
		}
	}

	/* 复位内部按钮鼠标Press状态 */
	void resetInternalButtonPressStatus()
	{
		QMap<QString, InternalButton>::Iterator iter = m_oInterBtns.begin();
		while (iter != m_oInterBtns.end())
		{
			if (iter.value().mousePress)
			{
				iter.value().mousePress = false;
				m_pWidget->update(QRegion(iter.value().position));
			}
			++iter;
		}
	}

	/* 获取文件绘制矩形 */
	QRect getDrawTextRect() const
	{
		QRect dr = m_pWidget->contentsRect();
		QMapIterator<QString, InternalButton> iter(m_oInterBtns);
		while (iter.hasNext())
		{
			iter.next();
			if (!iter.value().visible) continue;

			switch (iter.value().alignPos)
			{
			case kd::apVRightCenter:
			case kd::apVRightTop:
			case kd::apVRightBottom:
				if (dr.right() > iter.value().position.left())
				{
					dr.setRight(iter.value().position.left());
				}
				break;
			case kd::apVLeftCenter:
			case kd::apVLeftTop:
			case kd::apVLeftBottom:
				if (dr.left() < iter.value().position.right())
				{
					dr.setLeft(iter.value().position.right());
				}
				break;
			case kd::apHTopCenter:
				if (dr.top() < iter.value().position.bottom())
				{
					dr.setTop(iter.value().position.bottom());
				}
				break;
			case kd::apHBottomCenter:
				if (dr.bottom() > iter.value().position.top())
				{
					dr.setBottom(iter.value().position.top());
				}
				break;
			}
		}

		if (!m_pWidget->icon().isNull())
		{
			if (m_pWidget->toolButtonStyle() == Qt::ToolButtonTextUnderIcon)
			{
				dr.setTop(dr.top() + m_pWidget->iconSize().width());
			}
			else if (m_pWidget->toolButtonStyle() == Qt::ToolButtonTextBesideIcon)
			{
				int nLeft = m_pWidget->iconSize().width() + 6;
				if (dr.left() < nLeft)
				{
					dr.setLeft(nLeft);
				}
			}
		}
		return dr;
	}

	/* 成员变量 */
	KTabButton* m_pWidget;
	QString m_sText;
	Qt::TextElideMode m_eElideMode = Qt::ElideRight;
	QVariant m_vTabId = QVariant();
	QObject* m_pUserObject = nullptr;
	QSize m_oInterBtnSize = QSize(16, 16);
	QMap<QString, InternalButton> m_oInterBtns;
};

/*-----------------------------------------------------------------------------
* KTabButton
* 标签按钮
-----------------------------------------------------------------------------*/
KTabButton::KTabButton(QWidget *parent) 
	: QToolButton(parent),
	d(new KTabButtonPrivate(this))
{
    setMouseTracking(true);
}

KTabButton::~KTabButton()
{
	delete d;
}

/* 文字省略方式 */
Qt::TextElideMode KTabButton::textElideMode() const
{
	return d->m_eElideMode;
}

void KTabButton::setTextElideMode(const Qt::TextElideMode value)
{
	if (d->m_eElideMode != value)
	{
		d->m_eElideMode = value;
		update();
	}
}

/* 标识 */
QVariant KTabButton::tabId() const
{
	return d->m_vTabId;
}

void KTabButton::setTabId(const QVariant& id)
{
	d->m_vTabId = id;
}

/* 用户数据 */
QObject* KTabButton::userObject() const
{
	return d->m_pUserObject;
}

void KTabButton::setUserObject(QObject* obj)
{
	d->m_pUserObject = obj;
}

/* 添加内部按钮 */
void KTabButton::addInternalButton(const QString& sBtnId, 
	kd::AlignPositon alignPos, const QPoint& offset, 
	const QIcon& icon, const QIcon& hoverIcon, 
	bool bEnabled, bool bVisible)
{
	InternalButton btn;
	btn.id = sBtnId.isEmpty() ? "0" : sBtnId;
	btn.enabled = bEnabled;
	btn.visible = bVisible;
	btn.alignPos = alignPos;
	btn.offset = offset;
	btn.icon = icon;
	btn.hoverIcon = hoverIcon;
	btn.position = d->calcInternalBtnPosition(btn);
	d->m_oInterBtns[btn.id] = btn;

	update();
}

/* 移除内部按钮 */
void KTabButton::removeInternalButton(const QString& sBtnId)
{
	d->m_oInterBtns.remove(sBtnId);
	update();
}

void KTabButton::removeInternalButtons()
{
	d->m_oInterBtns.clear();
	update();
}

/* 隐藏/显示内部按钮 */
void KTabButton::setInternalButtonVisible(const QString& sBtnId, bool bVisible)
{
	if (d->m_oInterBtns.contains(sBtnId))
	{
		InternalButton btn = d->m_oInterBtns[sBtnId];
		if (btn.visible != bVisible)
		{
			btn.visible = bVisible;
			d->m_oInterBtns[sBtnId] = btn;
			update();
		}
	}
}

/* 启用/禁止内部按钮 */
void KTabButton::setInternalButtonEnabled(const QString& sBtnId, bool bEnabled)
{
	if (d->m_oInterBtns.contains(sBtnId))
	{
		InternalButton btn = d->m_oInterBtns[sBtnId];
		if (btn.enabled != bEnabled)
		{
			btn.enabled = bEnabled;
			d->m_oInterBtns[sBtnId] = btn;
			update();
		}
	}
}

/* 检查内部按钮是否存在 */
bool KTabButton::checkInternalButtonExisted(const QString& sBtnId)
{
	return d->m_oInterBtns.contains(sBtnId);
}

/* 设置内部按钮大小 */
QSize KTabButton::internalButtonSize() const
{
	return d->m_oInterBtnSize;
}

void KTabButton::setInternalButtonSize(const QSize& size)
{
	d->m_oInterBtnSize = size;
	d->calcInternalBtnPositions();
}

/* 检查鼠标是否在内部按钮上 */
void KTabButton::checkMouseOnButton(const QPoint& pos)
{
	QMap<QString, InternalButton>::Iterator iter = d->m_oInterBtns.begin();
	while (iter != d->m_oInterBtns.end())
	{
		if (iter.value().visible)
		{
			if (iter.value().position.contains(pos))
			{
				if (iter.value().enabled && !iter.value().mouseOver)
				{
					iter.value().mouseOver = true;
					update(QRegion(iter.value().position));
				}
			}
			else if (iter.value().mouseOver)
			{
				iter.value().mouseOver = false;
				update(QRegion(iter.value().position));
			}
		}
		++iter;
	}
}

/* 检查鼠标是否在内部按钮上按下 */
bool KTabButton::checkMousePressOnButton(const QPoint& pos)
{
	QMap<QString, InternalButton>::Iterator iter = d->m_oInterBtns.begin();
	while (iter != d->m_oInterBtns.end())
	{
		if (iter.value().visible)
		{
			if (iter.value().position.contains(pos))
			{
				if (iter.value().enabled)
				{
					iter.value().mousePress = true;
					update(QRegion(iter.value().position));
					emit onInternalButtonClicked(iter.key());
					return true;
				}
				return false;
			}
		}
		++iter;
	}
	return false;
}

void KTabButton::mouseMoveEvent(QMouseEvent* evt)
{
	QToolButton::mouseMoveEvent(evt);
	
	checkMouseOnButton(evt->pos());
}

void KTabButton::mousePressEvent(QMouseEvent *evt)
{
	if (checkMousePressOnButton(evt->pos()))
	{
		evt->accept();
		return;
	}

	QToolButton::mousePressEvent(evt);
}

void KTabButton::mouseReleaseEvent(QMouseEvent *evt)
{
	// 执行父类的方法
	QToolButton::mouseReleaseEvent(evt);

	d->resetInternalButtonPressStatus();
}

void KTabButton::leaveEvent(QEvent * evt)
{
	// 执行父类的方法
	QToolButton::leaveEvent(evt);

	d->resetInternalButtonOverStatus();
}

void KTabButton::resizeEvent(QResizeEvent* evt)
{
	QToolButton::resizeEvent(evt);

	d->calcInternalBtnPositions();
}

void KTabButton::paintEvent(QPaintEvent *)
{
	// 默认绘制
	//QToolButton::paintEvent(evt);
	
	// 自定义绘制
	QStylePainter painter(this);
	QStyleOptionToolButton opt;
	initStyleOption(&opt);
	opt.text = "";
	painter.drawComplexControl(QStyle::CC_ToolButton, opt);

	// 绘制文字
	QRect dr = d->getDrawTextRect();
	QFontMetrics fm = QFontMetrics(font());
	QString sText = fm.elidedText(text(), d->m_eElideMode, dr.width());
	painter.drawText(dr, Qt::AlignCenter, sText);

	// 绘制内部按钮
	QMapIterator<QString, InternalButton> iter(d->m_oInterBtns);
	while (iter.hasNext())
	{
		iter.next();
		InternalButton btn = std::move(iter.value());
		if (!btn.visible) continue;

		if (btn.enabled)
		{
			if (btn.mousePress)
			{
				QRect rt = btn.position;
				rt.moveTo(rt.topLeft() + QPoint(1, 1));
				btn.hoverIcon.paint(&painter, rt, Qt::AlignCenter, QIcon::Active);
			}
			else if (btn.mouseOver)
				btn.hoverIcon.paint(&painter, btn.position, Qt::AlignCenter, QIcon::Normal);
			else
				btn.icon.paint(&painter, btn.position, Qt::AlignCenter, QIcon::Normal);
		}
		else
		{
			btn.icon.paint(&painter, btn.position, Qt::AlignCenter, QIcon::Disabled);
		}
	}
}
