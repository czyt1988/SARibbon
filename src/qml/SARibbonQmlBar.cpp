#include "SARibbonQmlBar.h"
#include "SARibbonQmlCategory.h"
#include "SARibbonQmlContextCategory.h"
#include "SARibbonQmlQuickAccessBar.h"
#include "SARibbonQmlButtonGroup.h"
#include "SARibbonQmlMenuItem.h"
#include "SARibbonQmlApplicationWindow.h"
#include "SARibbonQmlTab.h"
#include "SARibbonQmlMetrics.h"
#include "SARibbonQmlTheme.h"
#include "SARibbonQmlTypes.h"
#include "SARibbonQmlWindowAgent.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <SARibbonCore/SARibbonThemeData.h>
#include <QFontMetrics>
#include <QQuickWindow>
#include <QScreen>
#include <QWindow>

namespace SARibbonQml {

RibbonBar::RibbonBar(QQuickItem* parent) : RibbonQuickHost(parent)
{
    // system dark mode auto switch, mirroring SARibbonMainWindow/SARibbonWidget:
    // when the OS is dark and the theme is still the default, start on Dark.
    // Opt out with RibbonTheme.followSystemDarkMode = false before the bar exists
    if (SA::isEnableSystemDarkModeAutoSwitch() && SA::isOperatingSystemInDarkMode()
        && SARibbon::Core::SARibbonThemeData::instance()->theme() == SARibbonTheme::RibbonThemeOffice2021Blue) {
        RibbonTheme::instance()->setCurrentTheme(int(SARibbonTheme::RibbonThemeDark));
    }
    // band highlight follows the theme (same core fp the widgets ThemeManager
    // installs); recompute the published band data on theme switches
    connect(SARibbon::Core::SARibbonThemeData::instance(), &SARibbon::Core::SARibbonThemeData::themeChanged, this,
            [this]() { polish(); });
    // RTL flip re-runs the title-rect engine pass (SA::saIsRTL() re-read)
    connect(RibbonTheme::instance(), &RibbonTheme::rtlChanged, this, [this]() { polish(); });
}

RibbonBar::~RibbonBar()
{
}

QUrl RibbonBar::leafUrl() const
{
    return SARibbonQmlLeafUrls::barLeaf();
}

void RibbonBar::componentComplete()
{
    RibbonQuickHost::componentComplete();
    // children (tabs/categories) complete before the parent, so the register
    // lists are already filled here
    ensureQmlLeaf();
    attachWindowAgent();
    polish();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonBar::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChange(newGeometry, oldGeometry);
#else
void RibbonBar::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.size() != oldGeometry.size()) {
        polish();  // width comes from anchors/consumer: relayout on every resize
    }
}

int RibbonBar::currentIndex() const
{
    return mCurrentIndex;
}

void RibbonBar::setCurrentIndex(int idx)
{
    // bound by the EFFECTIVE tab row (normal tabs + active context tabs; the
    // tab row may be longer than the category list when tabs are declared
    // beyond the categories; those show an empty page exactly like a QTabBar
    // with more pages than stacked widgets)
    if (mCurrentIndex == idx || idx < 0 || idx >= qMax(effectiveTabCount(), 1)) {
        return;
    }
    mCurrentIndex = idx;
    Q_EMIT currentIndexChanged();
    polish();
}

QString RibbonBar::applicationLabel() const
{
    return mApplicationLabel;
}

void RibbonBar::setApplicationLabel(const QString& label)
{
    if (mApplicationLabel == label) {
        return;
    }
    mApplicationLabel = label;
    Q_EMIT applicationLabelChanged();
    polish();
}

RibbonEnums::RibbonStyle RibbonBar::ribbonStyle() const
{
    return mRibbonStyle;
}

int RibbonBar::styleRowCount(RibbonEnums::RibbonStyle style)
{
    if (int(style) & int(RibbonEnums::RibbonStyleSingleRow)) {
        return 1;
    }
    if (int(style) & int(RibbonEnums::RibbonStyleTwoRow)) {
        return 2;
    }
    return 3;
}

bool RibbonBar::styleIsCompact(RibbonEnums::RibbonStyle style)
{
    return int(style) & int(RibbonEnums::RibbonStyleCompact);
}

void RibbonBar::setRibbonStyle(RibbonEnums::RibbonStyle style)
{
    if (mRibbonStyle == style) {
        return;
    }
    mRibbonStyle = style;
    mTabOnTitle  = styleIsCompact(style);
    Q_EMIT ribbonStyleChanged();
    propagateRibbonStyle();
    polish();
}

RibbonEnums::Alignment RibbonBar::tabAlignment() const
{
    return mTabAlignment;
}

void RibbonBar::setTabAlignment(RibbonEnums::Alignment alignment)
{
    if (mTabAlignment == alignment) {
        return;
    }
    mTabAlignment = alignment;
    Q_EMIT tabAlignmentChanged();
    polish();
}

bool RibbonBar::isMinimumMode() const
{
    return mMinimumMode;
}

void RibbonBar::setMinimumMode(bool on)
{
    if (mMinimumMode == on) {
        return;
    }
    mMinimumMode = on;
    Q_EMIT minimumModeChanged();
    polish();
}

qreal RibbonBar::buttonMaximumAspectRatio() const
{
    return mButtonMaximumAspectRatio;
}

void RibbonBar::setButtonMaximumAspectRatio(qreal fac)
{
    if (qFuzzyCompare(mButtonMaximumAspectRatio, fac)) {
        return;
    }
    mButtonMaximumAspectRatio = fac;
    Q_EMIT buttonMaximumAspectRatioChanged();
    propagateLayoutFactors();
}

qreal RibbonBar::largeButtonMinimumWidthRatio() const
{
    return mLargeButtonMinimumWidthRatio;
}

void RibbonBar::setLargeButtonMinimumWidthRatio(qreal fac)
{
    // zero and negatives are meaningful (they drop the height-based minimum
    // width of large buttons), so the guard must tolerate them explicitly
    const bool same = (qFuzzyIsNull(mLargeButtonMinimumWidthRatio) && qFuzzyIsNull(fac))
                      || qFuzzyCompare(mLargeButtonMinimumWidthRatio, fac);
    if (same) {
        return;
    }
    mLargeButtonMinimumWidthRatio = fac;
    Q_EMIT largeButtonMinimumWidthRatioChanged();
    propagateLayoutFactors();
}

int RibbonBar::systemButtonStripWidth() const
{
    return mSystemButtonStripWidth;
}

void RibbonBar::setSystemButtonStripWidth(int w)
{
    // negatives have no geometric meaning here: they would push the right
    // hosts past the bar edge, so clamp on the way in
    const int strip = qMax(w, 0);
    if (mSystemButtonStripWidth == strip) {
        return;
    }
    mSystemButtonStripWidth = strip;
    Q_EMIT systemButtonStripWidthChanged();
    polish();
}

