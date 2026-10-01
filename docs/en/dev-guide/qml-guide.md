# SARibbonQml Development Guide (3.0)

> Applies to 3.0. SARibbonQml is the new QML front end sharing the same core
> layout engines as the widgets front end.

## Module Overview

- **Build switch**: `SARIBBON_BUILD_QML=ON` (default OFF)
- **import**: `import SARibbon 3.0`
- **Dependencies**: SARibbonCore + Qt (Core/Gui/Qml/Quick/**QuickControls2** —
  leaf popups use Controls Popup/ToolTip) only — linking
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
| `RibbonPanel` | type | panel structural host (drives PanelLayoutEngine; child registration is generic over any `RibbonLayoutItemHost`) |
| `RibbonToolButton` | type | button host (text/iconSource/proportion/checkable/**three popupModes**/menuItems menu/disabled state) |
| `RibbonControlContainer` | type | control container host (the `control` property embeds any QQuickItem: ComboBox/CheckBox/SpinBox/TextField/...; widgets SARibbonCtrlContainer counterpart) |
| `RibbonMenuItem` | type | declarative menu entry (text/iconSource/enabled/separator) attached to a button's `menuItems` |
| `RibbonContextCategory` | type | context category host (contextTitle/contextColor/active; activation appends colored tabs and publishes the band; widgets SARibbonContextCategory counterpart) |
| `RibbonGallery` | type | gallery host (Large proportion + horizontal expanding + stretchFactor joins the core engine's weighted distribution; grid metrics via core `calcGalleryGridCellSize`) |
| `RibbonGalleryGroup` | type | gallery group (groupTitle + items, default property items) |
| `RibbonGalleryItem` | type | gallery entry (text/iconSource/enabled/toolTip) |
| `RibbonSeparator` | type | panel separator (Large proportion, own column, 1px line; widgets SARibbonSeparatorWidget counterpart) |
| `Ribbon` | uncreatable | enum holder (`Ribbon.Large` / `Ribbon.ThreeRowMode` / `Ribbon.MenuButtonPopup` / `Ribbon.RibbonStyleCompactTwoRow` / ...) |

Access enums through the `Ribbon.` prefix (e.g. `proportion: Ribbon.Large`).

## Shared Host Bases

When adding a structural host, do NOT copy the leaf boilerplate — inherit:

- `RibbonQuickHost` (`src/qml/host/`): owns the visual leaf lifecycle (creation
  trilogy + safe teardown), exposes the uniform `qmlLeaf` handshake property;
  subclasses only implement `leafUrl()`.
- `RibbonLayoutItemHost`: panel-child base = `RibbonQuickHost` + the core layout
  contract (`SARibbonAbstractLayoutItem`). The base implements the
  isHidden/applyGeometry/debugName/expandingDirections defaults and the large
  row height context plumbing; subclasses only add `sizeHint()` (gallery-like
  items also override stretchFactor). `RibbonPanel` collects children through a
  `qobject_cast<RibbonLayoutItemHost*>`.

## Button Popup Modes

`popupMode` follows `QToolButton::ToolButtonPopupMode` semantics (same values):

- `MenuButtonPopup`: the button splits into an action zone and a menu zone; the
  hit zones are host-published as `actionRect`/`menuRect` (geometry authority in
  C++; the leaf only renders them and binds its MouseAreas);
- `InstantPopup`: the whole button is the menu zone (no action zone);
- `DelayedPopup`: the whole button stays the action zone; press-and-hold opens
  the menu (leaf-side timer).

Menu entries are `RibbonMenuItem` objects (`QQmlListProperty`); activation is
always mediated by the host's `menuTriggered` signal (leaf row click ->
`activateMenuItem(index)`), so tests can drive it without a windowed popup;
disabled entries and separators are ignored. A disabled host (`enabled: false`)
swallows clicks in `click()`; the leaf renders the grey state via opacity.

## Context Categories

```qml
RibbonContextCategory {
    contextTitle: "context"; contextColor: "#2d7d9a"; active: false
    RibbonCategory { title: "Page1" ... }   // pages declared as children
}
```

