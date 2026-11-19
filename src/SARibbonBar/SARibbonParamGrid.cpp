#include "SARibbonParamGrid.h"
#include "SARibbonPanel.h"
#include "SARibbonPanelLayout.h"
#include "SARibbonBar.h"

#include <QLabel>
#include <QEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include <QFontMetrics>
#include <QPointer>
#include <QVector>
#include <QDebug>

namespace
{
// Extra width added to the widest label text, keeps the label and the field from sticking together
const int cLabelTextPadding = 10;
// Field column never gets narrower than this, otherwise a combo box degenerates into a sliver
const int cMinimumFieldWidth = 40;
// Fallback row height used before any child has a usable sizeHint
const int cFallbackRowHeight = 16;
}  // namespace

class SARibbonParamGrid::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonParamGrid)
public:
    struct Row
    {
        int id { 0 };
        QLabel* leading { nullptr };
        QLabel* trailing { nullptr };
        QWidget* field { nullptr };
        SARibbonParamGrid::LabelSides sides { SARibbonParamGrid::LabelLeading };
        bool span { false };
    };

    PrivateData(SARibbonParamGrid* p) : q_ptr(p)
    {
    }

    int indexOfId(int rowId) const
    {
        const QList< Row >& rowsRef = rows;
        for (int i = 0; i < rowsRef.size(); ++i) {
            if (rowsRef.at(i).id == rowId) {
                return i;
            }
        }
        return -1;
    }

    void markDirty()
    {
        metricsDirty = true;
    }

    int rowBudget() const;
    void updateMetrics();
    QVector< int > distributeRows(int totalRows, int rowsPerBlock) const;
    void layoutChildren();
    void notifySizeHintChanged();
    void attachToPanel(SARibbonPanel* p);
    void connectRibbonBar();
    void showLabel(QWidget* w, bool on) const;

public:
    // One side-by-side column block; every block owns its own label column width so a short label in one
    // block is not padded by a long label that happens to live in another block
    struct Block
    {
        int rowCount { 0 };            ///< rows of this block
        int leadingWidth { 0 };        ///< leading label column width of this block
        int trailingWidth { 0 };       ///< trailing label column width of this block
        int blockWidth { 0 };          ///< full block width
        int minimumBlockWidth { 0 };   ///< block width using the minimum field width
        int x { 0 };                   ///< left edge relative to the grid
    };

public:
    QList< Row > rows;
    int nextRowId { 1 };
    int maxRows { 0 };
    SARibbonParamGrid::FlowPolicy flowPolicy { SARibbonParamGrid::BalancedFlow };
    int labelWidth { -1 };
    int labelMinWidth { 0 };
    int labelMaxWidth { 200 };
    Qt::Alignment labelAlignment { Qt::AlignLeft | Qt::AlignVCenter };
    int rowSpacing { 2 };
    int columnSpacing { 6 };
    int labelSpacing { 4 };
    QPointer< SARibbonPanel > panel;
    QMetaObject::Connection modeConnection;
    // metrics
    bool metricsDirty { true };
    int naturalRowHeight { 0 };
    int fieldColumnWidth { 0 };
    int fieldMinimumWidth { 0 };
    int blockCount { 0 };
    int rowsPerBlock { 0 };
    int totalWidth { 0 };
    int minimumTotalWidth { 0 };
    QVector< Block > blocks;
    int lastNotifiedWidth { -1 };
    int lastBudget { -1 };
};

/**
 * \if ENGLISH
 * @brief Row budget of one column block
 * @details An explicit setMaxRows() wins; otherwise the enclosing panel's row count is used, and when the grid
 *          is not inside a ribbon panel the budget is derived from the current height.
 * @return Maximum number of rows one block may hold, always >= 1
 * \endif
 *
 * \if CHINESE
 * @brief 一个列块最多容纳的行数
 * @details 显式 setMaxRows() 优先；否则使用所在面板的行数；不在丝带面板内时按当前高度推导。
 * @return 每块最多行数，最少为 1
 * \endif
 */
int SARibbonParamGrid::PrivateData::rowBudget() const
{
    if (maxRows > 0) {
        return maxRows;
    }
    if (panel) {
        switch (panel->panelLayoutMode()) {
        case SARibbonPanel::ThreeRowMode: {
            return 3;
        }
        case SARibbonPanel::TwoRowMode: {
            return 2;
        }
        case SARibbonPanel::SingleRowMode: {
            return 1;
        }
        }
    }
    // Not inside a panel: derive the budget from the available height so the grid still works in a dialog
    const int available = q_ptr->height();
    const int rowHeight = (naturalRowHeight > 0) ? naturalRowHeight : cFallbackRowHeight;
    if (available <= 0) {
        return 1;
    }
    return qMax(1, available / rowHeight);
}

/**
 * \if EN
 * @brief Recompute every cached metric: natural row height, column widths, block count and total width
 * \endif
 *
 * \if CHINESE
 * @brief 重算全部缓存指标：自然行高、各列列宽、列块数与总宽
 * \endif
 */