QString RibbonBar::windowTitle() const
{
    return mWindowTitle;
}

void RibbonBar::setWindowTitle(const QString& title)
{
    if (mWindowTitle == title) {
        return;
    }
    mWindowTitle = title;
    Q_EMIT windowTitleChanged();
    // the title text feeds the minimum-width title reservation: recompute on
    // every change (relayout re-reads it through the core engine)
    polish();
}

bool RibbonBar::isFramelessActive() const
{
    return mWindowAgent != nullptr && mWindowAgent->window() != nullptr && mWindowAgent->isFramelessEnabled();
}

RibbonWindowAgent* RibbonBar::windowAgent() const
{
    return mWindowAgent;
}

void RibbonBar::setWindowAgent(RibbonWindowAgent* agent)
{
    if (mWindowAgent == agent) {
        return;
    }
    if (mWindowAgent) {
        detachWindowAgent();
    }
    mWindowAgent = agent;
    if (mWindowAgent) {
        if (mWindowAgent->parent() == nullptr) {
            mWindowAgent->setParent(this);
        }
        // strip width follows the agent's frameless state and glyph metrics;
        // every flip re-runs the layout with a new reservation
        connect(mWindowAgent, &RibbonWindowAgent::buttonWidthChanged, this, [this]() {
            if (mWindowAgent) {
                setSystemButtonStripWidth(mWindowAgent->stripWidth());
            }
        });
        connect(mWindowAgent, &RibbonWindowAgent::framelessEnabledChanged, this, [this]() {
            if (mWindowAgent) {
                setSystemButtonStripWidth(mWindowAgent->stripWidth());
            }
            Q_EMIT framelessActiveChanged();
        });
        connect(mWindowAgent, &RibbonWindowAgent::windowChanged, this, [this]() {
            Q_EMIT framelessActiveChanged();
            if (mWindowAgent && mWindowAgent->window()) {
                setWindowTitle(mWindowAgent->window()->title());
            }
            // setup() rebuilds the QWK context and re-applies only the title
            // bar item — the hit-test registration set does not survive the
            // recreation, so re-apply it here (no-op while detached/disabled)
            syncHitTestVisible();
        });
        connect(mWindowAgent, &QObject::destroyed, this, [this]() {
            mWindowAgent = nullptr;
            setSystemButtonStripWidth(0);
            Q_EMIT framelessActiveChanged();
            Q_EMIT windowAgentChanged();
        });
        setSystemButtonStripWidth(mWindowAgent->stripWidth());
        // the property can be assigned after componentComplete (dynamic
        // frameless toggling); attach immediately in that case
        if (isComponentComplete()) {
            attachWindowAgent();
        }
    }
    Q_EMIT windowAgentChanged();
}

bool RibbonBar::setSystemButton(const QString& kind, QQuickItem* item)
{
    return mWindowAgent ? mWindowAgent->setSystemButton(kind, item) : false;
}

/**
 * \if ENGLISH
 * @brief Push the two width factors through the whole host tree
 * @details Mirrors SARibbonBar::setButtonMaximumAspectRatio, which walks every
 *          category (and the QML bar additionally walks the pages of every
 *          declared context category, since those ride the same style). The
 *          category relays to its panels, the panel relays to its buttons, and
 *          each button drops its engine sizeHint cache entry, so the panels
 *          repack on the next polish without a relayout call here.
 * \endif
 *
 * \if CHINESE
 * @brief 把两个宽度系数下发到整棵宿主树
 * @details 对应 SARibbonBar::setButtonMaximumAspectRatio：遍历全部 category
 *          （QML 侧还要遍历每个已声明上下文分类的页面，它们跟随同一样式）。
 *          category 转给其面板，面板转给其按钮，每个按钮丢弃引擎 sizeHint 缓存，
 *          因此下一次 polish 时面板会自动重新装箱，此处无需再请求重排。
 * \endif
 */
void RibbonBar::propagateLayoutFactors()
{
    for (RibbonCategory* cat : mCategories) {
        cat->applyLayoutFactors(mButtonMaximumAspectRatio, mLargeButtonMinimumWidthRatio);
    }
    for (RibbonContextCategory* ctx : mContexts) {
        for (RibbonCategory* page : ctx->categories()) {
            page->applyLayoutFactors(mButtonMaximumAspectRatio, mLargeButtonMinimumWidthRatio);
        }
    }
    polish();
}

void RibbonBar::propagateRibbonStyle()
{
    // widgets setRibbonStyle parity: three-row keeps word wrap + panel
    // titles; single-row hides titles and switches buttons to icon-right
    // text; two-row drops word wrap but keeps titles
    const int rows = styleRowCount(mRibbonStyle);
    const bool showTitle = (rows != 1);
    const bool wordWrap  = (rows == 3);
    const bool iconRight = (rows == 1);
    for (RibbonCategory* cat : mCategories) {
        cat->applyRibbonStyle(rows, showTitle, wordWrap, iconRight);
    }
    // context category pages ride the same style
    for (RibbonContextCategory* ctx : mContexts) {
        for (RibbonCategory* page : ctx->categories()) {
            page->applyRibbonStyle(rows, showTitle, wordWrap, iconRight);
        }
    }
}

int RibbonBar::tabBarHeight() const
{
    return mTabBarHeight;
}

int RibbonBar::titleBarHeight() const
{
    return mTitleBarHeight;
}

int RibbonBar::categoryRowY() const
{
    return mCategoryRowY;
}

QRectF RibbonBar::applicationButtonRect() const
{
    return mApplicationButtonRect;
}

void RibbonBar::registerCategory(RibbonCategory* category)
{
    if (!mCategories.contains(category)) {
        mCategories.append(category);
        category->setParentItem(this);
        // a category registered after the style was set must match it
        const int rows = styleRowCount(mRibbonStyle);
        category->applyRibbonStyle(rows, rows != 1, rows == 3, rows == 1);
        category->applyLayoutFactors(mButtonMaximumAspectRatio, mLargeButtonMinimumWidthRatio);
        // auto tabs track their category title (addCategoryPage semantics)
        const int idx = mCategories.size() - 1;
        if (idx < mTabs.size() && mAutoTabs.contains(mTabs[ idx ])) {
            mTabs[ idx ]->setText(category->title());
        }
        connect(category, &RibbonCategory::titleChanged, this, [this, category]() {
            const int i = mCategories.indexOf(category);
            if (i >= 0 && i < mTabs.size() && mAutoTabs.contains(mTabs[ i ])) {
                mTabs[ i ]->setText(category->title());
            }
        });
        polish();
    }
}

void RibbonBar::unregisterCategory(RibbonCategory* category)
{
    const int idx = mCategories.indexOf(category);
    if (idx < 0) {
        return;
    }
    disconnect(category, nullptr, this, nullptr);
    mCategories.remove(idx);
    mHiddenCategories.removeOne(category);
    polish();
}

int RibbonBar::categoryCount() const
{
    return mCategories.size();
}

