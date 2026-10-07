#include "SARibbonQmlCategory.h"
#include "SARibbonQmlPanel.h"
#include "SARibbonQmlToolButton.h"
#include "SARibbonQmlBar.h"
#include "SARibbonQmlTheme.h"
#include "SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QPropertyAnimation>
#include <QVariantList>
#include <QWheelEvent>

namespace SARibbonQml {

namespace {
// adapter item bridging a RibbonPanel into the category contract (the panel is a
// QQuickItem, not a QWidgetItem; its layout item face is this thin wrapper)
class CategoryItemAdapter : public SARibbon::Core::SARibbonAbstractCategoryItem
{
public:
    explicit CategoryItemAdapter(RibbonPanel* panel) : mPanel(panel) {}
    QSize sizeHint() const override;
    bool isHidden() const override { return !mPanel || !mPanel->isVisible(); }
    Qt::Orientations expandingDirections() const override { return Qt::Orientations(); }
    void applyGeometry(const QRect& rect) override;

    RibbonPanel* mPanel;
};
}  // namespace

RibbonCategory::RibbonCategory(QQuickItem* parent) : RibbonQuickHost(parent)
{
    // RTL flip re-runs the engine pass (SA::saIsRTL() re-read on polish)
    connect(RibbonTheme::instance(), &RibbonTheme::rtlChanged, this, [this]() { polish(); });
    // A scrolled category offsets panels outside its own rectangle; widgets clips
    // them by being a native widget, QML needs the explicit clip (otherwise panels
    // scrolled past an edge paint over whatever sits next to the category)
    setClip(true);
    // Publish a complete scroll state from the start: the arrow overlay leaf is
    // created before the first engine pass, and its bindings read these maps'
    // keys directly. An empty map is still a truthy object in QML, so "flags ?
    // flags.left : false" would assign undefined and warn once per category.
    mScrollFlags.insert(QStringLiteral("left"), false);
    mScrollFlags.insert(QStringLiteral("right"), false);
    mScrollGeometry.insert(QStringLiteral("leftX"), 0);
    mScrollGeometry.insert(QStringLiteral("rightX"), 0);
    mScrollGeometry.insert(QStringLiteral("width"), 0);
    mScrollGeometry.insert(QStringLiteral("height"), 0);
}

RibbonCategory::~RibbonCategory()
{
    // same teardown shape as the base class leaf (NOTES B44): detach the scene
    // graph only, the QObject child cleanup deletes the item while the engine
    // is still alive
    if (mScrollOverlay) {
        mScrollOverlay->setParentItem(nullptr);
        mScrollOverlay = nullptr;
    }
}

QUrl RibbonCategory::leafUrl() const
{
    return SARibbonQmlLeafUrls::categoryLeaf();
}

void RibbonCategory::componentComplete()
{
    RibbonQuickHost::componentComplete();
    ensureQmlLeaf();
    ensureScrollOverlay();
}

/**
 * \if ENGLISH
 * @brief Create the arrow overlay leaf above the panels
 * @details The background leaf is z=-1 and the panels are z=0 siblings, so arrows
 * drawn inside it would be covered exactly when they are needed. The overlay is a
 * second visual item created from its own qrc file and raised to z=1; it declares
 * `property QtObject cppHost` but performs no handshake, so the host's `qmlLeaf`
 * keeps pointing at the background leaf.
 * \endif
 *
 * \if CHINESE
 * @brief 创建位于 panel 之上的箭头覆盖层叶子
 * @details 背景叶子 z=-1，panel 是 z=0 的兄弟，箭头画在背景叶子里恰好在需要时被 panel
 * 盖住。覆盖层是从独立 qrc 文件创建的第二个视觉项并抬到 z=1；它声明 `property QtObject
 * cppHost` 但不做握手回写，因此宿主的 `qmlLeaf` 仍指向背景叶子。
 * \endif
 */
void RibbonCategory::ensureScrollOverlay()
{
    if (mScrollOverlay) {
        return;
    }
    QQuickItem* overlay = createVisualLeaf(this, SARibbonQmlLeafUrls::categoryScrollLeaf());
    if (overlay) {
        overlay->setZ(1);
        mScrollOverlay = overlay;
    }
}

/**
 * \if ENGLISH
 * @brief Re-publish the arrow flags and rectangles, emitting only on real change
 * @details The rectangles come from the core pure function that widgets also
 * calls, so both front ends place 12px full-height arrows and swap them under RTL.
 * \endif
 *
 * \if CHINESE
 * @brief 重新发布箭头可见标志与矩形，仅在真正变化时发信号
 * @details 矩形来自 widgets 也在调用的 core 纯函数，因此两个前端都摆放 12px 满高箭头，
 * 并在 RTL 下互换左右。
 * \endif
 */
void RibbonCategory::publishScrollState(const SARibbon::Core::SARibbonScrollFlags& flags)
{
    QVariantMap newFlags;
    newFlags.insert(QStringLiteral("left"), flags.showLeft);
    newFlags.insert(QStringLiteral("right"), flags.showRight);
    if (newFlags != mScrollFlags) {
        mScrollFlags = newFlags;
        Q_EMIT scrollButtonFlagsChanged();
    }

    const SARibbon::Core::SARibbonScrollButtonRects rects =
        SARibbon::Core::scrollButtonRects(int(width()), int(height()), SA::saIsRTL());
    QVariantMap geometry;
    geometry.insert(QStringLiteral("leftX"), rects.left.x());
    geometry.insert(QStringLiteral("rightX"), rects.right.x());
    geometry.insert(QStringLiteral("width"), rects.left.width());
    geometry.insert(QStringLiteral("height"), rects.left.height());
    if (geometry != mScrollGeometry) {
        mScrollGeometry = geometry;
        Q_EMIT scrollButtonGeometryChanged();
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonCategory::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChange(newGeometry, oldGeometry);
#else
void RibbonCategory::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.size() != oldGeometry.size()) {
        polish();  // the bar host sizes this item: relayout panels on every resize
    }
}

QString RibbonCategory::title() const
{
    return mTitle;
}

void RibbonCategory::setTitle(const QString& t)
{
    if (mTitle == t) {
        return;
    }
    mTitle = t;
    Q_EMIT titleChanged();
    polish();
}

int RibbonCategory::scrollPosition() const
{
    return mScrollXBase;
}

void RibbonCategory::setScrollPosition(int pos)
{
    // engine-clamped target: the logical position always goes through
    // clampScrollOffset, whether it is set by a wheel step, an arrow click or
    // an intermediate value of the scroll animation
    const int clamped = SARibbon::Core::clampScrollOffset(pos, mTotalWidth, int(width()), SA::saIsRTL());
    if (mScrollXBase == clamped) {
        return;
    }
    mScrollXBase = clamped;
    Q_EMIT scrollPositionChanged();
    polish();
}

/**
 * \if ENGLISH
 * @brief Scroll by a signed delta without animation (widgets scroll parity)
 * \endif
 *
 * \if CHINESE
 * @brief 无动画地滚动一个带符号增量（对齐 widgets SARibbonCategoryLayout::scroll）
 * \endif
 */
void RibbonCategory::scrollBy(int delta)
{
    if (mScrollAnimation && mScrollAnimation->state() == QAbstractAnimation::Running) {
        mScrollAnimation->stop();
    }
    setScrollPosition(mScrollXBase + delta);
}

/**
 * \if ENGLISH
 * @brief Scroll by a signed delta through the animation (widgets scrollByAnimate parity)
 * \endif
 *
 * \if CHINESE
 * @brief 经动画滚动一个带符号增量（对齐 widgets scrollByAnimate）
 * \endif
 */
void RibbonCategory::scrollByAnimate(int delta)
{
    scrollToByAnimate(mScrollXBase + delta);
}

/**
 * \if ENGLISH
 * @brief Scroll to an absolute position through the animation
 * @details Same shape as widgets SARibbonCategoryLayout::scrollToByAnimate: the
 * target is clamped by the core function, a running animation is restarted from
 * the current position, duration and OutQuad easing come from the core constant.
 * Two guards are added on purpose: with the animation switched off (or a
 * non-positive duration) the position is set directly, and a target equal to the
 * current position does not start an empty animation — an empty run would keep
 * isAnimatingScroll() true for 300ms and swallow wheel events.
 * \endif
 *
 * \if CHINESE
 * @brief 经动画滚动到绝对位置
 * @details 与 widgets SARibbonCategoryLayout::scrollToByAnimate 同形：目标值经 core
 * 钳制，运行中的动画从当前位置重启，时长与 OutQuad 缓动取自 core 常量。此处刻意多两道
 * 守卫：关闭动画（或时长非正）时直接设值；目标等于当前位置时不启动空动画——空跑会让
 * isAnimatingScroll() 保持 true 达 300ms 并吞掉滚轮事件。
 * \endif
 */
void RibbonCategory::scrollToByAnimate(int target)
{
    const int clamped = SARibbon::Core::clampScrollOffset(target, mTotalWidth, int(width()), SA::saIsRTL());
    if (!mUseAnimatingScroll || mAnimationDuration <= 0 || clamped == mScrollXBase) {
        scrollBy(clamped - mScrollXBase);
        return;
    }
    if (!mScrollAnimation) {
        mScrollAnimation = new QPropertyAnimation(this, "scrollPosition", this);
        mScrollAnimation->setEasingCurve(QEasingCurve::OutQuad);
        connect(mScrollAnimation, &QPropertyAnimation::stateChanged, this, [this]() {
            Q_EMIT isAnimatingScrollChanged();
        });
    }
    mScrollAnimation->setDuration(mAnimationDuration);
    if (isAnimatingScroll() && clamped == mTargetScrollPosition) {
        return;  // already heading there
    }
    mTargetScrollPosition = clamped;
    if (mScrollAnimation->state() == QAbstractAnimation::Running) {
        mScrollAnimation->stop();
    }
    mScrollAnimation->setStartValue(mScrollXBase);
    mScrollAnimation->setEndValue(mTargetScrollPosition);
    mScrollAnimation->start();
}

/**
 * \if ENGLISH
 * @brief One arrow button click: sign and magnitude come from the core step function
 * @details widgets always animates arrow clicks regardless of isUseAnimating. Here
 * the animated route is taken as well, except when the host was explicitly told
 * not to animate — useAnimatingScroll then wins over the entry point, so the
 * property means the same thing for the wheel and for the arrows.
 * \endif
 *
 * \if CHINESE
 * @brief 一次箭头按钮点击：符号与步长由 core 纯函数给出
 * @details widgets 的箭头点击恒定走动画（不受 isUseAnimating 影响）。此处同样默认走动画，
 * 唯一例外是宿主被显式关闭动画——此时 useAnimatingScroll 优先于入口，使该属性对滚轮和
 * 箭头表达同一件事。
 * \endif
 */
void RibbonCategory::scrollByButton(bool isLeftButton)
{
    const int step   = SARibbon::Core::scrollButtonStep(int(width()), isLeftButton, SA::saIsRTL());
    const int target = SARibbon::Core::clampScrollOffset(mScrollXBase + step, mTotalWidth, int(width()), SA::saIsRTL());
    scrollToByAnimate(target);
}

/**
 * \if ENGLISH
 * @brief Wheel scrolling (widgets SARibbonCategory::doWheelEvent parity)
 * @details Delta selection and step scaling go through the core pure functions;
 * events arriving while the animation runs are ignored, exactly as widgets does.
 * When the content fits, the event is passed on and a leftover offset is reset.
 * \endif
 *
 * \if CHINESE
 * @brief 滚轮滚动（对齐 widgets SARibbonCategory::doWheelEvent）
 * @details 增量选取与步长缩放走 core 纯函数；动画进行中到达的事件被忽略，与 widgets 一致。
 * 内容能完整显示时事件继续上抛，并把残余偏移复位。
 * \endif
 */
void RibbonCategory::wheelEvent(QWheelEvent* event)
{
    if (mUseAnimatingScroll && isAnimatingScroll()) {
        event->ignore();
        return;
    }
    if (mTotalWidth > int(width())) {
        const int delta = SARibbon::Core::wheelScrollDelta(event->pixelDelta(), event->angleDelta());
        const int step  = SARibbon::Core::scaledWheelStep(mWheelScrollStep, delta);
        if (mUseAnimatingScroll) {
            scrollByAnimate(step);
        } else {
            scrollBy(step);
        }
        event->accept();
        return;
    }
    event->ignore();
    if (0 != mScrollXBase) {
        if (mUseAnimatingScroll) {
            scrollToByAnimate(0);
        } else {
            setScrollPosition(0);
        }
    }
}

int RibbonCategory::wheelScrollStep() const
{
    return mWheelScrollStep;
}

void RibbonCategory::setWheelScrollStep(int step)
{
    if (mWheelScrollStep == step) {
        return;
    }
    mWheelScrollStep = step;
    Q_EMIT wheelScrollStepChanged();
}

bool RibbonCategory::isUseAnimatingScroll() const
{
    return mUseAnimatingScroll;
}

void RibbonCategory::setUseAnimatingScroll(bool use)
{
    if (mUseAnimatingScroll == use) {
        return;
    }
    mUseAnimatingScroll = use;
    Q_EMIT useAnimatingScrollChanged();
}

int RibbonCategory::animationDuration() const
{
    return mAnimationDuration;
}

void RibbonCategory::setAnimationDuration(int ms)
{
    if (mAnimationDuration == ms) {
        return;
    }
    mAnimationDuration = ms;
    if (mScrollAnimation) {
        mScrollAnimation->setDuration(ms);
    }
    Q_EMIT animationDurationChanged();
}

bool RibbonCategory::isAnimatingScroll() const
{
    return mScrollAnimation && mScrollAnimation->state() == QAbstractAnimation::Running;
}

QVariantMap RibbonCategory::scrollButtonFlags() const
{
    return mScrollFlags;
}

QVariantMap RibbonCategory::scrollButtonGeometry() const
{
    return mScrollGeometry;
}

void RibbonCategory::registerPanel(RibbonPanel* panel)
{
    if (!mPanels.contains(panel)) {
        mPanels.append(panel);
        // apply the last pushed bar style so dynamically added panels match
        applyRibbonStyle(mStyleRowCount, mStyleShowPanelTitle, mStyleWordWrap, mStyleIconRightText);
        panel->applyLayoutFactors(mButtonMaximumAspectRatio, mLargeButtonMinimumWidthRatio);
        // panel implicit sizes are the layout hints: any change re-runs relayout
        connect(panel, &QQuickItem::implicitWidthChanged, this, [this]() { polish(); });
        connect(panel, &QQuickItem::implicitHeightChanged, this, [this]() { polish(); });
        polish();
    }
}

void RibbonCategory::applyRibbonStyle(int rowCount, bool showPanelTitle, bool wordWrap, bool iconRightText)
{
    mStyleRowCount       = rowCount;
    mStyleShowPanelTitle = showPanelTitle;
    mStyleWordWrap       = wordWrap;
    mStyleIconRightText  = iconRightText;
    const RibbonEnums::LayoutMode mode = (rowCount <= 1)  ? RibbonEnums::SingleRowMode
                                         : (rowCount == 2) ? RibbonEnums::TwoRowMode
                                                           : RibbonEnums::ThreeRowMode;
    for (RibbonPanel* panel : mPanels) {
        panel->applyRibbonStyle(mode, showPanelTitle, wordWrap, iconRightText);
    }
    polish();
}

void RibbonCategory::applyLayoutFactors(qreal buttonMaximumAspectRatio, qreal largeButtonMinimumWidthRatio)
{
    // widgets SARibbonCategory::setButtonMaximumAspectRatio parity: remember the
    // pushed pair (panels registered later inherit) and forward it downward
    mButtonMaximumAspectRatio     = buttonMaximumAspectRatio;
    mLargeButtonMinimumWidthRatio = largeButtonMinimumWidthRatio;
    for (RibbonPanel* panel : mPanels) {
        panel->applyLayoutFactors(buttonMaximumAspectRatio, largeButtonMinimumWidthRatio);
    }
}

void RibbonCategory::unregisterPanel(RibbonPanel* panel)
{
    if (mPanels.removeOne(panel)) {
        disconnect(panel, nullptr, this, nullptr);
        polish();
    }
}

int RibbonCategory::panelCount() const
{
    return mPanels.size();
}

RibbonPanel* RibbonCategory::panelAt(int index) const
{
    return (index >= 0 && index < mPanels.size()) ? mPanels[ index ] : nullptr;
}

int RibbonCategory::panelIndex(RibbonPanel* panel) const
{
    return mPanels.indexOf(panel);
}

RibbonPanel* RibbonCategory::panelByObjectName(const QString& objName) const
{
    if (objName.isEmpty()) {
        return nullptr;
    }
    for (RibbonPanel* panel : mPanels) {
        if (panel->objectName() == objName) {
            return panel;
        }
    }
    return nullptr;
}

/**
 * \if ENGLISH
 * @brief Create a panel host at runtime and slot it into the row
 * @details Widgets SARibbonCategory::insertPanel parity. The host is C++-made,
 *          so componentComplete never runs for it and the visual leaf has to be
 *          requested explicitly — the same route RibbonBar::createAutoTab takes.
 *          Registration goes through setParentItem, which makes itemChange the
 *          single place that pushes the inherited bar style and layout factors
 *          down; only the resulting order is repaired here. The QObject parent
 *          is this category, so the panel dies with it.
 * \endif
 *
 * \if CHINESE
 * @brief 运行时创建一个面板宿主并插入到排列中
 * @details 对应 widgets SARibbonCategory::insertPanel。宿主由 C++ 创建，因此
 *          componentComplete 不会为它运行，视觉叶子必须显式索取——与
 *          RibbonBar::createAutoTab 走的是同一条路。登记经 setParentItem 完成，
 *          这让 itemChange 成为唯一下发继承样式与布局系数的地方；此处只修正
 *          由此得到的顺序。QObject 父项是本 category，面板随其一同销毁。
 * \endif
 */
RibbonPanel* RibbonCategory::insertPanel(const QString& title, int index)
{
    RibbonPanel* panel = new RibbonPanel();
    panel->setParent(this);
    panel->setPanelTitle(title);
    panel->setParentItem(this);  // itemChange -> registerPanel (appends)
    panel->ensureQmlLeaf();
    const int last = mPanels.size() - 1;
    const int to   = (index < 0 || index > last) ? last : index;
    if (last > 0 && to != last) {
        mPanels.move(last, to);
    }
    polish();
    return panel;
}

bool RibbonCategory::removePanel(RibbonPanel* panel)
{
    if (!panel || !mPanels.contains(panel)) {
        return false;
    }
    unregisterPanel(panel);
    panel->setParentItem(nullptr);
    panel->setParent(nullptr);
    panel->deleteLater();
    polish();
    return true;
}

bool RibbonCategory::movePanel(int from, int to)
{
    if (from < 0 || from >= mPanels.size() || to < 0 || to >= mPanels.size() || from == to) {
        return false;
    }
    mPanels.move(from, to);
    polish();
    return true;
}

int RibbonCategory::contentWidth() const
{
    return mTotalWidth;
}

QVariantList RibbonCategory::separatorXs() const
{
    return mSeparatorXs;
}

void RibbonCategory::itemChange(ItemChange change, const ItemChangeData& data)
{
    if (change == QQuickItem::ItemChildAddedChange) {
        // declaration order (sibling componentComplete runs reversed)
        if (RibbonPanel* p = qobject_cast< RibbonPanel* >(data.item)) {
            registerPanel(p);
        }
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonPanel* p = qobject_cast< RibbonPanel* >(data.item)) {
            unregisterPanel(p);
        }
    } else if (change == QQuickItem::ItemVisibleHasChanged) {
        polish();
    }
    RibbonQuickHost::itemChange(change, data);
}

