#include "RibbonGallery.h"
#include "RibbonGalleryGroup.h"
#include "RibbonGalleryItem.h"
#include "../metrics/RibbonMetrics.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <QQuickItem>

namespace SARibbonQml {

namespace {
// widgets parity: SARibbonGallery::setGalleryButtonMaximumWidth default 15
constexpr int kButtonStripWidth = 15;
// widgets parity: SARibbonGallery minimum width (addGallery gives it a Large
// column; without expanding distribution this is the base width)
constexpr int kGalleryBaseWidth = 200;
}  // namespace

RibbonGallery::RibbonGallery(QQuickItem* parent) : RibbonLayoutItemHost(parent)
{
    // widgets addGallery parity: the gallery rides a Large cell and expands
    // horizontally into the panel's extra width
    rowProportion = SARibbon::Core::SARibbonRowProportion::Large;
}

RibbonGallery::~RibbonGallery()
{
}

QUrl RibbonGallery::leafUrl() const
{
    return SARibbonQmlLeafUrls::galleryLeaf();
}

QQmlListProperty< RibbonGalleryGroup > RibbonGallery::groups()
{
    return QQmlListProperty< RibbonGalleryGroup >(this, this, &RibbonGallery::appendGroupCb, &RibbonGallery::groupCountCb,
                                                  &RibbonGallery::groupAtCb, &RibbonGallery::clearGroupsCb);
}

int RibbonGallery::groupCount() const
{
    return mGroups.size();
}

RibbonGalleryGroup* RibbonGallery::groupAt(int index) const
{
    return (index >= 0 && index < mGroups.size()) ? mGroups[ index ] : nullptr;
}

int RibbonGallery::currentGroupIndex() const
{
    return mCurrentGroupIndex;
}

void RibbonGallery::setCurrentGroupIndex(int idx)
{
    if (idx < 0 || idx >= mGroups.size() || mCurrentGroupIndex == idx) {
        return;
    }
    mCurrentGroupIndex = idx;
    setScrollRow(0);  // new group starts at its first row
    Q_EMIT currentGroupIndexChanged();
    updateGridMetrics();
}

int RibbonGallery::stretchFactor() const
{
    return mStretchFactor;
}

void RibbonGallery::setStretchFactor(int factor)
{
    if (mStretchFactor == factor) {
        return;
    }
    mStretchFactor = factor;
    Q_EMIT stretchFactorChanged();
    invalidatePanelLayout();
}

int RibbonGallery::displayRow() const
{
    return mDisplayRow;
}

void RibbonGallery::setDisplayRow(int rows)
{
    const int clamped = qBound(1, rows, 3);
    if (mDisplayRow == clamped) {
        return;
    }
    mDisplayRow = clamped;
    Q_EMIT displayRowChanged();
    updateGridMetrics();
}

int RibbonGallery::gridMinimumWidth() const
{
    return mGridMinimumWidth;
}

void RibbonGallery::setGridMinimumWidth(int w)
{
    if (mGridMinimumWidth == w) {
        return;
    }
    mGridMinimumWidth = w;
    Q_EMIT gridMinimumWidthChanged();
    updateGridMetrics();
}

int RibbonGallery::scrollRow() const
{
    return mScrollRow;
}

void RibbonGallery::setScrollRow(int row)
{
    const int maxRow = qMax(totalRows() - mDisplayRow, 0);
    const int clamped = qBound(0, row, maxRow);
    if (mScrollRow == clamped) {
        return;
    }
    mScrollRow = clamped;
    Q_EMIT scrollRowChanged();
}

QSize RibbonGallery::gridSize() const
{
    return mGridSize;
}

int RibbonGallery::gridColumns() const
{
    return mGridColumns;
}

int RibbonGallery::totalRows() const
{
    return mTotalRows;
}

int RibbonGallery::buttonStripWidth() const
{
    return kButtonStripWidth;
}

void RibbonGallery::scrollUp()
{
    setScrollRow(scrollRow() - 1);
}

void RibbonGallery::scrollDown()
{
    setScrollRow(scrollRow() + 1);
}

void RibbonGallery::activateItem(int index)
{
    RibbonGalleryGroup* group = groupAt(mCurrentGroupIndex);
    if (!group) {
        return;
    }
    RibbonGalleryItem* item = group->itemAt(index);
    if (!item || !item->isEnabled()) {
        return;
    }
    Q_EMIT triggered(item, index);
}

QSize RibbonGallery::sizeHint() const
{
    // Large cell: height comes from the engine (resultGeometry height =
    // largeHeight); the width hint is the base width — the core panel engine
    // distributes the panel's extra width over expanding items (stretchFactor)
    const SARibbon::Core::SARibbonMetrics& m = RibbonMetrics::instance()->coreMetrics();
    const int largeH = largeButtonHeightContext() > 0
                           ? largeButtonHeightContext()
                           : m.calcCategoryHeight(true, false) - m.panelTitleHeight - 4 - 2;
    return QSize(kGalleryBaseWidth, qMax(largeH, 22));
}

Qt::Orientations RibbonGallery::expandingDirections() const
{
    return Qt::Horizontal;  // widgets SARibbonGallery parity (expanding item)
}

void RibbonGallery::componentComplete()
{
    RibbonLayoutItemHost::componentComplete();
    ensureQmlLeaf();
    updateGridMetrics();
}

void RibbonGallery::largeHeightContextChanged()
{
    invalidatePanelLayout();  // engine redistributes; grid metrics follow on applyGeometry
}

void RibbonGallery::applyGeometry(const QRect& rect)
{
    RibbonLayoutItemHost::applyGeometry(rect);
    updateGridMetrics();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonGallery::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonLayoutItemHost::geometryChange(newGeometry, oldGeometry);
#else
void RibbonGallery::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonLayoutItemHost::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.size() != oldGeometry.size()) {
        updateGridMetrics();
    }
}