- Activation is driven by the **`active` property** — NOT the item visible
  (the context is a transparent structural container; the bar layout controls
  page visibility). While active, the bar appends one colored tab per page
  (`RibbonTab.contextColor`); deactivation removes them and clamps
  currentIndex.
- Bands are published through the bar's `contextBands` property (a list of
  x/width/title/color/highlight/textColor maps) and rendered by the bar leaf
  (z=-1, under the tabs); the highlight color comes from core
  `SARibbonThemeData::themeContextHighlight` (the same fp the widgets
  ThemeManager installs).
- **Known trap**: C++-created context tabs fire the bar's itemChange when
  parented onto it — they MUST be excluded via `isOwnedContextTab`, or they
  get adopted as normal tabs (eating an auto-tab slot).

## Gallery

```qml
RibbonGallery {
    stretchFactor: 1
    RibbonGalleryGroup { groupTitle: "Files"; RibbonGalleryItem { ... } ... }
}
```

- The contract face matches the widgets gallery: Large proportion +
  `expandingDirections()==Qt::Horizontal` + `stretchFactor()` joining the core
  panel engine's weighted extra-width distribution (`recalcExpandGeomArray`).
- Grid cell sizes derive from the core **`SA::calcGalleryGridCellSize`**
  (moved from the widgets SARibbonGalleryGroup::recalcGridSize — shared by
  both front ends); the host publishes gridSize/gridColumns/totalRows/
  scrollRow, the leaf only places cells inside those metrics.
- The model classes (GalleryGroup/GalleryItem) declare their default property
  via `Q_CLASSINFO("DefaultProperty", ...)` so declarative children join the
  lists directly; Repeater delegates carry no QObject parent (see leaf rules).

## Six Ribbon Styles

`RibbonBar.ribbonStyle` (enum values mirror the widgets
`SARibbonBar::RibbonStyleFlag` bits) follows the widgets setRibbonStyle
propagation semantics:

- **Loose/Compact**: Compact (tabOnTitle) rides the tab row on the title
  strip (`categoryRowY` collapses to the title height, the bar loses one
  tabBarHeight);
- **ThreeRow/TwoRow/SingleRow**: the row count propagates to every panel
  (context category pages included) as layoutMode; three-row keeps word
  wrap, single-row hides panel titles and enables iconRightText (buttons
  render icon-left/text-right regardless of proportion,
  `effectiveButtonType` parity);
- Propagation chain: bar -> category -> panel (rows + title switch) ->
  buttons (wordWrap/iconRightText); panels/buttons registered later
  inherit through the style fields stored at each level;
- Category heights derive from core
  `SARibbonMetrics::calcCategoryHeight(three, single)` (single row drops the
  panel title strip); total bar height = `calcMainBarHeight(tabOnTitle)`.

```qml
RibbonBar { ribbonStyle: Ribbon.RibbonStyleCompactTwoRow }
```

## Visual Leaf Rules

- Colors/sizes MUST bind to the RibbonTheme/RibbonMetrics singletons; a literal
  color value in a leaf is a review-reject (project-specific enhancement).
- Host<->leaf pairing (uniform handshake contract): the leaf root declares
  `property QtObject cppHost` (injected via C++ setProperty);
  `onCppHostChanged` assigns itself back to the host's inherited `qmlLeaf`
  property (doubles as the test reachability entry).
- Leaf creation trilogy (base class `ensureQmlLeaf()`): `QQmlComponent` ->
  `create()` -> `setProperty("cppHost")` -> double setParent; check
  `errorString()` and `QFile::exists(qrc path)` on failure.
- Destruction: the base class does `setParent(nullptr)` + `deleteLater()`;
  NEVER direct delete.
- Popups (menus etc.) use the Controls `Popup` with custom rows (Repeater
  delegates), colored through RibbonTheme tokens. NOTE: Repeater delegates
  carry no QObject parent — C++ tests must walk the `childItems()` item tree;
  `findChild` cannot reach them.

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
