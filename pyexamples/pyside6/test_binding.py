"""
PySideSARibbon binding smoke tests
==================================

Verifies that the PySide6 bindings for SARibbon are correctly generated
and that key classes, enums, and methods are accessible from Python.

Run:
  python pyexamples/pyside6/test_binding.py
"""
import sys
import traceback

PASS = 0
FAIL = 0


def check(label, fn):
    """Run a check function and report pass/fail."""
    global PASS, FAIL
    try:
        fn()
        print(f"  [PASS] {label}")
        PASS += 1
    except Exception as e:
        print(f"  [FAIL] {label}: {e}")
        traceback.print_exc()
        FAIL += 1


def main():
    global PASS, FAIL

    print("=" * 60)
    print("PySideSARibbon Binding Smoke Tests")
    print("=" * 60)

    # --- Import ---
    print("\n[1] Import")

    def test_import():
        from PySideSARibbon import saribbon
        assert saribbon is not None
    check("import PySideSARibbon.saribbon", test_import)

    from PySideSARibbon import saribbon

    # --- Enums ---
    print("\n[2] Enums")

    def test_global_enums():
        assert hasattr(saribbon, "SARibbonAlignment")
        assert hasattr(saribbon, "SARibbonTheme")
        assert hasattr(saribbon, "SARibbonMainWindowStyleFlag")
    check("global enums accessible", test_global_enums)

    def test_ribbon_bar_enums():
        assert hasattr(saribbon.SARibbonBar, "RibbonStyleFlag")
        assert hasattr(saribbon.SARibbonBar, "RibbonMode")
        # Test enum values
        assert saribbon.SARibbonBar.RibbonStyleLooseThreeRow is not None
        assert saribbon.SARibbonBar.RibbonStyleCompactTwoRow is not None
        assert saribbon.SARibbonBar.MinimumRibbonMode is not None
        assert saribbon.SARibbonBar.NormalRibbonMode is not None
    check("SARibbonBar enums", test_ribbon_bar_enums)

    def test_panel_enums():
        assert hasattr(saribbon.SARibbonPanel, "PanelLayoutMode")
    check("SARibbonPanel enums", test_panel_enums)

    def test_tool_button_enums():
        assert hasattr(saribbon.SARibbonToolButton, "RibbonButtonType")
    check("SARibbonToolButton enums", test_tool_button_enums)

    # --- Classes ---
    print("\n[3] Classes")

    def test_core_classes():
        for cls in [
            "SARibbonMainWindow",
            "SARibbonBar",
            "SARibbonCategory",
            "SARibbonPanel",
            "SARibbonPanelLabel",
            "SARibbonToolButton",
            "SARibbonTabBar",
            "SARibbonButtonGroupWidget",
            "SARibbonQuickAccessBar",
            "SARibbonGallery",
            "SARibbonGalleryGroup",
            "SARibbonGalleryItem",
            "SARibbonGalleryButton",
            "SARibbonGalleryViewport",
            "SARibbonContextCategory",
            "SARibbonPanelItem",
        ]:
            assert hasattr(saribbon, cls), f"Missing class: {cls}"
    check("core classes accessible", test_core_classes)

    def test_new_classes():
        for cls in [
            "SARibbonWidget",
            "SARibbonMenu",
            "SARibbonApplicationButton",
            "SARibbonColorToolButton",
            "SARibbonActionsManager",
            "SARibbonActionsManagerModel",
            "SARibbonCustomizeData",
            "SARibbonCustomizeWidget",
            "SARibbonCustomizeDialog",
        ]:
            assert hasattr(saribbon, cls), f"Missing new class: {cls}"
    check("new classes accessible", test_new_classes)

    # --- Functional Tests (require QApplication) ---
    print("\n[4] Functional Tests")

    from PySide6.QtWidgets import QApplication, QTextEdit
    from PySide6.QtGui import QAction, QIcon
    from PySide6.QtCore import Qt

    app = QApplication.instance() or QApplication(sys.argv)

    def test_main_window():
        win = saribbon.SARibbonMainWindow()
        assert win is not None
        ribbon = win.ribbonBar()
        assert ribbon is not None
        assert isinstance(ribbon, saribbon.SARibbonBar)
    check("create SARibbonMainWindow + ribbonBar()", test_main_window)

    def test_ribbon_style():
        win = saribbon.SARibbonMainWindow()
        ribbon = win.ribbonBar()
        ribbon.setRibbonStyle(saribbon.SARibbonBar.RibbonStyleLooseThreeRow)
        style = ribbon.currentRibbonStyle()
        assert style == saribbon.SARibbonBar.RibbonStyleLooseThreeRow
    check("setRibbonStyle / currentRibbonStyle", test_ribbon_style)

    def test_category_panel():
        win = saribbon.SARibbonMainWindow()
        ribbon = win.ribbonBar()
        cat = ribbon.addCategoryPage("Test")
        assert isinstance(cat, saribbon.SARibbonCategory)
        assert cat.categoryName() == "Test"
        panel = cat.addPanel("Panel1")
        assert isinstance(panel, saribbon.SARibbonPanel)
        assert panel.panelName() == "Panel1" or panel.name() == "Panel1"
    check("addCategoryPage / addPanel", test_category_panel)

    def test_actions():
        win = saribbon.SARibbonMainWindow()
        ribbon = win.ribbonBar()
        cat = ribbon.addCategoryPage("Actions")
        panel = cat.addPanel("Actions")

        act_large = QAction("Large")
        panel.addLargeAction(act_large)

        act_small = QAction("Small")
        panel.addSmallAction(act_small)

        panel.addSeparator()
    check("addLargeAction / addSmallAction / addSeparator", test_actions)

    def test_gallery():
        win = saribbon.SARibbonMainWindow()
        ribbon = win.ribbonBar()
        cat = ribbon.addCategoryPage("Gallery")
        panel = cat.addPanel("Gallery")
        gallery = panel.addGallery()
        assert isinstance(gallery, saribbon.SARibbonGallery)

        group = gallery.addGalleryGroup()
        assert isinstance(group, saribbon.SARibbonGalleryGroup)

        gallery.setCurrentViewGroup(group)
        current = gallery.currentViewGroup()
        assert current is not None
    check("addGallery / addGalleryGroup / setCurrentViewGroup", test_gallery)

    def test_context_category():
        win = saribbon.SARibbonMainWindow()
        ribbon = win.ribbonBar()
        ctx = ribbon.addContextCategory("Context", Qt.GlobalColor.red)
        assert isinstance(ctx, saribbon.SARibbonContextCategory)
        cat = ctx.addCategoryPage("CtxPage")
        assert isinstance(cat, saribbon.SARibbonCategory)
        ribbon.showContextCategory(ctx)
        assert ribbon.isContextCategoryVisible(ctx)
        ribbon.hideContextCategory(ctx)
        assert not ribbon.isContextCategoryVisible(ctx)
    check("addContextCategory / showContextCategory / hideContextCategory", test_context_category)

    def test_minimum_mode():
        win = saribbon.SARibbonMainWindow()
        ribbon = win.ribbonBar()
        ribbon.setMinimumMode(True)
        assert ribbon.isMinimumMode()
        ribbon.setMinimumMode(False)
        assert not ribbon.isMinimumMode()
    check("setMinimumMode / isMinimumMode", test_minimum_mode)

    def test_theme():
        win = saribbon.SARibbonMainWindow()
        win.setRibbonTheme(saribbon.SARibbonTheme.RibbonThemeDark)
        theme = win.ribbonTheme()
        assert theme == saribbon.SARibbonTheme.RibbonThemeDark
    check("setRibbonTheme / ribbonTheme", test_theme)

    def test_ribbon_widget():
        widget = saribbon.SARibbonWidget()
        assert widget is not None
        assert isinstance(widget.ribbonBar(), saribbon.SARibbonBar)
    check("SARibbonWidget creation", test_ribbon_widget)

    def test_application_button():
        btn = saribbon.SARibbonApplicationButton("File")
        assert btn is not None
        assert btn.text() == "File"
        win = saribbon.SARibbonMainWindow()
        ribbon = win.ribbonBar()
        ribbon.setApplicationButton(btn)
        assert ribbon.applicationButton() is not None
    check("SARibbonApplicationButton", test_application_button)

    def test_ribbon_menu():
        menu = saribbon.SARibbonMenu("Test Menu")
        assert menu is not None
        sub = menu.addRibbonMenu("Submenu")
        assert isinstance(sub, saribbon.SARibbonMenu)
        act = menu.addAction("Item")
        assert act is not None
    check("SARibbonMenu", test_ribbon_menu)

    def test_actions_manager():
        win = saribbon.SARibbonMainWindow()
        ribbon = win.ribbonBar()
        mgr = saribbon.SARibbonActionsManager(ribbon)
        assert isinstance(mgr, saribbon.SARibbonActionsManager)
        assert hasattr(saribbon.SARibbonActionsManager, "CommonlyUsedActionTag")
        assert hasattr(saribbon.SARibbonActionsManager, "NotInRibbonCategoryTag")
        mgr.setTagName(saribbon.SARibbonActionsManager.CommonlyUsedActionTag, "Common")
        name = mgr.tagName(saribbon.SARibbonActionsManager.CommonlyUsedActionTag)
        assert name == "Common"
    check("SARibbonActionsManager", test_actions_manager)

    def test_customize_data():
        data = saribbon.SARibbonCustomizeData()
        assert not data.isValid()
        data2 = saribbon.SARibbonCustomizeData(
            saribbon.SARibbonCustomizeData.AddCategoryActionType
        )
        assert data2.isValid()
        assert (
            data2.actionType()
            == saribbon.SARibbonCustomizeData.AddCategoryActionType
        )
    check("SARibbonCustomizeData", test_customize_data)

    def test_color_tool_button():
        btn = saribbon.SARibbonColorToolButton()
        assert isinstance(btn, saribbon.SARibbonToolButton)
        from PySide6.QtGui import QColor
        btn.setColor(QColor(255, 0, 0))
        assert btn.color() == QColor(255, 0, 0)
        btn.setColorStyle(saribbon.SARibbonColorToolButton.ColorUnderIcon)
        assert (
            btn.colorStyle()
            == saribbon.SARibbonColorToolButton.ColorUnderIcon
        )
    check("SARibbonColorToolButton", test_color_tool_button)

    def test_signals():
        win = saribbon.SARibbonMainWindow()
        ribbon = win.ribbonBar()
        # Check signal exists
        assert hasattr(ribbon, "actionTriggered")
        assert hasattr(ribbon, "currentRibbonTabChanged")
        assert hasattr(ribbon, "ribbonStyleChanged")
        assert hasattr(ribbon, "ribbonModeChanged")
        assert hasattr(ribbon, "applicationButtonClicked")
    check("signals accessible", test_signals)

    # --- Summary ---
    print("\n" + "=" * 60)
    print(f"Results: {PASS} passed, {FAIL} failed")
    print("=" * 60)
    return 0 if FAIL == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
