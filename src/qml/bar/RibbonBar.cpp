#include "RibbonBar.h"
#include "../category/RibbonCategory.h"
#include "../tab/RibbonTab.h"
#include "../metrics/RibbonMetrics.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QFontMetrics>

namespace SARibbonQml {

RibbonBar::RibbonBar(QQuickItem* parent) : RibbonQuickHost(parent)
{
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
    // bound by the TAB row (the tab row may be longer than the category list
    // when tabs are declared beyond the categories; those show an empty page
    // exactly like a QTabBar with more pages than stacked widgets)
    if (mCurrentIndex == idx || idx < 0 || (!mTabs.isEmpty() && idx >= mTabs.size())) {
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
            registerTab(t);
        }
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonCategory* c = qobject_cast< RibbonCategory* >(data.item)) {
            unregisterCategory(c);
        } else if (RibbonTab* t = qobject_cast< RibbonTab* >(data.item)) {
            unregisterTab(t);
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
    if (mCurrentIndex >= mTabs.size() && !mTabs.isEmpty()) {
        mCurrentIndex = mTabs.size() - 1;
        Q_EMIT currentIndexChanged();
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

    // 1. tab row: plain sequence (no Repeater), starting after the app button;
    //    text-driven width (widgets tabSizeHint: text width + hspace, min 50,
    //    office-2021 QSS adds 5+5 margins)
    const int tabBarY = titleH;
    const int tabSpacing = 2;
    int x = (hasAppButton ? appBtnW : 0) + 4;
    for (int i = 0; i < mTabs.size(); ++i) {
        RibbonTab* tab = mTabs[ i ];
        const int textW = fm.horizontalAdvance(tab->text());
        const int tabW  = qMax(50, textW + 24);
        tab->setCurrent(i == mCurrentIndex);
        tab->setPosition(QPointF(x, tabBarY));
        tab->setSize(QSizeF(tabW, tabH));
        x += tabW + tabSpacing;
    }

    // 2. current category below the tab row
    const int categoryY = tabBarY + tabH;
    for (int i = 0; i < mCategories.size(); ++i) {
        RibbonCategory* cat = mCategories[ i ];
        cat->setVisible(i == mCurrentIndex);
        cat->setPosition(QPointF(0, categoryY));
        cat->setSize(QSizeF(width(), catH));
    }
    setImplicitHeight(categoryY + catH);

    // 3. title free area: core engine (TitleRectInput)
    SARibbon::Core::SARibbonBarGeometryEngine::TitleRectInput input;
    input.isRTL               = SA::saIsRTL();
    input.isCompactStyle      = false;
    input.ribbonWidth         = int(width());
    input.border              = QMargins(0, 0, 0, 0);
    input.validTitleBarHeight = titleH;
    input.hasQuickAccessBar   = false;
    input.systemButtonSize    = QSize(120, titleH);  // P0: reserved right strip
    input.hasContextTabs      = false;
    input.tabBarGeometry      = QRect(8, tabBarY, qMin(x - 8 - tabSpacing, int(width()) - 8), tabH);
    mTitleRect = SARibbon::Core::SARibbonBarGeometryEngine::layoutTitleRect(input);

    mTabBarHeight   = tabH;
    mTitleBarHeight = titleH;
    mCategoryRowY   = categoryY;
    Q_EMIT layoutChanged();
}

}