void SARibbonParamGrid::PrivateData::updateMetrics()
{
    const int budget = qMax(1, rowBudget());
    if (!metricsDirty && budget == lastBudget) {
        return;
    }
    metricsDirty = false;
    lastBudget   = budget;

    const QFontMetrics fm(q_ptr->font());
    const QList< Row >& rowsRef = rows;

    int rowHeight     = 0;
    int fieldWidth    = 0;
    int fieldMinWidth = 0;
    for (const Row& r : rowsRef) {
        if (r.field) {
            rowHeight = qMax(rowHeight, qMax(r.field->sizeHint().height(), r.field->minimumSizeHint().height()));
            // minimumSize() carries setMinimumWidth(), which QWidgetItem uses as a hard lower bound
            const int fieldHintWidth =
                qMax(r.field->sizeHint().width(), qMax(r.field->minimumSizeHint().width(), r.field->minimumSize().width()));
            fieldMinWidth = qMax(fieldMinWidth, fieldHintWidth);
            if (!r.span) {
                fieldWidth = qMax(fieldWidth, fieldHintWidth);
            }
        }
    }

    naturalRowHeight  = qMax(rowHeight, cFallbackRowHeight);
    fieldColumnWidth  = qMax(fieldWidth, cMinimumFieldWidth);
    fieldMinimumWidth = qMax(fieldMinWidth, cMinimumFieldWidth);

    blocks.clear();
    blockCount        = 0;
    rowsPerBlock      = 0;
    totalWidth        = 0;
    minimumTotalWidth = 0;

    const int totalRows = rowsRef.size();
    if (totalRows <= 0) {
        return;
    }

    // Span rows take part in the row distribution like ordinary rows: they occupy one row of their block and
    // span the whole block width. A block that only holds span rows is sized by the span content, so a hint
    // row does not reserve an empty label + field column.
    const QVector< int > counts = distributeRows(totalRows, budget);
    blockCount                  = counts.size();

    const int maxLabelWidth = qMax(labelMinWidth, labelMaxWidth);
    int firstRow = 0;
    int x        = 0;
    int minTotal = 0;
    for (int c : counts) {
        Block blk;
        blk.rowCount = c;

        // Label columns are per block: a short label in one block is not padded by a long label of another
        int leadingTextW  = 0;
        int trailingTextW = 0;
        int spanWidth     = 0;
        int spanMinWidth  = 0;
        bool anyLeading   = false;
        bool anyTrailing  = false;
        bool anyField     = false;
        for (int k = 0; k < c; ++k) {
            const Row& r = rowsRef.at(firstRow + k);
            if (r.span) {
                if (r.field) {
                    spanWidth =
                        qMax(spanWidth, qMax(r.field->sizeHint().width(), r.field->minimumSize().width()));
                    spanMinWidth = qMax(spanMinWidth,
                                        qMax(r.field->minimumSizeHint().width(), r.field->minimumSize().width()));
                }
                continue;
            }
            anyField = true;
            if (r.leading && (r.sides & SARibbonParamGrid::LabelLeading) && !r.leading->text().isEmpty()) {
                anyLeading   = true;
                leadingTextW = qMax(leadingTextW, fm.horizontalAdvance(r.leading->text()));
            }
            if (r.trailing && (r.sides & SARibbonParamGrid::LabelTrailing) && !r.trailing->text().isEmpty()) {
                anyTrailing   = true;
                trailingTextW = qMax(trailingTextW, fm.horizontalAdvance(r.trailing->text()));
            }
        }
        if (anyLeading) {
            blk.leadingWidth =
                (labelWidth >= 0) ? labelWidth : qBound(labelMinWidth, leadingTextW + cLabelTextPadding, maxLabelWidth);
        }
        if (anyTrailing) {
            blk.trailingWidth =
                (labelWidth >= 0) ? labelWidth : qBound(labelMinWidth, trailingTextW + cLabelTextPadding, maxLabelWidth);
        }
        if (anyField) {
            const int segments  = 1 + (blk.leadingWidth > 0 ? 1 : 0) + (blk.trailingWidth > 0 ? 1 : 0);
            const int labelGaps = labelSpacing * qMax(0, segments - 1);
            blk.blockWidth        = blk.leadingWidth + fieldColumnWidth + blk.trailingWidth + labelGaps;
            blk.minimumBlockWidth = blk.leadingWidth + fieldMinimumWidth + blk.trailingWidth + labelGaps;
        } else {
            // A span-only block has no label/field columns: its width is the span content itself
            blk.blockWidth        = qMax(spanWidth, cMinimumFieldWidth);
            blk.minimumBlockWidth = blk.blockWidth;
        }
        // A wide hint can be wider than the label + field columns of a mixed block
        blk.blockWidth        = qMax(blk.blockWidth, spanWidth);
        blk.minimumBlockWidth = qMax(blk.minimumBlockWidth, spanMinWidth);
        blk.x                 = x;
        x += blk.blockWidth + columnSpacing;
        minTotal += blk.minimumBlockWidth + columnSpacing;
        rowsPerBlock = qMax(rowsPerBlock, c);
        firstRow += c;
        blocks.append(blk);
    }
    minimumTotalWidth = qMax(0, minTotal - columnSpacing);
    totalWidth        = blocks.isEmpty() ? 0 : (blocks.last().x + blocks.last().blockWidth);
}

/**
 * \if ENGLISH
 * @brief Split the logical rows over the column blocks according to the flow policy
 * @param totalRows Rows to distribute (span rows take part like ordinary rows)
 * @param rowsPerBlock Row budget of one block
 * @return Row count of every block, empty when there is nothing to distribute
 * \endif
 *
 * \if CHINESE
 * @brief 按流式策略把逻辑行分配到各列块
 * @param totalRows 参与分块的行数（span 行与普通行一样参与分配）
 * @param rowsPerBlock 每块行数预算
 * @return 每个列块的行数；没有行时返回空数组
 * \endif
 */