void RibbonCategory::updatePolish()
{
    relayout();
}

void RibbonCategory::relayout()
{
    if (!mScrollOverlay && isComponentComplete()) {
        ensureScrollOverlay();
    }
    if (mPanels.isEmpty() || width() <= 0) {
        publishScrollState(SARibbon::Core::SARibbonScrollFlags());
        return;
    }
    // collect per-panel size hints (panel implicit sizes as inputs)
    SARibbon::Core::SARibbonCategorySizeHints hints;
    hints.panelSizes.resize(mPanels.size());
    hints.separatorSizes.resize(mPanels.size());
    QVector< SARibbon::Core::SARibbonAbstractCategoryItem* > items;
    items.reserve(mPanels.size());
    for (int i = 0; i < mPanels.size(); ++i) {
        RibbonPanel* p = mPanels[ i ];
        CategoryItemAdapter* adapter = new CategoryItemAdapter(p);
        const QSize hint = QSize(int(p->implicitWidth()), int(p->implicitHeight()));
        hints.panelSizes[ i ]   = QSize(hint.width(), int(height()));
        hints.separatorSizes[ i ] = QSize(1, int(height()));
        hints.totalWidth += hint.width() + 1;
        items.append(adapter);
    }

    SARibbon::Core::SARibbonCategoryLayoutEngine::Input input;
    input.categoryWidth = int(width());
    input.height        = int(height());
    input.margins       = QMargins(0, 0, 0, 0);
    input.isRTL         = SA::saIsRTL();
    input.xBase         = mScrollXBase;
    input.sizeHints     = hints;

    auto r = mEngine.layout(items, input);
    mTotalWidth = r.totalWidth;

    // Consume the engine's scroll outputs (widgets adapter parity): the flags
    // drive the arrow overlay, and the non-scrolling branch resets the base so a
    // shrunk content never stays offset (engine Result::newXBase). Both flags
    // false is exactly the "content fits" case, see core scrollButtonFlags.
    if (!r.scrollFlags.showLeft && !r.scrollFlags.showRight && mScrollXBase != r.newXBase) {
        mScrollXBase = r.newXBase;
        Q_EMIT scrollPositionChanged();
    }
    publishScrollState(r.scrollFlags);

    // publish the engine-written separator geometry to the leaf (the QSS
    // margin-top/bottom 3px of `SARibbonCategory > SARibbonSeparatorWidget`
    // stays leaf-side; only the x positions are geometry authority)
    QVariantList separators;
    for (int i = 0; i < items.size(); ++i) {
        if (!items[ i ]->isHidden() && !items[ i ]->isSeparatorHidden) {
            separators.append(QVariant(qreal(items[ i ]->resultSeparatorGeometry.x())));
        }
    }
    if (separators != mSeparatorXs) {
        mSeparatorXs = separators;
        Q_EMIT separatorXsChanged();
    }

    for (int i = 0; i < items.size(); ++i) {
        items[ i ]->applyGeometry(items[ i ]->resultGeometry);
    }
    qDeleteAll(items);
    setImplicitWidth(qreal(mTotalWidth));
}

// ---- CategoryItemAdapter ----
QSize CategoryItemAdapter::sizeHint() const
{
    return QSize(mPanel->implicitWidth(), mPanel->implicitHeight());
}

void CategoryItemAdapter::applyGeometry(const QRect& rect)
{
    mPanel->setPosition(rect.topLeft());
    mPanel->setSize(rect.size());
}

}
