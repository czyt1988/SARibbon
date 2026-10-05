#include "SARibbonQmlApplicationWindow.h"

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Constructor
 * @param parent parent item (the RibbonBar it is declared in)
 * @details Creates a hidden structural container; the content only renders
 *          through the presentation layer the bar leaf builds on demand.
 * \endif
 *
 * \if CHINESE
 * @brief 构造函数
 * @param parent 父项（声明所在的 RibbonBar）
 * @details 创建一个隐藏的结构容器；内容只经 bar 叶子按需构建的展示层渲染。
 * \endif
 */
RibbonApplicationWindow::RibbonApplicationWindow(QQuickItem* parent) : QQuickItem(parent)
{
    setAcceptedMouseButtons(Qt::NoButton);  // structural container
    setVisible(false);                      // shows only through the app button
}

/**
 * \if ENGLISH
 * @brief Destructor
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * \endif
 */
RibbonApplicationWindow::~RibbonApplicationWindow()
{
}

/**
 * \if ENGLISH
 * @brief Whether the window is currently shown through the application button
 * @return true while the presentation layer is open
 * \endif
 *
 * \if CHINESE
 * @brief 窗口当前是否经应用按钮展示
 * @return 展示层打开期间为 true
 * \endif
 */
bool RibbonApplicationWindow::isPopupVisible() const
{
    return mPopupVisible;
}

/**
 * \if ENGLISH
 * @brief Sets the shown state and follows it on the item visibility
 * @param on the new shown state
 * @details The leaf drives this at the presentation layer's aboutToShow /
 *          closed boundaries; the flag therefore also covers the exit
 *          animation span, and the item stays hidden while parked in the bar.
 * \endif
 *
 * \if CHINESE
 * @brief 设置展示状态并同步条目可见性
 * @param on 新的展示状态
 * @details 叶子在展示层的 aboutToShow / closed 边界驱动此属性；标志因此
 *          覆盖退出动画全程，条目在 bar 内停靠期间保持隐藏。
 * \endif
 */
void RibbonApplicationWindow::setPopupVisible(bool on)
{
    if (mPopupVisible == on) {
        return;
    }
    mPopupVisible = on;
    setVisible(on);
    Q_EMIT popupVisibleChanged();
}

/**
 * \if ENGLISH
 * @brief Gets the covered fraction of the window width
 * @return ratio in [0.05, 1.0]; 1.0 covers the whole window
 * \endif
 *
 * \if CHINESE
 * @brief 读取窗口宽度的覆盖比例
 * @return [0.05, 1.0] 区间的比例；1.0 覆盖整个窗口
 * \endif
 */
qreal RibbonApplicationWindow::coverageRatio() const
{
    return mCoverageRatio;
}

/**
 * \if ENGLISH
 * @brief Sets the covered fraction of the window width
 * @param ratio requested ratio; NaN is ignored, other values clamp into [0.05, 1.0]
 * @details The presentation layer sizes itself from the window metrics times
 *          this ratio, so fullscreen, two-thirds and any custom coverage are
 *          the same knob.
 * \endif
 *
 * \if CHINESE
 * @brief 设置窗口宽度的覆盖比例
 * @param ratio 请求的比例；NaN 被忽略，其余值夹取到 [0.05, 1.0]
 * @details 展示层按窗口度量乘以该比例定尺寸，全屏、2/3 与任意自定义
 *          覆盖共用这一个旋钮。
 * \endif
 */
void RibbonApplicationWindow::setCoverageRatio(qreal ratio)
{
    if (qIsNaN(ratio)) {
        return;
    }
    const qreal bounded = qBound(0.05, ratio, 1.0);
    if (qFuzzyCompare(mCoverageRatio, bounded)) {
        return;
    }
    mCoverageRatio = bounded;
    Q_EMIT coverageRatioChanged();
}

/**
 * \if ENGLISH
 * @brief Whether the presentation layer's top-right close cross is offered
 * @return true when the cross should render
 * \endif
 *
 * \if CHINESE
 * @brief 展示层是否提供右上角关闭叉按钮
 * @return 应渲染叉按钮时为 true
 * \endif
 */
bool RibbonApplicationWindow::isShowCloseButton() const
{
    return mShowCloseButton;
}

/**
 * \if ENGLISH
 * @brief Enables or disables the presentation layer's close cross
 * @param on whether the cross should render
 * @details The cross is the mandated exit for fullscreen coverage (no outside
 *          area exists to click); it can be dropped for partial coverage.
 * \endif
 *
 * \if CHINESE
 * @brief 开关展示层的关闭叉按钮
 * @param on 是否渲染叉按钮
 * @details 叉按钮是全屏覆盖的必备退出途径（不存在可点击的外部区域）；
 *          部分覆盖时可以关闭它。
 * \endif
 */
void RibbonApplicationWindow::setShowCloseButton(bool on)
{
    if (mShowCloseButton == on) {
        return;
    }
    mShowCloseButton = on;
    Q_EMIT showCloseButtonChanged();
}

/**
 * \if ENGLISH
 * @brief Gets the enter/exit animation effect
 * @return the configured AnimationEffect
 * \endif
 *
 * \if CHINESE
 * @brief 读取进出动画效果
 * @return 已配置的 AnimationEffect
 * \endif
 */
RibbonApplicationWindow::AnimationEffect RibbonApplicationWindow::animation() const
{
    return mAnimation;
}

/**
 * \if ENGLISH
 * @brief Sets the enter/exit animation effect
 * @param effect the new AnimationEffect
 * \endif
 *
 * \if CHINESE
 * @brief 设置进出动画效果
 * @param effect 新的 AnimationEffect
 * \endif
 */
void RibbonApplicationWindow::setAnimation(AnimationEffect effect)
{
    if (mAnimation == effect) {
        return;
    }
    mAnimation = effect;
    Q_EMIT animationChanged();
}

/**
 * \if ENGLISH
 * @brief Gets the animation duration
 * @return duration in milliseconds
 * \endif
 *
 * \if CHINESE
 * @brief 读取动画时长
 * @return 毫秒数
 * \endif
 */
int RibbonApplicationWindow::animationDuration() const
{
    return mAnimationDuration;
}

/**
 * \if ENGLISH
 * @brief Sets the animation duration
 * @param ms duration in milliseconds; negative values clamp to 0
 * \endif
 *
 * \if CHINESE
 * @brief 设置动画时长
 * @param ms 毫秒数；负值夹取到 0
 * \endif
 */
void RibbonApplicationWindow::setAnimationDuration(int ms)
{
    const int bounded = qMax(ms, 0);
    if (mAnimationDuration == bounded) {
        return;
    }
    mAnimationDuration = bounded;
    Q_EMIT animationDurationChanged();
}

/**
 * \if ENGLISH
 * @brief Requests a close from inner content
 * @details Emits closeRequested, which the bar routes back into the
 *          presentation layer (the leaf's closeApplicationWindow chain).
 * \endif
 *
 * \if CHINESE
 * @brief 由内部内容发起关闭请求
 * @details 发射 closeRequested，bar 将其路由回展示层（叶子的
 *          closeApplicationWindow 链）。
 * \endif
 */
void RibbonApplicationWindow::close()
{
    Q_EMIT closeRequested();
}

}
