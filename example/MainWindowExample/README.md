# MainWindowExample

## Description

The primary example for SARibbon. Demonstrates the complete feature set of the Ribbon UI in a single application. This is the best starting point for developers new to SARibbon.

The example shows a full-featured main window with ribbon bar, application button, quick access toolbar, contextual categories, gallery widgets, and a text editing area. It covers all six ribbon layout styles, theme switching, ribbon customization via dialog, action persistence, and RTL support.

## Key Classes

- `MainWindow` — inherits `SARibbonMainWindow`, demonstrates comprehensive ribbon integration
- `ApplicationWidget` — custom application menu widget shown when clicking the top-left button
- `SARibbonBar` — the ribbon bar itself, managing categories and layout styles
- `SARibbonCategory` — tab pages that host panels and controls
- `SARibbonPanel` — logical groupings of actions within a category
- `SARibbonGalleryGroup` — style gallery with thumbnail items
- `SARibbonQuickAccessBar` — small toolbar above the ribbon tabs
- `SARibbonActionsManager` — action registration for ribbon customization and persistence
- `SARibbonCustomizeDialog` — dialog allowing users to customize ribbon layout at runtime

## Building

```bash
# From the SARibbon build directory
cmake --build . --target MainWindowExample
```

## What to Look For

Run the executable and explore the ribbon tabs. Click the application button in the top-left corner to see the custom menu. Use the "Style" panel to switch between six layout styles (Loose/Compact, 1-3 rows). Try the theme dropdown to switch between the six built-in themes. Contextual categories appear when triggered from the ribbon. The "Customize" action opens a dialog to reorganize panels and actions.
