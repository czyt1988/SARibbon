#include "RibbonPanel.h"
#include "../host/RibbonLayoutItemHost.h"
#include "../category/RibbonCategory.h"
#include "../metrics/RibbonMetrics.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QQuickItem>

namespace SARibbonQml {

RibbonPanel::RibbonPanel(QQuickItem* parent) : RibbonQuickHost(parent)
{
}

RibbonPanel::~RibbonPanel()
{
}

QUrl RibbonPanel::leafUrl() const
{
    return SARibbonQmlLeafUrls::panelLeaf();
}

QString RibbonPanel::panelTitle() const
{
    return mPanelTitle;
}

void RibbonPanel::setPanelTitle(const QString& t)
{
    if (mPanelTitle == t) {
        return;
    }
    mPanelTitle = t;
    Q_EMIT panelTitleChanged();
    polish();
}

RibbonEnums::LayoutMode RibbonPanel::layoutMode() const
{
    return mLayoutMode;
}

void RibbonPanel::setLayoutMode(RibbonEnums::LayoutMode mode)
{
    if (mLayoutMode == mode) {
        return;
    }
    mLayoutMode = mode;
    Q_EMIT layoutModeChanged();
    polish();
}

void RibbonPanel::registerChildItem(RibbonLayoutItemHost* item)
{
    if (!mChildItems.contains(item)) {
        mChildItems.append(item);
        polish();
    }
}

void RibbonPanel::unregisterChildItem(RibbonLayoutItemHost* item)
{
    if (mChildItems.removeOne(item)) {
        mEngine.removeFromCache(item);
        polish();
    }
}

void RibbonPanel::invalidateChildCache(SARibbon::Core::SARibbonAbstractLayoutItem* item)
{
    // engine caches button sizeHints keyed by largeHeight only: a text/proportion
    // change must drop the entry explicitly, then a fresh pass repacks the columns
    mEngine.removeFromCache(item);
    polish();
}

int RibbonPanel::rowCountForMode() const
{
    switch (mLayoutMode) {
    case RibbonEnums::ThreeRowMode:
        return 3;
    case RibbonEnums::TwoRowMode:
        return 2;
    case RibbonEnums::SingleRowMode:
        return 1;
    }
    return 3;
}

void RibbonPanel::componentComplete()
{
    RibbonQuickHost::componentComplete();
    ensureQmlLeaf();
    polish();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonPanel::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChange(newGeometry, oldGeometry);
#else
void RibbonPanel::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.size() != oldGeometry.size()) {
        polish();  // the category host sizes this item: re-run the engine on resize
    }
}

QRectF RibbonPanel::titleGeometry() const
{
    return QRectF(mLastTitleGeometry);
}

void RibbonPanel::itemChange(ItemChange change, const ItemChangeData& data)
{
    if (change == QQuickItem::ItemChildAddedChange) {
        // declaration order (sibling componentComplete runs reversed)
        if (RibbonLayoutItemHost* item = qobject_cast< RibbonLayoutItemHost* >(data.item)) {
            registerChildItem(item);
        }
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonLayoutItemHost* item = qobject_cast< RibbonLayoutItemHost* >(data.item)) {
            unregisterChildItem(item);
        }
    } else if (change == QQuickItem::ItemVisibleHasChanged) {
        polish();
    }
    RibbonQuickHost::itemChange(change, data);
}

void RibbonPanel::updatePolish()
{
    runLayout();
}

void RibbonPanel::runLayout()
{
    if (mChildItems.isEmpty()) {
        return;
    }
    SARibbon::Core::SARibbonPanelLayoutEngine::Input input;
    input.rowCount        = rowCountForMode();
    input.showPanelTitle  = !mPanelTitle.isEmpty();
    input.hasTitleLabel   = true;
    input.hasOptionAction = false;
    input.isRTL           = SA::saIsRTL();
    input.contentsMargins = QMargins(2, 2, 2, 2);  // widgets parity (SARibbonPanelLayout)
    input.spacing         = 2;
    input.titleTextWidth  = -1;  // no option button in P0; title width only feeds min width
    input.optionBtnSize   = QSize();
    input.titleHeight     = RibbonMetrics::instance()->panelTitleHeight();
    input.titleSpace      = 2;
    input.fontMetrics     = RibbonMetrics::instance()->coreMetrics().fontMetrics();
    input.previousSizeHintWidth = mLastSizeHint.width();

    QVector< SARibbon::Core::SARibbonAbstractLayoutItem* > items;
    items.reserve(mChildItems.size());
    for (RibbonLayoutItemHost* item : mChildItems) {
        items.append(item);
    }

    SARibbon::Core::SARibbonPanelLayoutEngine::Result r = mEngine.layout(items, QRect(0, 0, int(width()), int(height())), input);

    mLastSizeHint = r.sizeHint;
    mLastColumnCount = r.columnCount;
    mLastLargeHeight = r.largeHeight;
    if (r.titleGeometry != mLastTitleGeometry) {
        mLastTitleGeometry = r.titleGeometry;
        Q_EMIT titleGeometryChanged();
    }
    // publish implicit sizes even at zero geometry: the sizeHint derives from the
    // C++ item hints (metrics-driven, rect-independent), and the category reads
    // implicitWidth as its layout hint — gating this on our own size would
    // deadlock the hint chain (category waits for the panel, panel for category)
    setImplicitWidth(qMax(qreal(r.sizeHint.width()), width()));
    setImplicitHeight(qreal(r.sizeHint.height()));

    if (width() <= 0 || height() <= 0) {
        return;  // degenerate rect: hints published, item placement skipped
    }
    // apply engine outputs to the REAL quick items
    for (RibbonLayoutItemHost* item : mChildItems) {
        if (!item->isHidden()) {
            item->applyGeometry(item->resultGeometry);
        }
    }
    // publish the fresh large row height to every item (widgets parity: the
    // large-proportion sizeHint width depends on the panel's largeButtonHeight,
    // so the width hint must follow the height whenever it changes). A change
    // invalidates the item hints and re-requests polish; the second pass
    // repacks with the final widths (the engine cache is keyed on largeHeight
    // and drops together with this notification)
    for (RibbonLayoutItemHost* item : mChildItems) {
        item->setLargeButtonHeightContext(r.largeHeight);
    }
}

}
