#ifndef RIBBONCATEGORY_H
#define RIBBONCATEGORY_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonQuickHost.h"
#include <SARibbonCore/SARibbonCategoryLayoutEngine.h>
#include <SARibbonCore/SARibbonToolButtonLayout.h>
#include <QVariantMap>
#include <QVector>

class QPropertyAnimation;

namespace SARibbonQml {

class RibbonPanel;

/**
 * \if ENGLISH
 * @brief Category structural host: drives the core CategoryLayoutEngine (plan-04 S4)
 * @details Panel arrangement via SARibbonCategoryLayoutEngine. Scrolling is
 * host-animated: the wheel/arrow entry points feed a QPropertyAnimation on
 * `scrollPosition` (duration and OutQuad easing shared with widgets through the
 * core constants) and every animated step re-runs the engine pass, so panel
 * positions stay under C++ geometry authority instead of being interpolated by
 * the leaf (plan-04 WS-A3; supersedes the older "leaf Behavior on x" note).
 * Arrow button rectangles and per-click steps come from the core pure functions
 * `scrollButtonRects` / `scrollButtonStep` and are published to an overlay leaf.
 * \endif
 *
 * \if CHINESE
 * @brief Category 结构宿主：驱动 core 的 CategoryLayoutEngine（计划 04 S4）
 * @details panel 排布经 SARibbonCategoryLayoutEngine。滚动由宿主驱动动画：滚轮与箭头
 *          入口喂给 `scrollPosition` 上的 QPropertyAnimation（时长与 OutQuad 缓动经
 *          core 常量与 widgets 共用），每一动画步都重跑一遍引擎，因此 panel 位置始终
 *          留在 C++ 几何权威手里，而不是交给叶子插值（计划 04 WS-A3；取代早期"叶子
 *          Behavior on x"的注释说法）。箭头按钮矩形与单次点击步长取自 core 纯函数
 *          `scrollButtonRects` / `scrollButtonStep`，发布给覆盖层叶子。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonCategory : public RibbonQuickHost
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int scrollPosition READ scrollPosition WRITE setScrollPosition NOTIFY scrollPositionChanged)
    Q_PROPERTY(QVariantList separatorXs READ separatorXs NOTIFY separatorXsChanged)
    Q_PROPERTY(QVariantMap scrollButtonFlags READ scrollButtonFlags NOTIFY scrollButtonFlagsChanged)
    Q_PROPERTY(QVariantMap scrollButtonGeometry READ scrollButtonGeometry NOTIFY scrollButtonGeometryChanged)
    Q_PROPERTY(bool isAnimatingScroll READ isAnimatingScroll NOTIFY isAnimatingScrollChanged)
    Q_PROPERTY(int wheelScrollStep READ wheelScrollStep WRITE setWheelScrollStep NOTIFY wheelScrollStepChanged)
    Q_PROPERTY(bool useAnimatingScroll READ isUseAnimatingScroll WRITE setUseAnimatingScroll NOTIFY useAnimatingScrollChanged)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)
public:
    explicit RibbonCategory(QQuickItem* parent = nullptr);
    ~RibbonCategory() override;

    QString title() const;
    void setTitle(const QString& t);

    int scrollPosition() const;
    void setScrollPosition(int pos);

    // Panel separator x positions (engine-written resultSeparatorGeometry),
    // re-published per relayout for the visual leaf to render
    QVariantList separatorXs() const;

    // Scroll arrow visibility, published as {"left": bool, "right": bool} from
    // the engine Result::scrollFlags the widgets adapter also consumes
    QVariantMap scrollButtonFlags() const;

    // Scroll arrow rectangles, published as {"leftX","rightX","width","height"}
    // computed by the core pure function scrollButtonRects (RTL already applied)
    QVariantMap scrollButtonGeometry() const;

    // True while the scroll animation is running (wheel events are dropped then,
    // widgets SARibbonCategory::doWheelEvent parity)
    bool isAnimatingScroll() const;

    int wheelScrollStep() const;
    void setWheelScrollStep(int step);

    bool isUseAnimatingScroll() const;
    void setUseAnimatingScroll(bool use);

    int animationDuration() const;
    void setAnimationDuration(int ms);

    // Scroll by a signed delta immediately (widgets SARibbonCategoryLayout::scroll parity)
    Q_INVOKABLE void scrollBy(int delta);

    // Scroll by a signed delta through the animation (scrollByAnimate parity)
    Q_INVOKABLE void scrollByAnimate(int delta);

    // Scroll to an absolute position through the animation (scrollToByAnimate parity)
    Q_INVOKABLE void scrollToByAnimate(int target);

    // One arrow button click: the core step function decides sign and magnitude
    Q_INVOKABLE void scrollByButton(bool isLeftButton);

    void registerPanel(RibbonPanel* panel);
    void unregisterPanel(RibbonPanel* panel);

    // Registered panel queries (WS-C2: the customizer addresses panels through
    // them, widgets SARibbonCategory::panelIndex/panelByObjectName parity)
    int panelCount() const;
    SARibbonQml::RibbonPanel* panelAt(int index) const;
    int panelIndex(RibbonPanel* panel) const;
    SARibbonQml::RibbonPanel* panelByObjectName(const QString& objName) const;

    // Runtime panel creation / removal (WS-C2, widgets insertPanel/removePanel
    // parity). insertPanel builds a host, gives it a visual leaf through the
    // same path createAutoTab uses for C++-created hosts and returns it with
    // the caller owning its lifetime through the QObject parent. removePanel
    // destroys the host and everything declared under it
    Q_INVOKABLE SARibbonQml::RibbonPanel* insertPanel(const QString& title, int index);
    Q_INVOKABLE bool removePanel(RibbonPanel* panel);
    Q_INVOKABLE bool movePanel(int from, int to);

    // Style push from the bar (ribbonStyle propagation chain); panels
    // registered later inherit through the stored fields
    void applyRibbonStyle(int rowCount, bool showPanelTitle, bool wordWrap, bool iconRightText);

    // Layout factor push from the bar (widgets SARibbonCategory::
    // setButtonMaximumAspectRatio parity); panels registered later inherit
    // through the stored fields
    void applyLayoutFactors(qreal buttonMaximumAspectRatio, qreal largeButtonMinimumWidthRatio);

    Q_INVOKABLE int contentWidth() const;

Q_SIGNALS:
    void titleChanged();
    void scrollPositionChanged();
    void separatorXsChanged();
    void scrollButtonFlagsChanged();
    void scrollButtonGeometryChanged();
    void isAnimatingScrollChanged();
    void wheelScrollStepChanged();
    void useAnimatingScrollChanged();
    void animationDurationChanged();

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void updatePolish() override;
    void wheelEvent(QWheelEvent* event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#else
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#endif
    void itemChange(ItemChange change, const ItemChangeData& data) override;

private:
    void relayout();
    // Create the arrow overlay leaf (a second visual item, z above the panels:
    // the background leaf is z=-1 and would be covered by them)
    void ensureScrollOverlay();
    // Re-publish flags + arrow rectangles, emitting only on real change
    void publishScrollState(const SARibbon::Core::SARibbonScrollFlags& flags);

    QString mTitle;
    int mScrollXBase = 0;
    int mTotalWidth = 0;
    int mStyleRowCount = 3;       ///< last pushed bar style rows (default LooseThreeRow)
    bool mStyleShowPanelTitle = true;
    bool mStyleWordWrap = true;
    bool mStyleIconRightText = false;
    qreal mButtonMaximumAspectRatio     = SARibbon::Core::ToolButtonLayoutConstants::BUTTON_MAX_ASPECT_RATIO_DEFAULT;
    qreal mLargeButtonMinimumWidthRatio = SARibbon::Core::ToolButtonLayoutConstants::LARGE_BUTTON_MIN_WIDTH_RATIO;
    int mWheelScrollStep { 400 };              ///< widgets SARibbonCategory default
    bool mUseAnimatingScroll { true };         ///< widgets isUseAnimating default
    int mAnimationDuration { SARibbon::Core::SCROLL_ANIMATION_DURATION };
    int mTargetScrollPosition = 0;
    QVariantMap mScrollFlags;
    QVariantMap mScrollGeometry;
    QPropertyAnimation* mScrollAnimation = nullptr;
    QQuickItem* mScrollOverlay = nullptr;
    QVector< RibbonPanel* > mPanels;
    QVariantList mSeparatorXs;
    SARibbon::Core::SARibbonCategoryLayoutEngine mEngine;
};

}
#endif  // RIBBONCATEGORY_H
