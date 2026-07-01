# Python 绑定使用指南

本文档介绍如何在 Python 中使用 SARibbon 绑定，涵盖 PyQt5、PyQt6 和 PySide6 三种绑定。

## 三种绑定概览

| 绑定 | 模块名 | 构建工具 | 包名 |
|------|--------|----------|------|
| PyQt5 | `PyQtSARibbon.saribbon` | SIP + PyQt-builder | `PyQtSARibbon` |
| PyQt6 | `PyQtSARibbon.saribbon` | SIP + PyQt-builder | `PyQtSARibbon` |
| PySide6 | `PySideSARibbon.saribbon` | Shiboken6 + CMake | `PySideSARibbon` |

### 导入方式

```python
# PyQt5
from PyQtSARibbon import saribbon

# PyQt6
from PyQtSARibbon import saribbon

# PySide6
from PySideSARibbon import saribbon
```

### 绑定差异

在编写跨绑定兼容的代码时，需注意以下差异：

| 差异点 | PyQt5 | PyQt6 / PySide6 |
|--------|-------|-----------------|
| QAction 导入 | `from PyQt5.QtWidgets import QAction` | `from PyQt6.QtGui import QAction` / `from PySide6.QtGui import QAction` |
| Qt 颜色枚举 | `Qt.red` | `Qt.GlobalColor.red` |
| app.exec | `app.exec_()` | `app.exec()` |
| QButtonGroup 信号 | `buttonClicked[int]` | `idClicked(int)` |
| 枚举类型 | SIP 中部分枚举映射为 int | PySide6 中为原生 Python Enum |
| SARibbonTheme | SIP: `setRibbonTheme(int)` | PySide6: `setRibbonTheme(SARibbonTheme)` |

### 类覆盖差异

PySide6 绑定比 SIP 绑定暴露了更多类：

| 类 | PyQt5/6 (SIP) | PySide6 (Shiboken6) |
|----|:---:|:---:|
| SARibbonMainWindow, SARibbonBar, SARibbonCategory, SARibbonPanel | ✓ | ✓ |
| SARibbonToolButton, SARibbonTabBar, SARibbonContextCategory | ✓ | ✓ |
| SARibbonGallery, SARibbonGalleryGroup, SARibbonGalleryItem | ✓ | ✓ |
| SARibbonButtonGroupWidget, SARibbonQuickAccessBar | ✓ | ✓ |
| SARibbonPanelItem, SARibbonPanelLabel | ✓ | ✓ |
| **SARibbonWidget** | ✗ | ✓ |
| **SARibbonMenu** | ✗ | ✓ |
| **SARibbonApplicationButton** | ✗ | ✓ |
| **SARibbonColorToolButton** | ✗ | ✓ |
| **SARibbonActionsManager, SARibbonActionsManagerModel** | ✗ | ✓ |
| **SARibbonCustomizeData, SARibbonCustomizeWidget, SARibbonCustomizeDialog** | ✗ | ✓ |

## 快速开始

### 基本示例（PySide6）

```python
import sys
from PySide6.QtWidgets import QApplication
from PySide6.QtGui import QAction, QIcon
from PySideSARibbon import saribbon

app = QApplication(sys.argv)

# Create ribbon main window
win = saribbon.SARibbonMainWindow()
win.setWindowTitle("My Ribbon App")

# Get the ribbon bar
ribbon = win.ribbonBar()
ribbon.setRibbonStyle(saribbon.SARibbonBar.RibbonStyleLooseThreeRow)

# Add a category (tab page)
home = ribbon.addCategoryPage("Home")

# Add a panel (button group)
file_panel = home.addPanel("File")

# Add actions
act_new = QAction(QIcon("new.svg"), "New")
file_panel.addLargeAction(act_new)

act_open = QAction(QIcon("open.svg"), "Open")
file_panel.addSmallAction(act_open)

# Set central widget
from PySide6.QtWidgets import QTextEdit
editor = QTextEdit()
win.setCentralWidget(editor)

win.show()
sys.exit(app.exec())
```

## 界面层次结构

```
SARibbonMainWindow
├── SARibbonBar (ribbon bar)
│   ├── SARibbonApplicationButton (top-left button)
│   ├── SARibbonCategory (tab page)
│   │   └── SARibbonPanel (panel / button group)
│   │       ├── QAction via addLargeAction / addSmallAction
│   │       ├── QWidget via addWidget / addSmallWidget / addLargeWidget
│   │       └── SARibbonGallery (gallery)
│   ├── SARibbonContextCategory (hidden tab, shown on demand)
│   ├── SARibbonQuickAccessBar (top toolbar)
│   └── SARibbonButtonGroupWidget (right button group)
└── Central widget (QTextEdit, etc.)
```