// ---- groups list property callbacks ----
void RibbonGallery::appendGroupCb(QQmlListProperty< RibbonGalleryGroup >* prop, RibbonGalleryGroup* group)
{
    auto* self = static_cast< RibbonGallery* >(prop->data);
    if (self && group && !self->mGroups.contains(group)) {
        self->mGroups.append(group);
        group->setParent(self);
        // item list changes alter the row count of the grid
        QObject::connect(group, &RibbonGalleryGroup::itemsChanged, self, [self]() { self->updateGridMetrics(); });
        self->emitGroupsChanged();
    }
}

RibbonGallery::ListIndex RibbonGallery::groupCountCb(QQmlListProperty< RibbonGalleryGroup >* prop)
{
    auto* self = static_cast< RibbonGallery* >(prop->data);
    return self ? self->mGroups.size() : ListIndex(0);
}

RibbonGalleryGroup* RibbonGallery::groupAtCb(QQmlListProperty< RibbonGalleryGroup >* prop, ListIndex index)
{
    auto* self = static_cast< RibbonGallery* >(prop->data);
    return self ? self->groupAt(int(index)) : nullptr;
}

void RibbonGallery::clearGroupsCb(QQmlListProperty< RibbonGalleryGroup >* prop)
{
    auto* self = static_cast< RibbonGallery* >(prop->data);
    if (self && !self->mGroups.isEmpty()) {
        for (RibbonGalleryGroup* g : self->mGroups) {
            self->disconnect(g, nullptr, self, nullptr);
        }
        self->mGroups.clear();
        self->emitGroupsChanged();
    }
}

void RibbonGallery::emitGroupsChanged()
{
    Q_EMIT groupsChanged();
    if (mCurrentGroupIndex >= mGroups.size()) {
        mCurrentGroupIndex = qMax(mGroups.size() - 1, 0);
        Q_EMIT currentGroupIndexChanged();
    }
    updateGridMetrics();
}

void RibbonGallery::updateGridMetrics()
{
    // cell size from the shared core helper (identical to the widgets
    // SARibbonGalleryGroup::recalcGridSize for the same inputs)
    const int bodyH = qMax(int(height()) - 2, 0);
    mGridSize = SA::calcGalleryGridCellSize(bodyH, mDisplayRow, mGridMinimumWidth, 0);
    // columns: available width minus the scroll strip, at least one column
    const int availW = qMax(int(width()) - kButtonStripWidth - 2, 1);
    mGridColumns = qMax(availW / qMax(mGridSize.width(), 1), 1);
    // total rows of the current group
    RibbonGalleryGroup* group = groupAt(mCurrentGroupIndex);
    const int count = group ? group->itemCount() : 0;
    mTotalRows = (count + mGridColumns - 1) / mGridColumns;
    // re-clamp the scroll position against the new row count
    setScrollRow(mScrollRow);
    Q_EMIT gridMetricsChanged();
}

}
