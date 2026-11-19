# 参数网格 (SARibbonParamGrid)

## 为什么需要参数网格

Ribbon 面板天生适合放按钮：每个按钮用 `SARibbonPanelItem::RowProportion` 声明自己占几行，面板按列把按钮排整齐。但面板里要放的往往不是按钮，而是一组**参数**——通道、窗函数、幅值口径、频率分辨率……每项都是「标签 + 一个输入控件」。

把这类参数直接用 `addSmallWidget()` 逐个塞进面板会碰到两个问题：

1. **标签列与控件列不对齐**。每个标签都是控件的文本或独立控件，宽度只按自己的文字算，于是「通道」「窗函数」「频率分辨率」三行的控件左边界参差不齐，看起来像随手摆的。
2. **切换行数时被压扁**。面板在 3 行 / 2 行 / 1 行模式之间切换时行高会变；纵向堆叠的参数一旦超过面板行数，就被压缩、裁切或叠在一起。

`SARibbonParamGrid` 就是为这两个问题准备的：它把一组行放进同一个网格，**所有行共享标签列宽**，并在面板行数变化时**自动横向分列**。

## 三列制模型

网格中的一行固定是三段结构：

```text
+----------+------------------+----------+
| 前标签   |      控件        |  后标签  |
+----------+------------------+----------+
   leading        field            trailing
```

用 `SARibbonParamGrid::LabelSides` 选择这一行占用哪些单元格：

| 枚举值 | 取值 | 布局 |
|--------|------|------|
| `LabelNone` | `0x0` | 只有控件 |
| `LabelLeading` | `0x1` | 前标签 + 控件 |
| `LabelTrailing` | `0x2` | 控件 + 后标签 |
| `LabelBoth` | `LabelLeading` + `LabelTrailing` | 前标签 + 控件 + 后标签 |

同一网格内所有行共用同一套列宽，因此无论每行标签长短，控件列都严格对齐。

## 快速上手

把网格当成一个普通控件加到面板里即可，推荐用 `addLargeWidget()` 让它占满面板高度：

```cpp
#include "SARibbonParamGrid.h"
#include "SARibbonPanel.h"

// panel 是 SARibbonCategory::addPanel() 得到的面板
SARibbonParamGrid* grid = new SARibbonParamGrid(panel);
grid->setObjectName(QStringLiteral("paramGrid"));

QComboBox* channel = new QComboBox(grid);
channel->setMinimumWidth(120);
grid->addRow(tr("Channel"), channel);

QDoubleSpinBox* resolution = new QDoubleSpinBox(grid);
resolution->setSuffix(tr(" Hz"));
resolution->setMinimumWidth(112);
grid->addRow(tr("Resolution"), resolution);

// 跨列只读提示
grid->addSpanRow(new QLabel(tr("Δf = 1.0 Hz"), grid));

panel->addLargeWidget(grid);
```

`addRow()` / `addSpanRow()` 会把控件重新 setParent 到网格，网格接管这些控件的生命周期。

### addRow 的四个重载

| 重载 | 说明 |
|------|------|
| `int addRow(QWidget* field)` | 只有控件 |
| `int addRow(const QString& leading, QWidget* field)` | 前标签 + 控件 |
| `int addRow(const QString& leading, QWidget* field, const QString& trailing)` | 前标签 + 控件 + 后标签 |
| `int addRow(QWidget* field, LabelSides sides, const QString& leading = QString(), const QString& trailing = QString())` | 显式指定标签侧 |

返回值为行 id（从 1 开始），字段控件为空时返回 `-1`。行 id 用于后续操作：

```cpp
int id = grid->addRow(tr("Window"), windowCombo);

grid->setRowText(id, tr("Window Function"), QString());  // 改标签文本
grid->setRowLabelSides(id, SARibbonParamGrid::LabelBoth); // 改占用的标签侧
QWidget* field = grid->fieldAt(id);                        // 取回控件
QLabel* leading = grid->leadingLabel(id);                  // 取前标签（没有则为 nullptr）
grid->removeRow(id);                                       // 删除一行
grid->clearRows();                                         // 清空所有行
```

### addSpanRow

`addSpanRow(QWidget*)` 加入的行会横跨它所在**整个列块**的宽度（不是整个网格），适合放只读提示、分隔线、状态文本。被跨列的行和普通行一样参与行数分配。

## 标签列宽

### 自动宽度

默认情况下，标签列宽按当前字体自动计算：

- 前标签列宽 = 所有前标签文本中最宽者的像素宽度 + 10，再 clamp 到 `[0, 200]`（默认下限 0，标签列不会为了"统一"被撑宽）；
- 后标签列宽同理独立计算；
- 若网格里没有任何非空的前标签（或后标签），对应列宽为 0，不会被预留。
- 标签列与控件列之间的间隙由 `setLabelSpacing(px)` 控制，默认 4px（加上自动宽度里的 10px padding，文字与控件之间约 14px）。

字体变化或翻译切换后，调用 `recalcColumnWidths()` 重新计算；`addRow()`、`setRowText()` 内部已经会重算，通常不需要手动调用。

### 标签与控件的间距

如果觉得标签离控件太近或太远，用 `setLabelSpacing(px)` 调整（默认 4）：

```cpp
grid->setLabelSpacing(2);   // 更紧凑
grid->setLabelSpacing(12);  // 更宽松
```

行高不在这一层控制：行高始终跟随丝带的面板行网格（见下一节），网格只用这个间隙微调标签与控件的水平距离。

