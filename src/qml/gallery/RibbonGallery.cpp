#include "RibbonGallery.h"
#include "RibbonGalleryGroup.h"
#include "RibbonGalleryItem.h"
#include "../metrics/RibbonMetrics.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <QQuickItem>
#include <QFontMetrics>

namespace SARibbonQml {

namespace {
// widgets parity: SARibbonGallery::setGalleryButtonMaximumWidth default 15
constexpr int kButtonStripWidth = 15;
// widgets parity: SARibbonGallery minimum width (addGallery gives it a Large
// column; without expanding distribution this is the base width)
constexpr int kGalleryBaseWidth = 200;
// widgets parity: SARibbonGalleryGroup::PrivateData sets spacing(1)
constexpr int kCellSpacing = 1;

// Map the QML-facing caption style onto the core enumeration the metrics
// helper switches on (values are pinned by static_assert in SARibbonQmlTypes.h)
SA::GalleryCaptionStyle toCoreCaptionStyle(RibbonEnums::GalleryCaptionStyle style)
{
    switch (style) {
    case RibbonEnums::GalleryIconOnly:
        return SA::GalleryCaptionStyle::None;
    case RibbonEnums::GalleryIconWithText:
        return SA::GalleryCaptionStyle::SingleLine;
    case RibbonEnums::GalleryIconWithWordWrapText:
    default:
        return SA::GalleryCaptionStyle::WordWrap;
    }
}
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
    // the current mark is group-scoped: an index would address a different
    // entry in the new group (widgets selection follows the visible group)
    if (mCurrentItemIndex >= 0) {
        mCurrentItemIndex = -1;
        Q_EMIT currentItemIndexChanged();
        Q_EMIT currentItemChanged(nullptr, -1);
    }
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

/**
 * \if ENGLISH
 * @brief Caption band style of every grid cell
 * @details Widgets setGalleryGroupStyle parity: the same three styles, and the
 *          band height comes from the core helper the widgets group feeds
 *          setIconSize with, so switching style re-derives the icon box exactly
 *          like the widgets recalcGridSize does.
 * \endif
 *
 * \if CHINESE
 * @brief 全部网格单元的标题带样式
 * @details 对应 widgets 的 setGalleryGroupStyle：同样三种样式，标题带高度取自
 *          widgets 组喂给 setIconSize 的同一个 core 函数，因此切换样式会像
 *          widgets recalcGridSize 一样重新推导图标盒。
 * \endif
 */
RibbonEnums::GalleryCaptionStyle RibbonGallery::captionStyle() const
{
    return mCaptionStyle;
}

void RibbonGallery::setCaptionStyle(RibbonEnums::GalleryCaptionStyle style)
{
    if (mCaptionStyle == style) {
        return;
    }
    mCaptionStyle = style;
    Q_EMIT captionStyleChanged();
    updateGridMetrics();
}

int RibbonGallery::currentItemIndex() const
{
    return mCurrentItemIndex;
}

/**
 * \if ENGLISH
 * @brief Mark a cell of the current group as current
 * @details Qt::ItemIsSelectable parity: a disabled or non-selectable entry is
 *          refused (the stored mark stays untouched, exactly like a widgets
 *          selection model leaving the selection where it was). Any negative
 *          index collapses to the -1 sentinel and clears the mark.
 *          Activation is NOT gated by this — activateItem still fires
 *          triggered for a non-selectable cell, which is what the widgets
 *          QAbstractItemView::clicked does.
 * \endif
 *
 * \if CHINESE
 * @brief 把当前组的某个单元标为当前项
 * @details 对应 Qt::ItemIsSelectable：禁用或不可选择的条目被拒绝（已存的标记保持
 *          不变，正如 widgets 选择模型把选择留在原位）。任何负下标都归一到 -1 这
 *          个哨兵值并清除标记。激活**不**受此约束——activateItem 对不可选择的单元
 *          照样发 triggered，这与
 *          widgets 的 QAbstractItemView::clicked 行为一致。
 * \endif
 */
void RibbonGallery::setCurrentItemIndex(int index)
{
    // every negative index means "no current cell"; collapsing them keeps the
    // published value a single sentinel instead of an open range
    const int wanted = qMax(index, -1);
    if (wanted == mCurrentItemIndex) {
        return;
    }
    if (wanted >= 0) {
        RibbonGalleryGroup* group = groupAt(mCurrentGroupIndex);
        RibbonGalleryItem* item   = group ? group->itemAt(wanted) : nullptr;
        if (!item || !item->isEnabled() || !item->isSelectable()) {
            return;
        }
    }
    mCurrentItemIndex = wanted;
    Q_EMIT currentItemIndexChanged();
    RibbonGalleryGroup* group = groupAt(mCurrentGroupIndex);
    Q_EMIT currentItemChanged(group ? group->itemAt(mCurrentItemIndex) : nullptr, mCurrentItemIndex);
}

RibbonGalleryItem* RibbonGallery::currentItem() const
{
    RibbonGalleryGroup* group = groupAt(mCurrentGroupIndex);
    return group ? group->itemAt(mCurrentItemIndex) : nullptr;
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

/**
 * \if ENGLISH
 * @brief Caption band height of one grid cell
 * @details Published together with cellIconWidth/cellIconHeight as three plain
 *          ints (not one QSize) so every leaf binding stays in the
 *          single-dependency shape `cppHost ? cppHost.x : fallback` that
 *          survives teardown (NOTES B48).
 * \endif
 *
 * \if CHINESE
 * @brief 单个网格单元的标题带高度
 * @details 与 cellIconWidth/cellIconHeight 一起以三个独立 int 发布（而非一个
 *          QSize），使叶子的每条绑定都保持 `cppHost ? cppHost.x : fallback`
 *          这种单依赖形状，从而能安全度过析构（NOTES B48）。
 * \endif
 */
int RibbonGallery::captionHeight() const
{
    return mCaptionHeight;
}

/**
 * \if ENGLISH
 * @brief Icon box width of one grid cell
 * \endif
 *
 * \if CHINESE
 * @brief 单个网格单元的图标盒宽度
 * \endif
 */
int RibbonGallery::cellIconWidth() const
{
    return mCellIconWidth;
}

/**
 * \if ENGLISH
 * @brief Icon box height of one grid cell
 * \endif
 *
 * \if CHINESE
 * @brief 单个网格单元的图标盒高度
 * \endif
 */
int RibbonGallery::cellIconHeight() const
{
    return mCellIconHeight;
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
    // a click moves the current mark first (refused for non-selectable entries,
    // which still activate — widgets QAbstractItemView::clicked parity)
    setCurrentItemIndex(index);
    Q_EMIT triggered(item, index);
}

/**
 * \if ENGLISH
 * @brief Publish a leaf hover event
 * @details The leaf reports the cell index under the pointer; the host resolves
 *          it to an entry and publishes hovered, then forwards the very same
 *          event through the current group's hovered signal (widgets
 *          SARibbonGallery::hovered forwards SARibbonGalleryGroup::hovered).
 *          A negative index means the pointer left the grid, published as
 *          nullptr/-1 so consumers can clear a preview. Hover is not gated by
 *          selectable: an unselectable cell still reports hover, it just never
 *          becomes current.
 * \endif
 *
 * \if CHINESE
 * @brief 发布叶子的悬停事件
 * @details 叶子上报指针所在的单元下标；宿主把它解析成条目并发出 hovered，随后把
 *          同一事件经当前组的 hovered 信号转发出去（对应 widgets 的
 *          SARibbonGallery::hovered 转发 SARibbonGalleryGroup::hovered）。负下标
 *          表示指针离开网格，以 nullptr/-1 发布，便于消费者清除预览。悬停不受
 *          selectable 约束：不可选择的单元照样上报悬停，只是永远不会成为当前项。
 * \endif
 */
void RibbonGallery::notifyCellHovered(int index)
{
    RibbonGalleryGroup* group = groupAt(mCurrentGroupIndex);
    RibbonGalleryItem* item   = (index >= 0 && group) ? group->itemAt(index) : nullptr;
    const int resolved        = item ? index : -1;
    Q_EMIT hovered(item, resolved);
    if (group) {
        Q_EMIT group->hovered(item, resolved);
    }
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
        // item list changes alter the row count of the grid AND may bring in
        // items whose selectable flag the host has to watch (re-wiring drops
        // the old connections first, so this cannot double-connect)
        QObject::connect(group, &RibbonGalleryGroup::itemsChanged, self, [self, group]() {
            self->watchGroupItems(group);
            self->updateGridMetrics();
        });
        self->watchGroupItems(group);
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

/**
 * \if ENGLISH
 * @brief Watch the selectable flag of every item in a group
 * @details Called when a group joins the gallery and again whenever its item
 *          list changes. Clearing selectable on the entry the gallery marks as
 *          current must drop that mark — the widgets side gets this from its
 *          selection model, which refuses to select a non-selectable index and
 *          re-validates the selection when the flags change. Connections are
 *          dropped first so re-wiring after an itemsChanged cannot double-fire.
 * \endif
 *
 * \if CHINESE
 * @brief 监听组内每个条目的 selectable 标志
 * @details 组加入画廊时调用一次，条目列表变化后再调用一次。若把画廊当前标记的那
 *          个条目的可选择性清掉，该标记必须随之失效——widgets 侧由选择模型完成
 *          同样的事（它拒绝选中不可选择的索引，并在标志变化时重新校验选择）。
 *          连接前先断开，避免 itemsChanged 之后重复接线导致信号双发。
 * \endif
 */
void RibbonGallery::watchGroupItems(RibbonGalleryGroup* group)
{
    if (!group) {
        return;
    }
    for (int i = 0; i < group->itemCount(); ++i) {
        RibbonGalleryItem* item = group->itemAt(i);
        if (!item) {
            continue;
        }
        disconnect(item, nullptr, this, nullptr);
        connect(item, &RibbonGalleryItem::selectableChanged, this, [this]() { validateCurrentItemIndex(); });
    }
}

/**
 * \if ENGLISH
 * @brief Drop the current mark when it became invalid
 * @details Invalid means out of range for the current group, disabled, or no
 *          longer selectable. Runs on item-flag changes and on every grid
 *          metrics rebuild (item list changes shift the indices).
 * \endif
 *
 * \if CHINESE
 * @brief 当前标记失效时将其清除
 * @details 失效指超出当前组范围、被禁用或不再可选择。在条目标志变化时以及每次
 *          网格度量重建时执行（条目列表变化会移动下标）。
 * \endif
 */
void RibbonGallery::validateCurrentItemIndex()
{
    if (mCurrentItemIndex < 0) {
        return;
    }
    RibbonGalleryGroup* group = groupAt(mCurrentGroupIndex);
    RibbonGalleryItem* item   = group ? group->itemAt(mCurrentItemIndex) : nullptr;
    if (item && item->isEnabled() && item->isSelectable()) {
        return;
    }
    mCurrentItemIndex = -1;
    Q_EMIT currentItemIndexChanged();
    Q_EMIT currentItemChanged(nullptr, -1);
}

void RibbonGallery::updateGridMetrics()
{
    // cell size from the shared core helper (identical to the widgets
    // SARibbonGalleryGroup::recalcGridSize for the same inputs)
    const int bodyH = qMax(int(height()) - 2, 0);
    mGridSize = SA::calcGalleryGridCellSize(bodyH, mDisplayRow, mGridMinimumWidth, 0);
    // icon box + caption band from the same core helper the widgets group uses,
    // so the leaf reserves exactly the band the widgets delegate paints into;
    // only the line-spacing input comes from this side (RibbonMetrics, not a
    // widget's fontMetrics())
    const QFontMetrics fm = RibbonMetrics::instance()->coreMetrics().fontMetrics();
    const SA::GalleryCellMetrics cm
        = SA::calcGalleryCellMetrics(mGridSize.width(), mGridSize.height(), fm.lineSpacing(), kCellSpacing, toCoreCaptionStyle(mCaptionStyle));
    mCaptionHeight  = cm.captionHeight;
    mCellIconWidth  = cm.iconSize.width();
    mCellIconHeight = cm.iconSize.height();
    // columns: available width minus the scroll strip, at least one column
    const int availW = qMax(int(width()) - kButtonStripWidth - 2, 1);
    mGridColumns = qMax(availW / qMax(mGridSize.width(), 1), 1);
    // total rows of the current group
    RibbonGalleryGroup* group = groupAt(mCurrentGroupIndex);
    const int count = group ? group->itemCount() : 0;
    mTotalRows = (count + mGridColumns - 1) / mGridColumns;
    // re-clamp the scroll position against the new row count
    setScrollRow(mScrollRow);
    // the item list may have shrunk under the current mark
    validateCurrentItemIndex();
    Q_EMIT gridMetricsChanged();
}

}
