#include "RibbonColorGrid.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QMargins>
#include <QQuickItem>

namespace SARibbonQml {

namespace {
// widgets parity: SAColorGridWidget::PrivateData sets
// mGridLayout->setContentsMargins(1, 1, 1, 1)
constexpr int kLayoutContentsMargin = 1;

int ceilDiv(int value, int divisor)
{
    if (divisor <= 0) {
        return 0;
    }
    return (value + divisor - 1) / divisor;
}
}  // namespace

/**
 * \if ENGLISH
 * @brief Constructor
 * @param parent Parent item
 * \endif
 *
 * \if CHINESE
 * @brief 构造函数
 * @param parent 父项
 * \endif
 */
RibbonColorGrid::RibbonColorGrid(QQuickItem* parent) : RibbonQuickHost(parent)
{
    updateGridMetrics();
}

/**
 * \if ENGLISH
 * @brief Destructor
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * \endif
 */
RibbonColorGrid::~RibbonColorGrid() = default;

QList< QColor > RibbonColorGrid::colorList() const
{
    return mColors;
}

void RibbonColorGrid::setColorList(const QList< QColor >& colors)
{
    if (mColors == colors) {
        return;
    }
    mColors = colors;
    // the checked index addresses a color: it may point at a different swatch
    // (or past the end) after the list changed
    if (mCheckedIndex >= mColors.size()) {
        mCheckedIndex = -1;
        Q_EMIT checkedIndexChanged();
    }
    Q_EMIT colorListChanged();
    updateGridMetrics();
}

int RibbonColorGrid::colorCount() const
{
    return mColors.size();
}

int RibbonColorGrid::columnCount() const
{
    return mColumnCount;
}

void RibbonColorGrid::setColumnCount(int c)
{
    if (mColumnCount == c) {
        return;
    }
    mColumnCount = c;
    Q_EMIT columnCountChanged();
    updateGridMetrics();
}

int RibbonColorGrid::spacing() const
{
    return mHorizontalSpacing;
}

void RibbonColorGrid::setSpacing(int v)
{
    if (mHorizontalSpacing == v && mVerticalSpacing == v) {
        return;
    }
    mHorizontalSpacing = v;
    mVerticalSpacing   = v;
    Q_EMIT spacingChanged();
    updateGridMetrics();
}

int RibbonColorGrid::horizontalSpacing() const
{
    return mHorizontalSpacing;
}

void RibbonColorGrid::setHorizontalSpacing(int v)
{
    if (mHorizontalSpacing == v) {
        return;
    }
    mHorizontalSpacing = v;
    Q_EMIT spacingChanged();
    updateGridMetrics();
}

int RibbonColorGrid::verticalSpacing() const
{
    return mVerticalSpacing;
}

void RibbonColorGrid::setVerticalSpacing(int v)
{
    if (mVerticalSpacing == v) {
        return;
    }
    mVerticalSpacing = v;
    Q_EMIT spacingChanged();
    updateGridMetrics();
}

QSize RibbonColorGrid::colorIconSize() const
{
    return mIconSize;
}

void RibbonColorGrid::setColorIconSize(const QSize& s)
{
    if (mIconSize == s) {
        return;
    }
    mIconSize = s;
    Q_EMIT colorIconSizeChanged();
    updateGridMetrics();
}

int RibbonColorGrid::cellMargin() const
{
    return mCellMargin;
}

void RibbonColorGrid::setCellMargin(int v)
{
    const int m = qMax(0, v);
    if (mCellMargin == m) {
        return;
    }
    mCellMargin = m;
    Q_EMIT cellMarginChanged();
    updateGridMetrics();
}

bool RibbonColorGrid::isColorCheckable() const
{
    return mColorCheckable;
}

void RibbonColorGrid::setColorCheckable(bool on)
{
    if (mColorCheckable == on) {
        return;
    }
    mColorCheckable = on;
    if (!on && mCheckedIndex >= 0) {
        // widgets parity: a non-checkable button cannot stay checked
        const QColor c = mColors.at(mCheckedIndex);
        mCheckedIndex  = -1;
        Q_EMIT checkedIndexChanged();
        Q_EMIT colorToggled(c, false);
    }
    Q_EMIT colorCheckableChanged();
}

int RibbonColorGrid::checkedIndex() const
{
    return mCheckedIndex;
}

void RibbonColorGrid::setCheckedIndex(int index)
{
    if (mCheckedIndex == index) {
        return;
    }
    applyCheckedIndex(index);
}

QColor RibbonColorGrid::currentCheckedColor() const
{
    if (mCheckedIndex < 0 || mCheckedIndex >= mColors.size()) {
        return QColor();
    }
    return mColors.at(mCheckedIndex);
}

/**
 * \if ENGLISH
 * @brief Drop the checked swatch, leaving the grid without a selection
 * @details Counterpart of SAColorGridWidget::clearCheckedState, which has to
 *          defeat the exclusive button group to uncheck a button; here the
 *          host owns the index, so clearing it is direct. The old swatch is
 *          still reported through colorToggled, exactly like the widgets
 *          button whose toggled signal fires inside setChecked(false).
 * \endif
 *
 * \if CHINESE
 * @brief 清除勾选状态，网格变为无选中
 * @details 对应 SAColorGridWidget::clearCheckedState——widgets 侧为了在互斥按钮组
 *          里取消勾选必须先临时关掉 exclusive；这里索引由宿主自己持有，直接清除
 *          即可。旧色块仍会通过 colorToggled 报告，与 widgets 里 setChecked(false)
 *          触发 toggled 的行为一致。
 * \endif
 */
void RibbonColorGrid::clearCheckedState()
{
    if (mCheckedIndex < 0) {
        return;
    }
    const QColor c = (mCheckedIndex < mColors.size()) ? mColors.at(mCheckedIndex) : QColor();
    mCheckedIndex  = -1;
    Q_EMIT checkedIndexChanged();
    Q_EMIT colorToggled(c, false);
}

bool RibbonColorGrid::isHorizontalSpacerToRight() const
{
    return mHorizontalSpacerToRight;
}

void RibbonColorGrid::setHorizontalSpacerToRight(bool on)
{
    if (mHorizontalSpacerToRight == on) {
        return;
    }
    mHorizontalSpacerToRight = on;
    Q_EMIT horizontalSpacerToRightChanged();
    updateGridMetrics();
}

int RibbonColorGrid::spacerWidth() const
{
    return mSpacerWidth;
}

void RibbonColorGrid::setSpacerWidth(int w)
{
    const int v = qMax(0, w);
    if (mSpacerWidth == v) {
        return;
    }
    mSpacerWidth = v;
    Q_EMIT spacerWidthChanged();
    updateGridMetrics();
}

/**
 * \if ENGLISH
 * @brief Set the minimum height of one row
 * @param row Row index
 * @param minSize Minimum height, 0 clears the override
 * @details Counterpart of SAColorGridWidget::setRowMinimumHeight (a
 *          QGridLayout row minimum). Rows grow, never shrink: a row taller
 *          than its minimum keeps its cell height, and the cells of a grown
 *          row stay top-aligned inside it (QGridLayout places a widget at the
 *          top of a taller row by default).
 * \endif
 *
 * \if CHINESE
 * @brief 设置某一行的最小高度
 * @param row 行索引
 * @param minSize 最小高度，0 表示取消该行的约束
 * @details 对应 SAColorGridWidget::setRowMinimumHeight（QGridLayout 的行最小高）。
 *          行只会长高不会变矮：高于最小值的行保持其单元高度，被撑高的行里单元
 *          仍然顶部对齐（QGridLayout 默认把控件放在较高行的顶部）。
 * \endif
 */
void RibbonColorGrid::setRowMinimumHeight(int row, int minSize)
{
    if (row < 0) {
        return;
    }
    const int v = qMax(0, minSize);
    if (row < mRowMinimumHeights.size() && mRowMinimumHeights.at(row) == v) {
        return;
    }
    while (mRowMinimumHeights.size() <= row) {
        mRowMinimumHeights.append(0);
    }
    mRowMinimumHeights[ row ] = v;
    updateGridMetrics();
}

int RibbonColorGrid::rowMinimumHeight(int row) const
{
    if (row < 0 || row >= mRowMinimumHeights.size()) {
        return 0;
    }
    return mRowMinimumHeights.at(row);
}

QColor RibbonColorGrid::colorAt(int index) const
{
    if (index < 0 || index >= mColors.size()) {
        return QColor();
    }
    return mColors.at(index);
}

int RibbonColorGrid::gridColumns() const
{
    return mGridColumns;
}

int RibbonColorGrid::gridRows() const
{
    return mGridRows;
}

int RibbonColorGrid::cellWidth() const
{
    return mCellWidth;
}

int RibbonColorGrid::cellHeight() const
{
    return mCellHeight;
}

QVariantList RibbonColorGrid::cellRects() const
{
    return mCellRects;
}

QVariantList RibbonColorGrid::cellNoneColor() const
{
    return mCellNoneColor;
}

int RibbonColorGrid::noneColorSlashInset() const
{
    return mNoneColorSlashInset;
}

/**
 * \if ENGLISH
 * @brief Leaf callback: a swatch was pressed
 * @param index Cell index; out-of-range indices are ignored
 * \endif
 *
 * \if CHINESE
 * @brief 叶子回调：某个色块被按下
 * @param index 单元索引；越界索引被忽略
 * \endif
 */
void RibbonColorGrid::notifyCellPressed(int index)
{
    if (index < 0 || index >= mColors.size()) {
        return;
    }
    Q_EMIT colorPressed(mColors.at(index));
}

/**
 * \if ENGLISH
 * @brief Leaf callback: a swatch was released
 * @param index Cell index; out-of-range indices are ignored
 * \endif
 *
 * \if CHINESE
 * @brief 叶子回调：某个色块被释放
 * @param index 单元索引；越界索引被忽略
 * \endif
 */
void RibbonColorGrid::notifyCellReleased(int index)
{
    if (index < 0 || index >= mColors.size()) {
        return;
    }
    Q_EMIT colorReleased(mColors.at(index));
}

/**
 * \if ENGLISH
 * @brief Leaf callback: a swatch was clicked
 * @param index Cell index; out-of-range indices are ignored
 * @details The checked state is flipped before colorClicked is emitted, so a
 *          handler that reads currentCheckedColor already sees the new color
 *          (the widgets button group toggles inside the same release that
 *          produces clicked).
 * \endif
 *
 * \if CHINESE
 * @brief 叶子回调：某个色块被点击
 * @param index 单元索引；越界索引被忽略
 * @details 先翻转勾选状态再发 colorClicked，因此处理器读 currentCheckedColor 时
 *          已经是新颜色（widgets 里产生 clicked 的同一次释放中按钮组就已切换）。
 * \endif
 */
void RibbonColorGrid::activateCell(int index)
{
    if (index < 0 || index >= mColors.size()) {
        return;
    }
    applyCheckedIndex(index);
    Q_EMIT colorClicked(mColors.at(index));
}

QUrl RibbonColorGrid::leafUrl() const
{
    return SARibbonQmlLeafUrls::colorGridLeaf();
}

void RibbonColorGrid::componentComplete()
{
    RibbonQuickHost::componentComplete();
    ensureQmlLeaf();
    updateGridMetrics();
}

/**
 * \if ENGLISH
 * @brief Recompute the grid geometry and publish it to the leaf
 * @details Mirrors the widgets derivation. A cell is an icon-only
 *          SAColorToolButton, whose size hint is the icon size grown by its
 *          margins on both axes; the QGridLayout adds 1px contents margins and
 *          the configured gaps. Row count is ceil(count / columns) with an
 *          unlimited column count meaning a single row
 *          (SAColorGridWidget::PrivateData::updateGridColor). The trailing
 *          spring sits in a column of its own and holds no widget, so
 *          QGridLayout counts no extra gap for it: it adds its width only.
 *          The swatch fills the cell inset by cellMargin on all four sides:
 *          calcSizeOfToolButtonIconOnly hands the whole button rect to
 *          colorRect when the icon is null, so a row grown through
 *          setRowMinimumHeight grows its swatches with it.
 * \endif
 *
 * \if CHINESE
 * @brief 重新计算网格几何并发布给叶子
 * @details 复现 widgets 的推导。一个单元即 icon-only 的 SAColorToolButton，其
 *          sizeHint 是图标尺寸在两个方向上各加上边距；QGridLayout 再添 1px 内容
 *          边距与配置的间隔。行数为 ceil(数量 / 列数)，列数不限定时只有一行
 *          （SAColorGridWidget::PrivateData::updateGridColor）。尾部弹簧独占一列且
 *          列内无 widget，QGridLayout 不为它再计一个间隔：只增加弹簧自身的宽度。
 *          色块铺满单元四周内缩 cellMargin 后的区域：图标为空时
 *          calcSizeOfToolButtonIconOnly 把整个按钮矩形交给 colorRect，所以由
 *          setRowMinimumHeight 撑高的行其色块也随之变高。
 * \endif
 */
void RibbonColorGrid::updateGridMetrics()
{
    const int n = mColors.size();

    int cols = mColumnCount;
    int rows = 1;
    if (cols <= 0) {
        cols = n;  // unlimited columns: everything on a single row
    } else {
        rows = ceilDiv(n, cols);
    }
    if (n == 0) {
        rows = 0;
        cols = 0;
    }
    // QGridLayout gives an empty column zero width, so a partially filled last
    // row only counts the columns that really hold a cell
    const int occupiedCols = qMin(cols, n);

    // widgets parity: a cell is an icon-only SAColorToolButton, and QGridLayout
    // honours the larger of its size hint and minimum size hint (core derives
    // both so the two front ends cannot drift apart)
    const QSize cellSize = SA::colorGridCellSize(mIconSize, mCellMargin);

    const int baseCellW = cellSize.width();
    const int baseCellH = cellSize.height();

    mGridColumns = occupiedCols;
    mGridRows    = rows;
    mCellWidth   = baseCellW;
    mCellHeight  = baseCellH;

    // The slash of a "no color" swatch comes from core so both front ends draw
    // the identical mark; only the horizontal inset is needed here because the
    // leaf knows the swatch rect it is drawing into. The swatch is the cell
    // inset by cellMargin on all four sides (widgets getButtonRect)
    const QSize swatchSize(baseCellW - 2 * mCellMargin, baseCellH - 2 * mCellMargin);
    mNoneColorSlashInset = SA::noneColorSlashLine(QRect(QPoint(0, 0), swatchSize)).x1();

    mCellRects.clear();
    mCellRects.reserve(n);
    mCellNoneColor.clear();
    mCellNoneColor.reserve(n);

    int y = kLayoutContentsMargin;
    for (int r = 0; r < rows; ++r) {
        const int rowH = effectiveRowHeight(r);
        // A QToolButton is vertically fixed, so QGridLayout centres it inside a
        // row grown by setRowMinimumHeight instead of stretching it (the surplus
        // goes to the bottom because the offset is floored)
        const int cellY = y + (rowH - baseCellH) / 2;
        for (int c = 0; c < cols; ++c) {
            const int index = r * cols + c;
            if (index >= n) {
                break;
            }
            const int x = kLayoutContentsMargin + c * (baseCellW + mHorizontalSpacing);
            mCellRects.append(QVariant::fromValue(QRect(x, cellY, baseCellW, baseCellH)));
            mCellNoneColor.append(QVariant::fromValue(!mColors.at(index).isValid()));
        }
        y += rowH + mVerticalSpacing;
    }

    int totalW = 2 * kLayoutContentsMargin;
    if (occupiedCols > 0) {
        totalW += occupiedCols * baseCellW + (occupiedCols - 1) * mHorizontalSpacing;
        if (mHorizontalSpacerToRight) {
            // The trailing QSpacerItem is the only item of its column, and
            // QGridLayout counts a spacing gap up to the last column that holds a
            // widget — the spacer column contributes its own width but no extra
            // gap. Measured on SAColorGridWidget at spacing 2 and 4.
            totalW += mSpacerWidth;
        }
    }
    int totalH = 2 * kLayoutContentsMargin;
    if (rows > 0) {
        totalH = y - mVerticalSpacing + kLayoutContentsMargin;
    }

    setImplicitSize(totalW, totalH);
    Q_EMIT gridMetricsChanged();
}

/**
 * \if ENGLISH
 * @brief Height of one laid-out row
 * @param row Row index
 * @return The cell height, raised to the row minimum when one was set
 * \endif
 *
 * \if CHINESE
 * @brief 某一行的实际布局高度
 * @param row 行索引
 * @return 单元高度；若该行设了最小高度则取其较大者
 * \endif
 */
int RibbonColorGrid::effectiveRowHeight(int row) const
{
    return qMax(mCellHeight, rowMinimumHeight(row));
}

/**
 * \if ENGLISH
 * @brief Flip the exclusive check to one cell
 * @param index Cell to check; a negative index or an index equal to the
 *        current one leaves the selection untouched
 * @details The exclusive group of the widgets grid (QButtonGroup with
 *          setExclusive(true)) reports the outgoing swatch before the incoming
 *          one, so colorToggled is emitted in that order. Re-clicking the
 *          checked cell keeps it checked, which is what an exclusive button
 *          group does.
 * \endif
 *
 * \if CHINESE
 * @brief 把互斥勾选切换到某个单元
 * @param index 要勾选的单元；负索引或与当前相同的索引都不改变选中
 * @details widgets 网格的互斥按钮组（QButtonGroup 且 setExclusive(true)）先报告
 *          取消的色块再报告新选中的，因此 colorToggled 按此顺序发出。重复点击已
 *          勾选的单元会保持勾选，这正是互斥按钮组的行为。
 * \endif
 */
void RibbonColorGrid::applyCheckedIndex(int index)
{
    if (!mColorCheckable) {
        // widgets parity: non-checkable buttons never enter the checked state
        return;
    }
    if (index == mCheckedIndex) {
        return;
    }
    if (index >= mColors.size()) {
        index = -1;
    }
    const int previous = mCheckedIndex;
    mCheckedIndex      = index;
    Q_EMIT checkedIndexChanged();
    if (previous >= 0 && previous < mColors.size()) {
        Q_EMIT colorToggled(mColors.at(previous), false);
    }
    if (index >= 0) {
        Q_EMIT colorToggled(mColors.at(index), true);
    }
}

}
