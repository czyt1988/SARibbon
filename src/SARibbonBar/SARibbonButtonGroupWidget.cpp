#include "SARibbonButtonGroupWidget.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QMargins>
#include <QChildEvent>
#include <QActionEvent>
#include <QWidgetAction>
#include <QApplication>
#include "SARibbonControlButton.h"
#include "SARibbonElementManager.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonPannel.h"

//===================================================
// SAPrivateRibbonButtonGroupWidgetItem
//===================================================

class SAPrivateRibbonButtonGroupWidgetItem
{
public:
    QAction* action;
    QWidget* widget;
    bool customWidget;
    bool operator==(QAction* action);
    bool operator==(const SAPrivateRibbonButtonGroupWidgetItem& w);

    SAPrivateRibbonButtonGroupWidgetItem();
    SAPrivateRibbonButtonGroupWidgetItem(QAction* a, QWidget* w, bool cw);
};

//===================================================
// SARibbonButtonGroupWidget::PrivateData
//===================================================
class SARibbonButtonGroupWidget::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonButtonGroupWidget)
public:
    PrivateData(SARibbonButtonGroupWidget* p);
    void init();

public:
    QList< SAPrivateRibbonButtonGroupWidgetItem > mItems;  ///< 用于记录所有管理的item
    int mFixheight;     ///内部控件的统一高度
    int mItemMargin;    ///间距
};

SARibbonButtonGroupWidget::PrivateData::PrivateData(SARibbonButtonGroupWidget* p)
    : q_ptr(p), mFixheight(20), mItemMargin(0)
{
}

void SARibbonButtonGroupWidget::PrivateData::init()
{
    QHBoxLayout* layout = new QHBoxLayout(q_ptr);
    layout->setContentsMargins(0, mItemMargin, 0, mItemMargin);
    layout->setSpacing(0);
    q_ptr->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

//===================================================
// SARibbonButtonGroupWidgetItem
//===================================================

bool SAPrivateRibbonButtonGroupWidgetItem::operator==(QAction* action)
{
    return (this->action == action);
}

bool SAPrivateRibbonButtonGroupWidgetItem::operator==(const SAPrivateRibbonButtonGroupWidgetItem& w)
{
    return (this->action == w.action);
}

SAPrivateRibbonButtonGroupWidgetItem::SAPrivateRibbonButtonGroupWidgetItem()
    : action(nullptr), widget(nullptr), customWidget(false)
{
}

SAPrivateRibbonButtonGroupWidgetItem::SAPrivateRibbonButtonGroupWidgetItem(QAction* a, QWidget* w, bool cw)
    : action(a), widget(w), customWidget(cw)
{
}

//===================================================
// SARibbonButtonGroupWidget
//===================================================

SARibbonButtonGroupWidget::SARibbonButtonGroupWidget(QWidget* parent)
    : QFrame(parent), d_ptr(new SARibbonButtonGroupWidget::PrivateData(this))
{
    d_ptr->init();
}

SARibbonButtonGroupWidget::~SARibbonButtonGroupWidget()
{
    for (SAPrivateRibbonButtonGroupWidgetItem& item : d_ptr->mItems) {
        if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(item.action)) {
            if (item.customWidget) {
                widgetAction->releaseWidget(item.widget);
            }
        }
    }
}

QAction* SARibbonButtonGroupWidget::addAction(QAction* a, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    SARibbonPannel::setActionToolButtonStyleProperty(a, buttonStyle);
    SARibbonPannel::setActionToolButtonPopupModeProperty(a, popMode);
    QWidget::addAction(a);
    return (a);
}

/**
 * @brief 生成action
 * @note action的所有权归SARibbonButtonGroupWidget
 * @param text
 * @param icon
 * @param popMode
 * @return
 */
QAction* SARibbonButtonGroupWidget::addAction(const QString& text, const QIcon& icon, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    QAction* a = new QAction(icon, text, this);
    addAction(a, buttonStyle, popMode);
    return (a);
}

QAction* SARibbonButtonGroupWidget::addMenu(QMenu* menu, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
	QAction* a = menu->menuAction();
	addAction(a, buttonStyle, popMode);
    return (a);
}

QAction* SARibbonButtonGroupWidget::addSeparator()
{
    QAction* a = new QAction(this);

    a->setSeparator(true);
    addAction(a);
    return (a);
}

QAction* SARibbonButtonGroupWidget::addWidget(QWidget* w)
{
    QWidgetAction* a = new QWidgetAction(this);

    a->setDefaultWidget(w);
    w->setAttribute(Qt::WA_Hover);
    addAction(a);
    return (a);
}

