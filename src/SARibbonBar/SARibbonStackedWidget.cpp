#include "SARibbonStackedWidget.h"
#include <QEventLoop>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPropertyAnimation>
#include <QLayout>
#ifndef SARIBBONSTACKEDWIDGET_DEBUG_PRINT
#define SARIBBONSTACKEDWIDGET_DEBUG_PRINT 0
#endif
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
#include <QDebug>
#endif
/**
 * \if ENGLISH
 * @brief Private data class for SARibbonStackedWidget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonStackedWidget的私有数据类
 * \endif
 */
class SARibbonStackedWidget::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonStackedWidget)
public:
    QEventLoop* eventLoop { nullptr };
    bool useAnimation { false };                ///< Whether to use animation
    QPropertyAnimation* animation { nullptr };  ///< Animation object
    QRect normalGeometry;                       ///< Geometry position in normal state
    bool isAnimating { false };  ///< Mark whether animation is in progress, this is necessary, it marks whether animation is started earlier than animation->state()
    int animationWidgetHeight { 0 };  ///< Window height during animation
public:
    PrivateData(SARibbonStackedWidget* p) : q_ptr(p)
    {
    }

    /**
     * \if ENGLISH
     * @brief Check if animation is running
     * @return true if animation is running, false otherwise
     * \endif
     *
     * \if CHINESE
     * @brief 检查动画是否正在运行
     * @return 如果动画正在运行返回 true，否则返回 false
     * \endif
     */
    bool isAnimationRunning() const
    {
        return (isAnimating || animation->state() == QAbstractAnimation::Running);
    }
};

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonStackedWidget
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonStackedWidget 构造函数
 * @param parent 父窗口
 * \endif
 */
