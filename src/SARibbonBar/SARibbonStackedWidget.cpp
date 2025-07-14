#include "SARibbonStackedWidget.h"
#include <QEventLoop>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QApplication>

#define SARIBBONSTACKEDWIDGET_DEBUG_PRINT 0
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
#include <QDebug>
#endif
/**
 * @brief The SARibbonStackedWidgetPrivate class
 */
class SARibbonStackedWidget::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonStackedWidget)
public:
    QEventLoop* eventLoop { nullptr };

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
}

/**
 * @brief 设置弹出模式
 *
 * 在此模式下，窗口将：
 * - 设置为 Qt::Popup 标志
 * - 无边框窗口
 * - 显示为面板样式
 *
 * @note 切换模式时会自动处理窗口隐藏和显示
 */
void SARibbonStackedWidget::setPopupMode()
{
    if (isPopupMode()) {
        return;
    }
    bool wasVisible = isVisible();
    hide();  // 先隐藏防止闪烁
    setMouseTracking(true);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setFrameShape(QFrame::Panel);
    if (wasVisible) {
        // 恢复可见状态
        show();
    }
}

/**
 * @brief 检查当前是否处于弹出模式
 * @return 如果是弹出模式返回 true，否则返回 false
 */
bool SARibbonStackedWidget::isPopupMode() const
{
    return (windowFlags() & Qt::Popup);
}

/**
 * @brief 设置正常模式 和普通的stackwidget一样
 *
 * 在此模式下，窗口将：
 * - 设置为常规部件模式 (Qt::Widget)
 * - 无边框窗口
 * - 不显示框架
 *
 * @note 如果存在模态事件循环，将退出该循环
 */
void SARibbonStackedWidget::setNormalMode()
{
    if (isNormalMode()) {
        return;
    }
    if (d_ptr->eventLoop) {
        d_ptr->eventLoop->exit();
        d_ptr->eventLoop = nullptr;
    }
    bool wasVisible = isVisible();
    hide();
    setMouseTracking(false);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setFrameShape(QFrame::NoFrame);
    if (wasVisible) {
        show();
    }
}

/**
 * @brief 检查当前是否处于正常模式
 * @return 如果是正常模式返回 true，否则返回 false
 */
bool SARibbonStackedWidget::isNormalMode() const
{
    return (!isPopupMode());
}

/**
 * @brief 在弹出模式下以模态方式运行事件循环
 *
 * 此方法仅在弹出模式下有效：
 * 1. 显示窗口
 * 2. 启动局部事件循环
 * 3. 窗口隐藏时退出事件循环
 *
 * @note 在正常模式下调用此方法仅显示窗口
 * @warning 不要在事件循环中再次调用此方法
 */
void SARibbonStackedWidget::exec()
{
    if (!isPopupMode()) {
        show();
        return;
    }
    show();
    QEventLoop event;
    d_ptr->eventLoop = &event;
    event.exec();
    d_ptr->eventLoop = nullptr;  // 确保重置
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
        Q_EMIT hidWindow();
    }

    QStackedWidget::hideEvent(e);
}

void SARibbonStackedWidget::resizeEvent(QResizeEvent* e)
{
    // 先调用基类处理当前显示的部件
    QStackedWidget::resizeEvent(e);

    // 确保所有子部件都填满整个区域
    const QSize newSize = e->size();
    for (int i = 0; i < count(); ++i) {
        QWidget* innerWidget = widget(i);
        if (!innerWidget)
            continue;

        if (i == currentIndex()) {
            // 确保当前部件也填满整个区域
            if (innerWidget->size() != newSize) {
                innerWidget->setGeometry(0, 0, newSize.width(), newSize.height());
            }
            continue;
        }

        // 设置非活动部件的位置和大小
        innerWidget->move(0, 0);
        innerWidget->setFixedSize(newSize);
        // innerWidget->setGeometry(0, 0, newSize.width(), newSize.height());
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
        qDebug() << "SARibbonStackedWidget resizeEvent,set innerWidget to size:" << newSize << ",and widget geo is "
                 << innerWidget->geometry();
#endif
        // 通知部件布局可能需要更新
        QApplication::postEvent(innerWidget, new QEvent(QEvent::LayoutRequest));
    }
}
