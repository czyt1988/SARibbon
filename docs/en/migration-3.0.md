# SARibbon 3.0 Migration Guide

> For users upgrading from SARibbon 2.x (<= 2.9.5) to 3.0.

## Summary

SARibbon 3.0 is a **source-level highly compatible** major refactor: all widget
class names, public APIs, signals and behavior are unchanged (the layout
algorithms are proven byte-identical to 2.9.5 through golden geometry tests).
Most user projects only need a handful of build-system renames.

## Build Requirements

| Item | 2.x | 3.0 |
|------|-----|-----|
| CMake | 3.15+ | **3.21+** |
| Qt | 5.12+ | **5.15+** |
| C++ | C++14 (C++17 with frameless) | **C++17** (all modules) |

## Include Paths

```cpp
// 2.x
#include <SARibbonBar/SARibbonBar.h>
// 3.0 (recommended)
#include <SARibbonWidgets/SARibbonBar.h>
```

The legacy path `<SARibbonBar/...>` keeps working through compatibility
forwarding headers (the install tree ships an `include/SARibbonBar/` forwarding
directory) for one release cycle.

## CMake Consumption

```cmake
# 2.x
find_package(SARibbonBar REQUIRED)
target_link_libraries(app PRIVATE SARibbonBar::SARibbonBar)

# 3.0 (recommended)
find_package(SARibbon 3.0 REQUIRED COMPONENTS Widgets)   # or Core / Qml
target_link_libraries(app PRIVATE SARibbon::Widgets)
```

The legacy package name `SARibbonBar` stays available as a compatibility shell
for one release cycle; the in-tree bare target name `SARibbonBar` and the
`SARibbonBar::SARibbonBar` alias are kept as well.

## Option Renames

| 2.x | 3.0 |
|-----|-----|
| `BUILD_TESTS` | `SARIBBON_BUILD_TESTS` (old name shimmed for one version) |
| — | `SARIBBON_BUILD_WIDGETS` (core-only combination) |
| — | `SARIBBON_BUILD_QML` (default OFF) |
| — | `SARIBBON_INSTALL` (OFF for add_subdirectory embedding) |

## Single-File Distribution

`src/SARibbon.h/.cpp` is no longer committed to the repository. Get the
products:

```bash
# generate locally (4 artifacts: SARibbonCore.h/.cpp and SARibbonWidgets.h/.cpp)
cd tools && bash Amalgamate.sh
```

or download the packaged products from the GitHub Release assets. StaticExample
now builds guarded: when the products are missing it warns and skips without
breaking the rest of the examples.

## Python Bindings

All three distribution/import names are unchanged (`PyQtSARibbon` /
`PyQt6SARibbon` / `PySideSARibbon`), version 3.0.0. Internally the bindings now
compile the three-module source tree (with a core-header mirror mechanism);
users are unaffected.

## Notes

- The `SARibbonPannel` spelling was already fixed in 2.9.x (`SARibbonPanel`);
  3.0 needs no alias.
- The metric comparison (tabBarHeight/categoryHeight etc., 7 items) matches
  2.9.5 exactly: see [docs/3.0/metrics-comparison.md](../3.0/metrics-comparison.md).

## QML (new capability)

3.0 adds the `SARibbonQml` module (`import SARibbon 3.0`): C++ structural hosts
drive the exact same core layout engines as the widgets front end; QML only
renders. Applications call once before `engine.load()`:

```cpp
saRibbonRegisterQmlTypes(&engine);
```

Note: singleton registration uses the callback-form API and is safe with
multiple engines; see the QML development docs.
