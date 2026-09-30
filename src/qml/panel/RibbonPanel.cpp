#include "RibbonPanel.h"
#include "../button/RibbonToolButton.h"
#include "../category/RibbonCategory.h"
#include "../metrics/RibbonMetrics.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QQuickItem>

namespace SARibbonQml {

RibbonPanel::RibbonPanel(QQuickItem* parent) : QQuickItem(parent)
{
}

RibbonPanel::~RibbonPanel()
{
    // leaf destruction: unparent + deleteLater, NEVER direct delete (a QML item may
    // sit inside its own mouse-handling call stack, KDDW Group.cpp same rule)
    if (mPanelQmlItem) {
        mPanelQmlItem->setParentItem(nullptr);
        mPanelQmlItem->setParent(nullptr);
        mPanelQmlItem->deleteLater();
        mPanelQmlItem = nullptr;
    }
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

QQuickItem* RibbonPanel::panelQmlItem() const
{
    return mPanelQmlItem;
}

void RibbonPanel::setPanelQmlItem(QQuickItem* item)
{
    if (mPanelQmlItem == item) {
        return;
    }
    mPanelQmlItem = item;
    Q_EMIT panelQmlItemChanged();
}

void RibbonPanel::registerChildItem(RibbonToolButton* item)
{
    if (!mChildButtons.contains(item)) {
        mChildButtons.append(item);
        polish();
    }
}

void RibbonPanel::unregisterChildItem(RibbonToolButton* item)
{
    if (mChildButtons.removeOne(item)) {
        mEngine.removeFromCache(item);
        polish();
    }
}

void RibbonPanel::invalidateChildCache(RibbonToolButton* item)
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
    QQuickItem::componentComplete();
    ensureQmlItem();
    polish();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonPanel::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
#else
void RibbonPanel::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.size() != oldGeometry.size()) {
        polish();  // the category host sizes this item: re-run the engine on resize
    }
}

void RibbonPanel::ensureQmlItem()
{
    if (mPanelQmlItem) {
        return;
    }
    QQuickItem* leaf = createVisualLeaf(this, SARibbonQmlLeafUrls::panelLeaf(), "panelCpp");
    if (leaf && !mPanelQmlItem) {
        setPanelQmlItem(leaf);  // handshake assigns it; fallback keeps the pair intact
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
        if (RibbonToolButton* btn = qobject_cast< RibbonToolButton* >(data.item)) {
            registerChildItem(btn);
        }
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonToolButton* btn = qobject_cast< RibbonToolButton* >(data.item)) {
            unregisterChildItem(btn);
        }
    } else if (change == QQuickItem::ItemVisibleHasChanged) {
        polish();
    }
    QQuickItem::itemChange(change, data);
}

void RibbonPanel::updatePolish()
{
    runLayout();
}

void RibbonPanel::runLayout()
{
    if (mChildButtons.isEmpty()) {
        return;
    }
    SARibbon::Core::SARibbonPanelLayoutEngine::Input input;
    input.rowCount        = rowCountForMode();
    input.showPanelTitle  = !mPanelTitle.isEmpty();
    input.hasTitleLabel   = true;
    input.hasOptionAction = false;
    input.isRTL           = SA::saIsRTL();
    input.contentsMargins = QMargins(1, 1, 1, 1);
    input.spacing         = 2;
    input.titleTextWidth  = -1;  // no option button in P0; title width only feeds min width
    input.optionBtnSize   = QSize();
    input.titleHeight     = RibbonMetrics::instance()->panelTitleHeight();
    input.titleSpace      = 2;
    input.fontMetrics     = RibbonMetrics::instance()->coreMetrics().fontMetrics();
    input.previousSizeHintWidth = mLastSizeHint.width();

    QVector< SARibbon::Core::SARibbonAbstractLayoutItem* > items;
    items.reserve(mChildButtons.size());
    for (RibbonToolButton* b : mChildButtons) {
        items.append(b);
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
    // C++ button hints (metrics-driven, rect-independent), and the category reads
    // implicitWidth as its layout hint — gating this on our own size would
    // deadlock the hint chain (category waits for the panel, panel for category)
    setImplicitWidth(qMax(qreal(r.sizeHint.width()), width()));
    setImplicitHeight(qreal(r.sizeHint.height()));

    if (width() <= 0 || height() <= 0) {
        return;  // degenerate rect: hints published, button placement skipped
    }
    // apply engine outputs to the REAL quick items
    for (RibbonToolButton* b : mChildButtons) {
        if (!b->isHidden()) {
            b->applyGeometry(b->resultGeometry);
        }
    }
}

}