RibbonCategory* RibbonBar::categoryAt(int index) const
{
    return (index >= 0 && index < mCategories.size()) ? mCategories[ index ] : nullptr;
}

int RibbonBar::categoryIndex(RibbonCategory* category) const
{
    return mCategories.indexOf(category);
}

RibbonCategory* RibbonBar::categoryByObjectName(const QString& objName) const
{
    if (objName.isEmpty()) {
        return nullptr;
    }
    for (RibbonCategory* cat : mCategories) {
        if (cat->objectName() == objName) {
            return cat;
        }
    }
    return nullptr;
}

RibbonQuickAccessBar* RibbonBar::quickAccessBar() const
{
    return mQuickAccessBar;
}

/**
 * \if ENGLISH
 * @brief Create a category at runtime and keep the tab row paired with it
 * @details Widgets SARibbonBar::insertCategoryPage parity. The category is
 *          C++-made, so its visual leaf has to be requested explicitly —
 *          RibbonBar::createAutoTab already established that route. The tab row
 *          needs care: syncTabCount only ever appends, so inserting into the
 *          middle would leave the new category paired with the wrong tab. When
 *          the row is entirely auto-generated (the case a customize record
 *          always meets, since explicit tabs are declarations) the tab is
 *          created here and moved into the same slot; a row with declared tabs
 *          keeps its own pairing and only the category order changes.
 * \endif
 *
 * \if CHINESE
 * @brief 运行时创建一个 category 并保持 tab 行与之配对
 * @details 对应 widgets SARibbonBar::insertCategoryPage。category 由 C++ 创建，
 *          因此视觉叶子必须显式索取——RibbonBar::createAutoTab 已经确立了这条
 *          路径。tab 行需要当心：syncTabCount 只会在末尾追加，所以往中间插入会
 *          让新 category 配到错误的 tab 上。当整行 tab 都是自动生成时（定制记录
 *          总会遇到这种情形，因为显式 tab 来自声明），tab 在此创建并搬到同一
 *          槽位；带声明 tab 的行保留自己的配对，只改变 category 顺序。
 * \endif
 */
RibbonCategory* RibbonBar::insertCategory(const QString& title, int index)
{
    const int at = (index < 0 || index > mCategories.size()) ? mCategories.size() : index;
    // an all-auto (or still empty) tab row is grown here so the pairing holds
    const bool autoRow = mTabs.isEmpty() || (mAutoTabs.size() == mTabs.size());
    RibbonCategory* category = new RibbonCategory();
    category->setParent(this);
    category->setTitle(title);
    registerCategory(category);  // setParentItem + style push + append
    category->ensureQmlLeaf();
    const int last = mCategories.size() - 1;
    if (at < last) {
        mCategories.move(last, at);
    }
    if (autoRow) {
        // createAutoTab only builds + registers; ownership bookkeeping is the
        // caller's job, exactly as in syncTabCount
        RibbonTab* tab = createAutoTab(at);  // title from mCategories[at]
        mAutoTabs.append(tab);
        moveTabSlot(mTabs.size() - 1, at);
    }
    polish();
    return category;
}

bool RibbonBar::removeCategory(RibbonCategory* category)
{
    const int idx = mCategories.indexOf(category);
    if (idx < 0) {
        return false;
    }
    // the paired auto tab is owned here and must go first, otherwise
    // syncTabCount would recreate it on the next relayout
    if (idx < mTabs.size() && mAutoTabs.contains(mTabs[ idx ])) {
        unregisterTab(mTabs[ idx ]);
    }
    unregisterCategory(category);
    category->setParentItem(nullptr);
    category->setParent(nullptr);
    category->deleteLater();
    polish();
    return true;
}

bool RibbonBar::moveCategory(int from, int to)
{
    if (from < 0 || from >= mCategories.size() || to < 0 || to >= mCategories.size() || from == to) {
        return false;
    }
    mCategories.move(from, to);
    // widgets moveCategory parity: the tab follows its category
    if (from < mTabs.size() && to < mTabs.size()) {
        moveTabSlot(from, to);
    }
    polish();
    return true;
}

void RibbonBar::moveTabSlot(int from, int to)
{
    if (from < 0 || from >= mTabs.size() || to < 0 || to >= mTabs.size() || from == to) {
        return;
    }
    mTabs.move(from, to);
    // rebuild the auto subset in row order: syncTabCount's shrink test compares
    // the two tails, which only holds while both lists stay consistently ordered
    QVector< RibbonTab* > autos;
    autos.reserve(mAutoTabs.size());
    for (int i = 0; i < mTabs.size(); ++i) {
        if (mAutoTabs.contains(mTabs[ i ])) {
            autos.append(mTabs[ i ]);
        }
    }
    mAutoTabs = autos;
    polish();
}

void RibbonBar::showCategory(RibbonCategory* category)
{
    if (!category || !mCategories.contains(category) || !mHiddenCategories.contains(category)) {
        return;
    }
    mHiddenCategories.removeOne(category);
    polish();
}

void RibbonBar::hideCategory(RibbonCategory* category)
{
    if (!category || !mCategories.contains(category) || mHiddenCategories.contains(category)) {
        return;
    }
    mHiddenCategories.append(category);
    category->setVisible(false);
    polish();
}

bool RibbonBar::isCategoryHidden(RibbonCategory* category) const
{
    return mHiddenCategories.contains(category);
}

void RibbonBar::registerTab(RibbonTab* tab)
{
    if (!mTabs.contains(tab)) {
        const int idx = mTabs.size();
        mTabs.append(tab);
        tab->setParentItem(this);
        // clicking a tab selects the category of the same index
        connect(tab, &RibbonTab::clicked, this, [this, tab]() {
            const int idx = mTabs.indexOf(tab);
            if (idx >= 0) {
                setCurrentIndex(idx);
            }
        });
        // an explicit tab takes over the pairing slot: drop the auto tab that
        // may already sit there and inherit the category title as fallback
        if (idx < mCategories.size()) {
            connect(mCategories[ idx ], &RibbonCategory::titleChanged, tab, [this, tab]() {
                const int i = mTabs.indexOf(tab);
                if (i >= 0 && i < mCategories.size() && tab->text().isEmpty()) {
                    tab->setText(mCategories[ i ]->title());
                }
            });
            if (tab->text().isEmpty()) {
                tab->setText(mCategories[ idx ]->title());
            }
        }
        polish();
    }
}

void RibbonBar::unregisterTab(RibbonTab* tab)
{
    if (mAutoTabs.contains(tab)) {
        // auto tabs are owned here: unparent + deleteLater, NEVER direct delete
        mAutoTabs.removeOne(tab);
        mTabs.removeOne(tab);
        tab->setParentItem(nullptr);
        tab->setParent(nullptr);
        tab->deleteLater();
        polish();
        return;
    }
    if (mTabs.removeOne(tab)) {
        disconnect(tab, &RibbonTab::clicked, this, nullptr);
        polish();
    }
}