QVector< int > SARibbonParamGrid::PrivateData::distributeRows(int totalRows, int rowsPerBlock) const
{
    QVector< int > counts;
    const int budget = qMax(1, rowsPerBlock);
    if (totalRows <= 0) {
        return counts;
    }
    const int blocks = (totalRows + budget - 1) / budget;
    if (flowPolicy == SARibbonParamGrid::BalancedFlow) {
        const int base = totalRows / blocks;
        const int rem  = totalRows % blocks;
        for (int i = 0; i < blocks; ++i) {
            counts.append(base + (i < rem ? 1 : 0));
        }
    } else {
        int left = totalRows;
        for (int i = 0; i < blocks; ++i) {
            const int c = qMin(budget, left);
            counts.append(c);
            left -= c;
        }
    }
    return counts;
}

/**
 * \if ENGLISH
 * @brief Show or hide a label without churning visibility state
 * \endif
 *
 * \if CHINESE
 * @brief 显示或隐藏标签，状态未变化时不做任何操作
 * \endif
 */
void SARibbonParamGrid::PrivateData::showLabel(QWidget* w, bool on) const
{
    if (!w) {
        return;
    }
    if (on && w->isHidden()) {
        w->show();
    } else if (!on && !w->isHidden()) {
        w->hide();
    }
}

/**
 * \if ENGLISH
 * @brief Lay out every child
 * @details The row height follows the ribbon row grid and every column block uses its own label column width.
 *          Span rows are full-width rows below the blocks, children are vertically centered inside their row
 *          and never stretched, so a 22 px ribbon row never clips a 24 px combo.
 * \endif
 *
 * \if CHINESE
 * @brief 重排所有子控件
 * @details 行高跟随丝带行网格，每个列块用自己那块标签列宽；span 行是列块下方的整宽行。
 *          子控件在行内垂直居中、不拉伸，22px 的丝带行不会把 24px 的下拉框裁掉。
 *          不在面板里时退化为自然行高、贴顶排列。
 * \endif
 */
void SARibbonParamGrid::PrivateData::layoutChildren()
{
    updateMetrics();

    const int totalRows = rows.size();
    if (totalRows <= 0) {
        return;
    }

    const QRect area = q_ptr->rect();
    const bool rtl   = (q_ptr->layoutDirection() == Qt::RightToLeft);

    // Line height mirrors the ribbon row grid, not the number of content rows: a two-row grid inside a
    // three-row panel keeps the first two ribbon rows and leaves the third one empty, so it lines up with
    // the small items of the neighbouring panels. Outside a panel the natural row height is used.
    int panelRows = 0;
    if (panel) {
        switch (panel->panelLayoutMode()) {
        case SARibbonPanel::ThreeRowMode: {
            panelRows = 3;
            break;
        }
        case SARibbonPanel::TwoRowMode: {
            panelRows = 2;
            break;
        }
        case SARibbonPanel::SingleRowMode: {
            panelRows = 1;
            break;
        }
        }
    }
    const int usableHeight = area.height();
    const int rowHeight    = (panelRows > 0) ? qMax(1, (usableHeight - (panelRows - 1) * rowSpacing) / panelRows)
                                             : qMax(1, naturalRowHeight);

    // Fit a child into its row without ever exceeding the row height
    auto fittedHeight = [](QWidget* w, int maxHeight) -> int {
        if (!w) {
            return maxHeight;
        }
        int h = qMin(w->sizeHint().height(), maxHeight);
        h     = qMax(h, qMin(w->minimumSizeHint().height(), maxHeight));
        return qMax(h, 1);
    };

    auto placeWidget = [&](QWidget* w, int ltrX, int rowTop, int width, int maxHeight) {
        if (!w) {
            return;
        }
        const int h = fittedHeight(w, maxHeight);
        const int x = rtl ? (area.left() + (area.width() - ltrX - width)) : (area.left() + ltrX);
        w->setGeometry(x, rowTop + qMax(0, (maxHeight - h) / 2), qMax(0, width), h);
    };

    int rowIndex = 0;
    for (int b = 0; b < blocks.size(); ++b) {
        const Block& blk = blocks.at(b);
        const int fieldOffset = (blk.leadingWidth > 0) ? (blk.leadingWidth + labelSpacing) : 0;
        const int trailingOffset = fieldOffset + fieldColumnWidth + ((blk.trailingWidth > 0) ? labelSpacing : 0);
        for (int r = 0; r < blk.rowCount; ++r) {
            if (rowIndex >= rows.size()) {
                break;
            }
            Row& row        = rows[rowIndex];
            const int rowTop = area.top() + r * (rowHeight + rowSpacing);
            ++rowIndex;

            if (row.span) {
                // A span row spans its whole block; in a block of span rows that block is sized by the span
                placeWidget(row.field, blk.x, rowTop, blk.blockWidth, rowHeight);
                showLabel(row.leading, false);
                showLabel(row.trailing, false);
                if (row.field) {
                    row.field->show();
                }
                continue;
            }

            if (blk.leadingWidth > 0 && row.leading) {
                placeWidget(row.leading, blk.x, rowTop, blk.leadingWidth, rowHeight);
                showLabel(row.leading, (row.sides & SARibbonParamGrid::LabelLeading) && !row.leading->text().isEmpty());
            }
            if (row.trailing) {
                if (blk.trailingWidth > 0) {
                    placeWidget(row.trailing, blk.x + trailingOffset, rowTop, blk.trailingWidth, rowHeight);
                    showLabel(row.trailing,
                              (row.sides & SARibbonParamGrid::LabelTrailing) && !row.trailing->text().isEmpty());
                } else {
                    showLabel(row.trailing, false);
                }
            }
            if (row.field) {
                placeWidget(row.field, blk.x + fieldOffset, rowTop, fieldColumnWidth, rowHeight);
                row.field->show();
            }
            // Leading label hidden when this row has no leading side
            if (row.leading && !(row.sides & SARibbonParamGrid::LabelLeading)) {
                showLabel(row.leading, false);
            }
        }
    }

    notifySizeHintChanged();
}

