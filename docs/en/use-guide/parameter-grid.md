# Parameter Grid (SARibbonParamGrid)

## Why a Parameter Grid

Ribbon panels are designed for buttons: each button declares how many rows it occupies through `SARibbonPanelItem::RowProportion`, and the panel arranges them into tidy columns. But a panel often holds a group of **parameters** instead—channel, window function, amplitude basis, frequency resolution—each being a label plus an input widget.

Adding such parameters one by one with `addSmallWidget()` runs into two problems:

1. **Labels and fields do not line up.** Every label is part of a separate widget, so its width only follows its own text. The fields of "Channel", "Window", and "Frequency Resolution" end up with ragged left edges.
2. **Switching row modes squeezes the rows.** The panel changes its row height when it switches between 3-row, 2-row, and single-row modes. A vertically stacked parameter list that exceeds the panel's row budget gets compressed, clipped, or overlapped.

`SARibbonParamGrid` solves both: it places a group of rows in one grid, **shares a single label-column width across all rows**, and **reflows into side-by-side column blocks** when the panel row count changes.

## The Three-Column Model

One row always follows a three-segment structure:

```text
+----------+------------------+----------+
| Leading  |      Field       | Trailing |
+----------+------------------+----------+
   leading        field            trailing
```

`SARibbonParamGrid::LabelSides` selects which cells a row occupies:

| Enum value | Value | Layout |
|------------|-------|--------|
| `LabelNone` | `0x0` | field only |
| `LabelLeading` | `0x1` | leading label + field |
| `LabelTrailing` | `0x2` | field + trailing label |
| `LabelBoth` | `LabelLeading` + `LabelTrailing` | leading label + field + trailing label |

All rows in a grid share the same column widths, so the field column stays aligned no matter how long each label is.

## Quick Start

Treat the grid as an ordinary widget and add it to a panel. `addLargeWidget()` is recommended so the grid fills the panel height:

```cpp
#include "SARibbonParamGrid.h"
#include "SARibbonPanel.h"

// panel comes from SARibbonCategory::addPanel()
SARibbonParamGrid* grid = new SARibbonParamGrid(panel);
grid->setObjectName(QStringLiteral("paramGrid"));

QComboBox* channel = new QComboBox(grid);
channel->setMinimumWidth(120);
grid->addRow(tr("Channel"), channel);

QDoubleSpinBox* resolution = new QDoubleSpinBox(grid);
resolution->setSuffix(tr(" Hz"));
resolution->setMinimumWidth(112);
grid->addRow(tr("Resolution"), resolution);

// A read-only hint spanning the block width
grid->addSpanRow(new QLabel(tr("Δf = 1.0 Hz"), grid));

panel->addLargeWidget(grid);
```

`addRow()` / `addSpanRow()` reparent the widgets into the grid, which takes ownership of them.

### The four addRow overloads

| Overload | Description |
|----------|-------------|
| `int addRow(QWidget* field)` | field only |
| `int addRow(const QString& leading, QWidget* field)` | leading label + field |
| `int addRow(const QString& leading, QWidget* field, const QString& trailing)` | leading label + field + trailing label |
| `int addRow(QWidget* field, LabelSides sides, const QString& leading = QString(), const QString& trailing = QString())` | explicit label sides |

The return value is a row id (starting at 1), or `-1` when the field is null. The id is used for later operations:

```cpp
int id = grid->addRow(tr("Window"), windowCombo);

grid->setRowText(id, tr("Window Function"), QString());   // change label text
grid->setRowLabelSides(id, SARibbonParamGrid::LabelBoth); // change label sides
QWidget* field = grid->fieldAt(id);                       // get the field back
QLabel* leading = grid->leadingLabel(id);                 // leading label (nullptr if none)
grid->removeRow(id);                                      // remove one row
grid->clearRows();                                        // remove every row
```

### addSpanRow

A row added with `addSpanRow(QWidget*)` spans the full width of its **column block** (not the whole grid). It is meant for read-only hints, separators, and status text. Span rows take part in row distribution just like ordinary rows.

## Label Column Width

### Automatic width

By default the label column width is computed from the current font:

- leading column width = pixel width of the widest leading label + 10, clamped to `[0, 200]` (the lower bound defaults to 0, so a short label column is never padded wider than needed);
- the trailing column is computed independently with the same rule;
- when the grid has no non-empty leading labels (or no trailing labels), that column width is 0 and no space is reserved.
- the horizontal gap between a label column and the field column is controlled by `setLabelSpacing(px)`, default 4 px (plus the 10 px text padding, roughly 14 px between text and field).

After a font change or a translation switch, call `recalcColumnWidths()` to recompute. `addRow()` and `setRowText()` already do this internally, so manual calls are rarely needed.

### Label-to-field gap

If the label sits too close to its field (or too far), tune it with `setLabelSpacing(px)` (default 4):

```cpp
grid->setLabelSpacing(2);   // tighter
grid->setLabelSpacing(12);  // looser
```

The row height is not controlled here: it always follows the ribbon panel row grid (see the next section); this gap only affects the horizontal distance between a label and its field.

### Overriding with a fixed width