void RibbonBar::registerContext(RibbonContextCategory* ctx)
{
    if (!ctx || mContexts.contains(ctx)) {
        return;
    }
    mContexts.append(ctx);
    syncContextSignals(ctx);
    rebuildContextTabs(ctx);
    polish();
}

void RibbonBar::unregisterContext(RibbonContextCategory* ctx)
{
    const int idx = mContexts.indexOf(ctx);
    if (idx < 0) {
        return;
    }
    mContexts.remove(idx);
    disconnect(ctx, nullptr, this, nullptr);
    // drop the owned context tabs (unparent + deleteLater, NEVER direct delete)
    const auto tabs = mContextTabs.value(ctx);
    for (RibbonTab* tab : tabs) {
        mTabs.removeOne(tab);  // not in mTabs, but harmless if that ever changes
        tab->setParentItem(nullptr);
        tab->setParent(nullptr);
        tab->deleteLater();
    }
    mContextTabs.remove(ctx);
    polish();
}

/**
 * \if ENGLISH
 * @brief Every category page owned by a declared context category
 * @details The customize tree walks the declared main categories through
 *          categoryAt and then appends these, which reproduces the widgets
 *          categoryPages order (main row first, context pages after) without
 *          depending on context activation: a customization dialog lists what
 *          the ribbon declares, not what happens to be active while it is open.
 * \endif
 *
 * \if CHINESE
 * @brief 由已声明上下文类别持有的全部 category 页
 * @details 定制树先通过 categoryAt 遍历声明的主类别，再追加这些页，从而复现
 *          widgets categoryPages 的顺序（主类别在前、上下文页在后），且不依赖
 *          上下文的激活状态：定制对话框列出的是 ribbon 声明了什么，而不是打开
 *          期间恰好激活了什么。
 * \endif
 */
QVector< RibbonCategory* > RibbonBar::contextCategories() const
{
    QVector< RibbonCategory* > cats;
    for (RibbonContextCategory* ctx : mContexts) {
        cats += ctx->categories();
    }
    return cats;
}

bool RibbonBar::isContextCategory(RibbonCategory* category) const
{
    return category ? contextCategories().contains(category) : false;
}

bool RibbonBar::isOwnedContextTab(RibbonTab* tab) const
{
    // owned context page tabs never join the normal tab registration (their
    // parenting onto the bar fires the same child-added notification)
    for (auto it = mContextTabs.constBegin(); it != mContextTabs.constEnd(); ++it) {
        if (it.value().contains(tab)) {
            return true;
        }
    }
    return false;
}

void RibbonBar::syncContextSignals(RibbonContextCategory* ctx)
{    // activation and identity changes re-run the layout (tab row + bands);
    // page list changes additionally rebuild the owned tabs
    connect(ctx, &RibbonContextCategory::activeChanged, this, [this]() { polish(); });
    connect(ctx, &RibbonContextCategory::contextTitleChanged, this, [this]() { polish(); });
    connect(ctx, &RibbonContextCategory::contextColorChanged, this, [this]() { polish(); });
    connect(ctx, &RibbonContextCategory::categoryPagesChanged, this, [this, ctx]() {
        rebuildContextTabs(ctx);
        polish();
    });
    // page title changes track into the owned tabs (auto-tab parity)
    for (RibbonCategory* page : ctx->categories()) {
        connect(page, &RibbonCategory::titleChanged, this, [this, page]() {
            for (auto it = mContextTabs.begin(); it != mContextTabs.end(); ++it) {
                const int i = it.key()->categories().indexOf(page);
                if (i >= 0 && i < it.value().size()) {
                    it.value()[ i ]->setText(page->title());
                }
            }
        });
    }
}

void RibbonBar::rebuildContextTabs(RibbonContextCategory* ctx)
{
    // (re)create one tab per page: C++-created like the auto tabs (no
    // componentComplete, so the leaf is created explicitly), colored with
    // the context color, initially hidden until activation includes them.
    // NOTE: the tabs are inserted into mContextTabs BEFORE parenting them
    // onto the bar — setParentItem fires the bar's itemChange, and the
    // RibbonTab branch must not adopt owned context tabs as normal tabs
    // (that would corrupt the auto-tab count and eat a normal tab's slot).
    auto old = mContextTabs.value(ctx);
    for (RibbonTab* tab : old) {
        tab->setParentItem(nullptr);
        tab->setParent(nullptr);
        tab->deleteLater();
    }
    QVector< RibbonTab* > tabs;
    const auto pages = ctx->categories();
    for (RibbonCategory* page : pages) {
        // a page added after the bar-level push must still inherit the factors
        page->applyLayoutFactors(mButtonMaximumAspectRatio, mLargeButtonMinimumWidthRatio);
        RibbonTab* tab = new RibbonTab();
        tab->setText(page->title());
        tab->setContextColor(ctx->contextColor());
        // clicking switches the current index (the effective row includes
        // these tabs while active)
        connect(tab, &RibbonTab::clicked, this, [this, tab]() {
            const int idx = effectiveTabs().indexOf(tab);
            if (idx >= 0) {
                setCurrentIndex(idx);
            }
        });
        tabs.append(tab);
    }
    mContextTabs.insert(ctx, tabs);
    for (RibbonTab* tab : tabs) {
        tab->setParent(this);
        tab->setParentItem(this);
        tab->ensureQmlLeaf();
        tab->setVisible(false);
    }
}

int RibbonBar::effectiveTabCount() const
{
    return effectiveTabs().size();
}

QVector< RibbonTab* > RibbonBar::effectiveTabs() const
{
    QVector< RibbonTab* > tabs;
    tabs.reserve(mTabs.size());
    for (int i = 0; i < mTabs.size(); ++i) {
        // a user-hidden category takes its paired tab off the row as well
        // (widgets hideCategory parity)
        if (i < mCategories.size() && mHiddenCategories.contains(mCategories[ i ])) {
            continue;
        }
        tabs.append(mTabs[ i ]);
    }
    for (RibbonContextCategory* ctx : mContexts) {
        if (ctx->isActive()) {
            tabs += mContextTabs.value(ctx);
        }
    }
    return tabs;
}

QVector< RibbonCategory* > RibbonBar::effectiveCategories() const
{
    QVector< RibbonCategory* > cats;
    cats.reserve(mCategories.size());
    for (RibbonCategory* cat : mCategories) {
        if (!mHiddenCategories.contains(cat)) {
            cats.append(cat);
        }
    }
    for (RibbonContextCategory* ctx : mContexts) {
        if (ctx->isActive()) {
            cats += ctx->categories();
        }
    }
    return cats;
}

QVariantList RibbonBar::contextBands() const
{
    return mBands;
}

