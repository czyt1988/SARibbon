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

## Breaking API Changes (pre-freeze refactor)

### Placement property channel removed (plan 07)

2.x smuggled placement parameters (row proportion / popup mode / button style)
through `_sa_RowProportion` dynamic properties on the QAction. The six static
property accessors on `SARibbonPanel` are **removed**:

- `setActionRowProportionProperty` / `getActionRowProportionProperty`
- `setActionToolButtonPopupModeProperty` / `getActionToolButtonPopupModeProperty`
- `setActionToolButtonStyleProperty` / `getActionToolButtonStyleProperty`

Migration: **use the parameterized overloads**. `addAction(act, rp)`,
`addAction(act, popMode, rp)` and `addLargeAction` / `addMediumAction` /
`addSmallAction` (with their popMode overloads) keep their signatures and
semantics — delete the "set property, then addAction" lines. Placement is now a
per-panel record; the same action can carry different proportions in different
panels without interference.

### Command-level isCanCustomize flag relocated (plan 07)

The `_sa_isCanCustomize` dynamic property on a QAction is no longer the
command-level channel: use `SARibbonActionsManager::setCanCustomize(act)` /
`isCanCustomize(act)` instead (actions not registered with a manager are never
customizable). The `isCanCustomize` Q_PROPERTY on category/panel is unchanged.

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

## The QAction command layer (widgets <-> QML migration story)

Both front ends of 3.0 share **one command model**: QAction is the only
command abstraction (architecture contract D1). A widgets application moving
to QML keeps its backend QAction creation / connection / shortcut / state
code **unchanged** and writes only view declarations in QML:

```cpp
// C++ backend — identical for the widgets and the QML front end
mActionSave = new QAction(QIcon(":/save.svg"), tr("Save"), this);
mActionSave->setShortcut(QKeySequence("Ctrl+S"));
mActionSave->setCheckable(true);
```

```qml
// QML view — commands bind through the action property, placement stays
// on the button instance
RibbonPanel {
    panelTitle: qsTr("Main")
    RibbonToolButton { action: backend.actionSave; proportion: Ribbon.Large }
}
```

The reverse (QML -> widgets) works the same way: a `RibbonAction` (a QAction
subclass) feeds a widgets panel directly through `QAction::icon/text/triggered`.
The same QAction may sit in a panel, the quick access bar, a menu and a
gallery at once — one independent view per placement, state synced for free;
`QAction::shortcut` really fires through the bar-level matcher (no double
triggering with the QML `Shortcut` type). The customize system addresses
commands by the action's `objectName`; the customize XML of one front end
loads in the other.

Plain-declarative buttons (no action) keep first-class semantics (the
widgets `addWidget` parity). Menus are QAction lists (`menuActions`); the
2.x `RibbonMenuItem` type is gone.