/**
 * \if ENGLISH
 * @brief Tell the enclosing panel that the grid width changed
 * @details SARibbonPanelLayout caches the sizeHint of every widget, so the cache entry has to be dropped or the
 *          panel keeps laying the grid out with its previous width.
 * \endif
 *
 * \if CHINESE
 * @brief 通知所在面板网格宽度变了
 * @details SARibbonPanelLayout 按控件缓存 sizeHint，必须丢掉该缓存，否则面板一直按旧宽度排这个网格。
 * \endif
 */
void SARibbonParamGrid::PrivateData::notifySizeHintChanged()
{
    if (totalWidth == lastNotifiedWidth) {
        return;
    }
    lastNotifiedWidth = totalWidth;
    if (panel) {
        if (SARibbonPanelLayout* lay = panel->panelLayout()) {
            lay->invalidateButtonSizeHintCache(q_ptr);
        }
    }
    q_ptr->updateGeometry();
}

/**
 * \if ENGLISH
 * @brief Attach the grid to a panel and listen to its ribbon bar row-mode changes
 * \endif
 *
 * \if CHINESE
 * @brief 把网格挂到面板上，并监听所在 ribbon bar 的行模式变化
 * \endif
 */
void SARibbonParamGrid::PrivateData::attachToPanel(SARibbonPanel* p)
{
    if (panel == p && modeConnection) {
        return;
    }
    QObject::disconnect(modeConnection);
    modeConnection = QMetaObject::Connection();
    panel          = p;
    connectRibbonBar();
    markDirty();
    updateMetrics();
    layoutChildren();
}

/**
 * \if ENGLISH
 * @brief Resolve the ribbon bar of the enclosing panel and connect to its row-mode signal
 * @details The panel is usually added to a category after the widget already exists, so this is retried from
 *          showEvent() until the bar can be resolved.
 * \endif
 *
 * \if CHINESE
 * @brief 找到所在面板的 ribbon bar 并连上行模式信号
 * @details 控件通常先被创建、面板随后才挂进 category，所以 showEvent() 里会重试直到找到 bar。
 * \endif
 */
void SARibbonParamGrid::PrivateData::connectRibbonBar()
{
    QObject::disconnect(modeConnection);
    modeConnection = QMetaObject::Connection();
    if (!panel) {
        return;
    }
    SARibbonBar* bar = panel->ribbonBar();
    if (!bar) {
        return;
    }
    modeConnection = QObject::connect(bar,
                                      &SARibbonBar::panelLayoutModeChanged,
                                      q_ptr,
                                      [ this ](SARibbonPanel::PanelLayoutMode) {
                                          markDirty();
                                          updateMetrics();
                                          layoutChildren();
                                      });
}

/**
 * \if ENGLISH
 * @brief Constructor of SARibbonParamGrid
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonParamGrid 构造函数
 * \endif
 */
SARibbonParamGrid::SARibbonParamGrid(QWidget* parent) : QWidget(parent), d_ptr(new SARibbonParamGrid::PrivateData(this))
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // The panel usually exists already when the grid is constructed with it as parent
    if (SARibbonPanel* p = qobject_cast< SARibbonPanel* >(parentWidget())) {
        d_ptr->attachToPanel(p);
    }
}

/**
 * \if ENGLISH
 * @brief Destructor of SARibbonParamGrid
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonParamGrid 析构函数
 * \endif
 */
SARibbonParamGrid::~SARibbonParamGrid()
{
}

/**
 * \if ENGLISH
 * @brief Add a row that only carries a field
 * @param field Field widget, the grid takes ownership by reparenting it
 * @return Row id (>= 1), or -1 when field is null
 * \endif
 *
 * \if CHINESE
 * @brief 追加一行，只有控件、没有标签
 * @param field 控件，网格会通过改父对象接管它
 * @return 行 id（>= 1）；field 为空时返回 -1
 * \endif
 */
int SARibbonParamGrid::addRow(QWidget* field)
{
    return addRow(field, LabelLeading, QString(), QString());
}

/**
 * \if ENGLISH
 * @brief Add a row with a leading label
 * @param leading Leading label text
 * @param field Field widget
 * @return Row id (>= 1), or -1 when field is null
 * \endif
 *
 * \if CHINESE
 * @brief 追加一行，标签在控件前
 * @param leading 前标签文本
 * @param field 控件
 * @return 行 id（>= 1）；field 为空时返回 -1
 * \endif
 */
int SARibbonParamGrid::addRow(const QString& leading, QWidget* field)
{
    return addRow(field, LabelLeading, leading, QString());
}

/**
 * \if ENGLISH
 * @brief Add a row with both a leading and a trailing label
 * \endif
 *
 * \if CHINESE
 * @brief 追加一行，同时有前标签和后标签
 * \endif
 */