### 覆盖为固定宽度

需要精确控制时，用 `setLabelColumnWidth(px)` 固定标签列宽；传入负值恢复自动宽度：

```cpp
grid->setLabelColumnWidth(80);   // 固定 80px
grid->setLabelColumnWidth(-1);   // 恢复自动宽度
```

`labelColumnWidth()` 返回当前前标签列宽（网格没有前标签时返回 0）。

## 行数自适应与分列

### setMaxRows(0) 跟随面板

`setMaxRows(0)`（默认值）表示**每个列块的行数预算跟随所在面板的 `panelLayoutMode()`**：

| 面板布局模式 | 每个列块最多行数 |
|--------------|------------------|
| `SARibbonPanel::ThreeRowMode` | 3 |
| `SARibbonPanel::TwoRowMode` | 2 |
| `SARibbonPanel::SingleRowMode` | 1 |

网格会自己监听 `SARibbonBar::panelLayoutModeChanged` 信号，在样式切换时重新分列，不需要业务代码再连一遍信号。

### 行高跟随丝带行网格

行高不是按内容行数均分，而是**跟随面板的行网格**：`行高 = (可用高度 − 行间距) / 面板行数`。所以：

- 3 行面板里只有 2 行的网格，占满前两行、第三行留空，和相邻面板的小号项对齐；
- 2 行面板里 2 行的网格正好铺满；
- 1 行面板里每个列块正好一行。

控件高度取 `min(控件 sizeHint 高度, 行高)` 并垂直居中，所以 22px 的丝带行不会把 24px 的下拉框裁掉。列块之间横向排列、每块内部从上往下排行，块内剩余高度留在底部。

列块数固定为 `ceil(总行数 / 每块行数)`。以 4 行参数为例：

```text
3 行模式（每块 3 行）→ 2 列 × 2 行（BalancedFlow）
+-------------+-------------+
| 标签 | 控件 | 标签 | 控件 |
+-------------+-------------+
| 标签 | 控件 | 标签 | 控件 |
+-------------+-------------+

2 行模式（每块 2 行）→ 2 列 × 2 行
1 行模式（每块 1 行）→ 4 列 × 1 行
```

### flowPolicy：余下的行怎么摊

每块行数由 `SARibbonParamGrid::FlowPolicy` 决定：

| 策略 | 行为 |
|------|------|
| `BalancedFlow`（默认） | 行尽量均匀摊到各列块；除不尽时前面的列块多一行 |
| `ColumnFirstFlow` | 先填满前面的列块（每块最多 `maxRows` 行），再开新列块 |

例如 4 行、行预算 3 时，`BalancedFlow` 得到 `2 + 2`；`ColumnFirstFlow` 得到 `3 + 1`。

### 显式指定行预算

不跟随面板时，可以直接给固定预算：

```cpp
grid->setMaxRows(2);   // 永远每块最多 2 行
grid->setMaxRows(0);   // 恢复跟随面板
```

`currentRows()` 返回当前最高列块的行数，`currentColumns()` 返回当前列块数，可用于调试布局：

```cpp
qDebug() << grid->rowCount() << grid->currentRows() << grid->currentColumns();
```

## 注意事项

> **⚠️ 面板高度由 Ribbon 行高决定**
> 网格本身不决定高度：行高 = (可用高度 − 行间距) / 面板行数，控件在行内垂直居中，且高度不会超过行高。因此放进网格的控件要足够紧凑——一个 `sizeHint()` 很高的控件（比如日历）在 1 行模式下几乎必被裁切。

> **⚠️ 网格宽度不拉伸**
> 网格刻意保持 `QSizePolicy::Preferred`，面板只按 `sizeHint()` 报的宽度给它，不会把相邻面板的宽度抢过来。想控制宽度，请给字段控件设 `setMinimumWidth()`（控件列宽取所有控件 `sizeHint()` / `minimumSizeHint()` / `minimumSize()` 的最大值）。

> **⚠️ 宽度超出时靠丝带横向滚动**
> 单行模式下 4 行参数会摊成 4 列，面板可能比窗口还宽。SARibbon 不会压缩网格，而是让该分类页进入横向滚动（面板两端的滚动按钮或鼠标滚轮），保持列宽与对齐不变。

> **⚠️ 面板必须能被解析到**
> 网格在创建时从 `parentWidget()` 解析所在面板，并在显示事件里重试；拿到面板后才会连接该面板的 ribbon bar。如果网格被包在中间容器里（直接父对象不是 `SARibbonPanel`），请显式调用 `attachToPanel(panel)`，否则行数不会跟随面板切换。

## API 速查

| 分组 | 方法 |
|------|------|
| 内容 | `addRow`（4 个重载）、`addSpanRow`、`removeRow`、`clearRows`、`rowCount`、`fieldAt` |
| 标签 | `setRowText`、`setRowLabelSides`、`leadingLabel`、`trailingLabel`、`setLabelAlignment`、`labelColumnWidth`、`setLabelColumnWidth`、`setLabelColumnWidthRange`、`setLabelSpacing`、`recalcColumnWidths` |
| 行列策略 | `setMaxRows`、`maxRows`、`setFlowPolicy`、`flowPolicy`、`currentRows`、`currentColumns`、`setRowSpacing`、`setColumnSpacing` |
| 面板集成 | `attachToPanel`、`panel` |

相关阅读：[SARibbon布局方式](./layout-of-SARibbon.md)、[Ribbon按钮布局说明](./layout-of-ribbonbutton.md)。
