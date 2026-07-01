# Python Bindings Usage Guide

This document covers how to use SARibbon bindings in Python, for all three bindings: PyQt5, PyQt6, and PySide6.

## Three Bindings Overview

| Binding | Module Name | Build Tool | Package Name |
|---------|-------------|------------|--------------|
| PyQt5 | `PyQtSARibbon.saribbon` | SIP + PyQt-builder | `PyQtSARibbon` |
| PyQt6 | `PyQtSARibbon.saribbon` | SIP + PyQt-builder | `PyQtSARibbon` |
| PySide6 | `PySideSARibbon.saribbon` | Shiboken6 + CMake | `PySideSARibbon` |

### Import

```python
# PyQt5
from PyQtSARibbon import saribbon

# PyQt6
from PyQtSARibbon import saribbon

# PySide6
from PySideSARibbon import saribbon
```

### Binding Differences

When writing cross-binding compatible code, note these differences:

| Difference | PyQt5 | PyQt6 / PySide6 |
|------------|-------|-----------------|
| QAction import | `from PyQt5.QtWidgets import QAction` | `from PyQt6.QtGui import QAction` / `from PySide6.QtGui import QAction` |
| Qt color enum | `Qt.red` | `Qt.GlobalColor.red` |
| app.exec | `app.exec_()` | `app.exec()` |
| QButtonGroup signal | `buttonClicked[int]` | `idClicked(int)` |
| Enum type | SIP maps some enums to int | PySide6 uses native Python Enum |
| SARibbonTheme | SIP: `setRibbonTheme(int)` | PySide6: `setRibbonTheme(SARibbonTheme)` |

### Class Coverage Differences

PySide6 bindings expose more classes than SIP bindings:

| Class | PyQt5/6 (SIP) | PySide6 (Shiboken6) |
|-------|:---:|:---:|
| SARibbonMainWindow, SARibbonBar, SARibbonCategory, SARibbonPanel | Yes | Yes |
| SARibbonToolButton, SARibbonTabBar, SARibbonContextCategory | Yes | Yes |
| SARibbonGallery, SARibbonGalleryGroup, SARibbonGalleryItem | Yes | Yes |
| SARibbonButtonGroupWidget, SARibbonQuickAccessBar | Yes | Yes |
| SARibbonPanelItem, SARibbonPanelLabel | Yes | Yes |
| **SARibbonWidget** | No | Yes |
| **SARibbonMenu** | No | Yes |
| **SARibbonApplicationButton** | No | Yes |
| **SARibbonColorToolButton** | No | Yes |
| **SARibbonActionsManager, SARibbonActionsManagerModel** | No | Yes |
| **SARibbonCustomizeData, SARibbonCustomizeWidget, SARibbonCustomizeDialog** | No | Yes |

## Quick Start

### Basic Example (PySide6)

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

## UI Hierarchy

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

## Core API

### SARibbonMainWindow

SARibbonMainWindow is a QMainWindow subclass with a built-in SARibbonBar.

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

Manages all categories, panels, and the application button.

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

Represents a single tab page in the ribbon.

```python
home = ribbon.addCategoryPage("Home")
home.categoryName()        # "Home"
home.setCategoryName("Start")
```

### SARibbonPanel

A panel is a functional group within a category.

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

A gallery is a popup grid widget with scrollable groups.

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

Color selection button, inherits from SARibbonToolButton.

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

The application button in the top-left corner.

```python
btn = saribbon.SARibbonApplicationButton("File")
ribbon.setApplicationButton(btn)
```

### SARibbonWidget (PySide6)

A widget with a RibbonBar (for non-MainWindow scenarios).

```python
widget = saribbon.SARibbonWidget()
ribbon = widget.ribbonBar()
```

### SARibbonMenu (PySide6)

Ribbon-specific menu with submenu and widget support.

```python
menu = saribbon.SARibbonMenu("Test Menu")
sub = menu.addRibbonMenu("Submenu")
menu.addAction("Item")
```

## Available Classes

### All Bindings

