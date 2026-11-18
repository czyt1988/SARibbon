#ifndef SARIBBONPARAMGRID_H
#define SARIBBONPARAMGRID_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QList>
class QLabel;
class SARibbonPanel;

/**
 * \if ENGLISH
 * @brief Parameter grid for ribbon panels
 * @details `SARibbonParamGrid` places a group of "label + field" rows inside a ribbon panel. All rows share
 *          one label-column width and one field-column width, so labels and fields line up instead of looking
 *          ragged. When the enclosing panel switches its row count (3 / 2 / 1 rows), the grid reflows its rows
 *          into several side-by-side column blocks so that nothing gets clipped, overlapped or squeezed.
 *
 * A row follows the same Prefix|Widget|Suffix model as @ref SARibbonLineWidgetContainer:
 * - `LabelLeading`  : | label | field |
 * - `LabelTrailing` : | field | label |
 * - `LabelBoth`     : | label | field | label |
 * - `LabelNone`     : | field |
 *
 * Rows are distributed into blocks with `ceil(rowCount / maxRows)` blocks; see @ref FlowPolicy for how the
 * remainder rows are spread. A row added with addSpanRow() stretches across its whole block.
 *
 * @note The grid keeps a Preferred width policy on purpose: the ribbon panel gives its items the width reported
 *       by sizeHint() and never stretches them, so the grid cannot steal width from its neighbouring panels.
 * @see SARibbonPanel::addLargeWidget, SARibbonLineWidgetContainer, SARibbonBar::panelLayoutModeChanged
 * \endif
 *
 * \if CHINESE
 * @brief 丝带面板的参数网格
 * @details `SARibbonParamGrid` 把一组「标签 + 控件」行放进丝带面板。所有行共享同一个标签列宽和控件列宽，
 *          标签与控件因此左边界齐平，不会参差不齐。当所在面板切换行数（3 / 2 / 1 行）时，网格会把行
 *          重新摊成若干并排的列块，避免内容被裁切、重叠或压扁。
 *
 * 一行的布局沿用 @ref SARibbonLineWidgetContainer 的 Prefix|Widget|Suffix 模型：
 * - `LabelLeading`  : | 标签 | 控件 |
 * - `LabelTrailing` : | 控件 | 标签 |
 * - `LabelBoth`     : | 标签 | 控件 | 标签 |
 * - `LabelNone`     : | 控件 |
 *
 * 列块数固定为 `ceil(行数 / maxRows)`；余下几行怎么摊见 @ref FlowPolicy。用 addSpanRow() 加入的行
 * 会横跨整个列块。
 *
 * @note 网格刻意保持 Preferred 宽度策略：丝带面板给 item 的宽度就是 sizeHint() 报的宽度、且不会拉伸，
 *       因此网格不会抢走相邻面板的宽度。
 * @see SARibbonPanel::addLargeWidget, SARibbonLineWidgetContainer, SARibbonBar::panelLayoutModeChanged
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonParamGrid : public QWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonParamGrid)
public:
    // Label side(s) of one row
    enum LabelSides
    {
        LabelNone     = 0x0,                          ///< | field |
        LabelLeading  = 0x1,                          ///< | label | field |
        LabelTrailing = 0x2,                          ///< | field | label |
        LabelBoth     = LabelLeading | LabelTrailing  ///< | label | field | label |
    };
    Q_ENUM(LabelSides)

    // How the rows that do not fit one block are spread over the remaining blocks
    enum FlowPolicy
    {
        BalancedFlow,    ///< spread rows as evenly as possible across blocks
        ColumnFirstFlow  ///< fill each block up to the row budget before starting the next
    };
    Q_ENUM(FlowPolicy)
public:
    // Constructor
    explicit SARibbonParamGrid(QWidget* parent = nullptr);
    // Destructor
    ~SARibbonParamGrid() override;

    // --- content ---
    // Add a row with a field only, returns the row id (>= 1) or -1 on failure
    int addRow(QWidget* field);
    // Add a row with a leading label
    int addRow(const QString& leading, QWidget* field);
    // Add a row with leading and trailing labels
    int addRow(const QString& leading, QWidget* field, const QString& trailing);
    // Add a row with explicit label sides, returns the row id or -1 on failure
    int addRow(QWidget* field, SARibbonParamGrid::LabelSides sides, const QString& leading = QString(),
               const QString& trailing = QString());
    // Add a row whose widget spans the whole block width, returns the row id or -1 on failure
    int addSpanRow(QWidget* widget);
    // Remove a row by id, returns false when the id is unknown
    bool removeRow(int rowId);
    // Remove all rows
    void clearRows();
    // Number of rows
    int rowCount() const;
    // Field widget of a row, nullptr when the id is unknown
    QWidget* fieldAt(int rowId) const;

    // --- labels ---
    // Set the leading/trailing text of a row
    void setRowText(int rowId, const QString& leading, const QString& trailing = QString());
    // Set the label sides of a row
    void setRowLabelSides(int rowId, SARibbonParamGrid::LabelSides sides);
    // Leading label of a row, nullptr when the row has none
    QLabel* leadingLabel(int rowId) const;
    // Trailing label of a row, nullptr when the row has none
    QLabel* trailingLabel(int rowId) const;
    // Alignment used by the row labels
    void setLabelAlignment(Qt::Alignment alignment);
    // Current label column width in pixels
    int labelColumnWidth() const;
    // Fixed label column width in pixels, pass a negative value to restore the automatic width
    void setLabelColumnWidth(int px);
    // Range used by the automatic label column width
    void setLabelColumnWidthRange(int minPx, int maxPx);
    // Recompute the column widths, call it after a font or translation change
    void recalcColumnWidths();

    // --- row/column policy ---
    // Row budget per block, 0 means "follow the enclosing panel" (default)
    void setMaxRows(int rows);
    // Row budget per block, 0 means automatic
    int maxRows() const;
    // How rows are spread over the blocks
    void setFlowPolicy(SARibbonParamGrid::FlowPolicy policy);
    // How rows are spread over the blocks
    SARibbonParamGrid::FlowPolicy flowPolicy() const;
    // Number of rows in the tallest block right now
    int currentRows() const;
    // Number of side-by-side blocks right now
    int currentColumns() const;
    // Vertical spacing between rows in pixels
    void setRowSpacing(int px);
    // Horizontal spacing between blocks in pixels
    void setColumnSpacing(int px);

    // --- panel integration ---
    // Attach the grid to a panel, the enclosing panel is resolved automatically when not called
    void attachToPanel(SARibbonPanel* panel);
    // Panel this grid is attached to, may be nullptr
    SARibbonPanel* panel() const;

public:
    // Size hint, the panel only takes the width from it
    QSize sizeHint() const override;
    // Minimum size hint
    QSize minimumSizeHint() const override;

protected:
    // React to resize, font, style and parent changes
    bool event(QEvent* e) override;
    // Reposition the children after a resize
    void resizeEvent(QResizeEvent* e) override;
    // Attach to the enclosing panel when shown
    void showEvent(QShowEvent* e) override;

private:
    // Recompute the geometry of all children
    void layoutChildren();
};

#endif  // SARIBBONPARAMGRID_H