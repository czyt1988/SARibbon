"""
SARibbon Python Demo
Demonstrates basic usage of SARibbon via PyQt5 Python bindings.
"""
import sys

from PyQt5.QtWidgets import (
    QApplication,
    QAction,
    QTextEdit,
    QFileDialog,
    QMessageBox,
)
from PyQt5.QtGui import QIcon, QFont
from PyQt5.QtCore import Qt

from PyQtSARibbon import saribbon


def main():
    app = QApplication(sys.argv)

    # Create ribbon main window
    win = saribbon.SARibbonMainWindow()
    win.setWindowTitle("SARibbon Python Demo")
    win.resize(1024, 680)

    # Get the ribbon bar
    ribbon = win.ribbonBar()
    ribbon.setRibbonStyle(saribbon.SARibbonBar.RibbonStyleLooseThreeRow)

    # === Home Category ===
    home_category = ribbon.addCategoryPage("Home")

    # -- File Panel --
    file_panel = home_category.addPanel("File")

    act_new = QAction("New")
    act_new.setShortcut("Ctrl+N")
    file_panel.addLargeAction(act_new)

    act_open = QAction("Open")
    act_open.setShortcut("Ctrl+O")
    file_panel.addLargeAction(act_open)

    act_save = QAction("Save")
    act_save.setShortcut("Ctrl+S")
    file_panel.addLargeAction(act_save)

    act_save_as = QAction("Save As")
    file_panel.addSmallAction(act_save_as)

    # -- Edit Panel --
    edit_panel = home_category.addPanel("Edit")

    act_undo = QAction("Undo")
    edit_panel.addSmallAction(act_undo)

    act_redo = QAction("Redo")
    edit_panel.addSmallAction(act_redo)

    edit_panel.addSeparator()

    act_cut = QAction("Cut")
    edit_panel.addSmallAction(act_cut)

    act_copy = QAction("Copy")
    edit_panel.addSmallAction(act_copy)

    act_paste = QAction("Paste")
    edit_panel.addSmallAction(act_paste)

    # -- View Panel --
    view_panel = home_category.addPanel("View")
    view_panel.setExpanding(True)

    act_zoom_in = QAction("Zoom In")
    view_panel.addLargeAction(act_zoom_in)

    act_zoom_out = QAction("Zoom Out")
    view_panel.addLargeAction(act_zoom_out)

    # === Insert Category ===
    insert_category = ribbon.addCategoryPage("Insert")

    gallery_panel = insert_category.addPanel("Gallery")

    gallery = gallery_panel.addGallery()
    actions = []
    for i in range(12):
        act = QAction("Item {}".format(i + 1))
        actions.append(act)
    group = gallery.addCategoryActions("Items", actions)
    gallery.setCurrentViewGroup(group)

    # === Context Category Example ===
    ctx = ribbon.addContextCategory("Drawing Tools", Qt.red)
    ctx_category = ctx.addCategoryPage("Format")
    format_panel = ctx_category.addPanel("Shape")

    act_fill = QAction("Fill")
    format_panel.addLargeAction(act_fill)

    act_outline = QAction("Outline")
    format_panel.addLargeAction(act_outline)

    # Add central widget
    editor = QTextEdit()
    editor.setFont(QFont("Consolas", 11))
    editor.setPlaceholderText("SARibbon Python Demo - type here...")
    win.setCentralWidget(editor)

    # Connect some signals
    act_new.triggered.connect(lambda: editor.clear())
    act_open.triggered.connect(lambda: _open_file(editor, win))
    act_save.triggered.connect(lambda: _show_msg(win, "Save triggered"))

    ribbon.actionTriggered.connect(
        lambda act: print("Action triggered: {}".format(act.text()))
    )

    win.show()
    sys.exit(app.exec_())


def _open_file(editor, parent):
    path, _ = QFileDialog.getOpenFileName(parent, "Open File", "", "Text Files (*.txt);;All Files (*)")
    if path:
        with open(path, "r", encoding="utf-8") as f:
            editor.setPlainText(f.read())


def _show_msg(parent, text):
    QMessageBox.information(parent, "Info", text)


if __name__ == "__main__":
    main()
