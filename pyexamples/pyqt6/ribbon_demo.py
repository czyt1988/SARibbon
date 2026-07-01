"""
SARibbon Python Demo (PyQt6)
============================

A self-contained demo showing how to use SARibbon via PyQt6 Python bindings.

This demo is inspired by the C++ MainWindowExample and showcases:
  - SARibbonMainWindow with ribbon bar, styles, and themes
  - Multiple categories & panels with large/small actions
  - Ribbon style switching (6 styles via radio buttons) and theme switching (6 themes)
  - Gallery with grouped items
  - Context categories (show / hide / toggle)
  - QuickAccessBar, right button group, application button
  - Color tool buttons
  - Word-wrap, font size, alignment controls
  - Hide ribbon (minimum mode) toggle
  - Signal connections

Prerequisites:
  pip install PyQt6
  Build and install PyQtSARibbon first (see tools/build_python_bindings.bat --pyqt6)

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
    QRadioButton,
    QLabel,
    QComboBox,
    QButtonGroup,
)
from PyQt6.QtGui import QAction, QIcon, QFont, QColor
from PyQt6.QtCore import Qt

from PyQtSARibbon import saribbon

# Icons are in the shared pyexamples/icon/ directory
ICON_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "icon")


def icon(name):
    """Load an SVG icon from the shared icon directory."""
    return QIcon(os.path.join(ICON_DIR, name))


def main():
    app = QApplication(sys.argv)

    # ------------------------------------------------------------------
    # Create the ribbon main window
    # SARibbonMainWindow is a QMainWindow subclass that provides a
    # built-in SARibbonBar as the menu widget.
    # ------------------------------------------------------------------
    win = saribbon.SARibbonMainWindow()
    win.setWindowTitle("SARibbon Python Demo (PyQt6)")
    win.resize(1280, 800)
    win.setWindowIcon(icon("SA.svg"))

    # ------------------------------------------------------------------
    # Get the ribbon bar and configure the initial style
    # ribbonBar() returns the SARibbonBar that manages all categories,
    # panels, and actions.
    # ------------------------------------------------------------------
    ribbon = win.ribbonBar()
    ribbon.setRibbonStyle(saribbon.SARibbonBar.RibbonStyleLooseThreeRow)

    # RowProportion enum — used by addWidget() to control how tall a
    # custom widget should be within a panel:
    #   SARibbonPanelItem.Large  — fills the full panel height
    #   SARibbonPanelItem.Medium — half height (two-row mode only)
    #   SARibbonPanelItem.Small  — one row
    RP = saribbon.SARibbonPanelItem

    # ==================================================================
    # Category: Home
    # A category is a single tab page in the ribbon.  Each category
    # contains one or more panels.
    # ==================================================================
    home = ribbon.addCategoryPage("Home")

    # ------------------------------------------------------------------
    # Panel: File
    # addLargeAction() places an action with icon-above-text layout.
    # addSmallAction() places an action with icon-beside-text layout.
    # ------------------------------------------------------------------
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

    # ------------------------------------------------------------------
    # Panel: Edit
    # addSeparator() inserts a visual separator between groups of
    # small actions.
    # ------------------------------------------------------------------
    edit_panel = home.addPanel("Edit")

    act_undo = QAction(icon("undo.svg"), "Undo")
    act_undo.setShortcut("Ctrl+Z")
    edit_panel.addSmallAction(act_undo)

    act_redo = QAction(icon("redo.svg"), "Redo")
    act_redo.setShortcut("Ctrl+Y")
    edit_panel.addSmallAction(act_redo)

    edit_panel.addSeparator()

    act_cut = QAction(icon("remove.svg"), "Cut")
    edit_panel.addSmallAction(act_cut)

    act_copy = QAction(icon("item.svg"), "Copy")
    edit_panel.addSmallAction(act_copy)

    act_paste = QAction(icon("folder-checkmark.svg"), "Paste")
    edit_panel.addSmallAction(act_paste)

    # ------------------------------------------------------------------
    # Panel: Font
    # setCheckable(True) makes an action toggle between checked/unchecked
    # states, like a bold/italic toggle button.
    # ------------------------------------------------------------------
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

    # ------------------------------------------------------------------
    # Panel: Ribbon Style
    # Demonstrates how to switch between the 6 built-in ribbon styles
    # and 6 built-in themes at runtime.
    #
    # A QButtonGroup ensures only one radio button is checked at a
    # time.  When the user selects a different style, we call
    # ribbon.setRibbonStyle() and sync the word-wrap checkbox.
    # ------------------------------------------------------------------
    style_panel = home.addPanel("Ribbon Style")

    # --- Style radio buttons ---
    # The 6 styles control layout density:
    #   Loose / Compact  — Compact hides panel titles
    #   ThreeRow / TwoRow — number of action rows per panel
    #   SingleRow        — ultra-compact, icon-right-text (Outlook 2025 style)
    style_button_group = QButtonGroup(style_panel)

    styles = [
        ("Office 3-Row",        saribbon.SARibbonBar.RibbonStyleLooseThreeRow),
        ("WPS 3-Row",           saribbon.SARibbonBar.RibbonStyleCompactThreeRow),
        ("Office 2-Row",        saribbon.SARibbonBar.RibbonStyleLooseTwoRow),
        ("WPS 2-Row",           saribbon.SARibbonBar.RibbonStyleCompactTwoRow),
        ("Loose Single Row",    saribbon.SARibbonBar.RibbonStyleLooseSingleRow),
        ("Compact Single Row",  saribbon.SARibbonBar.RibbonStyleCompactSingleRow),
    ]

    for i, (label, style_val) in enumerate(styles):
        rb = QRadioButton(label)
        if i == 0:
            rb.setChecked(True)
        style_panel.addSmallWidget(rb)
        style_button_group.addButton(rb, style_val)

    # QButtonGroup.idClicked(int) fires only when a new button is
    # selected (not when the old one is deselected).  The int is the
    # style enum value we passed to addButton().
    style_button_group.idClicked.connect(
        lambda style_id: _on_style_changed(ribbon, saribbon.SARibbonBar.RibbonStyleFlag(style_id), act_wordwrap)
    )

    # --- Theme combo box ---
    # Themes control the QSS (color scheme) applied to the ribbon.
    theme_lbl = QLabel("Theme:")
    style_panel.addWidget(theme_lbl, RP.Small)

    theme_combo = QComboBox()
    themes = [
        ("Windows 7",        saribbon.SARibbonTheme.RibbonThemeWindows7),
        ("Office 2013",      saribbon.SARibbonTheme.RibbonThemeOffice2013),
        ("Office 2016 Blue", saribbon.SARibbonTheme.RibbonThemeOffice2016Blue),
        ("Office 2021 Blue", saribbon.SARibbonTheme.RibbonThemeOffice2021Blue),
        ("Dark",             saribbon.SARibbonTheme.RibbonThemeDark),
        ("Dark 2",           saribbon.SARibbonTheme.RibbonThemeDark2),
    ]
    for label, _ in themes:
        theme_combo.addItem(label)
    theme_combo.setCurrentIndex(3)  # Office 2021 Blue is the default theme
    theme_combo.currentIndexChanged.connect(
        lambda idx: win.setRibbonTheme(themes[idx][1])
    )
    style_panel.addWidget(theme_combo, RP.Small)

    # ------------------------------------------------------------------
    # Panel: View
    # setExpanding(True) makes this panel stretch to fill available width.
    # Contains toggle actions for word wrap, hide ribbon, and context
    # category visibility.
    # ------------------------------------------------------------------
    view_panel = home.addPanel("View")
    view_panel.setExpanding(True)

    act_zoom_in = QAction(icon("largerFont.svg"), "Zoom In")
    view_panel.addLargeAction(act_zoom_in)

    act_zoom_out = QAction(icon("smallFont.svg"), "Zoom Out")
    view_panel.addLargeAction(act_zoom_out)

    view_panel.addSeparator()

    # Word Wrap — toggles whether action text wraps to multiple lines
    act_wordwrap = QAction(icon("wordwrap.svg"), "Word Wrap")
    act_wordwrap.setCheckable(True)
    act_wordwrap.setChecked(ribbon.isEnableWordWrap())
    view_panel.addSmallAction(act_wordwrap)

    # Hide Ribbon — switches to minimum mode (only tab bar visible)
    # ribbonModeChanged signal keeps the action in sync if the user
    # changes mode via the built-in minimize button.
    act_hide_ribbon = QAction(icon("hideRibbon.svg"), "Hide Ribbon")
    act_hide_ribbon.setCheckable(True)
    view_panel.addSmallAction(act_hide_ribbon)

    # Show/Hide Minimize Button — controls whether the small chevron
    # button for toggling minimum mode is visible on the ribbon.
    act_show_hide_btn = QAction(icon("showHideButton.svg"), "Show\nMin Button")
    act_show_hide_btn.setCheckable(True)
    act_show_hide_btn.setChecked(ribbon.haveShowMinimumModeButton())
    view_panel.addSmallAction(act_show_hide_btn)

    # Show Context — toggles visibility of the context category
    act_show_ctx = QAction(icon("showContext.svg"), "Show Context")
    act_show_ctx.setCheckable(True)
    view_panel.addSmallAction(act_show_ctx)

    # ==================================================================
    # Category: Insert — Gallery
    # A Gallery is a popup grid of actions with scrollable groups.
    # addCategoryActions() creates a SARibbonGalleryGroup and populates
    # it with the given QAction list.
    # ==================================================================
    insert = ribbon.addCategoryPage("Insert")

    gallery_panel = insert.addPanel("Gallery")
    gallery = gallery_panel.addGallery()

    file_icons = [
        "gallery/Document-File.svg", "gallery/Download-File.svg",
        "gallery/Drive-File.svg", "gallery/Dropbox-File.svg",
        "gallery/Email-File.svg", "gallery/Encode-File.svg",
        "gallery/Favorit-File.svg", "gallery/File-Error.svg",
        "gallery/File-Readonly.svg", "gallery/File-Settings.svg",
        "gallery/Presentation-File.svg",
    ]
    app_icons = [
        "gallery/Photoshop.svg", "gallery/Internet-Explorer.svg",
        "gallery/Illustrator.svg", "gallery/Google-Maps.svg",
        "gallery/Adobe.svg", "gallery/Word.svg",
    ]

    file_actions = [QAction(icon(s), "File {}".format(i + 1))
                    for i, s in enumerate(file_icons)]
    app_actions = [QAction(icon(s), s.split("/")[-1].replace(".svg", ""))
                   for s in app_icons]

    grp_files = gallery.addCategoryActions("Files", file_actions)
    gallery.addCategoryActions("Apps", app_actions)
    gallery.setCurrentViewGroup(grp_files)

    # ------------------------------------------------------------------
    # Panel: Shape (on the Insert category)
    # Demonstrates alignment actions as small buttons.
    # ------------------------------------------------------------------
    shape_panel = insert.addPanel("Shape")

    act_fill = QAction(icon("action.svg"), "Fill")
    shape_panel.addLargeAction(act_fill)

    act_outline = QAction(icon("layout.svg"), "Outline")
    shape_panel.addLargeAction(act_outline)

    act_align_left = QAction(icon("al-left.svg"), "Left")
    shape_panel.addSmallAction(act_align_left)

    act_align_center = QAction(icon("al-center.svg"), "Center")
    shape_panel.addSmallAction(act_align_center)

    act_align_right = QAction(icon("al-right.svg"), "Right")
    shape_panel.addSmallAction(act_align_right)

    # ==================================================================
    # Category: Color
    # SARibbonColorToolButton is a specialized SARibbonToolButton that
    # displays a color swatch.  Use addSmallWidget()/addLargeWidget()
    # (not addWidget()) because it is a tool button, not a plain widget.
    #
    # ColorFillToIcon fills the icon area with the current color.
    # setButtonType(LargeButton) makes it display as a large button.
    # ==================================================================
    color_cat = ribbon.addCategoryPage("Color")
    color_panel = color_cat.addPanel("Color")

    color_specs = [
        ("#E74C3C", "Red"),
        ("#3498DB", "Blue"),
        ("#2ECC71", "Green"),
        ("#F39C12", "Orange"),
        ("#9B59B6", "Purple"),
        ("#1ABC9C", "Teal"),
    ]

    for hex_color, label in color_specs:
        color_btn = saribbon.SARibbonColorToolButton()
        color_btn.setColor(QColor(hex_color))
        color_btn.setColorStyle(saribbon.SARibbonColorToolButton.ColorFillToIcon)
        color_btn.setText(label)
        color_panel.addSmallWidget(color_btn)

    color_panel.addSeparator()

    large_color = saribbon.SARibbonColorToolButton()
    large_color.setColor(QColor("#FF0000"))
    large_color.setButtonType(saribbon.SARibbonToolButton.LargeButton)
    large_color.setColorStyle(saribbon.SARibbonColorToolButton.ColorFillToIcon)
    large_color.setText("Large Color")
    color_panel.addLargeWidget(large_color)

    # ==================================================================
    # Context Category
    # A context category is a special tab that is hidden by default
    # and shown only when relevant (e.g. when an object is selected).
    # The second argument is the highlight color for the context tab.
    #
    # showContextCategory() / hideContextCategory() toggle visibility.
    # ==================================================================
    ctx = ribbon.addContextCategory("Drawing Tools", Qt.GlobalColor.red)
    ctx_cat = ctx.addCategoryPage("Format")
    fmt_panel = ctx_cat.addPanel("Shape Format")

    act_fill_ctx = QAction(icon("action.svg"), "Fill")
    fmt_panel.addLargeAction(act_fill_ctx)

    act_outline_ctx = QAction(icon("layout.svg"), "Outline")
    fmt_panel.addLargeAction(act_outline_ctx)

    act_effect = QAction(icon("folder-star.svg"), "Effects")
    fmt_panel.addLargeAction(act_effect)

    # ==================================================================
    # QuickAccessBar
    # The QAB is the small toolbar at the very top of the window,
    # above the ribbon tabs.  addAction(icon, text) is a QToolBar
    # convenience method.
    # ==================================================================
    try:
        qab = ribbon.quickAccessBar()
        if qab is not None:
            qab.addAction(icon("save.svg"), "Quick Save")
            qab.addAction(icon("undo.svg"), "Undo")
            qab.addAction(icon("redo.svg"), "Redo")
    except Exception:
        pass

    # ==================================================================
    # Right button group
    # A small toolbar on the right side of the ribbon, useful for
    # help or other quick-access actions.
    # ==================================================================
    try:
        rbg = ribbon.rightButtonGroup()
        if rbg is not None:
            act_help = QAction(icon("help.svg"), "Help")
            rbg.addAction(act_help)
    except Exception:
        pass

    # ==================================================================
    # Application button
    # The large button in the top-left corner of the ribbon.
    # SARibbonApplicationButton is a QToolButton subclass; clicking it
    # would typically show a file menu.
    # ==================================================================
    app_btn = saribbon.SARibbonApplicationButton("File")
    ribbon.setApplicationButton(app_btn)

    # ==================================================================
    # Central widget
    # Like QMainWindow, SARibbonMainWindow needs a central widget.
    # ==================================================================
    editor = QTextEdit()
    editor.setFont(QFont("Consolas", 11))
    editor.setPlaceholderText("SARibbon Python Demo (PyQt6) - type here...")
    win.setCentralWidget(editor)

    # ==================================================================
    # Signal connections
    # ==================================================================

    # File actions
    act_new.triggered.connect(lambda: editor.clear())
    act_open.triggered.connect(lambda: _open_file(editor, win))
    act_save.triggered.connect(lambda: _show_msg(win, "Save triggered"))

    # Word Wrap — toggles QSS text wrapping on action labels
    act_wordwrap.toggled.connect(
        lambda on: ribbon.setEnableWordWrap(on)
    )

    # Hide Ribbon — setMinimumMode(True) collapses the ribbon to just
    # the tab bar; setMinimumMode(False) restores it.
    act_hide_ribbon.toggled.connect(
        lambda checked: ribbon.setMinimumMode(checked)
    )

    # Keep the action in sync with programmatic mode changes
    ribbon.ribbonModeChanged.connect(
        lambda mode: act_hide_ribbon.setChecked(
            mode == saribbon.SARibbonBar.MinimumRibbonMode
        )
    )

    # Show/Hide Min Button
    act_show_hide_btn.toggled.connect(
        lambda checked: ribbon.showMinimumModeButton(checked)
    )

    # Context category visibility
    act_show_ctx.toggled.connect(
        lambda checked: (
            ribbon.showContextCategory(ctx) if checked
            else ribbon.hideContextCategory(ctx)
        )
    )

    # Font formatting
    act_bold.toggled.connect(lambda on: _toggle_font(editor, "bold", on))
    act_italic.toggled.connect(lambda on: _toggle_font(editor, "italic", on))
    act_underline.toggled.connect(lambda on: _toggle_font(editor, "underline", on))
    act_larger.triggered.connect(lambda: _change_font_size(editor, +1))
    act_smaller.triggered.connect(lambda: _change_font_size(editor, -1))
    act_zoom_in.triggered.connect(lambda: _change_font_size(editor, +1))
    act_zoom_out.triggered.connect(lambda: _change_font_size(editor, -1))

    # Log every action triggered on the ribbon
    ribbon.actionTriggered.connect(
        lambda act: print("[PyQt6] Action: {}".format(act.text()))
    )

    # ==================================================================
    # Show the window (maximized, like the C++ example)
    # ==================================================================
    win.showMaximized()
    sys.exit(app.exec())


# ---------------------------------------------------------------------------
# Helper functions
# ---------------------------------------------------------------------------

def _on_style_changed(ribbon, style_id, act_wordwrap):
    """Apply the selected ribbon style and sync the word-wrap checkbox.

    Different styles have different default word-wrap behavior:
    - ThreeRow styles: word wrap is ON by default
    - TwoRow styles: word wrap is OFF by default
    """
    ribbon.setRibbonStyle(style_id)
    act_wordwrap.setChecked(ribbon.isEnableWordWrap())


def _toggle_font(editor, attr, on):
    """Toggle bold/italic/underline on the editor's font."""
    f = editor.font()
    if attr == "bold":
        f.setBold(on)
    elif attr == "italic":
        f.setItalic(on)
    elif attr == "underline":
        f.setUnderline(on)
    editor.setFont(f)


def _change_font_size(editor, delta):
    """Increase or decrease the editor font size by delta points."""
    f = editor.font()
    sz = f.pointSize() + delta
    if 6 <= sz <= 48:
        f.setPointSize(sz)
        editor.setFont(f)


def _open_file(editor, parent):
    """Open a file dialog and load text into the editor."""
    path, _ = QFileDialog.getOpenFileName(
        parent, "Open File", "", "Text Files (*.txt);;All Files (*)"
    )
    if path:
        with open(path, "r", encoding="utf-8") as f:
            editor.setPlainText(f.read())


def _show_msg(parent, text):
    """Show a simple information message box."""
    QMessageBox.information(parent, "Info", text)


if __name__ == "__main__":
    main()