int SARibbonParamGrid::addRow(const QString& leading, QWidget* field, const QString& trailing)
{
    return addRow(field, LabelBoth, leading, trailing);
}

/**
 * \if ENGLISH
 * @brief Add a row with explicit label sides
 * @details ``sides`` decides which cells the row occupies: LabelLeading keeps the field in the shared field
 *          column, so a checkbox added with LabelNone still lines up with the combo boxes above it.
 * @param field Field widget
 * @param sides Which label cells the row uses
 * @param leading Leading label text
 * @param trailing Trailing label text
 * @return Row id (>= 1), or -1 when field is null
 * \endif
 *
 * \if CHINESE
 * @brief 按显式标签侧追加一行
 * @details ``sides`` 决定这一行占用哪些单元格：LabelLeading 时控件仍落在共享控件列，所以用 LabelNone
 *          追加的复选框会和上方的下拉框左边界对齐。
 * @param field 控件
 * @param sides 该行使用哪些标签单元格
 * @param leading 前标签文本
 * @param trailing 后标签文本
 * @return 行 id（>= 1）；field 为空时返回 -1
 * \endif
 */
int SARibbonParamGrid::addRow(QWidget* field, SARibbonParamGrid::LabelSides sides, const QString& leading,
                              const QString& trailing)
{
    if (!field) {
        qWarning() << "SARibbonParamGrid::addRow: field widget is null";
        return -1;
    }
    SA_D(d);
    PrivateData::Row row;
    row.id    = d->nextRowId++;
    row.field = field;
    row.sides = sides;
    row.span  = false;
    if (field->parentWidget() != this) {
        field->setParent(this);
    }
    if (sides & LabelLeading) {
        row.leading = new QLabel(leading, this);
        row.leading->setAlignment(d->labelAlignment);
        row.leading->setObjectName(
            QStringLiteral("%1/leading-%2").arg(objectName().isEmpty() ? QStringLiteral("SARibbonParamGrid") : objectName())
                .arg(row.id));
    }
    if (sides & LabelTrailing) {
        row.trailing = new QLabel(trailing, this);
        row.trailing->setAlignment(d->labelAlignment);
        row.trailing->setObjectName(
            QStringLiteral("%1/trailing-%2").arg(objectName().isEmpty() ? QStringLiteral("SARibbonParamGrid") : objectName())
                .arg(row.id));
    }
    d->rows.append(row);
    d->markDirty();
    d->updateMetrics();
    d->layoutChildren();
    return row.id;
}

/**
 * \if ENGLISH
 * @brief Add a row that spans its column block
 * @details The row takes part in the row distribution like an ordinary row but spans the whole width of the
 *          block it lands in. A block that only holds span rows is sized by the span content, so a hint row
 *          does not reserve an empty label + field column.
 * @param widget Widget that spans the block, for example a read-only hint label
 * @return Row id (>= 1), or -1 when widget is null
 * \endif
 *
 * \if CHINESE
 * @brief 追加一条横跨所在列块的行
 * @details 该行和普通行一样参与行数分配，但横跨它落入的列块的整个宽度。只含 span 行的列块按 span 内容
 *          定宽，因此提示行不会预留一列空的标签/控件列。
 * @param widget 跨块的控件，例如只读提示标签
 * @return 行 id（>= 1）；widget 为空时返回 -1
 * \endif
 */
int SARibbonParamGrid::addSpanRow(QWidget* widget)
{
    if (!widget) {
        qWarning() << "SARibbonParamGrid::addSpanRow: widget is null";
        return -1;
    }
    SA_D(d);
    PrivateData::Row row;
    row.id    = d->nextRowId++;
    row.field = widget;
    row.sides = LabelNone;
    row.span  = true;
    if (widget->parentWidget() != this) {
        widget->setParent(this);
    }
    d->rows.append(row);
    d->markDirty();
    d->updateMetrics();
    d->layoutChildren();
    return row.id;
}

/**
 * \if ENGLISH
 * @brief Remove a row and delete its widgets
 * @param rowId Row id returned by addRow()/addSpanRow()
 * @return True when the row was removed
 * \endif
 *
 * \if CHINESE
 * @brief 移除一行并删除它的控件
 * @param rowId addRow()/addSpanRow() 返回的行 id
 * @return 是否成功移除
 * \endif
 */
bool SARibbonParamGrid::removeRow(int rowId)
{
    SA_D(d);
    const int index = d->indexOfId(rowId);
    if (index < 0) {
        return false;
    }
    const PrivateData::Row& row = d->rows.at(index);
    if (row.leading) {
        row.leading->deleteLater();
    }
    if (row.trailing) {
        row.trailing->deleteLater();
    }
    if (row.field) {
        row.field->hide();
        row.field->deleteLater();
    }
    d->rows.removeAt(index);
    d->markDirty();
    d->updateMetrics();
    d->layoutChildren();
    return true;
}

/**
 * \if ENGLISH
 * @brief Remove all rows and delete their widgets
 * \endif
 *
 * \if CHINESE
 * @brief 清空所有行并删除控件
 * \endif
 */
void SARibbonParamGrid::clearRows()
{
    SA_D(d);
    const QList< PrivateData::Row >& rowsRef = d->rows;
    for (const PrivateData::Row& row : rowsRef) {
        if (row.leading) {
            row.leading->deleteLater();
        }
        if (row.trailing) {
            row.trailing->deleteLater();
        }
        if (row.field) {
            row.field->hide();
            row.field->deleteLater();
        }
    }
    d->rows.clear();
    d->markDirty();
    d->updateMetrics();
    d->layoutChildren();
}

