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
// SARibbonButtonGroupWidget::PrivateData
//===================================================
class SARibbonButtonGroupWidget::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonButtonGroupWidget)
public:
    PrivateData(SARibbonButtonGroupWidget* p);
    void init();
    void removeAction(QAction* a);

public:
    QSize mIconSize { 20, 20 };
};

SARibbonButtonGroupWidget::PrivateData::PrivateData(SARibbonButtonGroupWidget* p) : q_ptr(p)
{
}

void SARibbonButtonGroupWidget::PrivateData::init()
{
    QHBoxLayout* layout = new QHBoxLayout(q_ptr);
    // 上下保留一点间隙
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(1);
    q_ptr->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

void SARibbonButtonGroupWidget::PrivateData::removeAction(QAction* a)
{
    QLayout* lay = q_ptr->layout();
    int c        = lay->count();
    QList< QLayoutItem* > willRemoveItems;
    for (int i = 0; i < c; ++i) {
        QLayoutItem* item          = lay->itemAt(i);
        SARibbonControlButton* btn = qobject_cast< SARibbonControlButton* >(item->widget());
        if (nullptr == btn) {
            continue;
        }
        if (a == btn->defaultAction()) {
            willRemoveItems.push_back(item);
        }
    }
    // 从尾部删除
    for (auto i = willRemoveItems.rbegin(); i != willRemoveItems.rend(); ++i) {
        lay->removeItem(*i);
    }
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
}

/**
 * @brief 设置图标尺寸
 * @param iconSize
 */
void SARibbonButtonGroupWidget::setIconSize(const QSize& ic)
{
    d_ptr->mIconSize = ic;
    iterateButton([ ic ](SARibbonControlButton* btn) -> bool {
        btn->setIconSize(ic);
        return true;
    });
}

/**
 * @brief 图标尺寸
 * @return
 */
QSize SARibbonButtonGroupWidget::iconSize() const
{
    return d_ptr->mIconSize;
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

SARibbonControlButton* SARibbonButtonGroupWidget::actionToRibbonControlToolButton(QAction* action)
{
    SARibbonControlButton* res = nullptr;
    iterateButton([ &res, action ](SARibbonControlButton* btn) -> bool {
        if (btn->defaultAction() == action) {
            res = btn;
            return false;  // 返回false退出迭代
        }
        return true;
    });
    return (res);
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
 * @brief 此函数会遍历SARibbonButtonGroupWidget下的所有SARibbonControlButton，执行函数指针(bool(SARibbonControlButton*))，函数指针返回false则停止迭代
 * @param fp
 * @return 中途迭代退出返回false
 */
bool SARibbonButtonGroupWidget::iterateButton(SARibbonButtonGroupWidget::FpButtonIterate fp)
{
    QLayout* lay = layout();
    int c        = lay->count();
    for (int i = 0; i < c; ++i) {
        auto item = lay->itemAt(i);
        if (!item) {
            continue;
        }
        SARibbonControlButton* btn = qobject_cast< SARibbonControlButton* >(item->widget());
        if (!btn) {
            continue;
        }
        if (!fp(btn)) {
            return false;
        }
    }
    return true;
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
    QAction* a = e->action();
    if (!a) {
        return;
    }

    switch (e->type()) {
    case QEvent::ActionAdded: {
        QWidget* w = nullptr;
        if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(a)) {
            widgetAction->setParent(this);
            w = widgetAction->requestWidget(this);
            if (w != nullptr) {
                w->setAttribute(Qt::WA_LayoutUsesWidgetRect);
                w->show();
            }
        } else if (a->isSeparator()) {
            SARibbonSeparatorWidget* sp = RibbonSubElementFactory->createRibbonSeparatorWidget(this);
            w                           = sp;
        }
        // 不是widget，自动生成ButtonTyle
        if (!w) {
            SARibbonControlButton* button = RibbonSubElementFactory->createRibbonControlButton(this);
            button->setAutoRaise(true);
            button->setIconSize(d_ptr->mIconSize);
            button->setFocusPolicy(Qt::NoFocus);
            button->setDefaultAction(a);
            // 属性设置
            QToolButton::ToolButtonPopupMode popMode = SARibbonPannel::getActionToolButtonPopupModeProperty(a);
            button->setPopupMode(popMode);
            Qt::ToolButtonStyle buttonStyle = SARibbonPannel::getActionToolButtonStyleProperty(a);
            button->setToolButtonStyle(buttonStyle);
            // 根据QAction的属性设置按钮的大小

            connect(button, &SARibbonToolButton::triggered, this, &SARibbonButtonGroupWidget::actionTriggered);
            w = button;
        }
        layout()->addWidget(w);
        updateGeometry();
    } break;

    case QEvent::ActionChanged: {
        // 让布局重新绘制
        layout()->invalidate();
        updateGeometry();
    } break;

    case QEvent::ActionRemoved: {
        d_ptr->removeAction(e->action());
        updateGeometry();
    } break;

    default:
        break;
    }
    QFrame::actionEvent(e);
}
