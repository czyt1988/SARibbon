"""
SARibbon Python Demo (PyQt6)
============================

A self-contained demo showing how to use SARibbon via PyQt6 Python bindings.

Features demonstrated:
  - SARibbonMainWindow with ribbon bar and style
  - Category, panel, large/small actions with icons
  - Gallery with grouped actions
  - Context category
  - Signal connections

Prerequisites:
  pip install PyQt6
  # Build and install PyQtSARibbon first (see tools/build_python_bindings.bat --pyqt6)

Run:
  python ribbon_demo.py
"""
import os
import sys

from PyQt6.QtWidgets import (
    QApplication,
    QTextEdit,
    QFileDialog,
    QMessageBox,
)
from PyQt6.QtGui import QAction, QIcon, QFont
from PyQt6.QtCore import Qt

from PyQtSARibbon import saribbon

# Share the same icon directory as the PyQt5 demo
ICON_DIR = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", "pyqt5", "icon"
)


def icon(name):
    """Load an SVG icon from the local icon directory."""
    return QIcon(os.path.join(ICON_DIR, name))


def main():
    app = QApplication(sys.argv)

    # Create ribbon main window
    win = saribbon.SARibbonMainWindow()
    win.setWindowTitle("SARibbon Python Demo (PyQt6)")
    win.resize(1024, 680)
    win.setWindowIcon(icon("SA.svg"))

    # Get the ribbon bar and set style
    ribbon = win.ribbonBar()
    ribbon.setRibbonStyle(saribbon.SARibbonBar.RibbonStyleLooseThreeRow)

    # ================================================================
    # Home Category
    # ================================================================
    home = ribbon.addCategoryPage("Home")

    # -- File Panel --
    file_panel = home.addPanel("File")

    act_new = QAction(icon("file.svg"), "New")
    act_new.setShortcut("Ctrl+N")
    file_panel.addLargeAction(act_new)

    act_open = QAction(icon("presentationFile.svg"), "Open")
    act_open.setShortcut("Ctrl+O")
    file_panel.addLargeAction(act_open)

    act_save = QAction(icon("save.svg"), "Save")
    act_save.setShortcut("Ctrl+S")
    file_panel.addLargeAction(act_save)

    act_save_as = QAction(icon("save.svg"), "Save As")
    file_panel.addSmallAction(act_save_as)

    # -- Edit Panel --
    edit_panel = home.addPanel("Edit")

    act_undo = QAction(icon("undo.svg"), "Undo")
    edit_panel.addSmallAction(act_undo)

    act_redo = QAction(icon("redo.svg"), "Redo")
    edit_panel.addSmallAction(act_redo)

    edit_panel.addSeparator()

    act_cut = QAction(icon("remove.svg"), "Cut")
    edit_panel.addSmallAction(act_cut)

    act_copy = QAction(icon("item.svg"), "Copy")
    edit_panel.addSmallAction(act_copy)

    act_paste = QAction(icon("folder-checkmark.svg"), "Paste")
    edit_panel.addSmallAction(act_paste)

    # -- Font Panel --
    font_panel = home.addPanel("Font")

    act_bold = QAction(icon("bold.svg"), "Bold")
    act_bold.setCheckable(True)
    font_panel.addSmallAction(act_bold)

    act_italic = QAction(icon("Italic.svg"), "Italic")
    act_italic.setCheckable(True)
    font_panel.addSmallAction(act_italic)

    act_underline = QAction(icon("Underline.svg"), "Underline")
    act_underline.setCheckable(True)
    font_panel.addSmallAction(act_underline)

    font_panel.addSeparator()

    act_larger = QAction(icon("largerFont.svg"), "Larger")
    font_panel.addSmallAction(act_larger)

    act_smaller = QAction(icon("smallFont.svg"), "Smaller")
    font_panel.addSmallAction(act_smaller)

    # -- View Panel --
    view_panel = home.addPanel("View")
    view_panel.setExpanding(True)

    act_zoom_in = QAction(icon("largerFont.svg"), "Zoom In")
    view_panel.addLargeAction(act_zoom_in)

    act_zoom_out = QAction(icon("smallFont.svg"), "Zoom Out")
    view_panel.addLargeAction(act_zoom_out)

    act_wordwrap = QAction(icon("wordwrap.svg"), "Word Wrap")
    act_wordwrap.setCheckable(True)
    view_panel.addSmallAction(act_wordwrap)

    # ================================================================
    # Insert Category — Gallery
    # ================================================================
    insert = ribbon.addCategoryPage("Insert")

    gallery_panel = insert.addPanel("Gallery")
    gallery = gallery_panel.addGallery()

    gallery_icons = [
        "gallery/Document-File.svg",
        "gallery/Download-File.svg",
        "gallery/Drive-File.svg",
        "gallery/Dropbox-File.svg",
        "gallery/Email-File.svg",
        "gallery/Encode-File.svg",
        "gallery/Favorit-File.svg",
        "gallery/File-Error.svg",
        "gallery/File-Readonly.svg",
        "gallery/File-Settings.svg",
        "gallery/Presentation-File.svg",
        "gallery/Photoshop.svg",
    ]
    actions = []
    for i, svg in enumerate(gallery_icons):
        act = QAction(icon(svg), "Item {}".format(i + 1))
        actions.append(act)
    group = gallery.addCategoryActions("Items", actions)
    gallery.setCurrentViewGroup(group)

    # ================================================================
    # Context Category — shown only when activated
    # ================================================================
    ctx = ribbon.addContextCategory("Drawing Tools", Qt.GlobalColor.red)
    ctx_category = ctx.addCategoryPage("Format")
    format_panel = ctx_category.addPanel("Shape")

    act_fill = QAction(icon("action.svg"), "Fill")
    format_panel.addLargeAction(act_fill)

    act_outline = QAction(icon("layout.svg"), "Outline")
    format_panel.addLargeAction(act_outline)

    # ================================================================
    # Central widget
    # ================================================================
    editor = QTextEdit()
    editor.setFont(QFont("Consolas", 11))
    editor.setPlaceholderText("SARibbon Python Demo (PyQt6) - type here...")
    win.setCentralWidget(editor)

    # ================================================================
    # Signal connections
    # ================================================================
    act_new.triggered.connect(lambda: editor.clear())
    act_open.triggered.connect(lambda: _open_file(editor, win))
    act_save.triggered.connect(lambda: _show_msg(win, "Save triggered"))
    act_wordwrap.triggered.connect(lambda: editor.setLineWrapMode(
        QTextEdit.LineWrapMode.WidgetWidth if act_wordwrap.isChecked()
        else QTextEdit.LineWrapMode.NoWrap
    ))

    ribbon.actionTriggered.connect(
        lambda act: print("Action triggered: {}".format(act.text()))
    )

    win.show()
    sys.exit(app.exec())


def _open_file(editor, parent):
    path, _ = QFileDialog.getOpenFileName(
        parent, "Open File", "", "Text Files (*.txt);;All Files (*)"
    )
    if path:
        with open(path, "r", encoding="utf-8") as f:
            editor.setPlainText(f.read())


def _show_msg(parent, text):
    QMessageBox.information(parent, "Info", text)


if __name__ == "__main__":
    main()
