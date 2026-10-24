#include "SARibbonApplicationWidget.h"
#include "SARibbonMainWindow.h"
#include <QEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QHideEvent>
#include <QMouseEvent>
#include <QVariantAnimation>
#include <QPropertyAnimation>
#include <QPainter>
#include <QColor>
#include "SARibbonMainWindow.h"

namespace SA {
/**
 * \if ENGLISH
 * @brief Internal translucent mask widget that covers the rest of the main window area
 * @details Left-clicking the mask hides the owner SARibbonApplicationWidget
 * \endif
 *
 * \if CHINESE
 * @brief 覆盖主窗口剩余区域的内部半透明遮罩
 * @details 鼠标左键点击遮罩将隐藏所属的 SARibbonApplicationWidget
 * \endif
 */
class SARibbonApplicationWidgetMask : public QWidget
{
public:
    // Constructor with the owner application widget and the parent widget (the main window)
    explicit SARibbonApplicationWidgetMask(SARibbonApplicationWidget* owner, QWidget* parent) : QWidget(parent), mOwner(owner)
    {
        setCursor(Qt::ArrowCursor);
    }
    // Set the mask color (alpha included)
    void setMaskColor(const QColor& c)
    {
        mMaskColor = c;
        update();
    }
    // Set the mask opacity (0.0~1.0), used by the fade-in animation; 1.0 is the full effect
    void setMaskOpacity(qreal op)
    {
        mMaskOpacity = op;
        update();
    }

protected:
    // Paints the translucent mask color
    void paintEvent(QPaintEvent* e) override
    {
        Q_UNUSED(e);
        QPainter p(this);
        QColor c = mMaskColor;
        c.setAlpha(int(c.alpha() * mMaskOpacity));
        p.fillRect(rect(), c);
    }
    // Left-click hides the owner application widget
    void mousePressEvent(QMouseEvent* e) override
    {
        if (e && (Qt::LeftButton == e->button())) {
            mOwner->hide();
            e->accept();
            return;
        }
        QWidget::mousePressEvent(e);
    }

private:
    SARibbonApplicationWidget* mOwner;
    QColor mMaskColor { 0, 0, 0, 120 };
    qreal mMaskOpacity { 1.0 };
};
}  // namespace SA

/**
 * \if ENGLISH
 * @brief Private data of SARibbonApplicationWidget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonApplicationWidget 的私有数据
 * \endif
 */
class SARibbonApplicationWidget::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonApplicationWidget)
public:
    PrivateData(SARibbonApplicationWidget* p);
    void ensureMaskWidget();
    void showMask(bool fadein);
    void hideMask();
    void syncMaskGeometry();

public:
    qreal mWidthRatio { 1.0 };               ///< width ratio against the parent window, range (0.0, 1.0]
    bool mMaskEnabled { false };             ///< whether the translucent mask is enabled
    QColor mMaskColor { 0, 0, 0, 120 };      ///< mask color with alpha
    bool mAnimationEnabled { true };         ///< whether the slide-in animation is enabled
    int mAnimationDuration { 250 };          ///< animation duration in milliseconds
    SA::SARibbonApplicationWidgetMask* mMaskWidget { nullptr };
    QPropertyAnimation* mSlideAnimation { nullptr };    ///< slides the widget in from the left edge
    QVariantAnimation* mMaskFadeAnimation { nullptr };  ///< fades the mask in
};

SARibbonApplicationWidget::PrivateData::PrivateData(SARibbonApplicationWidget* p) : q_ptr(p)
{
    mSlideAnimation = new QPropertyAnimation(p, "pos", p);
    mSlideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    mMaskFadeAnimation = new QVariantAnimation(p);
    mMaskFadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    QObject::connect(mMaskFadeAnimation, &QVariantAnimation::valueChanged, p, [ this ](const QVariant& v) {
        if (mMaskWidget) {
            mMaskWidget->setMaskOpacity(v.toReal());
        }
    });
}

/**
 * \if ENGLISH
 * @brief Lazily creates the mask widget as a child of the parent window
 * \endif
 *
 * \if CHINESE
 * @brief 惰性创建遮罩，遮罩的父对象为父窗口（主窗口）
 * \endif
 */