| Class | Description |
|-------|-------------|
| SARibbonMainWindow | Ribbon main window (replaces QMainWindow) |
| SARibbonBar | Main ribbon toolbar |
| SARibbonCategory | Ribbon category (tab page) |
| SARibbonPanel | Panel (functional group) |
| SARibbonPanelItem | Panel layout item (contains RowProportion enum) |
| SARibbonPanelLabel | Panel title label |
| SARibbonToolButton | Ribbon button |
| SARibbonTabBar | Tab bar |
| SARibbonContextCategory | Context category |
| SARibbonGallery | Gallery widget |
| SARibbonGalleryGroup | Gallery group |
| SARibbonGalleryItem | Gallery item |
| SARibbonButtonGroupWidget | Button group |
| SARibbonQuickAccessBar | Quick access toolbar |
| SARibbonGalleryButton | Gallery scroll button |
| SARibbonGalleryViewport | Gallery viewport |

### PySide6 Only

| Class | Description |
|-------|-------------|
| SARibbonWidget | Widget with RibbonBar |
| SARibbonMenu | Ribbon-specific menu |
| SARibbonApplicationButton | Application button |
| SARibbonColorToolButton | Color selection button |
| SARibbonActionsManager | Actions manager |
| SARibbonActionsManagerModel | Actions manager model |
| SARibbonCustomizeData | Customize data |
| SARibbonCustomizeWidget | Customize widget |
| SARibbonCustomizeDialog | Customize dialog |

## Panel Methods Reference

| Method | Description |
|--------|-------------|
| `addLargeAction(action)` | Add large button (icon above text) |
| `addSmallAction(action)` | Add small button (icon beside text) |
| `addMediumAction(action)` | Add medium button |
| `addLargeWidget(widget)` | Add large widget (for SARibbonToolButton etc.) |
| `addSmallWidget(widget)` | Add small widget |
| `addMediumWidget(widget)` | Add medium widget |
| `addWidget(widget, rowProportion)` | Add widget with row proportion (for QRadioButton/QComboBox etc.) |
| `addGallery()` | Add gallery widget |
| `addSeparator()` | Add separator |
| `setExpanding(bool)` | Set whether panel expands to fill remaining width |

## Enum Reference

### SARibbonBar.RibbonStyleFlag

| Value | Description |
|-------|-------------|
| `RibbonStyleLooseThreeRow` | Office style, 3 rows, shows panel titles |
| `RibbonStyleCompactThreeRow` | WPS style, 3 rows, hides panel titles |
| `RibbonStyleLooseTwoRow` | Office style, 2 rows |
| `RibbonStyleCompactTwoRow` | WPS style, 2 rows |
| `RibbonStyleLooseSingleRow` | Ultra-compact single row (Outlook 2025 style) |
| `RibbonStyleCompactSingleRow` | Compact single row |

### SARibbonTheme

| Value | Description |
|-------|-------------|
| `RibbonThemeWindows7` | Windows 7 theme |
| `RibbonThemeOffice2013` | Office 2013 theme |
| `RibbonThemeOffice2016Blue` | Office 2016 Blue theme |
| `RibbonThemeOffice2021Blue` | Office 2021 Blue theme (default) |
| `RibbonThemeDark` | Dark theme |
| `RibbonThemeDark2` | Dark theme 2 |

### SARibbonBar.RibbonMode

| Value | Description |
|-------|-------------|
| `NormalRibbonMode` | Normal mode (full ribbon visible) |
| `MinimumRibbonMode` | Minimum mode (tab bar only) |

### SARibbonPanelItem.RowProportion

| Value | Description |
|-------|-------------|
| `None` | Auto-detect |
| `Large` | Fills full panel height |
| `Medium` | Half height (three-row mode only) |
| `Small` | Single row height |

## Example Programs

```bash
# PySide6
python pyexamples/pyside6/ribbon_demo.py

# PyQt5
python pyexamples/pyqt5/ribbon_demo.py

# PyQt6
python pyexamples/pyqt6/ribbon_demo.py
```

Each demo showcases: multiple categories/panels, style switching, theme switching, Gallery, context categories, QuickAccessBar, color buttons, font controls, and more.

## Next Steps

- See `pyexamples/` for complete demo source code
- Refer to C++ API docs for more features
- See [Build Python Bindings](./build-python-bindings.md) and [Build PySide6 Bindings](./build-pyside6-bindings.md)