## 核心 API

### SARibbonMainWindow

SARibbonMainWindow 是 QMainWindow 的子类，内置 SARibbonBar。

```python
win = saribbon.SARibbonMainWindow()

# Get the ribbon bar
ribbon = win.ribbonBar()

# Set ribbon theme (6 built-in themes)
win.setRibbonTheme(saribbon.SARibbonTheme.RibbonThemeOffice2021Blue)

# Set central widget (same as QMainWindow)
win.setCentralWidget(editor)
```

### SARibbonBar

管理所有分类、面板和应用按钮。

```python
ribbon = win.ribbonBar()

# Set ribbon style (6 styles)
ribbon.setRibbonStyle(saribbon.SARibbonBar.RibbonStyleLooseThreeRow)

# Add a category
home = ribbon.addCategoryPage("Home")

# Context category (hidden by default)
ctx = ribbon.addContextCategory("Drawing Tools", Qt.GlobalColor.red)
ribbon.showContextCategory(ctx)
ribbon.hideContextCategory(ctx)

# Minimum mode (collapse ribbon to tab bar only)
ribbon.setMinimumMode(True)
ribbon.setMinimumMode(False)

# Word wrap for action text
ribbon.setEnableWordWrap(True)

# QuickAccessBar and right button group
qab = ribbon.quickAccessBar()
rbg = ribbon.rightButtonGroup()

# Application button
btn = saribbon.SARibbonApplicationButton("File")
ribbon.setApplicationButton(btn)
```

### SARibbonCategory

代表 Ribbon 中的一个标签页。

```python
home = ribbon.addCategoryPage("Home")
home.categoryName()        # "Home"
home.setCategoryName("Start")
```

### SARibbonPanel

面板是分类中的功能分组。

```python
panel = home.addPanel("File")

# Add actions (icon + text buttons)
panel.addLargeAction(action)   # icon above text
panel.addSmallAction(action)   # icon beside text
panel.addSeparator()           # visual separator

# Add widgets (for non-action controls like QComboBox, QRadioButton)
# RowProportion controls the widget's height within the panel:
#   SARibbonPanelItem.Large  — fills full panel height
#   SARibbonPanelItem.Medium — half height (two-row mode only)
#   SARibbonPanelItem.Small  — one row
RP = saribbon.SARibbonPanelItem
panel.addWidget(my_combobox, RP.Small)

# Add tool button widgets (SARibbonToolButton / SARibbonColorToolButton)
# Use addSmallWidget/addLargeWidget instead of addWidget
panel.addSmallWidget(tool_btn)
panel.addLargeWidget(tool_btn)

# Expanding panel (stretches to fill available width)
panel.setExpanding(True)
```

### SARibbonGallery

画廊是弹出式网格控件，支持多个可滚动分组。

```python
gallery = panel.addGallery()

# Create actions
actions = [QAction(icon, "Item {}".format(i+1)) for i in range(12)]

# Add a group
group = gallery.addCategoryActions("Items", actions)
gallery.setCurrentViewGroup(group)

# Multiple groups
group2 = gallery.addCategoryActions("Apps", more_actions)
```

### SARibbonColorToolButton (PySide6 only)

颜色选择按钮，继承自 SARibbonToolButton。

```python
btn = saribbon.SARibbonColorToolButton()
btn.setColor(QColor("#FF0000"))
btn.setColorStyle(saribbon.SARibbonColorToolButton.ColorFillToIcon)
btn.setText("Red")
panel.addSmallWidget(btn)  # use addSmallWidget, NOT addWidget

# Large variant
btn2 = saribbon.SARibbonColorToolButton()
btn2.setColor(QColor("#0000FF"))
btn2.setButtonType(saribbon.SARibbonToolButton.LargeButton)
btn2.setColorStyle(saribbon.SARibbonColorToolButton.ColorFillToIcon)
btn2.setText("Blue")
panel.addLargeWidget(btn2)
```

### SARibbonApplicationButton (PySide6)

左上角的应用按钮。

```python
btn = saribbon.SARibbonApplicationButton("File")
ribbon.setApplicationButton(btn)
```

### SARibbonWidget (PySide6)

带 RibbonBar 的 Widget（用于非 MainWindow 场景）。

```python
widget = saribbon.SARibbonWidget()
ribbon = widget.ribbonBar()
```

### SARibbonMenu (PySide6)

Ribbon 专用菜单，支持添加子菜单和控件。

```python
menu = saribbon.SARibbonMenu("Test Menu")
sub = menu.addRibbonMenu("Submenu")
menu.addAction("Item")
```

## 可用类列表

### 所有绑定共有