/**
 * \if ENGLISH
 * @brief Number of rows
 * \endif
 *
 * \if CHINESE
 * @brief 行数
 * \endif
 */
int SARibbonParamGrid::rowCount() const
{
    SA_DC(d);
    return d->rows.size();
}

/**
 * \if ENGLISH
 * @brief Field widget of a row
 * @return The widget, or nullptr when the id is unknown
 * \endif
 *
 * \if CHINESE
 * @brief 某一行的控件
 * @return 控件；行 id 未知时返回 nullptr
 * \endif
 */
QWidget* SARibbonParamGrid::fieldAt(int rowId) const
{
    SA_DC(d);
    const int index = d->indexOfId(rowId);
    return (index < 0) ? nullptr : d->rows.at(index).field;
}

/**
 * \if ENGLISH
 * @brief Set the leading and trailing text of a row
 * @details Call it from retranslateUi(): the column widths are recomputed, so a longer translated label widens
 *          the label column instead of pushing the field column around.
 * \endif
 *
 * \if CHINESE
 * @brief 设置某一行的前/后标签文本
 * @details 在 retranslateUi() 里调用：列宽会重算，因此换语言后更长的标签会撑宽标签列，而不是把控件列挤歪。
 * \endif
 */
void SARibbonParamGrid::setRowText(int rowId, const QString& leading, const QString& trailing)
{
    SA_D(d);
    const int index = d->indexOfId(rowId);
    if (index < 0) {
        return;
    }
    PrivateData::Row& row = d->rows[index];
    if (row.sides & LabelLeading) {
        if (!row.leading) {
            row.leading = new QLabel(this);
            row.leading->setAlignment(d->labelAlignment);
        }
        row.leading->setText(leading);
    }
    if (row.sides & LabelTrailing) {
        if (!row.trailing) {
            row.trailing = new QLabel(this);
            row.trailing->setAlignment(d->labelAlignment);
        }
        row.trailing->setText(trailing);
    }
    d->markDirty();
    d->updateMetrics();
    d->layoutChildren();
}

/**
 * \if ENGLISH
 * @brief Set which label cells a row uses
 * @details Labels that are no longer used are deleted; new ones are created on demand.
 * \endif
 *
 * \if CHINESE
 * @brief 设置某一行使用哪些标签单元格
 * @details 不再使用的标签会被删除，需要的新标签按需创建。
 * \endif
 */
void SARibbonParamGrid::setRowLabelSides(int rowId, SARibbonParamGrid::LabelSides sides)
{
    SA_D(d);
    const int index = d->indexOfId(rowId);
    if (index < 0) {
        return;
    }
    PrivateData::Row& row = d->rows[index];
    row.sides             = sides;
    if ((sides & LabelLeading) && !row.leading) {
        row.leading = new QLabel(this);
        row.leading->setAlignment(d->labelAlignment);
    }
    if (!(sides & LabelLeading) && row.leading) {
        row.leading->deleteLater();
        row.leading = nullptr;
    }
    if ((sides & LabelTrailing) && !row.trailing) {
        row.trailing = new QLabel(this);
        row.trailing->setAlignment(d->labelAlignment);
    }
    if (!(sides & LabelTrailing) && row.trailing) {
        row.trailing->deleteLater();
        row.trailing = nullptr;
    }
    d->markDirty();
    d->updateMetrics();
    d->layoutChildren();
}

/**
 * \if ENGLISH
 * @brief Leading label of a row
 * @return The label, or nullptr when the row has no leading cell
 * \endif
 *
 * \if CHINESE
 * @brief 某一行的前标签
 * @return 标签；该行没有前标签时返回 nullptr
 * \endif
 */
QLabel* SARibbonParamGrid::leadingLabel(int rowId) const
{
    SA_DC(d);
    const int index = d->indexOfId(rowId);
    return (index < 0) ? nullptr : d->rows.at(index).leading;
}

/**
 * \if ENGLISH
 * @brief Trailing label of a row
 * @return The label, or nullptr when the row has no trailing cell
 * \endif
 *
 * \if CHINESE
 * @brief 某一行的后标签
 * @return 标签；该行没有后标签时返回 nullptr
 * \endif
 */
QLabel* SARibbonParamGrid::trailingLabel(int rowId) const
{
    SA_DC(d);
    const int index = d->indexOfId(rowId);
    return (index < 0) ? nullptr : d->rows.at(index).trailing;
}

/**
 * \if ENGLISH
 * @brief Set the alignment used by the row labels
 * \endif
 *
 * \if CHINESE
 * @brief 设置行标签的对齐方式
 * \endif
 */
void SARibbonParamGrid::setLabelAlignment(Qt::Alignment alignment)
{
    SA_D(d);
    d->labelAlignment = alignment;
    const QList< PrivateData::Row >& rowsRef = d->rows;
    for (const PrivateData::Row& row : rowsRef) {
        if (row.leading) {
            row.leading->setAlignment(alignment);
        }
        if (row.trailing) {
            row.trailing->setAlignment(alignment);
        }
    }
}

/**
 * \if ENGLISH
 * @brief Current label column width in pixels
 * @return Leading label column width, 0 when the grid has no leading label
 * \endif
 *
 * \if CHINESE
 * @brief 当前标签列宽（像素）
 * @return 前标签列宽；网格没有前标签时返回 0
 * \endif
 */