SARibbonStackedWidget::SARibbonStackedWidget(QWidget* parent)
    : QStackedWidget(parent), d_ptr(new SARibbonStackedWidget::PrivateData(this))
{
    setNormalMode();
    setupAnimation();
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonStackedWidget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonStackedWidget 析构函数
 * \endif
 */
SARibbonStackedWidget::~SARibbonStackedWidget()
{
}

/**
 * \if ENGLISH
 * @brief Setup animation
 * \endif
 *
 * \if CHINESE
 * @brief 设置动画
 * \endif
 */
void SARibbonStackedWidget::setupAnimation()
{
    d_ptr->animation = new QPropertyAnimation(this, "animationWidgetHeight", this);
    d_ptr->animation->setEasingCurve(QEasingCurve::OutQuad);
    d_ptr->animation->setDuration(300);
    connect(d_ptr->animation, &QPropertyAnimation::finished, this, &SARibbonStackedWidget::onAnimationFinished);
}

/**
 * \if ENGLISH
 * @brief Get animation widget height
 * @return Animation widget height
 * \endif
 *
 * \if CHINESE
 * @brief 获取动画窗口高度
 * @return 动画窗口高度
 * \endif
 */
int SARibbonStackedWidget::animationWidgetHeight() const
{
    return d_ptr->animationWidgetHeight;
}

/**
 * \if ENGLISH
 * @brief Set animation widget height
 * @param h Height value
 * \endif
 *
 * \if CHINESE
 * @brief 设置动画窗口高度
 * @param h 高度值
 * \endif
 */
void SARibbonStackedWidget::setAnimationWidgetHeight(int h)
{
    if (d_ptr->animationWidgetHeight == h) {
        return;
    }

    d_ptr->animationWidgetHeight = h;

    if (d_ptr->isAnimationRunning() && isPopupMode()) {
        // 更新窗口大小和位置
        setFixedSize(d_ptr->normalGeometry.width(), h);
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
        qDebug() << "setAnimationWidgetHeight setFixedSize=" << d_ptr->normalGeometry.width() << "," << h;
#endif
    }
}

/**
 * \if ENGLISH
 * @brief Set window normalGeometry, since this window has animation, to prevent size setting from being overwritten by animation during animation process, this window's size setting uses setNormalSize
 * @details This function is equivalent to the following when there is no animation:
 * @details @code
 * @details setFixedSize(normalGeometry.width(),normalGeometry.height());
 * @details move(normalGeometry.x(),normalGeometry.y());
 * @details @endcode
 * @param normalGeometry
 * \endif
 *
 * \if CHINESE
 * @brief 设置窗口normalGeometry，由于此窗口会有动画，防止动画过程中设置尺寸又被动画覆盖，因此此窗口的尺寸设置使用setNormalSize
 * @details 此函数在没有动画的时候，等同于
 * @details @code
 * @details setFixedSize(normalGeometry.width(),normalGeometry.height());
 * @details move(normalGeometry.x(),normalGeometry.y());
 * @details @endcode
 * @param normalGeometry
 * \endif
 */
void SARibbonStackedWidget::setNormalGeometry(const QRect& normalGeometry)
{
    d_ptr->normalGeometry = normalGeometry;
    if (!d_ptr->isAnimationRunning()) {
        setFixedSize(d_ptr->normalGeometry.width(), d_ptr->normalGeometry.height());
        move(d_ptr->normalGeometry.x(), d_ptr->normalGeometry.y());
    }
}

/**
 * \if ENGLISH
 * @brief Get normal geometry
 * @return Normal geometry
 * \endif
 *
 * \if CHINESE
 * @brief 获取正常几何位置
 * @return 正常几何位置
 * \endif
 */
QRect SARibbonStackedWidget::normalGeometry() const
{
    return d_ptr->normalGeometry;
}

/**
 * \if ENGLISH
 * @brief Send layout request to inner widgets
 * @details This method will invalidate and recalculate the layout of child widgets
 * \endif
 *
 * \if CHINESE
 * @brief 对内部窗口发送布局请求
 * @details 这个方法会让子窗口布局失效同时重新计算布局
 * \endif
 */
void SARibbonStackedWidget::layoutRequestInnerWidgets()
{
    // 确保所有子部件都填满整个区域
    for (int i = 0; i < count(); ++i) {
        QWidget* innerWidget = widget(i);
        if (!innerWidget) {
            continue;
        }
        // 方法1 update（不生效）
        //  innerWidget->update();

        // 方法2 postEvent（不生效）
        //  QApplication::postEvent(innerWidget, new QEvent(QEvent::LayoutRequest));

        // 方法3 invalidate+activate（生效）
        if (auto lay = innerWidget->layout()) {
            lay->invalidate();
            lay->activate();
        }
    }
}

/**
 * \if ENGLISH
 * @brief Set popup mode
 * @details In this mode, the window will:
 * @details - Set Qt::Popup flag
 * @details - Frameless window
 * @details - Display as panel style
 * @note Switching modes will automatically handle window hiding and showing
 * \endif
 *
 * \if CHINESE
 * @brief 设置弹出模式
 * @details 在此模式下，窗口将：
 * @details - 设置为 Qt::Popup 标志
 * @details - 无边框窗口
 * @details - 显示为面板样式
 * @note 切换模式时会自动处理窗口隐藏和显示
 * \endif
 */
void SARibbonStackedWidget::setPopupMode()
{
    if (isPopupMode()) {
        return;
    }
    setMouseTracking(true);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setFrameShape(QFrame::Panel);
}

/**
 * \if ENGLISH
 * @brief Check if currently in popup mode
 * @return true if in popup mode, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 检查当前是否处于弹出模式
 * @return 如果是弹出模式返回 true，否则返回 false
 * \endif
 */
bool SARibbonStackedWidget::isPopupMode() const
{
    return (windowFlags() & Qt::Popup);
}

/**
 * \if ENGLISH
 * @brief Set normal mode, same as normal stackwidget
 * @details In this mode, the window will:
 * @details - Set to regular widget mode (Qt::Widget)
 * @details - Frameless window
 * @details - No frame display
 * @note If there is a modal event loop, it will exit that loop
 * \endif
 *
 * \if CHINESE
 * @brief 设置正常模式 和普通的stackwidget一样
 * @details 在此模式下，窗口将：
 * @details - 设置为常规部件模式 (Qt::Widget)
 * @details - 无边框窗口
 * @details - 不显示框架
 * @note 如果存在模态事件循环，将退出该循环
 * \endif
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
    // 停止动画并恢复最终位置
    if (d_ptr->isAnimationRunning()) {
        d_ptr->animation->stop();
        d_ptr->isAnimating = false;  // 停止后，一定要加上标记
        // 升级到qt6后，setFixedHeight容易出现异常，主要原因是setFixedHeight的实现如下：
        // void QWidget::setFixedHeight(int h) {
        //     // ...
        //     setMinimumSize(minimumSize().width(), h);  // 先设置最小高度
        //     setMaximumSize(maximumSize().width(), h);  // 再设置最大高度
        //     // ...
        // }
        // 如果调用前 maximumSize().height() 已经小于你要设置的 h，那么先调用 setMinimumSize(h)
        // 时，最小高度会大于当前最大高度，这会触发Qt6的断言检查。 因此在调用 setFixedHeight 前，先重置最大尺寸限制：
        if (maximumHeight() < d_ptr->normalGeometry.height()) {
            setMaximumHeight(QWIDGETSIZE_MAX);
        }
        setFixedHeight(d_ptr->normalGeometry.height());
    }
    setMouseTracking(false);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setFrameShape(QFrame::NoFrame);
}

/**
 * \if ENGLISH
 * @brief Check if currently in normal mode
 * @return true if in normal mode, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 检查当前是否处于正常模式
 * @return 如果是正常模式返回 true，否则返回 false
 * \endif
 */
bool SARibbonStackedWidget::isNormalMode() const
{
    return (!isPopupMode());
}

/**
 * \if ENGLISH
 * @brief Run event loop modally in popup mode
 * @details This method is only valid in popup mode:
 * @details 1. Show window
 * @details 2. Start local event loop
 * @details 3. Exit event loop when window is hidden
 * @note Only show window in normal mode
 * @warning Do not call this method again in event loop
 * \endif
 *
 * \if CHINESE
 * @brief 在弹出模式下以模态方式运行事件循环
 * @details 此方法仅在弹出模式下有效：
 * @details 1. 显示窗口
 * @details 2. 启动局部事件循环
 * @details 3. 窗口隐藏时退出事件循环
 * @note 在正常模式下调用此方法仅显示窗口
 * @warning 不要在事件循环中再次调用此方法
 * \endif
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
 * \if ENGLISH
 * @brief Similar to tabbar's moveTab function, swap the index of two widgets
 * @param from Source index
 * @param to Target index
 * @note This operation will trigger widgetRemoved(int index) signal
 * \endif
 *
 * \if CHINESE
 * @brief 类似tabbar的moveTab函数，交换两个窗口的index
 * @param from 源索引
 * @param to 目标索引
 * @note 此操作会触发widgetRemoved(int index)信号
 * \endif
 */
void SARibbonStackedWidget::moveWidget(int from, int to)
{
    QWidget* w = widget(from);

    removeWidget(w);
    insertWidget(to, w);
}

/**
 * \if ENGLISH
 * @brief Set whether to enable popup animation
 * @param on Enable or disable
 * \endif
 *
 * \if CHINESE
 * @brief 设置是否启用弹出动画
 * @param on 是否启用
 * \endif
 */
void SARibbonStackedWidget::setUseAnimation(bool on)
{
    d_ptr->useAnimation = on;
}

/**
 * \if ENGLISH
 * @brief Get animation enabled state
 * @return Animation enabled state
 * \endif
 *
 * \if CHINESE
 * @brief 获取动画启用状态
 * @return 动画启用状态
 * \endif
 */
bool SARibbonStackedWidget::isUseAnimation() const
{
    return d_ptr->useAnimation;
}

/**
 * \if ENGLISH
 * @brief Set animation duration (milliseconds)
 * @param duration Duration in milliseconds
 * \endif
 *
 * \if CHINESE
 * @brief 设置动画持续时间（毫秒）
 * @param duration 持续时间（毫秒）
 * \endif
 */
void SARibbonStackedWidget::setAnimationDuration(int duration)
{
    d_ptr->animation->setDuration(duration);
}

/**
 * \if ENGLISH
 * @brief Get animation duration
 * @return Animation duration in milliseconds
 * \endif
 *
 * \if CHINESE
 * @brief 获取动画持续时间
 * @return 动画持续时间（毫秒）
 * \endif
 */
int SARibbonStackedWidget::animationDuration() const
{
    return d_ptr->animation->duration();
}

/**
 * \if ENGLISH
 * @brief Animation finished slot function
 * \endif
 *
 * \if CHINESE
 * @brief 动画完成槽函数
 * \endif
 */
void SARibbonStackedWidget::onAnimationFinished()
{
    d_ptr->isAnimating = false;
    if (isPopupMode()) {
        // 完成显示后，把其它窗口的尺寸移动到位置
        if (height() != d_ptr->normalGeometry.height()) {
            // 恢复窗口到正常位置
            // 升级到qt6后，setFixedHeight容易出现异常，主要原因是setFixedHeight的实现如下：
            // void QWidget::setFixedHeight(int h) {
            //     // ...
            //     setMinimumSize(minimumSize().width(), h);  // 先设置最小高度
            //     setMaximumSize(maximumSize().width(), h);  // 再设置最大高度
            //     // ...
            // }
            // 如果调用前 maximumSize().height() 已经小于你要设置的 h，那么先调用 setMinimumSize(h) 时，最小高度会大于当前最大高度，这会触发Qt6的断言检查。
            // 因此在调用 setFixedHeight 前，先重置最大尺寸限制：
            if (maximumHeight() < d_ptr->normalGeometry.height()) {
                setMaximumHeight(QWIDGETSIZE_MAX);
            }
            setFixedHeight(d_ptr->normalGeometry.height());
        } else {
            updateInnerWidgetGeometry();
        }
    }
}

/**
 * \if ENGLISH
 * @brief Handle show event
 * @param e Show event
 * \endif
 *
 * \if CHINESE
 * @brief 处理显示事件
 * @param e 显示事件
 * \endif
 */
void SARibbonStackedWidget::showEvent(QShowEvent* e)
{
    if (isPopupMode() && d_ptr->useAnimation && !d_ptr->isAnimationRunning()) {
        // 这个必须放在最前面，否则setFixedHeight(0);就会把子窗口的尺寸改变
        d_ptr->isAnimating = true;
        // 标记为显示动画

        // 设置动画参数
        d_ptr->animation->setStartValue(0);
        d_ptr->animation->setEndValue(d_ptr->normalGeometry.height());

        // 启动动画
        d_ptr->animation->start();
        // 设置起始位置
        // 升级到qt6后，setFixedHeight容易出现异常，主要原因是setFixedHeight的实现如下：
        // void QWidget::setFixedHeight(int h) {
        //     // ...
        //     setMinimumSize(minimumSize().width(), h);  // 先设置最小高度
        //     setMaximumSize(maximumSize().width(), h);  // 再设置最大高度
        //     // ...
        // }
        // 如果调用前 maximumSize().height() 已经小于你要设置的 h，那么先调用 setMinimumSize(h)
        // 时，最小高度会大于当前最大高度，这会触发Qt6的断言检查。 因此在调用 setFixedHeight 前，先重置最大尺寸限制：
        if (maximumHeight() < d_ptr->normalGeometry.height()) {
            setMaximumHeight(QWIDGETSIZE_MAX);
        }
        setFixedHeight(0);
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
        qDebug() << "Starting show animation. Current state:" << d_ptr->animation->state()
                 << "Start value:" << d_ptr->animation->startValue().toInt()
                 << "End value:" << d_ptr->animation->endValue().toInt();
#endif

        // 确保动画已启动
        QCoreApplication::processEvents();
    } else {
        QStackedWidget::showEvent(e);
    }
}

/**
 * \if ENGLISH
 * @brief Handle hide event
 * @param e Hide event
 * \endif
 *
 * \if CHINESE
 * @brief 处理隐藏事件
 * @param e 隐藏事件
 * \endif
 */
void SARibbonStackedWidget::hideEvent(QHideEvent* e)
{
    if (!isPopupMode()) {
        // 非弹出模式，正常的隐藏
        QStackedWidget::hideEvent(e);
        return;
    }

    if (d_ptr->eventLoop) {
        d_ptr->eventLoop->exit();
        d_ptr->eventLoop = nullptr;
    }
    Q_EMIT hidWindow();
    QStackedWidget::hideEvent(e);
}

/**
 * \if ENGLISH
 * @brief Update inner widget geometry
 * \endif
 *
 * \if CHINESE
 * @brief 更新内部窗口几何位置
 * \endif
 */
void SARibbonStackedWidget::updateInnerWidgetGeometry()
{
    // 确保所有子部件都填满整个区域
    const QSize newSize = size();
    for (int i = 0; i < count(); ++i) {
        QWidget* innerWidget = widget(i);
        if (!innerWidget)
            continue;

        if (i == currentIndex()) {
            // 确保当前部件也填满整个区域
            if (innerWidget->size() != newSize) {
                innerWidget->move(0, 0);
                innerWidget->setFixedSize(newSize);
            }
            continue;
        }

        // 设置非活动部件的位置和大小
        innerWidget->move(0, 0);
        innerWidget->setFixedSize(newSize);
        // innerWidget->setGeometry(0, 0, newSize.width(), newSize.height());
        // 通知部件布局可能需要更新
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
        qDebug() << "SARibbonStackedWidget::resizeEvent [not Animation]innerWidget move to:" << innerWidget->geometry();
#endif
        QApplication::postEvent(innerWidget, new QEvent(QEvent::LayoutRequest));
    }
}

/**
 * \if ENGLISH
 * @brief Handle resize event
 * @param e Resize event
 * \endif
 *
 * \if CHINESE
 * @brief 处理尺寸改变事件
 * @param e 尺寸改变事件
 * \endif
 */
void SARibbonStackedWidget::resizeEvent(QResizeEvent* e)
{
    if (d_ptr->isAnimationRunning() && d_ptr->useAnimation) {
        // 正在动画过程中，移动当前窗口的位置
        QWidget* innerWidget = currentWidget();
        if (!innerWidget) {
            return;
        }
        int x = innerWidget->x();
        if (innerWidget->size() != d_ptr->normalGeometry.size()) {
            innerWidget->setFixedSize(d_ptr->normalGeometry.size());
        }
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
        qDebug() << "SARibbonStackedWidget::resizeEvent innerWidget geo:" << innerWidget->geometry();
#endif
        innerWidget->move(x, e->size().height() - d_ptr->normalGeometry.height());

#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
        qDebug() << "SARibbonStackedWidget::resizeEvent innerWidget move to:" << innerWidget->geometry();
#endif

    } else {
        // 没有动画，尺寸的改变记录下来
        d_ptr->normalGeometry = geometry();
        updateInnerWidgetGeometry();
    }
    // 调用基类处理
    QStackedWidget::resizeEvent(e);
}