QQmlListProperty< RibbonMenuItem > RibbonBar::applicationMenuItems()
{
    return QQmlListProperty< RibbonMenuItem >(this, this, &RibbonBar::appendAppMenuItemCb, &RibbonBar::appMenuItemCountCb, &RibbonBar::appMenuItemAtCb,
                                              &RibbonBar::clearAppMenuItemsCb);
}

int RibbonBar::applicationMenuItemCount() const
{
    return mAppMenuItems.size();
}

RibbonMenuItem* RibbonBar::applicationMenuItemAt(int index) const
{
    return (index >= 0 && index < mAppMenuItems.size()) ? mAppMenuItems[ index ] : nullptr;
}

bool RibbonBar::hasApplicationMenu() const
{
    return !mAppMenuItems.isEmpty();
}

void RibbonBar::activateApplicationMenuItem(int index)
{
    QVariantList path;
    path.append(index);
    activateApplicationMenuItemPath(path);
}

/**
 * \if ENGLISH
 * @brief Activate the application menu entry an index path addresses
 * @details Same contract as RibbonToolButton::activateMenuItemPath (checkable
 *          entries flip before applicationMenuTriggered fires, separators and
 *          disabled entries are refused). Unlike the button the bar does NOT
 *          close the popup here: the application menu leaf owns that decision,
 *          which is why the leaf closes it right after the call.
 * \endif
 *
 * \if CHINESE
 * @brief 激活索引路径指向的应用菜单项
 * @details 契约与 RibbonToolButton::activateMenuItemPath 相同（可勾选的菜单项在
 *          applicationMenuTriggered 之前翻转，分隔符与禁用项被拒绝）。与按钮不同的
 *          是这里不关闭弹窗：应用菜单的关闭决定权在叶子手里，所以叶子在调用之后
 *          自行关闭。
 * \endif
 */
void RibbonBar::activateApplicationMenuItemPath(const QVariantList& indexPath)
{
    RibbonMenuItem* item = RibbonMenuItem::resolvePath(mAppMenuItems, indexPath);
    if (!item || !item->isEnabled() || item->isSeparator()) {
        return;
    }
    item->activate();
    Q_EMIT applicationMenuTriggered(item);
}

QQuickItem* RibbonBar::applicationWindowItem() const
{
    return mApplicationWindow;
}

bool RibbonBar::hasApplicationWindow() const
{
    return mApplicationWindow != nullptr;
}

void RibbonBar::requestApplicationWindowClose()
{
    // route to the leaf popup (same invoke pattern as the button menus);
    // headless fallback keeps the visibility flag consistent for tests
    QQuickItem* leaf = qmlLeaf();
    if (leaf && QMetaObject::invokeMethod(leaf, "closeApplicationWindow")) {
        return;
    }
    if (mApplicationWindow) {
        mApplicationWindow->setPopupVisible(false);
    }
}

void RibbonBar::appendAppMenuItemCb(QQmlListProperty< RibbonMenuItem >* prop, RibbonMenuItem* item)
{
    auto* self = static_cast< RibbonBar* >(prop->data);
    if (self && item && !self->mAppMenuItems.contains(item)) {
        self->mAppMenuItems.append(item);
        item->setParent(self);
        Q_EMIT self->applicationMenuItemsChanged();
    }
}

RibbonBar::ListIndex RibbonBar::appMenuItemCountCb(QQmlListProperty< RibbonMenuItem >* prop)
{
    auto* self = static_cast< RibbonBar* >(prop->data);
    return self ? self->mAppMenuItems.size() : ListIndex(0);
}

RibbonMenuItem* RibbonBar::appMenuItemAtCb(QQmlListProperty< RibbonMenuItem >* prop, ListIndex index)
{
    auto* self = static_cast< RibbonBar* >(prop->data);
    return self ? self->applicationMenuItemAt(int(index)) : nullptr;
}

void RibbonBar::clearAppMenuItemsCb(QQmlListProperty< RibbonMenuItem >* prop)
{
    auto* self = static_cast< RibbonBar* >(prop->data);
    if (self && !self->mAppMenuItems.isEmpty()) {
        self->mAppMenuItems.clear();
        Q_EMIT self->applicationMenuItemsChanged();
    }
}

QRectF RibbonBar::titleRect() const
{
    return QRectF(mTitleRect);
}

int RibbonBar::minimumWidth() const
{
    return mMinimumWidth;
}

void RibbonBar::itemChange(ItemChange change, const ItemChangeData& data)
{
    if (change == QQuickItem::ItemChildAddedChange) {
        // declarative children arrive here in DECLARATION order; sibling
        // componentComplete runs in reverse creation order, so registration
        // must happen here to keep the tab/category sequence correct
        if (RibbonCategory* c = qobject_cast< RibbonCategory* >(data.item)) {
            registerCategory(c);
        } else if (RibbonTab* t = qobject_cast< RibbonTab* >(data.item)) {
            // owned context tabs ride the same child-added notification —
            // they must NOT join the normal tab row registration
            if (!isOwnedContextTab(t)) {
                registerTab(t);
            }
        } else if (RibbonContextCategory* ctx = qobject_cast< RibbonContextCategory* >(data.item)) {
            registerContext(ctx);
        } else if (RibbonQuickAccessBar* qab = qobject_cast< RibbonQuickAccessBar* >(data.item)) {
            if (!mQuickAccessBar) {
                mQuickAccessBar = qab;
                connect(qab, &RibbonQuickAccessBar::rowWidthChanged, this, [this]() { polish(); });
                polish();
            }
        } else if (RibbonButtonGroup* grp = qobject_cast< RibbonButtonGroup* >(data.item)) {
            if (!mRightButtonGroup) {
                mRightButtonGroup = grp;
                connect(grp, &RibbonButtonGroup::rowWidthChanged, this, [this]() { polish(); });
                polish();
            }
        } else if (RibbonApplicationWindow* aw = qobject_cast< RibbonApplicationWindow* >(data.item)) {
            if (!mApplicationWindow) {
                mApplicationWindow = aw;
                // inner-content close() routes to the leaf popup
                connect(aw, &RibbonApplicationWindow::closeRequested, this, [this]() { requestApplicationWindowClose(); });
                // the leaf reparents the item VISUALLY into its overlay
                // presentation on open (QML parent assignment moves only the
                // parentItem, the QObject parent stays the bar); destruction
                // is the only event that must revoke the registration
                connect(aw, &QObject::destroyed, this, [this, aw]() {
                    if (mApplicationWindow == aw) {
                        mApplicationWindow = nullptr;
                        Q_EMIT applicationWindowChanged();
                    }
                });
                Q_EMIT applicationWindowChanged();
            }
        }
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonCategory* c = qobject_cast< RibbonCategory* >(data.item)) {
            unregisterCategory(c);
        } else if (RibbonTab* t = qobject_cast< RibbonTab* >(data.item)) {
            if (!isOwnedContextTab(t)) {
                unregisterTab(t);
            }
        } else if (RibbonContextCategory* ctx = qobject_cast< RibbonContextCategory* >(data.item)) {
            unregisterContext(ctx);
        } else if (data.item == mQuickAccessBar) {
            mQuickAccessBar = nullptr;
            polish();
        } else if (data.item == mRightButtonGroup) {
            mRightButtonGroup = nullptr;
            polish();
        } else if (data.item == mApplicationWindow) {
            // a purely visual removal (the leaf's overlay presentation keeps
            // the QObject parent) must NOT unregister — only a real removal
            // (Loader teardown, dynamic reparent) does; destruction itself is
            // covered by the destroyed() hook registered on adoption
            if (data.item->parent() != this) {
                mApplicationWindow = nullptr;
                Q_EMIT applicationWindowChanged();
            }
        }
    } else if (change == QQuickItem::ItemVisibleHasChanged) {
        polish();
    } else if (change == QQuickItem::ItemSceneChange && data.window) {
        // the bar entered (or moved to) a window: attach the frameless agent
        // now that a QQuickWindow exists (QWK setup needs it). The title and
        // screen tracking live here (not inside the frameless-only attach)
        // because native-frame windows feed the same minimum-width rules:
        // the window title is the title reservation, the screen's available
        // width caps it. Handlers are idempotent, so a re-entry into the
        // same window only stacks harmless no-op calls
        setWindowTitle(data.window->title());
        connect(data.window, &QWindow::windowTitleChanged, this, [this](const QString& t) { setWindowTitle(t); });
        connect(data.window, &QWindow::screenChanged, this, [this]() { polish(); });
        attachWindowAgent();
    }
    RibbonQuickHost::itemChange(change, data);
}

