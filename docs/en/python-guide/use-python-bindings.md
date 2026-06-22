# Python Bindings Usage Guide

This document describes how to use the SARibbon bindings (PyQtSARibbon) in Python.

## Documentation Navigation

| Document | Content |
|----------|---------|
| [Build Python Bindings](./build-python-bindings.md) | Compile and install PyQtSARibbon |
| [Use Python Bindings](./use-python-bindings.md) | Use SARibbon in Python |

## Quick Start

### Basic Example

Here is the simplest PyQtSARibbon example:

`python
import sys
from PyQt5.QtWidgets import QApplication, QAction
from PyQtSARibbon import SARibbonBar, SARibbonCategory, SARibbonPanel

# Create application
app = QApplication(sys.argv)

# Create Ribbon toolbar
ribbon = SARibbonBar()

# Create category (tab)
home_category = SARibbonCategory("Home")
ribbon.addCategoryPage(home_category)

# Create panel
clipboard_panel = SARibbonPanel("Clipboard", home_category)
home_category.addPanel(clipboard_panel)

# Add actions
paste_action = QAction("Paste", clipboard_panel)
clipboard_panel.addLargeAction(paste_action)

cut_action = QAction("Cut", clipboard_panel)
clipboard_panel.addSmallAction(cut_action)

copy_action = QAction("Copy", clipboard_panel)
clipboard_panel.addSmallAction(copy_action)

# Show Ribbon
ribbon.show()

sys.exit(app.exec_())
`

## Core Concepts

PyQtSARibbon interface hierarchy:

`
SARibbonBar (Toolbar)
└── SARibbonCategory (Category/Tab)
    └── SARibbonPanel (Panel)
        ├── QAction (Action/Button)
        └── SARibbonGallery (Gallery)
`

### SARibbonBar - Main Toolbar

SARibbonBar is the container for the entire Ribbon control.

`python
from PyQtSARibbon import SARibbonBar

# Create Ribbon toolbar
ribbon = SARibbonBar()

# Set style
ribbon.setRibbonStyle(SARibbonBar.RibbonStyleLooseThreeRow)

# Get current selected category index
index = ribbon.currentIndex()

# Set current category
ribbon.setCurrentIndex(1)
`

### SARibbonCategory - Category (Tab)

SARibbonCategory represents a tab in the Ribbon.

`python
from PyQtSARibbon import SARibbonCategory

# Create category
category = SARibbonCategory("Home")

# Add to Ribbon
ribbon.addCategoryPage(category)

# Get category name
name = category.categoryName()

# Set category name
category.setCategoryName("Start")
`

### SARibbonPanel - Panel

SARibbonPanel is a functional group within a category.

`python
from PyQtSARibbon import SARibbonPanel

# Create panel
panel = SARibbonPanel("Clipboard", category)

# Add to category
category.addPanel(panel)

# Add actions
panel.addLargeAction(action)  # Large button
panel.addSmallAction(action)  # Small button
panel.addSeparator()          # Separator
`

## Adding Actions to Panels

### Large Button (Large Action)

Large buttons display icon and text, with icon on top.

`python
from PyQt5.QtWidgets import QAction
from PyQt5.QtGui import QIcon

# Create action
paste_action = QAction(QIcon("paste.png"), "Paste", panel)
paste_action.triggered.connect(lambda: print("Paste clicked"))

# Add large button
panel.addLargeAction(paste_action)
`

### Small Button (Small Action)

Small buttons display only a small icon and text.

`python
# Create actions
cut_action = QAction("Cut", panel)
copy_action = QAction("Copy", panel)

# Add small buttons
panel.addSmallAction(cut_action)
panel.addSmallAction(copy_action)
`

### Medium Button (Medium Action)

Medium buttons display icon and text side by side.

`python
# Add medium button
panel.addMediumAction(action)
`

### Separator

`python
# Add separator
panel.addSeparator()
`

## Creating Gallery

A gallery is a dropdown menu that displays multiple options.

`python
from PyQtSARibbon import SARibbonGallery

# Create gallery panel
gallery_panel = SARibbonPanel("Styles", category)
category.addPanel(gallery_panel)

# Add gallery
gallery = gallery_panel.addGallery()

# Create action list
actions = []
for i in range(12):
    action = QAction(f"Style {i+1}")
    actions.append(action)

# Add to gallery
group = gallery.addCategoryActions("Preset Styles", actions)

# Set current display
gallery.setCurrentViewGroup(group)
`

## Context Category

Context categories are tabs that appear under specific conditions (e.g., "Picture Tools" when an image is selected).

`python
from PyQt5.QtCore import Qt

# Create context category
context = ribbon.addContextCategory("Drawing Tools", Qt.red)

# Add category to context
format_category = context.addCategoryPage("Format")

# Add panel
format_panel = format_category.addPanel("Shape")
format_panel.addLargeAction(fill_action)

# Show context category
ribbon.setContextCategoryVisible(context, True)

# Hide context category
ribbon.setContextCategoryVisible(context, False)
`

## Complete Example

Here is a complete Ribbon application example:

