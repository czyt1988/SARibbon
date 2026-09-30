# SARibbonQml Development Guide (3.0)

> Applies to 3.0. SARibbonQml is the new QML front end sharing the same core
> layout engines as the widgets front end.

## Module Overview

- **Build switch**: `SARIBBON_BUILD_QML=ON` (default OFF)
- **import**: `import SARibbon 3.0`
- **Dependencies**: SARibbonCore + Qt (Core/Gui/Qml/Quick) only — linking
  SARibbonWidgets is FORBIDDEN (dependency matrix red line; the CI combination
  `Widgets=OFF Qml=ON` enforces it)

## Registration (imperative single track)

Call once before `engine.load()` (Qt5/Qt6 same code):

```cpp
#include <SARibbonQml/SARibbonQmlGlobal.h>

QQmlApplicationEngine engine;
saRibbonRegisterQmlTypes(&engine);   // REQUIRED: skipping it gives "module SARibbon is not installed"
engine.load(QUrl("qrc:///main.qml"));
```

**Singleton registration paradigm (important)**: this project's QML singletons
always use the CALLBACK form of `qmlRegisterSingletonType` with
`setObjectOwnership(CppOwnership)` inside the callback.
`qmlRegisterSingletonInstance` is FORBIDDEN — it hard-binds the instance to the
first engine; a second engine gets nullptr (verified against Qt 5.14/6.7 sources).

## P0 Type List

| QML type | Form | Description |
|----------|------|-------------|
| `RibbonTheme` | singleton | core theme data bridge (currentTheme, token colors) |
| `RibbonMetrics` | singleton | core metrics bridge (tabBarHeight etc.) |
| `RibbonBar` | type | bar structural host (tab row, height, title free area) |
| `RibbonCategory` | type | category structural host (panel layout + clamped scroll) |
| `RibbonTab` | type | tab host (text/current/contextColor) |
| `RibbonPanel` | type | panel structural host (drives PanelLayoutEngine) |
| `RibbonToolButton` | type | button host (text/iconSource/proportion) |
| `Ribbon` | uncreatable | enum holder (`Ribbon.Large` / `Ribbon.ThreeRowMode` / ...) |

Access enums through the `Ribbon.` prefix (e.g. `proportion: Ribbon.Large`).

## Visual Leaf Rules

- Colors/sizes MUST bind to the RibbonTheme/RibbonMetrics singletons; a literal
  color value in a leaf is a review-reject (project-specific enhancement).
- Host<->leaf pairing: the leaf root declares `property QtObject panelCpp`
  (injected via C++ setProperty); `onPanelCppChanged` assigns itself back to the
  host's `panelQmlItem` (handshake property, doubles as the test reachability
  entry).
- Leaf creation trilogy (host side): `QQmlComponent` -> `create()` ->
  `setProperty("panelCpp")` -> double setParent; check `errorString()` and
  `QFile::exists(qrc path)` on failure.
- Destruction: `setParent(nullptr)` + `deleteLater()`; NEVER direct delete.

## Iron Rule

Needing to "copy" a widgets algorithm on the QML side means the core is missing
a piece — stop and fix the core. Rewriting layout math in QML is FORBIDDEN.
Rendering may be duplicated.

- sizeHint chain (S5): core metrics -> host C++ `sizeHint()` -> engine boxing ->
  `resultGeometry` -> `applyGeometry`. NEVER derive from the QML leaf's
  implicitWidth/Height.

## Scroll Animation

Category scrolling animates with QML `Behavior on x` (frontend animation); the
target goes through the engine's `clampScrollOffset` — animation is
presentation, clamping is algorithm.