void SARibbonApplicationWidget::PrivateData::ensureMaskWidget()
{
    if (mMaskWidget) {
        return;
    }
    QWidget* par = q_ptr->parentWidget();
    if (nullptr == par) {
        return;
    }
    mMaskWidget = new SA::SARibbonApplicationWidgetMask(q_ptr, par);
    mMaskWidget->setMaskColor(mMaskColor);
    mMaskWidget->hide();
}

/**
 * \if ENGLISH
 * @brief Shows the mask below the application widget, optionally with a fade-in
 * @param fadein Whether to fade the mask in
 * \endif
 *
 * \if CHINESE
 * @brief 显示遮罩（位于应用窗口之下），可选淡入效果
 * @param fadein 是否淡入
 * \endif
 */
void SARibbonApplicationWidget::PrivateData::showMask(bool fadein)
{
    ensureMaskWidget();
    if (nullptr == mMaskWidget) {
        return;
    }
    syncMaskGeometry();
    if (fadein) {
        mMaskWidget->setMaskOpacity(0.0);
        mMaskWidget->show();
        mMaskWidget->raise();
        mMaskFadeAnimation->stop();
        mMaskFadeAnimation->setDuration(mAnimationDuration);
        mMaskFadeAnimation->setStartValue(0.0);
        mMaskFadeAnimation->setEndValue(1.0);
        mMaskFadeAnimation->start();
    } else {
        mMaskWidget->setMaskOpacity(1.0);
        mMaskWidget->show();
        mMaskWidget->raise();
    }
}

/**
 * \if ENGLISH
 * @brief Hides the mask if it exists
 * \endif
 *
 * \if CHINESE
 * @brief 隐藏遮罩（若已创建）
 * \endif
 */
void SARibbonApplicationWidget::PrivateData::hideMask()
{
    if (mMaskWidget) {
        mMaskWidget->hide();
    }
}

/**
 * \if ENGLISH
 * @brief Syncs the mask geometry to cover the whole parent window
 * \endif
 *
 * \if CHINESE
 * @brief 同步遮罩几何，使其覆盖整个父窗口
 * \endif
 */
void SARibbonApplicationWidget::PrivateData::syncMaskGeometry()
{
    if (mMaskWidget) {
        if (QWidget* par = q_ptr->parentWidget()) {
            mMaskWidget->setGeometry(0, 0, par->width(), par->height());
        }
    }
}

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonApplicationWidget
 * @param parent The parent SARibbonMainWindow
 * @details Creates a frameless widget that follows the parent window's size
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonApplicationWidget 构造函数
 * @param parent 父窗口 SARibbonMainWindow
 * @details 创建一个无框窗口，跟随父窗口大小变化
 * \endif
 */
SARibbonApplicationWidget::SARibbonApplicationWidget(SARibbonMainWindow* parent) : QFrame(parent), SA_RIBBON_IMPL_CONSTRUCT
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);  // 去除边框
    if (parent) {
        parent->installEventFilter(this);
        setGeometry(0, 0, parent->width(), parent->height());
    } else {
        setGeometry(0, 0, 300, 300);
    }
}

SARibbonApplicationWidget::~SARibbonApplicationWidget()
{
}

/**
 * \if ENGLISH
 * @brief Set the width ratio of the widget against its parent window
 * @param ratio Valid range is (0.0, 1.0]; 1.0 (default) means covering the whole parent window. Values outside the range fall back to 1.0
 * @details The widget is always anchored to the top-left corner of the parent window
 * \endif
 *
 * \if CHINESE
 * @brief 设置窗口宽度相对父窗口的比例
 * @param ratio 有效范围为 (0.0, 1.0]；1.0（默认）表示完全覆盖父窗口，范围外的值回退为 1.0
 * @details 窗口始终贴住父窗口左上角
 * \endif
 */
void SARibbonApplicationWidget::setWidthRatio(qreal ratio)
{
    SA_D(d);
    if (ratio <= 0.0 || ratio > 1.0) {
        ratio = 1.0;
    }
    if (d->mWidthRatio == ratio) {
        return;
    }
    d->mWidthRatio = ratio;
    if (QWidget* par = parentWidget()) {
        resizeToParent(par->size());
    }
}