void RibbonBar::placeTitleRowHosts(int titleH, int tabBarY, int tabH, int appBtnW, int systemStripW)
{
    if (mTabOnTitle) {
        // compact (WPS): the tab row rides the title row and shares it with
        // the right-side hosts (widgets resizeInCompactStyle parity) — the
        // right button group anchors at the strip edge and the quick access
        // bar joins it from the left, so the two read as one right-aligned
        // toolbar while the tabs keep whatever strip remains after the
        // application button
        const int rightEdge = int(width()) - systemStripW - 8;
        if (mRightButtonGroup) {
            const int w = mRightButtonGroup->rowWidth();
            mRightButtonGroup->setPosition(QPointF(qreal(qMax(rightEdge - w, 0)), 0));
            mRightButtonGroup->setSize(QSizeF(w, titleH));
        }
        if (mQuickAccessBar) {
            const int w = mQuickAccessBar->rowWidth();
            const int x = rightEdge - (mRightButtonGroup ? mRightButtonGroup->rowWidth() + 6 : 0) - w;
            mQuickAccessBar->setPosition(QPointF(qreal(qMax(x, appBtnW + 8)), 0));
            mQuickAccessBar->setSize(QSizeF(w, titleH));
        }
    } else {
        // loose (office): the quick access row stays after the application
        // button on the title row (widgets resizeInLooseStyle parity), and
        // the right button group rides the tab row's right end — BELOW the
        // system button strip, which only spans the title row. The strip is
        // therefore NOT subtracted here (widgets resizeInLooseStyle keeps
        // endX at the bar edge; the strip only feeds the title free rect),
        // otherwise the group floats one strip-width off the right border
        const int rightEdge = int(width()) - 8;
        if (mQuickAccessBar) {
            const int x = (appBtnW > 0 ? appBtnW : 0) + 8;
            mQuickAccessBar->setPosition(QPointF(x, 0));
            mQuickAccessBar->setSize(QSizeF(mQuickAccessBar->rowWidth(), titleH));
        }
        if (mRightButtonGroup) {
            const int w = mRightButtonGroup->rowWidth();
            mRightButtonGroup->setPosition(QPointF(qreal(qMax(rightEdge - w, 0)), tabBarY));
            mRightButtonGroup->setSize(QSizeF(w, tabH));
        }
    }
}

void RibbonBar::updatePolish()
{
    relayout();
}

RibbonTab* RibbonBar::createAutoTab(int index)
{
    // C++-created host (no QML context of its own): createVisualLeaf resolves
    // the engine through the parentItem chain (SARibbonQmlTypes.cpp fallback).
    // componentComplete() never runs for C++-created items, so the leaf is
    // created explicitly here instead.
    RibbonTab* tab = new RibbonTab();
    tab->setParent(this);
    tab->setParentItem(this);
    if (index < mCategories.size()) {
        tab->setText(mCategories[ index ]->title());
    }
    registerTab(tab);
    tab->ensureQmlLeaf();
    return tab;
}

void RibbonBar::syncTabCount()
{
    // one tab row entry per max(explicit tabs, categories): the tail beyond
    // the explicit declarations is auto-generated and bound to category titles
    const int needed = qMax(mTabs.size() - mAutoTabs.size(), mCategories.size());
    while (mTabs.size() < needed) {
        RibbonTab* tab = createAutoTab(mTabs.size());
        mAutoTabs.append(tab);
    }
    // shrink: drop trailing auto tabs that lost their category
    while (mTabs.size() > needed && !mAutoTabs.isEmpty() && mAutoTabs.last() == mTabs.last()) {
        RibbonTab* tab = mAutoTabs.takeLast();
        mTabs.removeLast();
        disconnect(tab, &RibbonTab::clicked, this, nullptr);
        tab->setParentItem(nullptr);
        tab->setParent(nullptr);
        tab->deleteLater();
    }
    // NOTE: currentIndex clamping happens in relayout against the EFFECTIVE
    // tab row (normal + active context tabs) — clamping here would wrongly
    // pull the index back while a context category is showing
}

void RibbonBar::attachWindowAgent()
{
    if (!mWindowAgent) {
        return;
    }
    QQuickWindow* win = window();
    if (!win) {
        return;  // not in a scene yet; itemChange(ItemSceneChange) retries
    }
    if (mWindowAgent->window() == win) {
        return;
    }
    mWindowAgent->setup(win);
    if (mWindowAgent->window()) {
        // THIS bar is the draggable title bar (widgets helper->setTitleBar
        // parity): QWK turns the whole bar rect into HTCAPTION, and every
        // interactive child must be registered as hit-test visible to
        // receive Qt mouse events again (widgets setHitTestVisible calls in
        // SARibbonMainWindow::setRibbonBar parity). Registered below:
        //   - tabs (relayout registers each effective tab)
        //   - every category page, main row and context pages alike (the
        //     panel area; widgets ribbonStackedWidget parity)
        //   - quick access bar / right button group (title row hosts)
        //   - application button (the leaf rect is interactive)
        //   - the leaf's system button row (RibbonWindowButtonRow)
        mWindowAgent->setTitleBarItem(this);
        setWindowTitle(win->title());
        // keep the leaf's title text in sync with the window title (the
        // ApplicationWindow `title` property is what users edit). The
        // connection itself lives in itemChange(ItemSceneChange) so
        // native-frame windows get the same sync
        // mirror the ribbon theme onto the DWM frame so the window border
        // and caption follow dark/light palettes
        connect(RibbonTheme::instance(), &RibbonTheme::paletteChanged, this, [this]() {
            if (mWindowAgent) {
                mWindowAgent->setDarkMode(RibbonTheme::instance()->isDark());
            }
        });
        mWindowAgent->setDarkMode(RibbonTheme::instance()->isDark());
        // initial interactive set (relayout keeps it fresh afterwards)
        syncHitTestVisible();
        Q_EMIT framelessActiveChanged();
    }
}