SARibbonControlToolButton *SARibbonButtonGroupWidget::actionToRibbonControlToolButton(QAction *action)
{
	for (auto obj : qAsConst(children())) {
		if (obj->isWidgetType()) {
			if (SARibbonControlToolButton* btn = qobject_cast< SARibbonControlToolButton* >(obj)) {
				if (btn->defaultAction() == action) {
					return btn;
				}
			}
		}
	}
	return (nullptr);
}

QSize SARibbonButtonGroupWidget::sizeHint() const
{
    return (layout()->sizeHint());
}

QSize SARibbonButtonGroupWidget::minimumSizeHint() const
{
    return (layout()->minimumSize());
}

void SARibbonButtonGroupWidget::resizeEvent(QResizeEvent *event)
{
//	int newH = geometry().size().height();
//	setItemHeight(newH - 2 * d_ptr->mItemMargin);
	QFrame::resizeEvent(event);
}

void SARibbonButtonGroupWidget::setItemHeight(int h)
{
	d_ptr->mFixheight = h;
	// 迭代已经保存的button
	const QObjectList& objlist = children();
	for (QObject* obj : objlist) {
		if (SARibbonControlToolButton* btn = qobject_cast< SARibbonControlToolButton* >(obj)) {
			btn->setFixedHeight(h);
			btn->setMinimumWidth(h);
		}
	}
}

int SARibbonButtonGroupWidget::itemHeight() const
{
	return d_ptr->mFixheight;
}

void SARibbonButtonGroupWidget::setItemMargin(int m)
{
	d_ptr->mItemMargin = m;
	layout()->setContentsMargins(0, m, 0, m);
}

int SARibbonButtonGroupWidget::itemMargin() const
{
    return d_ptr->mItemMargin;
}

/**
 * @brief 处理action的事件
 *
 * 这里处理了ActionAdded，ActionChanged，ActionRemoved三个事件
 * ActionAdded时会生成窗口
 * @param e
 */
void SARibbonButtonGroupWidget::actionEvent(QActionEvent* e)
{
    SAPrivateRibbonButtonGroupWidgetItem item;

    item.action = e->action();

    switch (e->type()) {
    case QEvent::ActionAdded: {
        if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(item.action)) {
            widgetAction->setParent(this);
            item.widget = widgetAction->requestWidget(this);
            if (item.widget != nullptr) {
                item.widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
                item.widget->show();
                //widget高度保持一致
                item.widget->setFixedHeight(d_ptr->mFixheight);
                item.customWidget = true;
            }
        } else if (item.action->isSeparator()) {
            SARibbonSeparatorWidget* sp = RibbonSubElementDelegate->createRibbonSeparatorWidget(this);
            sp->setTopBottomMargins(3, 3);
            item.widget = sp;
        }
        // 不是widget，自动生成ButtonTyle
        if (!item.widget) {
            //只使用small
            SARibbonToolButton::RibbonButtonType buttonType = SARibbonToolButton::SmallButton;
            SARibbonControlToolButton* button = RibbonSubElementDelegate->createRibbonControlToolButton(this);
            button->setAutoRaise(true);
            button->setFixedHeight(d_ptr->mFixheight);
            button->setMinimumWidth(d_ptr->mFixheight);
            button->setFocusPolicy(Qt::NoFocus);
            button->setButtonType(buttonType);
            button->setDefaultAction(item.action);
            // 属性设置
            QToolButton::ToolButtonPopupMode popMode = SARibbonPannel::getActionToolButtonPopupModeProperty(item.action);
            button->setPopupMode(popMode);
            Qt::ToolButtonStyle buttonStyle = SARibbonPannel::getActionToolButtonStyleProperty(item.action);
            button->setToolButtonStyle(buttonStyle);
            // 根据QAction的属性设置按钮的大小

			QObject::connect(button, &SARibbonToolButton::triggered, this, &SARibbonButtonGroupWidget::actionTriggered);
			item.widget = button;
        }
        layout()->addWidget(item.widget);
        d_ptr->mItems.append(item);
        updateGeometry();
    } break;

    case QEvent::ActionChanged: {
        // 让布局重新绘制
        layout()->invalidate();
    } break;

    case QEvent::ActionRemoved: {
        item.action->disconnect(this);
        auto i = d_ptr->mItems.begin();
        for (; i != d_ptr->mItems.end();) {
            QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(i->action);
            if ((widgetAction != 0) && i->customWidget) {
                widgetAction->releaseWidget(i->widget);
            } else {
                // destroy the QToolButton/QToolBarSeparator
                i->widget->hide();
                i->widget->deleteLater();
            }
            i = d_ptr->mItems.erase(i);
        }
        layout()->invalidate();
    } break;

    default:
        break;
    }
    QFrame::actionEvent(e);
}