/**
 * \if ENGLISH
 * @brief Get the width ratio
 * @return The width ratio
 * \endif
 *
 * \if CHINESE
 * @brief 获取宽度比例
 * @return 宽度比例
 * \endif
 */
qreal SARibbonApplicationWidget::widthRatio() const
{
    SA_DC(d);
    return d->mWidthRatio;
}

/**
 * \if ENGLISH
 * @brief Enable/disable the translucent mask covering the rest of the parent window
 * @param on True to enable the mask; clicking the mask hides this widget. Default off
 * \endif
 *
 * \if CHINESE
 * @brief 启用/禁用覆盖父窗口剩余区域的半透明遮罩
 * @param on true 启用遮罩，点击遮罩将隐藏本窗口；默认关闭
 * \endif
 */
void SARibbonApplicationWidget::setMaskWidgetEnabled(bool on)
{
    SA_D(d);
    if (d->mMaskEnabled == on) {
        return;
    }
    d->mMaskEnabled = on;
    if (on) {
        if (isVisible()) {
            d->showMask(false);
            raise();
        }
    } else {
        d->hideMask();
    }
}

/**
 * \if ENGLISH
 * @brief Check whether the mask is enabled
 * @return True if the mask is enabled
 * \endif
 *
 * \if CHINESE
 * @brief 判断遮罩是否启用
 * @return 启用返回 true
 * \endif
 */
bool SARibbonApplicationWidget::isMaskWidgetEnabled() const
{
    SA_DC(d);
    return d->mMaskEnabled;
}

/**
 * \if ENGLISH
 * @brief Set the mask color
 * @param color The mask color with alpha, the default is translucent black (0, 0, 0, 120)
 * \endif
 *
 * \if CHINESE
 * @brief 设置遮罩颜色
 * @param color 带透明度的颜色，默认为半透明黑 (0, 0, 0, 120)
 * \endif
 */
void SARibbonApplicationWidget::setMaskColor(const QColor& color)
{
    SA_D(d);
    d->mMaskColor = color;
    if (d->mMaskWidget) {
        d->mMaskWidget->setMaskColor(color);
    }
}

/**
 * \if ENGLISH
 * @brief Get the mask color
 * @return The mask color
 * \endif
 *
 * \if CHINESE
 * @brief 获取遮罩颜色
 * @return 遮罩颜色
 * \endif
 */
QColor SARibbonApplicationWidget::maskColor() const
{
    SA_DC(d);
    return d->mMaskColor;
}

/**
 * \if ENGLISH
 * @brief Enable/disable the slide-in animation when showing
 * @param on True to slide in from the left edge with the mask fading in. Default on
 * \endif
 *
 * \if CHINESE
 * @brief 启用/禁用显示时的滑入动画
 * @param on true 表示从左侧滑入且遮罩淡入；默认开启
 * \endif
 */
void SARibbonApplicationWidget::setAnimationEnabled(bool on)
{
    SA_D(d);
    d->mAnimationEnabled = on;
}

/**
 * \if ENGLISH
 * @brief Check whether the animation is enabled
 * @return True if the animation is enabled
 * \endif
 *
 * \if CHINESE
 * @brief 判断动画是否启用
 * @return 启用返回 true
 * \endif
 */
bool SARibbonApplicationWidget::isAnimationEnabled() const
{
    SA_DC(d);
    return d->mAnimationEnabled;
}

/**
 * \if ENGLISH
 * @brief Set the animation duration
 * @param ms Duration in milliseconds, default is 250
 * \endif
 *
 * \if CHINESE
 * @brief 设置动画时长
 * @param ms 毫秒数，默认 250
 * \endif
 */
void SARibbonApplicationWidget::setAnimationDuration(int ms)
{
    SA_D(d);
    d->mAnimationDuration = ms;
}

/**
 * \if ENGLISH
 * @brief Get the animation duration
 * @return Duration in milliseconds
 * \endif
 *
 * \if CHINESE
 * @brief 获取动画时长
 * @return 毫秒数
 * \endif
 */
int SARibbonApplicationWidget::animationDuration() const
{
    SA_DC(d);
    return d->mAnimationDuration;
}