void RibbonBar::detachWindowAgent()
{
    if (!mWindowAgent) {
        return;
    }
    disconnect(mWindowAgent, nullptr, this, nullptr);
    mWindowAgent->release();
    mWindowAgent = nullptr;
    setSystemButtonStripWidth(0);
    Q_EMIT framelessActiveChanged();
    Q_EMIT windowAgentChanged();
}

void RibbonBar::syncHitTestVisible()
{
    if (!mWindowAgent || !mWindowAgent->window()) {
        return;
    }
    // interactive hosts (widgets setHitTestVisible parity): tabs ride the
    // title row in compact styles and the tab row below it in loose ones —
    // register every effective tab either way, plus the title-row hosts.
    // NOTE: the leaf itself must NOT be registered — QWK excludes the whole
    // hit-test item geometry from the draggable area, and the leaf spans the
    // entire bar; registering it would kill the title drag everywhere.
    // Leaf-internal interactive zones (the app button MouseArea, the system
    // button row) are registered individually below by objectName lookup.
    for (RibbonTab* tab : effectiveTabs()) {
        if (tab) {
            mWindowAgent->setHitTestVisible(tab);
        }
    }
    // the whole category row is client area (widgets setHitTestVisible(
    // ribbonStackedWidget) parity): every category page — main row and
    // context pages alike — is registered wholesale, which hands panels,
    // galleries and all their controls back to the Qt event domain. QWK
    // checks item visibility at hit-test time, so pages register once and
    // are covered whenever they become the current page. Blank spaces in
    // the category row are NOT draggable, exactly like the widgets module
    // (and Office, where the ribbon body does not move the window)
    for (RibbonCategory* cat : mCategories) {
        mWindowAgent->setHitTestVisible(cat);
    }
    for (RibbonContextCategory* ctx : mContexts) {
        for (RibbonCategory* page : ctx->categories()) {
            mWindowAgent->setHitTestVisible(page);
        }
    }
    if (mQuickAccessBar) {
        mWindowAgent->setHitTestVisible(mQuickAccessBar);
    }
    if (mRightButtonGroup) {
        mWindowAgent->setHitTestVisible(mRightButtonGroup);
    }
    if (QQuickItem* leaf = qmlLeaf()) {
        // leaf-internal interactive zones, marked by objectName in RibbonBar.qml
        for (const char* name : { "sysButtonRow", "appButtonArea" }) {
            if (QQuickItem* zone = leaf->findChild< QQuickItem* >(QString::fromLatin1(name))) {
                mWindowAgent->setHitTestVisible(zone);
            }
        }
    }
}