`python
import sys
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QAction, QTextEdit,
    QFileDialog, QMessageBox
)
from PyQt5.QtGui import QFont
from PyQt5.QtCore import Qt
from PyQtSARibbon import SARibbonBar, SARibbonCategory, SARibbonPanel

class RibbonWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("SARibbon Python Example")
        self.resize(1024, 680)
        
        # Create Ribbon
        self.ribbon = SARibbonBar()
        self.setMenuWidget(self.ribbon)
        
        # Create central editor
        self.editor = QTextEdit()
        self.editor.setFont(QFont("Consolas", 11))
        self.setCentralWidget(self.editor)
        
        # Setup Ribbon interface
        self._setup_ribbon()
    
    def _setup_ribbon(self):
        # === Home Tab ===
        home = SARibbonCategory("Home")
        self.ribbon.addCategoryPage(home)
        
        # File panel
        file_panel = SARibbonPanel("File", home)
        home.addPanel(file_panel)
        
        new_action = QAction("New", file_panel)
        new_action.setShortcut("Ctrl+N")
        new_action.triggered.connect(self._on_new)
        file_panel.addLargeAction(new_action)
        
        open_action = QAction("Open", file_panel)
        open_action.setShortcut("Ctrl+O")
        open_action.triggered.connect(self._on_open)
        file_panel.addLargeAction(open_action)
        
        save_action = QAction("Save", file_panel)
        save_action.setShortcut("Ctrl+S")
        save_action.triggered.connect(self._on_save)
        file_panel.addLargeAction(save_action)
        
        # Edit panel
        edit_panel = SARibbonPanel("Edit", home)
        home.addPanel(edit_panel)
        
        undo_action = QAction("Undo", edit_panel)
        undo_action.triggered.connect(self.editor.undo)
        edit_panel.addSmallAction(undo_action)
        
        redo_action = QAction("Redo", edit_panel)
        redo_action.triggered.connect(self.editor.redo)
        edit_panel.addSmallAction(redo_action)
        
        edit_panel.addSeparator()
        
        cut_action = QAction("Cut", edit_panel)
        cut_action.triggered.connect(self.editor.cut)
        edit_panel.addSmallAction(cut_action)
        
        copy_action = QAction("Copy", edit_panel)
        copy_action.triggered.connect(self.editor.copy)
        edit_panel.addSmallAction(copy_action)
        
        paste_action = QAction("Paste", edit_panel)
        paste_action.triggered.connect(self.editor.paste)
        edit_panel.addSmallAction(paste_action)
        
        # === View Tab ===
        view = SARibbonCategory("View")
        self.ribbon.addCategoryPage(view)
        
        zoom_panel = SARibbonPanel("Zoom", view)
        view.addPanel(zoom_panel)
        
        zoom_in = QAction("Zoom In", zoom_panel)
        zoom_in.triggered.connect(self._zoom_in)
        zoom_panel.addLargeAction(zoom_in)
        
        zoom_out = QAction("Zoom Out", zoom_panel)
        zoom_out.triggered.connect(self._zoom_out)
        zoom_panel.addLargeAction(zoom_out)
    
    def _on_new(self):
        self.editor.clear()
    
    def _on_open(self):
        path, _ = QFileDialog.getOpenFileName(
            self, "Open File", "", "Text Files (*.txt);;All Files (*)"
        )
        if path:
            with open(path, "r", encoding="utf-8") as f:
                self.editor.setPlainText(f.read())
    
    def _on_save(self):
        QMessageBox.information(self, "Info", "Save function triggered")
    
    def _zoom_in(self):
        self.editor.zoomIn(1)
    
    def _zoom_out(self):
        self.editor.zoomOut(1)


def main():
    app = QApplication(sys.argv)
    window = RibbonWindow()
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
`

## Available Classes

PyQtSARibbon provides the following classes:

| Class | Description |
|-------|-------------|
| SARibbonBar | Main Ribbon toolbar |
| SARibbonCategory | Ribbon category (tab) |
| SARibbonPanel | Panel (functional group) |
| SARibbonToolButton | Ribbon button |
| SARibbonGallery | Gallery control |
| SARibbonGalleryGroup | Gallery group |
| SARibbonGalleryItem | Gallery item |
| SARibbonContextCategory | Context category |
| SARibbonButtonGroupWidget | Button group |
| SARibbonTabBar | Tab bar |
| SARibbonQuickAccessBar | Quick access toolbar |

## Panel Method Reference

### Adding Actions

| Method | Description |
|--------|-------------|
| addLargeAction(action) | Add large button (icon on top, text below) |
| addMediumAction(action) | Add medium button (icon and text side by side) |
| addSmallAction(action) | Add small button (icon only) |

### Adding Widgets

| Method | Description |
|--------|-------------|
| addLargeWidget(widget) | Add large widget |
| addMediumWidget(widget) | Add medium widget |
| addSmallWidget(widget) | Add small widget |

### Other Methods

| Method | Description |
|--------|-------------|
| addGallery() | Add gallery control |
| addMenu(menu) | Add menu button |
| addSeparator() | Add separator |

## Next Steps

- Check pyexamples/ribbon_demo.py for complete example code
- Refer to the C++ API documentation for more features