/**
 * \if ENGLISH
 * @brief Resizes the widget to match the parent's size multiplied by widthRatio
 * @param parentSize The size of the parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 将窗口大小调整为父窗口大小乘以宽度比例
 * @param parentSize 父窗口的大小
 * \endif
 */
void SARibbonApplicationWidget::resizeToParent(const QSize& parentSize)
{
    SA_D(d);
    qreal ratio = d->mWidthRatio;
    if (ratio <= 0.0 || ratio > 1.0) {
        ratio = 1.0;
    }
    setGeometry(0, 0, int(parentSize.width() * ratio), parentSize.height());
    d->syncMaskGeometry();
}

/**
 * \if ENGLISH
 * @brief Event filter to handle parent window events
 * @param obj The object that received the event
 * @param ev The event
 * @return True if the event was handled, false otherwise
 * @details Handles resize events to keep the widget in sync with parent size
 * \endif
 *
 * \if CHINESE
 * @brief 事件过滤器，处理父窗口事件
 * @param obj 接收事件的对象
 * @param ev 事件
 * @return 如果事件被处理返回true，否则返回false
 * @details 处理 resize 事件，保持窗口与父窗口大小同步
 * \endif
 */
bool SARibbonApplicationWidget::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj && ev && (obj == parent())) {
        switch (ev->type()) {
        case QEvent::Resize: {
            // 跟随父窗口的尺寸变化
            resizeToParent(static_cast< QResizeEvent* >(ev)->size());
            break;
        }
        default:
            break;
        }
    }
    return QFrame::eventFilter(obj, ev);
}

/**
 * \if ENGLISH
 * @brief Handles the show event
 * @param event The show event
 * @details Resizes the widget to match parent size, shows the mask below the widget, requests focus
 *          and optionally starts the slide-in animation from the left edge
 * \endif
 *
 * \if CHINESE
 * @brief 处理显示事件
 * @param event 显示事件
 * @details 将窗口大小调整为父窗口大小，显示遮罩（位于本窗口之下），请求焦点，
 *          并可选启动从左侧滑入的动画
 * \endif
 */
void SARibbonApplicationWidget::showEvent(QShowEvent* event)
{
    SA_D(d);
    QWidget* par = parentWidget();
    if (par) {
        resizeToParent(par->size());
    }
    // 遮罩显示并置于本窗口之下
    if (d->mMaskEnabled && par) {
        d->showMask(d->mAnimationEnabled);
        d->mMaskWidget->raise();
    }
    setFocus();  // 显示窗口时请求焦点
    raise();     // 确保本窗口位于遮罩之上
    // 从左侧滑入
    if (d->mAnimationEnabled) {
        d->mSlideAnimation->stop();
        d->mSlideAnimation->setDuration(d->mAnimationDuration);
        d->mSlideAnimation->setStartValue(QPoint(-width(), y()));
        d->mSlideAnimation->setEndValue(QPoint(0, y()));
        move(-width(), y());
        d->mSlideAnimation->start();
    }
    QFrame::showEvent(event);
}

/**
 * \if ENGLISH
 * @brief Handles the hide event
 * @param event The hide event
 * @details Stops the running animations and hides the mask
 * \endif
 *
 * \if CHINESE
 * @brief 处理隐藏事件
 * @param event 隐藏事件
 * @details 停止进行中的动画并隐藏遮罩
 * \endif
 */
void SARibbonApplicationWidget::hideEvent(QHideEvent* event)
{
    SA_D(d);
    d->mSlideAnimation->stop();
    d->mMaskFadeAnimation->stop();
    d->hideMask();
    QFrame::hideEvent(event);
}

/**
 * \if ENGLISH
 * @brief Handles key press events
 * @param ev The key event
 * @details Hides the widget when Escape key is pressed
 * \endif
 *
 * \if CHINESE
 * @brief 处理按键事件
 * @param ev 按键事件
 * @details 当按下 Escape 键时隐藏窗口
 * \endif
 */
void SARibbonApplicationWidget::keyPressEvent(QKeyEvent* ev)
{
    if (ev) {
        if (ev->key() == Qt::Key_Escape) {
            hide();
            ev->accept();
            return;
        }
    }
    QFrame::keyPressEvent(ev);
}
