# WidgetWithRibbon

## Description

Shows how to embed SARibbon into a plain `QWidget` hierarchy without using `SARibbonMainWindow`. This is useful when you need to integrate a ribbon bar into an existing widget-based application or when you want to embed the ribbon in a specific sub-area rather than as the main window chrome.

The example uses `SARibbonWidget` as the ribbon container, which provides the same ribbon functionality but as a child widget that can be placed in any layout. This is an alternative approach to `SARibbonMainWindow` when you need more control over the widget hierarchy.

## Key Classes

- `RibbonWidget` — inherits `SARibbonWidget`, provides the ribbon bar as a child widget with custom build logic
- `MainWidget` — top-level `QWidget` container that hosts both the ribbon and content area
- `InnerWidget` — content area widget below the ribbon, receives text updates from ribbon actions
- `SARibbonWidget` — the core widget that provides ribbon functionality outside of `SARibbonMainWindow`
- `SARibbonBar` — the actual ribbon bar managed by `SARibbonWidget`

## Building

```bash
# From the SARibbon build directory
cmake --build . --target WidgetWithRibbon
```

## What to Look For

The application window uses a standard QWidget instead of SARibbonMainWindow. The ribbon bar is positioned at the top of the window as a child widget within a layout. Actions on the ribbon interact with the inner content widget, demonstrating how ribbon actions communicate with unrelated widgets in the tree. Use this example when you need ribbon in a dialog, a dock widget, or any non-main-window context.
