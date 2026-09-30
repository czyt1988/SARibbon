#include "RibbonBar.h"
#include "../category/RibbonCategory.h"
#include "../tab/RibbonTab.h"
#include "../metrics/RibbonMetrics.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>

namespace SARibbonQml {

RibbonBar::RibbonBar(QQuickItem* parent) : QQuickItem(parent)
{
}

RibbonBar::~RibbonBar()
{
    // leaf destruction: unparent + deleteLater, NEVER direct delete (a QML item may
    // sit inside its own mouse-handling call stack, KDDW Group.cpp same rule)
    if (mBarQmlItem) {
        mBarQmlItem->setParentItem(nullptr);
        mBarQmlItem->setParent(nullptr);
        mBarQmlItem->deleteLater();
        mBarQmlItem = nullptr;
    }
}

void RibbonBar::componentComplete()
{
    QQuickItem::componentComplete();
    // children (tabs/categories) complete before the parent, so the register
    // lists are already filled here
    ensureQmlItem();
    polish();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonBar::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
#else
void RibbonBar::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.size() != oldGeometry.size()) {
        polish();  // width comes from anchors/consumer: relayout on every resize
    }
}

void RibbonBar::ensureQmlItem()
{
    if (mBarQmlItem) {
        return;
    }
    QQuickItem* leaf = createVisualLeaf(this, SARibbonQmlLeafUrls::barLeaf(), "barCpp");
    if (leaf && !mBarQmlItem) {
        setBarQmlItem(leaf);  // handshake assigns it; fallback keeps the pair intact
    }
}
int RibbonBar::currentIndex() const
{
    return mCurrentIndex;
}

void RibbonBar::setCurrentIndex(int idx)
{
    if (mCurrentIndex == idx || idx < 0 || (!mCategories.isEmpty() && idx >= mCategories.size())) {
        return;
    }
    mCurrentIndex = idx;
    Q_EMIT currentIndexChanged();
    polish();
}

QQuickItem* RibbonBar::barQmlItem() const
{
    return mBarQmlItem;
}

void RibbonBar::setBarQmlItem(QQuickItem* item)
{
    if (mBarQmlItem == item) {
        return;
    }
    mBarQmlItem = item;
    Q_EMIT barQmlItemChanged();
}

void RibbonBar::registerCategory(RibbonCategory* category)
{
    if (!mCategories.contains(category)) {
        mCategories.append(category);
        category->setParentItem(this);
        polish();
    }
}

void RibbonBar::unregisterCategory(RibbonCategory* category)
{
    if (mCategories.removeOne(category)) {
        polish();
    }
}

void RibbonBar::registerTab(RibbonTab* tab)
{
    if (!mTabs.contains(tab)) {
        mTabs.append(tab);
        tab->setParentItem(this);
        // clicking a tab selects the category of the same index
        connect(tab, &RibbonTab::clicked, this, [this, tab]() {
            const int idx = mTabs.indexOf(tab);
            if (idx >= 0) {
                setCurrentIndex(idx);
            }
        });
        polish();
    }
}

void RibbonBar::unregisterTab(RibbonTab* tab)
{
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
    QQuickItem::itemChange(change, data);
}

void RibbonBar::updatePolish()
{
    relayout();
}

void RibbonBar::relayout()
{
    if (width() <= 0) {
        return;
    }
    RibbonMetrics* metrics = RibbonMetrics::instance();
    const int tabH   = metrics->tabBarHeight();
    const int titleH = metrics->titleBarHeight();
    const int catH   = metrics->categoryHeight();

    // 1. tab row: plain sequence (no Repeater), from the left after 8px margin
    const int tabBarY = titleH;
    const int tabSpacing = 2;
    int x = 8;
    for (int i = 0; i < mTabs.size(); ++i) {
        RibbonTab* tab = mTabs[ i ];
        const int tabW = 60 + 8;  // fixed P0 tab width (text-driven sizing: 3.1+)
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
}

}
