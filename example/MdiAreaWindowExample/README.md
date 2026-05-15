# MdiAreaWindowExample

## Description

Demonstrates SARibbon integration with a MDI (Multiple Document Interface) workspace using `QMdiArea`. This example shows how to combine a ribbon bar with sub-window management, a common pattern in engineering and CAD applications.

The example creates sub-windows on demand via the ribbon. A custom corner widget (`m_mdiCornerWidget`) replaces the default MDI area corner, and window activation updates the ribbon state. Sub-window state changes (minimize, maximize, normal) are tracked to reflect the current context.

## Key Classes

- `MainWindow` — inherits `SARibbonMainWindow` with an embedded `QMdiArea`
- `SARibbonBar` — ribbon bar integrated into an MDI workspace
- `SARibbonCategory` — single main category providing window management actions
- `QMdiSubWindow` — child windows managed by the MDI area
- `QMdiArea` — the MDI container for hosting multiple sub-windows

## Building

```bash
# From the SARibbon build directory
cmake --build . --target MdiAreaWindowExample
```

## What to Look For

Use the ribbon to create new sub-windows. Each sub-window appears inside the central MDI area. The corner widget area is customized instead of using the default MDI scroll button arrangement. When switching between sub-windows, the ribbon can update to reflect the active window context. This is the go-to example for anyone building a multi-document application with SARibbon.
