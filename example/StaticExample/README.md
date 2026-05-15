# StaticExample

## Description

This example shows how to integrate SARibbon using the amalgamated single-file approach (`SARibbon.h` and `SARibbon.cpp`). Instead of linking against a built library, the two generated files are compiled directly into the application executable.

It demonstrates a self-contained CMake setup that bundles the ribbon source by including `${SARIBBON_DIR}/SARibbon.h` and `${SARIBBON_DIR}/SARibbon.cpp` in the project sources. The example builds a simple ribbon window with two categories, action buttons, and a theme-switching combo box.

## Key Classes

- `MainWindow` — inherits `SARibbonMainWindow`, sets up ribbon programmatically in the constructor
- `SARibbon` — the amalgamated single-header/single-source include, combining all SARibbon classes
- `SARibbonBar` — manages categories, panels, and layout styles
- `SARibbonCategory` — tab pages hosting ribbon panels
- `SARibbonPanel` — groups of actions with support for different item sizes (large, medium, small)

## Building

```bash
cd example/StaticExample
# StaticExample can be built standalone, no SARibbon library pre-built
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/Qt"
cmake --build build
```

Or from the main SARibbon build:

```bash
cmake --build . --target StaticExample
```

## What to Look For

The CMakeLists.txt shows the amalgamated integration pattern: `SARIBBON_DIR` points to the `src/` folder, and `SARibbon.h`/`SARibbon.cpp` are added directly to `PROJECT_SOURCES`. No `find_package(SARibbonBar)` is needed. The MainWindow code creates categories, panels, and actions entirely in C++ without Qt Designer. Note the comment about setting ribbon theme in the constructor being partially ineffective, with `QTimer::singleShot` as a workaround.
