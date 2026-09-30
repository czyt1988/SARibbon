#include "RibbonCategory.h"
#include "../panel/RibbonPanel.h"
#include "../button/RibbonToolButton.h"
#include "../bar/RibbonBar.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>

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

RibbonCategory::RibbonCategory(QQuickItem* parent) : QQuickItem(parent)
{
}

RibbonCategory::~RibbonCategory()
{
    // leaf destruction: unparent + deleteLater, NEVER direct delete (KDDW Group.cpp rule)
    if (mCategoryQmlItem) {
        mCategoryQmlItem->setParentItem(nullptr);
        mCategoryQmlItem->setParent(nullptr);
        mCategoryQmlItem->deleteLater();
        mCategoryQmlItem = nullptr;
    }
}

void RibbonCategory::componentComplete()
{
    QQuickItem::componentComplete();
    ensureQmlItem();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonCategory::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
#else
void RibbonCategory::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.size() != oldGeometry.size()) {
        polish();  // the bar host sizes this item: relayout panels on every resize
    }
}

void RibbonCategory::ensureQmlItem()
{
    if (mCategoryQmlItem) {
        return;
    }
    QQuickItem* leaf = createVisualLeaf(this, SARibbonQmlLeafUrls::categoryLeaf(), "categoryCpp");
    if (leaf && !mCategoryQmlItem) {
        setCategoryQmlItem(leaf);  // handshake assigns it; fallback keeps the pair intact
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

QQuickItem* RibbonCategory::categoryQmlItem() const
{
    return mCategoryQmlItem;
}

void RibbonCategory::setCategoryQmlItem(QQuickItem* item)
{
    if (mCategoryQmlItem == item) {
        return;
    }
    mCategoryQmlItem = item;
    Q_EMIT categoryQmlItemChanged();
}

int RibbonCategory::scrollPosition() const
{
    return mScrollXBase;
}

void RibbonCategory::setScrollPosition(int pos)
{
    // engine-clamped target (plan-04 S4: QML Behavior animates the visual x;
    // the logical position goes through clampScrollOffset)
    const int clamped = SARibbon::Core::clampScrollOffset(pos, mTotalWidth, int(width()), SA::saIsRTL());
    if (mScrollXBase == clamped) {
        return;
    }
    mScrollXBase = clamped;
    Q_EMIT scrollPositionChanged();
    polish();
}

void RibbonCategory::registerPanel(RibbonPanel* panel)
{
    if (!mPanels.contains(panel)) {
        mPanels.append(panel);
        // panel implicit sizes are the layout hints: any change re-runs relayout
        connect(panel, &QQuickItem::implicitWidthChanged, this, [this]() { polish(); });
        connect(panel, &QQuickItem::implicitHeightChanged, this, [this]() { polish(); });
        polish();
    }
}

void RibbonCategory::unregisterPanel(RibbonPanel* panel)
{
    if (mPanels.removeOne(panel)) {
        disconnect(panel, nullptr, this, nullptr);
        polish();
    }
}

int RibbonCategory::contentWidth() const
{
    return mTotalWidth;
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
    QQuickItem::itemChange(change, data);
}

void RibbonCategory::updatePolish()
{
    relayout();
}

void RibbonCategory::relayout()
{
    if (mPanels.isEmpty() || width() <= 0) {
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
