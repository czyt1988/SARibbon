# UseNativeFrameExample

## Description

This is the most comprehensive SARibbon example, demonstrating full integration with the `UseNativeFrame` style flag. It shows how to build a feature-rich ribbon application with a custom native window frame, contextual categories, quick access toolbar, and right-side button groups.

The example covers ribbon customization via `SARibbonCustomizeDialog`, action persistence through XML, runtime action visibility toggling, and dynamic title bar background color changes. It serves as the most detailed reference implementation after `MainWindowExample`.

## Key Classes

- `MainWindow` — inherits `SARibbonMainWindow` with `UseNativeFrame | UseRibbonMenuBar` style flags
- `SARibbonApplicationWidget` — custom application menu with rich content and search
- `SARibbonContextCategory` — contextual categories that appear conditionally
- `SARibbonQuickAccessBar` — quick access toolbar with customizable actions
- `SARibbonCustomizeWidget` — widget for runtime ribbon customization
- `SARibbonActionsManager` — action registration, persistence, and customization management
- `SARibbonButtonGroupWidget` — group of related buttons in a single panel item

## Building

```bash
cmake --build . --target UseNativeFrameExample
```

## What to Look For

The constructor passes both `UseNativeFrame` and `UseRibbonMenuBar` flags, providing a native-framed window with ribbon content. The example is split into helper methods: `createRibbonApplicationButton()`, `createCategoryMain()`, `createCategoryOther()`, `createContextCategory()`, `createQuickAccessBar()`, `createRightButtonGroup()`, and `createWindowButtonGroupBar()`. Look at how contextual categories are toggled via `onShowContextCategory()`, and how action visibility/enablement is controlled dynamically. The customize dialog integration and XML persistence show how to save and restore user customizations between sessions.
