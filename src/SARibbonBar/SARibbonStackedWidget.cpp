#include "SARibbonStackedWidget.h"
#include <QEventLoop>
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>

/**
 * @brief The SARibbonStackedWidgetPrivate class
 */
class SARibbonStackedWidget::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonStackedWidget)
public:
    QEventLoop* eventLoop { nullptr };
    bool isAutoResize { true };

public:
    PrivateData(SARibbonStackedWidget* p) : q_ptr(p)
    {
    }

    void init()
    {
        // Parent->setFocusPolicy(Qt::StrongFocus);
    }
};

SARibbonStackedWidget::SARibbonStackedWidget(QWidget* parent)
    : QStackedWidget(parent), d_ptr(new SARibbonStackedWidget::PrivateData(this))
{
    d_ptr->init();
    setNormalMode();
}

SARibbonStackedWidget::~SARibbonStackedWidget()
{
    if (d_ptr->eventLoop) {
        d_ptr->eventLoop->exit();
    }
}

void SARibbonStackedWidget::setPopupMode()
{
    setMouseTracking(true);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setFrameShape(QFrame::Panel);
}

bool SARibbonStackedWidget::isPopupMode() const
{
    return (windowFlags() & Qt::Popup);
}

void SARibbonStackedWidget::setNormalMode()
{
    if (d_ptr->eventLoop) {
        d_ptr->eventLoop->exit();
        d_ptr->eventLoop = nullptr;
    }
    setMouseTracking(false);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setFrameShape(QFrame::NoFrame);
}

bool SARibbonStackedWidget::isNormalMode() const
{
    return (!isPopupMode());
}

void SARibbonStackedWidget::exec()
{
    show();
    if (!isPopupMode()) {
        d_ptr->eventLoop = nullptr;
        return;
    }
    QEventLoop event;

    d_ptr->eventLoop = &event;
    event.exec();
    d_ptr->eventLoop = nullptr;
}

/**
 * @brief 设置stacked管理的窗口会随着stacked的大小变化而变化大小
 *
 * 默认为true
 * @param autoresize
 */
void SARibbonStackedWidget::setAutoResize(bool autoresize)
{
    d_ptr->isAutoResize = autoresize;
}

bool SARibbonStackedWidget::isAutoResize() const
{
    return (d_ptr->isAutoResize);
}

/**
 * @brief 类似tabbar的moveTab函数，交换两个窗口的index
 * @param from
 * @param to
 * @note 此操作会触发widgetRemoved(int index)信号
 */
void SARibbonStackedWidget::moveWidget(int from, int to)
{
    QWidget* w = widget(from);

    removeWidget(w);
    insertWidget(to, w);
}

void SARibbonStackedWidget::hideEvent(QHideEvent* e)
{
    if (isPopupMode()) {
        if (d_ptr->eventLoop) {
            d_ptr->eventLoop->exit();
        }
    }
    setFocus();
    emit hidWindow();

    QStackedWidget::hideEvent(e);
}