void RibbonBar::relayout()
{
    if (width() <= 0) {
        return;
    }
    syncTabCount();

    RibbonMetrics* metrics = RibbonMetrics::instance();
    const QFontMetrics fm  = metrics->coreMetrics().fontMetrics();
    const int tabH         = metrics->tabBarHeight();
    const int titleH       = metrics->titleBarHeight();
    // row-count aware category height (core calcCategoryHeight: single row
    // hides the panel title strip, hence the shorter body)
    const int styleRows    = styleRowCount(mRibbonStyle);
    const int catH         = metrics->categoryHeight(styleRows >= 3, styleRows <= 1);

    // 0. application button: spans the title row + the tab row (widgets
    // vertically-expanding app button), office-2021 tab-row-left placement;
    // with tabs on the title (compact styles) it spans the title row only
    const bool hasAppButton = !mApplicationLabel.isEmpty();
    int appBtnW = 0;
    const int tabBarY = mTabOnTitle ? qMax(titleH - tabH, 0) : titleH;
    if (hasAppButton) {
        appBtnW = qMax(50, fm.horizontalAdvance(mApplicationLabel) + 30);
        mApplicationButtonRect = QRectF(0, 1, appBtnW, tabBarY + tabH - 2);
    } else {
        mApplicationButtonRect = QRectF();
    }

    // 1. effective tab row = normal tabs + active context page tabs (context
    //    tabs appended in declaration order, widgets showContextCategory
    //    parity: appended at the end of the tab bar)
    const QVector< RibbonTab* > effTabs = effectiveTabs();
    const QVector< RibbonCategory* > effCats = effectiveCategories();
    // hide owned context tabs that are not on the effective row right now
    for (auto it = mContextTabs.constBegin(); it != mContextTabs.constEnd(); ++it) {
        const bool active = it.key()->isActive();
        for (RibbonTab* tab : it.value()) {
            tab->setVisible(active);
        }
    }
    if (mCurrentIndex >= effTabs.size() && !effTabs.isEmpty()) {
        mCurrentIndex = effTabs.size() - 1;
        Q_EMIT currentIndexChanged();
    }

    // 2. tab row geometry: plain sequence (no Repeater), starting after the
    //    app button; text-driven width (widgets tabSizeHint: text width +
    //    hspace, min 50, office-2021 QSS adds 5+5 margins). Compact styles
    //    ride the title row (tabOnTitle, widgets parity). The row can be
    //    aligned left/center/right inside the free strip (widgets
    //    setRibbonAlignment parity; front-end tab-row geometry, core engine
    //    stays authoritative for the title free area)
    const int tabSpacing = 2;
    // total row width first (alignment offset needs it)
    int rowWidth = -tabSpacing;
    for (int i = 0; i < effTabs.size(); ++i) {
        const int textW = fm.horizontalAdvance(effTabs[ i ]->text());
        rowWidth += qMax(50, textW + 24) + tabSpacing;
    }
    // right-side reservation keeps the tab row clear of the right hosts
    // (widgets tabBarWidth = endX - x parity): the right button group rides
    // the tab row in loose styles too, while compact styles additionally
    // share the row with the quick access bar. The frameless strip joins
    // the reservation only in compact styles — their tab row rides the
    // title row the strip spans, while the loose tab row sits below the
    // strip and must run to the bar's right edge (native frame: 0 anyway)
    int reservedRight = 4 + (mRightButtonGroup ? mRightButtonGroup->rowWidth() + 8 : 0);
    if (mTabOnTitle) {
        reservedRight += mSystemButtonStripWidth;
        if (mQuickAccessBar) {
            reservedRight += mQuickAccessBar->rowWidth() + 6;
        }
    }
    // widgets setRibbonAlignment parity: the row shifts inside the free strip
    // (left = after the app button, center/right = shifted; front-end tab-row
    // geometry — the core engine stays authoritative for the title free area)
    const int stripBegin = (hasAppButton ? appBtnW : 0) + 4;
    const int stripEnd   = int(width()) - reservedRight;
    int x = stripBegin;
    if (stripEnd > stripBegin && rowWidth < stripEnd - stripBegin) {
        switch (mTabAlignment) {
        case RibbonEnums::AlignCenter:
            x = stripBegin + (stripEnd - stripBegin - rowWidth) / 2;
            break;
        case RibbonEnums::AlignRight:
            x = stripEnd - rowWidth;
            break;
        case RibbonEnums::AlignLeft:
        default:
            break;
        }
    }
    QVector< QRectF > tabRects;
    tabRects.reserve(effTabs.size());
    for (int i = 0; i < effTabs.size(); ++i) {
        RibbonTab* tab = effTabs[ i ];
        const int textW = fm.horizontalAdvance(tab->text());
        const int tabW  = qMax(50, textW + 24);
        tab->setCurrent(i == mCurrentIndex);
        tab->setPosition(QPointF(x, tabBarY));
        tab->setSize(QSizeF(tabW, tabH));
        tabRects.append(QRectF(x, tabBarY, tabW, tabH));
        x += tabW + tabSpacing;
    }

    // 3. current category below the tab row (context pages included: they are
    //    children of the context item which sits at the bar origin, so the
    //    coordinates are already bar-relative). Minimum mode hides the whole
    //    category row (widgets setMinimumMode parity: only title + tabs stay)
    const int categoryY = tabBarY + tabH;
    // user-hidden categories are off the effective row, hence off the display
    for (RibbonCategory* cat : mHiddenCategories) {
        cat->setVisible(false);
    }
    for (int i = 0; i < effCats.size(); ++i) {
        RibbonCategory* cat = effCats[ i ];
        cat->setVisible(!mMinimumMode && i == mCurrentIndex);
        cat->setPosition(QPointF(0, categoryY));
        cat->setSize(QSizeF(width(), catH));
    }
    // context pages that dropped off the effective row must hide too
    for (RibbonContextCategory* ctx : mContexts) {
        if (!ctx->isActive()) {
            for (RibbonCategory* page : ctx->categories()) {
                page->setVisible(false);
            }
        }
    }
    setImplicitHeight(mMinimumMode ? categoryY : categoryY + catH);

    // 4. context bands: one per ACTIVE context, spanning its tabs (first tab
    //    left .. last tab right) from the bar top through the tab row; the
    //    highlight color comes from the SAME core fp the widgets ThemeManager
    //    installs (themeContextHighlight), the text color by luminance
    QVariantList bands;
    for (RibbonContextCategory* ctx : mContexts) {
        if (!ctx->isActive() || ctx->categories().isEmpty()) {
            continue;
        }
        const QVector< RibbonTab* >& tabs = mContextTabs.value(ctx);
        if (tabs.isEmpty()) {
            continue;
        }
        int first = -1, last = -1;
        for (RibbonTab* tab : tabs) {
            const int idx = effTabs.indexOf(tab);
            if (idx >= 0) {
                if (first < 0) {
                    first = idx;
                }
                last = idx;
            }
        }
        if (first < 0) {
            continue;
        }
        const QColor base = ctx->contextColor();
        SARibbon::Core::SARibbonFpContextCategoryHighlight fp
            = SARibbon::Core::SARibbonThemeData::themeContextHighlight(
                SARibbon::Core::SARibbonThemeData::instance()->theme());
        const QColor highlight = fp ? fp(base) : base;
        QVariantMap band;
        band[ QStringLiteral("x") ]        = tabRects[ first ].left();
        band[ QStringLiteral("width") ]    = tabRects[ last ].right() - tabRects[ first ].left();
        band[ QStringLiteral("title") ]    = ctx->contextTitle();
        band[ QStringLiteral("color") ]    = base;
        band[ QStringLiteral("highlight") ] = highlight;
        band[ QStringLiteral("textColor") ] = (base.lightness() > 150) ? QColor(Qt::black) : QColor(Qt::white);
        bands.append(band);
    }
    mBands = bands;

    // 5. title free area: core engine (TitleRectInput); the quick access
    //    row and the right group sit on the title row
    placeTitleRowHosts(titleH, tabBarY, tabH, appBtnW, mSystemButtonStripWidth);
    SARibbon::Core::SARibbonBarGeometryEngine::TitleRectInput input;
    input.isRTL               = SA::saIsRTL();
    input.isCompactStyle      = false;
    input.ribbonWidth         = int(width());
    input.border              = QMargins(0, 0, 0, 0);
    input.validTitleBarHeight = titleH;
    input.hasQuickAccessBar   = (mQuickAccessBar != nullptr);
    input.systemButtonSize    = QSize(mSystemButtonStripWidth, titleH);
    input.hasContextTabs      = !mBands.isEmpty();
    input.tabBarGeometry      = QRect(8, tabBarY, qMin(x - 8 - tabSpacing, int(width()) - 8), tabH);
    mTitleRect = SARibbon::Core::SARibbonBarGeometryEngine::layoutTitleRect(input);

    // 6. window minimum width (core calcMinimumWidth): the tab-row zone
    //    minimum reuses the exact placement sums above (stripBegin +
    //    rowWidth + reservedRight), the loose title-row zone adds the quick
    //    access row after the application button plus the system strip; the
    //    compact style shares one row, so the title rides inside it. The
    //    screen rules (title sacrifice, 2/3 overlap cap) apply in core with
    //    the window screen's available width
    SARibbon::Core::SARibbonBarGeometryEngine::MinimumWidthInput minInput;
    minInput.isCompactStyle = mTabOnTitle;
    minInput.tabRowWidth    = stripBegin + rowWidth + reservedRight;
    if (!mTabOnTitle) {
        minInput.titleRowWidth = (hasAppButton ? appBtnW : 0)
                                 + (mQuickAccessBar ? 8 + mQuickAccessBar->rowWidth() : 0)
                                 + mSystemButtonStripWidth;
    }
    minInput.titleTextWidth = fm.horizontalAdvance(mWindowTitle);
    if (QQuickWindow* win = window()) {
        if (QScreen* scr = win->screen()) {
            minInput.screenAvailableWidth = scr->availableGeometry().width();
        }
    }
    const int newMinimumWidth = SARibbon::Core::SARibbonBarGeometryEngine::calcMinimumWidth(minInput);
    if (newMinimumWidth != mMinimumWidth) {
        mMinimumWidth = newMinimumWidth;
        Q_EMIT minimumWidthChanged();
    }

    mTabBarHeight   = tabH;
    mTitleBarHeight = titleH;
    mCategoryRowY   = categoryY;
    Q_EMIT layoutChanged();
    // keep the frameless hit-test registration in step with the new tab row
    syncHitTestVisible();
}

}
