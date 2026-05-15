# MatlabUI

## Description

A streamlined example that mimics the ribbon interface style of MATLAB. It demonstrates how to create a simplified, clean ribbon layout with a custom QSS theme that departs from the built-in SARibbon themes.

Unlike the full MainWindowExample, this example focuses on essential ribbon features. It uses a bundled `theme-matlab.qss` stylesheet to apply a deep blue ribbon bar header, white tab background, and custom tab bar styling that resembles the MATLAB environment.

## Key Classes

- `MainWindow` — inherits `SARibbonMainWindow`, demonstrates a minimal but practical ribbon setup
- `SARibbonBar` — ribbon bar configured with a custom QSS theme instead of built-in themes
- `SARibbonCategory` — tab pages hosting panels with ribbon controls
- `SARibbonQuickAccessBar` — minimal quick access toolbar
- `SARibbonApplicationWidget` — application menu widget for the top-left button
- `SARibbonPanel` — panel groupings within categories

## Building

```bash
# From the SARibbon build directory
cmake --build . --target MatlabUI
```

## What to Look For

The most noticeable difference from the main example is the custom theme. The ribbon bar uses a dark blue (#004076) background with white-colored tab labels. Tab hover effects are implemented with top and side borders. This example shows how QSS can be used to completely restyle SARibbon without modifying C++ code. The `theme-matlab.qss` file serves as a practical starting point for custom theme design.
