#include "RibbonBar.h"
#include "../category/RibbonCategory.h"
#include "../context/RibbonContextCategory.h"
#include "../tab/RibbonTab.h"
#include "../metrics/RibbonMetrics.h"
#include "../theme/RibbonTheme.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <SARibbonCore/SARibbonThemeData.h>
#include <QFontMetrics>

namespace SARibbonQml {

RibbonBar::RibbonBar(QQuickItem* parent) : RibbonQuickHost(parent)
{
    // band highlight follows the theme (same core fp the widgets ThemeManager
    // installs); recompute the published band data on theme switches
    connect(SARibbon::Core::SARibbonThemeData::instance(), &SARibbon::Core::SARibbonThemeData::themeChanged, this,
            [this]() { polish(); });
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
    polish();
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
    int count = mTabs.size();
    for (RibbonContextCategory* ctx : mContexts) {
        if (ctx->isActive()) {
            count += ctx->categories().size();
        }
    }
    return count;
}

QVector< RibbonTab* > RibbonBar::effectiveTabs() const
{
    QVector< RibbonTab* > tabs = mTabs;
    for (RibbonContextCategory* ctx : mContexts) {
        if (ctx->isActive()) {
            tabs += mContextTabs.value(ctx);
        }
    }
    return tabs;
}

QVector< RibbonCategory* > RibbonBar::effectiveCategories() const
{
    QVector< RibbonCategory* > cats = mCategories;
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

QRectF RibbonBar::titleRect() const
{
    return QRectF(mTitleRect);
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
        }
    } else if (change == QQuickItem::ItemVisibleHasChanged) {
        polish();
    }
    RibbonQuickHost::itemChange(change, data);
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
    const int catH         = metrics->categoryHeight();

    // 0. application button: spans the title row + the tab row (widgets
    // vertically-expanding app button), office-2021 tab-row-left placement
    const bool hasAppButton = !mApplicationLabel.isEmpty();
    int appBtnW = 0;
    if (hasAppButton) {
        appBtnW = qMax(50, fm.horizontalAdvance(mApplicationLabel) + 30);
        mApplicationButtonRect = QRectF(0, 1, appBtnW, titleH + tabH - 2);
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
    //    hspace, min 50, office-2021 QSS adds 5+5 margins)
    const int tabBarY = titleH;
    const int tabSpacing = 2;
    int x = (hasAppButton ? appBtnW : 0) + 4;
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
    //    coordinates are already bar-relative)
    const int categoryY = tabBarY + tabH;
    for (int i = 0; i < effCats.size(); ++i) {
        RibbonCategory* cat = effCats[ i ];
        cat->setVisible(i == mCurrentIndex);
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
    setImplicitHeight(categoryY + catH);

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

    // 5. title free area: core engine (TitleRectInput)
    SARibbon::Core::SARibbonBarGeometryEngine::TitleRectInput input;
    input.isRTL               = SA::saIsRTL();
    input.isCompactStyle      = false;
    input.ribbonWidth         = int(width());
    input.border              = QMargins(0, 0, 0, 0);
    input.validTitleBarHeight = titleH;
    input.hasQuickAccessBar   = false;
    input.systemButtonSize    = QSize(120, titleH);  // P0: reserved right strip
    input.hasContextTabs      = !mBands.isEmpty();
    input.tabBarGeometry      = QRect(8, tabBarY, qMin(x - 8 - tabSpacing, int(width()) - 8), tabH);
    mTitleRect = SARibbon::Core::SARibbonBarGeometryEngine::layoutTitleRect(input);

    mTabBarHeight   = tabH;
    mTitleBarHeight = titleH;
    mCategoryRowY   = categoryY;
    Q_EMIT layoutChanged();
}

}
