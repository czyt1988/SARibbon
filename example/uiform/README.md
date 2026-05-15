# uiform

## Description

This example demonstrates how to use SARibbon with a Qt Designer `.ui` file. It shows the correct approach for integrating a ribbon bar into a UI that was designed visually rather than built programmatically.

The key takeaway is handling the interaction between Qt Designer's default `QMenuBar` and the `SARibbonBar`. The example creates a ribbon bar manually after `setupUi()` and demonstrates adding actions defined in the `.ui` file to ribbon panels.

## Key Classes

- `MainWindow` — inherits `SARibbonMainWindow`, uses a Qt Designer `.ui` file for UI definition
- `SARibbonBar` — created programmatically after `setupUi()` to replace the designer-defined menubar
- `SARibbonPanel` — hosts actions, including `QAction` instances defined in the `.ui` file
- `Ui::MainWindow` — generated UI class from `MainWindow.ui`

## Building

```bash
cmake --build . --target uiform
```

## What to Look For

The `.ui` file contains a `QMenuBar` and a `QAction` named `actionFile`. After `ui->setupUi(this)`, a new `SARibbonBar` is created and populated with a category and panel. The action from the `.ui` file (`ui->actionFile`) is added directly to the panel via `panel->addAction()`. The comments in `MainWindow.cpp` note that the designer's `QMenuBar` should ideally be removed from the `.ui` file to avoid unnecessary widget creation. Call `setRibbonBar(bar)` to install the ribbon.