int SARibbonParamGrid::labelColumnWidth() const
{
    SA_DC(d);
    const_cast< PrivateData* >(d)->updateMetrics();
    // Column blocks size their label columns independently; report the first block (0 when there is none)
    return d->blocks.isEmpty() ? 0 : d->blocks.first().leadingWidth;
}

/**
 * \if ENGLISH
 * @brief Fix the label column width
 * @param px Width in pixels, pass a negative value to restore the automatic width
 * \endif
 *
 * \if CHINESE
 * @brief 固定标签列宽
 * @param px 像素宽度；传负值恢复自动宽度
 * \endif
 */
void SARibbonParamGrid::setLabelColumnWidth(int px)
{
    SA_D(d);
    d->labelWidth = px;
    recalcColumnWidths();
}

/**
 * \if ENGLISH
 * @brief Set the range used by the automatic label column width
 * @param minPx Lower bound in pixels
 * @param maxPx Upper bound in pixels
 * \endif
 *
 * \if CHINESE
 * @brief 设置自动标签列宽的上下限
 * @param minPx 下限（像素）
 * @param maxPx 上限（像素）
 * \endif
 */
void SARibbonParamGrid::setLabelColumnWidthRange(int minPx, int maxPx)
{
    SA_D(d);
    d->labelMinWidth = qMax(0, minPx);
    d->labelMaxWidth = qMax(d->labelMinWidth, maxPx);
    recalcColumnWidths();
}

/**
 * \if ENGLISH
 * @brief Recompute the column widths and lay the children out again
 * @details Call it after a font or translation change; addRow()/setRowText() already do this.
 * \endif
 *
 * \if CHINESE
 * @brief 重算列宽并重新排布子控件
 * @details 字体或翻译变化后调用；addRow()/setRowText() 内部已经会做。
 * \endif
 */
void SARibbonParamGrid::recalcColumnWidths()
{
    SA_D(d);
    d->markDirty();
    d->updateMetrics();
    d->layoutChildren();
    updateGeometry();
}

/**
 * \if ENGLISH
 * @brief Row budget of one column block
 * @param rows Explicit budget, 0 means follow the enclosing panel (default)
 * \endif
 *
 * \if CHINESE
 * @brief 一个列块的行数预算
 * @param rows 显式预算；0 表示跟随所在面板（默认）
 * \endif
 */
void SARibbonParamGrid::setMaxRows(int rows)
{
    SA_D(d);
    d->maxRows = qMax(0, rows);
    d->markDirty();
    d->updateMetrics();
    d->layoutChildren();
    updateGeometry();
}

/**
 * \if ENGLISH
 * @brief Row budget of one column block, 0 means automatic
 * \endif
 *
 * \if CHINESE
 * @brief 一个列块的行数预算；0 表示自动
 * \endif
 */
int SARibbonParamGrid::maxRows() const
{
    SA_DC(d);
    return d->maxRows;
}

/**
 * \if ENGLISH
 * @brief Set how rows are spread over the column blocks
 * \endif
 *
 * \if CHINESE
 * @brief 设置逻辑行在列块之间的分配方式
 * \endif
 */
void SARibbonParamGrid::setFlowPolicy(SARibbonParamGrid::FlowPolicy policy)
{
    SA_D(d);
    d->flowPolicy = policy;
    d->markDirty();
    d->updateMetrics();
    d->layoutChildren();
    updateGeometry();
}

/**
 * \if ENGLISH
 * @brief How rows are spread over the column blocks
 * \endif
 *
 * \if CHINESE
 * @brief 逻辑行在列块之间的分配方式
 * \endif
 */
SARibbonParamGrid::FlowPolicy SARibbonParamGrid::flowPolicy() const
{
    SA_DC(d);
    return d->flowPolicy;
}

/**
 * \if ENGLISH
 * @brief Number of rows in the tallest block right now
 * \endif
 *
 * \if CHINESE
 * @brief 当前最高列块的行数
 * \endif
 */
int SARibbonParamGrid::currentRows() const
{
    SA_DC(d);
    const_cast< PrivateData* >(d)->updateMetrics();
    return d->rowsPerBlock;
}

/**
 * \if ENGLISH
 * @brief Number of side-by-side blocks right now
 * \endif
 *
 * \if CHINESE
 * @brief 当前并排列块的数量
 * \endif
 */
int SARibbonParamGrid::currentColumns() const
{
    SA_DC(d);
    const_cast< PrivateData* >(d)->updateMetrics();
    return d->blockCount;
}

/**
 * \if ENGLISH
 * @brief Vertical spacing between rows in pixels
 * \endif
 *
 * \if CHINESE
 * @brief 行间距（像素）
 * \endif
 */
void SARibbonParamGrid::setRowSpacing(int px)
{
    SA_D(d);
    d->rowSpacing = qMax(0, px);
    d->layoutChildren();
    updateGeometry();
}

/**
 * \if ENGLISH
 * @brief Horizontal spacing between blocks in pixels
 * \endif
 *
 * \if CHINESE
 * @brief 列块之间的水平间距（像素）
 * \endif
 */
void SARibbonParamGrid::setColumnSpacing(int px)
{
    SA_D(d);
    d->columnSpacing = qMax(0, px);
    d->markDirty();
    d->updateMetrics();
    d->layoutChildren();
    updateGeometry();
}

