# NormalMenuBarExample

## Description

This example demonstrates how to use SARibbon with the `UseNativeMenuBar` style flag. Instead of replacing the traditional menu bar with a ribbon bar, it keeps the native menubar while still using `SARibbonMainWindow` as the base class.

The example shows the minimal integration approach: the main window is constructed with `SARibbonMainWindowStyleFlag::UseNativeMenuBar`, and since `isUseRibbon()` returns false, a traditional Qt Designer UI is loaded normally. This is useful for applications that want to gradually introduce Ribbon features while keeping the existing menu bar.

## Key Classes

- `MainWindow` — inherits `SARibbonMainWindow` with `UseNativeMenuBar` style flag
- `SARibbonMainWindow` — base class that supports both ribbon and native menu bar modes
- `SARibbonMainWindowStyleFlag` — flag controlling whether ribbon or native menu bar is used

## Building

```bash
cmake --build . --target NormalMenuBarExample
```

## What to Look For

The constructor passes `SARibbonMainWindowStyleFlag::UseNativeMenuBar` to the parent class. The `isUseRibbon()` check guards the UI setup, which only runs when ribbon mode is off. This example is intentionally minimal and serves as a baseline for understanding how SARibbonMainWindow supports dual modes.
