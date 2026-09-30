#include "RibbonCategory.h"
#include "../panel/RibbonPanel.h"
#include "../button/RibbonToolButton.h"
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
        polish();
    }
}

void RibbonCategory::unregisterPanel(RibbonPanel* panel)
{
    if (mPanels.removeOne(panel)) {
        polish();
    }
}

int RibbonCategory::contentWidth() const
{
    return mTotalWidth;
}

void RibbonCategory::itemChange(ItemChange change, const ItemChangeData& data)
{
    if (change == QQuickItem::ItemChildRemovedChange) {
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
