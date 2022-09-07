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

class SARibbonButtonGroupWidgetItem
{
public:
    QAction* action;
    QWidget* widget;
    bool customWidget;
    bool operator==(QAction* action);
    bool operator==(const SARibbonButtonGroupWidgetItem& w);

    SARibbonButtonGroupWidgetItem();
    SARibbonButtonGroupWidgetItem(QAction* a, QWidget* w, bool cw);
};

class SARibbonButtonGroupWidgetPrivate
{
public:
    SARibbonButtonGroupWidget* Parent;
    QList< SARibbonButtonGroupWidgetItem > mItems;  ///< 用于记录所有管理的item
    SARibbonButtonGroupWidgetPrivate(SARibbonButtonGroupWidget* p)
    {
        Parent = p;
    }

    void init()
    {
        QHBoxLayout* layout = new QHBoxLayout(Parent);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        Parent->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    }
};

//////////////////////////////////////////////

bool SARibbonButtonGroupWidgetItem::operator==(QAction* action)
{
    return (this->action == action);
}

bool SARibbonButtonGroupWidgetItem::operator==(const SARibbonButtonGroupWidgetItem& w)
{
    return (this->action == w.action);
}

SARibbonButtonGroupWidgetItem::SARibbonButtonGroupWidgetItem() : action(nullptr), widget(nullptr), customWidget(false)
{
}

SARibbonButtonGroupWidgetItem::SARibbonButtonGroupWidgetItem(QAction* a, QWidget* w, bool cw)
    : action(a), widget(w), customWidget(cw)
{
}

//////////////////////////////////////////////

SARibbonButtonGroupWidget::SARibbonButtonGroupWidget(QWidget* parent)
    : QFrame(parent), m_d(new SARibbonButtonGroupWidgetPrivate(this))
{
    m_d->init();
}

SARibbonButtonGroupWidget::~SARibbonButtonGroupWidget()
{
    for (SARibbonButtonGroupWidgetItem& item : m_d->mItems) {
        if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(item.action)) {
            if (item.customWidget) {
                widgetAction->releaseWidget(item.widget);
            }
        }
    }
    delete m_d;
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
    SARibbonToolButton* btn = qobject_cast< SARibbonToolButton* >(m_d->mItems.back().widget);
    btn->setPopupMode(popMode);
    return (a);
}

QAction* SARibbonButtonGroupWidget::addMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode)
{
    QAction* a = menu->menuAction();

    addAction(a);
    SARibbonToolButton* btn = qobject_cast< SARibbonToolButton* >(m_d->mItems.back().widget);
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
    SARibbonButtonGroupWidgetItem item;

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
        m_d->mItems.append(item);
    } break;

    case QEvent::ActionChanged: {
        //让布局重新绘制
        layout()->invalidate();
    } break;

    case QEvent::ActionRemoved: {
        item.action->disconnect(this);
        auto i = m_d->mItems.begin();
        for (; i != m_d->mItems.end();) {
            QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(i->action);
            if ((widgetAction != 0) && i->customWidget) {
                widgetAction->releaseWidget(i->widget);
            } else {
                // destroy the QToolButton/QToolBarSeparator
                i->widget->hide();
                i->widget->deleteLater();
            }
            i = m_d->mItems.erase(i);
        }
        layout()->invalidate();
    } break;

    default:
        break;
    }
}
