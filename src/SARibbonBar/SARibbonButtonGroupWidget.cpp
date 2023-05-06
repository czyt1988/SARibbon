#include "SARibbonButtonGroupWidget.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QMargins>
#include <QChildEvent>
#include <QActionEvent>
#include <QWidgetAction>
#include <QApplication>
#include "SARibbonToolButton.h"
#include "SARibbonElementManager.h"
#include "SARibbonSeparatorWidget.h"

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
};

SARibbonButtonGroupWidget::PrivateData::PrivateData(SARibbonButtonGroupWidget* p) : q_ptr(p)
{
}

void SARibbonButtonGroupWidget::PrivateData::init()
{
    QHBoxLayout* layout = new QHBoxLayout(q_ptr);
    layout->setContentsMargins(0, 0, 0, 0);
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

QAction* SARibbonButtonGroupWidget::addAction(QAction* a)
{
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
QAction* SARibbonButtonGroupWidget::addAction(const QString& text, const QIcon& icon, QToolButton::ToolButtonPopupMode popMode)
{
    QAction* a = new QAction(icon, text, this);

    addAction(a);
    SARibbonToolButton* btn = qobject_cast< SARibbonToolButton* >(d_ptr->mItems.back().widget);
    btn->setPopupMode(popMode);
    return (a);
}

QAction* SARibbonButtonGroupWidget::addMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode)
{
    QAction* a = menu->menuAction();

    addAction(a);
    SARibbonToolButton* btn = qobject_cast< SARibbonToolButton* >(d_ptr->mItems.back().widget);
    btn->setPopupMode(popMode);
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

QSize SARibbonButtonGroupWidget::sizeHint() const
{
    return (layout()->sizeHint());
}

QSize SARibbonButtonGroupWidget::minimumSizeHint() const
{
    return (layout()->minimumSize());
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
                item.customWidget = true;
            }
        } else if (item.action->isSeparator()) {
            SARibbonSeparatorWidget* sp = RibbonSubElementDelegate->createRibbonSeparatorWidget(this);
            sp->setTopBottomMargins(3, 3);
            item.widget = sp;
        }
        //不是widget，自动生成SARibbonToolbutton
        if (!item.widget) {
            SARibbonToolButton* button = RibbonSubElementDelegate->createRibbonToolButton(this);
            button->setAutoRaise(true);
            button->setFocusPolicy(Qt::NoFocus);
            button->setButtonType(SARibbonToolButton::SmallButton);
            button->setToolButtonStyle(Qt::ToolButtonIconOnly);
            button->setDefaultAction(item.action);
            //根据QAction的属性设置按钮的大小

            QObject::connect(button, &SARibbonToolButton::triggered, this, &SARibbonButtonGroupWidget::actionTriggered);
            item.widget = button;
        }
        layout()->addWidget(item.widget);
        d_ptr->mItems.append(item);
    } break;

    case QEvent::ActionChanged: {
        //让布局重新绘制
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
}