Use `setLabelColumnWidth(px)` to pin the label column; pass a negative value to restore the automatic width:

```cpp
grid->setLabelColumnWidth(80);   // fixed 80 px
grid->setLabelColumnWidth(-1);   // back to automatic
```

`labelColumnWidth()` returns the current leading label width (0 when the grid has no leading labels).

## Row Adaptation and Column Blocks

### setMaxRows(0) follows the panel

`setMaxRows(0)` (the default) means the row budget per column block **follows the enclosing panel's `panelLayoutMode()`**:

| Panel layout mode | Max rows per block |
|-------------------|--------------------|
| `SARibbonPanel::ThreeRowMode` | 3 |
| `SARibbonPanel::TwoRowMode` | 2 |
| `SARibbonPanel::SingleRowMode` | 1 |

The grid listens to `SARibbonBar::panelLayoutModeChanged` itself and reflows when the style changes, so application code does not need to connect that signal again.

### Row height follows the ribbon row grid

The row height is not the available height divided by the number of content rows; it follows the panel row grid: `row height = (available height - row spacing) / panel row count`. Therefore:

- a two-row grid inside a three-row panel fills the first two ribbon rows and leaves the third one empty, lining up with the small items of the neighbouring panels;
- a two-row grid inside a two-row panel fills the panel exactly;
- in single-row mode every column block is exactly one row tall.

A field is `min(field sizeHint height, row height)` tall and vertically centered, so a 22 px ribbon row never clips a 24 px combo box. Column blocks run left to right, rows inside a block run top to bottom, and leftover block height stays at the bottom.

The number of blocks is `ceil(total rows / rows per block)`. For four parameter rows:

```text
3-row mode (3 rows per block) -> 2 columns x 2 rows (BalancedFlow)
+-------------+-------------+
| Label | Field | Label | Field |
+-------------+-------------+
| Label | Field | Label | Field |
+-------------+-------------+

2-row mode (2 rows per block) -> 2 columns x 2 rows
Single-row mode (1 row per block) -> 4 columns x 1 row
```

### flowPolicy: how remaining rows are spread

`SARibbonParamGrid::FlowPolicy` decides the row count of each block:

| Policy | Behavior |
|--------|----------|
| `BalancedFlow` (default) | spread rows as evenly as possible; the first blocks take the extra row |
| `ColumnFirstFlow` | fill earlier blocks up to `maxRows` before starting a new block |

For four rows with a budget of 3, `BalancedFlow` yields `2 + 2`, while `ColumnFirstFlow` yields `3 + 1`.

### Explicit row budget

When the grid should not follow the panel, set a fixed budget:

```cpp
grid->setMaxRows(2);   // always at most 2 rows per block
grid->setMaxRows(0);   // follow the panel again
```

`currentRows()` returns the row count of the tallest block and `currentColumns()` the number of blocks, which is handy when debugging the layout:

```cpp
qDebug() << grid->rowCount() << grid->currentRows() << grid->currentColumns();
```

## Caveats

> **⚠️ Panel height comes from the Ribbon row height**
> The grid does not decide its own height: the row height is `(available height - row spacing) / panel row count`, and each child is vertically centered and fitted to that row height. Keep the widgets inside a grid compact—a widget with a tall `sizeHint()` (a calendar, for example) will almost certainly be clipped in single-row mode.

> **⚠️ The grid does not stretch**
> The grid deliberately keeps `QSizePolicy::Preferred`: the panel gives it exactly the width reported by `sizeHint()` and it never steals width from neighbouring panels. To control width, call `setMinimumWidth()` on the field widgets (the field column is the maximum of their `sizeHint()`, `minimumSizeHint()`, and `minimumSize()` widths).

> **⚠️ Overflow is handled by ribbon horizontal scrolling**
> In single-row mode four parameter rows become four columns, which may be wider than the window. SARibbon does not compress the grid; the category becomes horizontally scrollable (edge scroll buttons or mouse wheel), preserving column widths and alignment.

> **⚠️ The panel must be resolvable**
> The grid resolves its panel from `parentWidget()` when it is created and retries on the show event; only then does it connect to that panel's ribbon bar. If the grid is wrapped in an intermediate container (its direct parent is not a `SARibbonPanel`), call `attachToPanel(panel)` explicitly, otherwise the row count will not follow the panel.

## API Reference

| Group | Methods |
|-------|---------|
| Content | `addRow` (4 overloads), `addSpanRow`, `removeRow`, `clearRows`, `rowCount`, `fieldAt` |
| Labels | `setRowText`, `setRowLabelSides`, `leadingLabel`, `trailingLabel`, `setLabelAlignment`, `labelColumnWidth`, `setLabelColumnWidth`, `setLabelColumnWidthRange`, `setLabelSpacing`, `recalcColumnWidths` |
| Row/column policy | `setMaxRows`, `maxRows`, `setFlowPolicy`, `flowPolicy`, `currentRows`, `currentColumns`, `setRowSpacing`, `setColumnSpacing` |
| Panel integration | `attachToPanel`, `panel` |

See also: [SARibbon Layout Methods](./layout-of-SARibbon.md), [Ribbon Button Layout Guide](./layout-of-ribbonbutton.md).