| 类名 | 说明 |
|------|------|
| SARibbonMainWindow | Ribbon 主窗口（替代 QMainWindow） |
| SARibbonBar | 主 Ribbon 工具栏 |
| SARibbonCategory | Ribbon 类别（标签页） |
| SARibbonPanel | 面板（功能分组） |
| SARibbonPanelItem | 面板布局项（含 RowProportion 枚举） |
| SARibbonPanelLabel | 面板标题标签 |
| SARibbonToolButton | Ribbon 按钮 |
| SARibbonTabBar | 标签栏 |
| SARibbonContextCategory | 上下文类别 |
| SARibbonGallery | 画廊控件 |
| SARibbonGalleryGroup | 画廊分组 |
| SARibbonGalleryItem | 画廊项目 |
| SARibbonButtonGroupWidget | 按钮组 |
| SARibbonQuickAccessBar | 快速访问工具栏 |
| SARibbonGalleryButton | 画廊滚动按钮 |
| SARibbonGalleryViewport | 画廊视口 |

### 仅 PySide6

| 类名 | 说明 |
|------|------|
| SARibbonWidget | 带 RibbonBar 的 Widget |
| SARibbonMenu | Ribbon 专用菜单 |
| SARibbonApplicationButton | 应用按钮 |
| SARibbonColorToolButton | 颜色选择按钮 |
| SARibbonActionsManager | 动作管理器 |
| SARibbonActionsManagerModel | 动作管理器模型 |
| SARibbonCustomizeData | 自定义数据 |
| SARibbonCustomizeWidget | 自定义界面窗口 |
| SARibbonCustomizeDialog | 自定义对话框 |

## 面板方法参考

| 方法 | 说明 |
|------|------|
| `addLargeAction(action)` | 添加大按钮（图标在上，文字在下） |
| `addSmallAction(action)` | 添加小按钮（图标和文字并排） |
| `addMediumAction(action)` | 添加中按钮 |
| `addLargeWidget(widget)` | 添加大控件（SARibbonToolButton 等用） |
| `addSmallWidget(widget)` | 添加小控件 |
| `addMediumWidget(widget)` | 添加中控件 |
| `addWidget(widget, rowProportion)` | 添加控件并指定行比例（QRadioButton/QComboBox 等用） |
| `addGallery()` | 添加画廊控件 |
| `addSeparator()` | 添加分隔符 |
| `setExpanding(bool)` | 设置面板是否扩展填充剩余空间 |

## 枚举参考

### SARibbonBar.RibbonStyleFlag

| 值 | 说明 |
|------|------|
| `RibbonStyleLooseThreeRow` | Office 风格，3 行，显示面板标题 |
| `RibbonStyleCompactThreeRow` | WPS 风格，3 行，不显示面板标题 |
| `RibbonStyleLooseTwoRow` | Office 风格，2 行 |
| `RibbonStyleCompactTwoRow` | WPS 风格，2 行 |
| `RibbonStyleLooseSingleRow` | 超紧凑单行（Outlook 2025 风格） |
| `RibbonStyleCompactSingleRow` | 紧凑单行 |

### SARibbonTheme

| 值 | 说明 |
|------|------|
| `RibbonThemeWindows7` | Windows 7 主题 |
| `RibbonThemeOffice2013` | Office 2013 主题 |
| `RibbonThemeOffice2016Blue` | Office 2016 蓝色主题 |
| `RibbonThemeOffice2021Blue` | Office 2021 蓝色主题（默认） |
| `RibbonThemeDark` | 深色主题 |
| `RibbonThemeDark2` | 深色主题 2 |

### SARibbonBar.RibbonMode

| 值 | 说明 |
|------|------|
| `NormalRibbonMode` | 正常模式（显示完整 Ribbon） |
| `MinimumRibbonMode` | 最小模式（仅显示标签栏） |

### SARibbonPanelItem.RowProportion

| 值 | 说明 |
|------|------|
| `None` | 自动判断 |
| `Large` | 填充整个面板高度 |
| `Medium` | 半高（仅三行模式） |
| `Small` | 单行高度 |

## 示例程序

```bash
# PySide6
python pyexamples/pyside6/ribbon_demo.py

# PyQt5
python pyexamples/pyqt5/ribbon_demo.py

# PyQt6
python pyexamples/pyqt6/ribbon_demo.py
```

每个 demo 展示了：多分类/面板、样式切换、主题切换、Gallery、上下文类别、QuickAccessBar、颜色按钮、字体控制等。

## 下一步

- 查看 `pyexamples/` 下的 demo 获取完整示例代码
- 参考 C++ API 文档了解更多功能
- 参考 [构建 Python 绑定](./build-python-bindings.md) 和 [构建 PySide6 绑定](./build-pyside6-bindings.md)