/**
 * \if ENGLISH
 * @brief Horizontal gap between a label column and the field column in pixels
 * @details The gap is added on top of the automatic label column width, so the default 4px plus the
 *          text padding keeps the label text and the field close together.
 * \endif
 *
 * \if CHINESE
 * @brief 标签列与控件列之间的水平间隙（像素）
 * @details 间隙加在自动标签列宽之外，默认 4px 加上文本 padding，让标签文字与控件保持紧凑。
 * \endif
 */
void SARibbonParamGrid::setLabelSpacing(int px)
{
    SA_D(d);
    d->labelSpacing = qMax(0, px);
    d->markDirty();
    d->updateMetrics();
    d->layoutChildren();
    updateGeometry();
}

/**
 * \if ENGLISH
 * @brief Horizontal gap between a label column and the field column
 * \endif
 *
 * \if CHINESE
 * @brief 标签列与控件列之间的水平间隙
 * \endif
 */
int SARibbonParamGrid::labelSpacing() const
{
    SA_DC(d);
    return d->labelSpacing;
}

/**
 * \if ENGLISH
 * @brief Attach the grid to a panel
 * @details The enclosing panel is resolved from the parent chain automatically; call this when the grid is put
 *          somewhere the chain cannot see, for example inside a wrapper widget.
 * \endif
 *
 * \if CHINESE
 * @brief 把网格挂到面板上
 * @details 默认会从父对象链自动解析所在面板；当网格被包在看不见面板的中间容器里时，显式调用本函数。
 * \endif
 */
void SARibbonParamGrid::attachToPanel(SARibbonPanel* panel)
{
    SA_D(d);
    d->attachToPanel(panel);
}

/**
 * \if ENGLISH
 * @brief Panel this grid is attached to
 * \return The panel, may be nullptr
 * \endif
 *
 * \if CHINESE
 * @brief 网格挂载的面板
 * @return 面板，可能为 nullptr
 * \endif
 */
SARibbonPanel* SARibbonParamGrid::panel() const
{
    SA_DC(d);
    return d->panel;
}

/**
 * \if ENGLISH
 * @brief Size hint of the grid
 * @details The ribbon panel only takes the width from this hint; the height is replaced by the panel's row
 *          geometry, so the height returned here is just "natural rows × row height".
 * \endif
 *
 * \if CHINESE
 * @brief 网格的尺寸提示
 * @details 丝带面板只取这里的宽度，高度会被面板的行几何替换，因此这里返回的高度只是「自然行数 × 行高」。
 * \endif
 */
QSize SARibbonParamGrid::sizeHint() const
{
    SA_DC(d);
    const_cast< PrivateData* >(d)->updateMetrics();
    const int layoutRows = d->rowsPerBlock;
    const int blockHeight =
        (layoutRows > 0) ? (layoutRows * d->naturalRowHeight + (layoutRows - 1) * d->rowSpacing) : 0;
    return QSize(d->totalWidth, blockHeight);
}

/**
 * \if ENGLISH
 * @brief Minimum size hint of the grid
 * \endif
 *
 * \if CHINESE
 * @brief 网格的最小尺寸提示
 * \endif
 */
QSize SARibbonParamGrid::minimumSizeHint() const
{
    SA_DC(d);
    const_cast< PrivateData* >(d)->updateMetrics();
    return QSize(d->minimumTotalWidth, d->naturalRowHeight);
}

/**
 * \if ENGLISH
 * @brief Event filter of the grid: font/style/direction changes and parent changes
 * \endif
 *
 * \if CHINESE
 * @brief 网格事件处理：字体/样式/方向变化与父对象变化
 * \endif
 */
bool SARibbonParamGrid::event(QEvent* e)
{
    SA_D(d);
    switch (e->type()) {
    case QEvent::ParentChange:
    case QEvent::Show:
    case QEvent::LayoutRequest: {
        SARibbonPanel* p = qobject_cast< SARibbonPanel* >(parentWidget());
        if (p && d->panel != p) {
            d->attachToPanel(p);
        } else if (p && !d->modeConnection) {
            d->connectRibbonBar();
        }
        break;
    }
    case QEvent::FontChange:
    case QEvent::StyleChange:
    case QEvent::LayoutDirectionChange: {
        d->markDirty();
        d->updateMetrics();
        d->layoutChildren();
        updateGeometry();
        break;
    }
    default:
        break;
    }
    return QWidget::event(e);
}

/**
 * \if ENGLISH
 * @brief Reposition the children after the panel resized the grid
 * \endif
 *
 * \if CHINESE
 * @brief 面板改变网格尺寸后重排子控件
 * \endif
 */
void SARibbonParamGrid::resizeEvent(QResizeEvent* e)
{
    SA_D(d);
    // Outside a panel the row budget is derived from the height, so the metrics have to follow
    if (!d->panel && d->maxRows <= 0) {
        d->markDirty();
    }
    d->updateMetrics();
    d->layoutChildren();
    QWidget::resizeEvent(e);
}

/**
 * \if ENGLISH
 * @brief Attach to the enclosing panel on first show
 * \endif
 *
 * \if CHINESE
 * @brief 首次显示时挂到所在面板上
 * \endif
 */
void SARibbonParamGrid::showEvent(QShowEvent* e)
{
    SA_D(d);
    if (!d->panel) {
        if (SARibbonPanel* p = qobject_cast< SARibbonPanel* >(parentWidget())) {
            d->attachToPanel(p);
        }
    } else if (!d->modeConnection) {
        d->connectRibbonBar();
    }
    QWidget::showEvent(e);
}