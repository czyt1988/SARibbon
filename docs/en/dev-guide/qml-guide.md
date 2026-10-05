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
| `RibbonCheckBox` | type (pure QML) | compact themed check box (14px indicator, tunable via `indicatorSide`; URL-registered, no C++ host) |
| `RibbonRadioButton` | type (pure QML) | compact themed radio button (14px ring; exclusivity via the Controls2 `ButtonGroup` attached property, the QButtonGroup counterpart) |
| `RibbonComboBox` | type (pure QML) | compact themed combo box (the dropdown list follows the theme tokens too, so dark themes do not break; `editable`/`model`/`textRole` and all native properties inherited) |
| `RibbonSpinBox` | type (pure QML) | compact themed spin box (Windows-style up/down stepper column on the trailing edge; `from`/`to`/`stepSize`/`prefix`/`suffix` and all native properties inherited) |
| `RibbonTextField` | type (pure QML) | compact themed single-line editor (`placeholderText`/`validator` and all native properties inherited) |
| `RibbonMenuItem` | type | declarative menu entry (text/iconSource/enabled/separator) attached to a button's `menuItems` |
| `RibbonContextCategory` | type | context category host (contextTitle/contextColor/active; activation appends colored tabs and publishes the band; widgets SARibbonContextCategory counterpart) |
| `RibbonGallery` | type | gallery host (Large proportion + horizontal expanding + stretchFactor joins the core engine's weighted distribution; grid metrics via core `calcGalleryGridCellSize`) |
| `RibbonGalleryGroup` | type | gallery group (groupTitle + items, default property items) |
| `RibbonGalleryItem` | type | gallery entry (text/iconSource/enabled/toolTip) |
| `RibbonSeparator` | type | panel separator (Large proportion, own column, 1px line; widgets SARibbonSeparatorWidget counterpart) |
| `RibbonQuickAccessBar` | type | quick access bar (small-button row on the title row after the app button; width feeds TitleRectInput.hasQuickAccessBar; widgets SARibbonQuickAccessBar counterpart) |
| `RibbonButtonGroup` | type | right button group (small-button row right-aligned before the system strip; widgets SARibbonButtonGroupWidget counterpart) |
| `RibbonApplicationWindow` | type | application window (office-backstage overlay behind the app button: coverageRatio coverage + slide/fade animation + top-right cross/Esc/outside click close, inner close() programmatic; click priority: window > menu > signal; widgets ApplicationWidget counterpart) |
| `Ribbon` | uncreatable | enum holder (`Ribbon.Large` / `Ribbon.ThreeRowMode` / `Ribbon.MenuButtonPopup` / `Ribbon.RibbonStyleCompactTwoRow` / ...) |

Access enums through the `Ribbon.` prefix (e.g. `proportion: Ribbon.Large`).

## Shared Host Bases

When adding a structural host, do NOT copy the leaf boilerplate — inherit:

- `RibbonQuickHost` (`src/qml/SARibbonQmlQuickHost.h`): owns the visual leaf lifecycle (creation
  trilogy + safe teardown), exposes the uniform `qmlLeaf` handshake property;
  subclasses only implement `leafUrl()`.
- `RibbonLayoutItemHost`: panel-child base = `RibbonQuickHost` + the core layout
  contract (`SARibbonAbstractLayoutItem`). The base implements the
  isHidden/applyGeometry/debugName/expandingDirections defaults and the large
  row height context plumbing; subclasses only add `sizeHint()` (gallery-like
  items also override stretchFactor). `RibbonPanel` collects children through a
  `qobject_cast<RibbonLayoutItemHost*>`.

## Basic Input Controls (URL-registered pure QML types)

`RibbonCheckBox` / `RibbonRadioButton` / `RibbonComboBox` / `RibbonSpinBox` /
`RibbonTextField` are **pure QML documents** registered through
`qmlRegisterType(QUrl("qrc:/SARibbon/RibbonXxx.qml"))` — a natural extension of
the imperative single track (still no qmldir/qmltypes install artifacts). They
carry **no C++ host**: when embedded into a panel the geometry authority belongs
to `RibbonControlContainer` (which stretches the control to the row height and
squeezes its padding); the files themselves only render. The visuals follow the
widgets QSS specializations for `SARibbonPanel > Q{CheckBox,RadioButton,ComboBox,LineEdit}`
(1px inputBorder frame, hover switches to inputFocus, selection in selectionBg),
and the combo dropdown follows the theme tokens as well. Every color binds to
`RibbonTheme` and the font to `RibbonMetrics.fontPointSize` (panel rows grow
with the ribbon font, and the text follows). The roots are `QtQuick.Templates`
template types — not the styled controls — so the look does not depend on the
application's `QQuickStyle`: these five files ARE the style implementation.

```qml
RibbonPanel {
    panelTitle: "widget test"
    RibbonControlContainer { text: "Check:"; control: RibbonCheckBox { } }
    RibbonControlContainer { text: "Spin:"; suffixText: "px";
        control: RibbonSpinBox { from: 0; to: 100; value: 12 } }
    RibbonControlContainer { text: "Font:";
        control: RibbonComboBox { model: [ "a", "b" ] } }
    RibbonControlContainer { control: RibbonRadioButton { text: "pick me" } }
    RibbonControlContainer { control: RibbonTextField { placeholderText: "type" } }
}
```

- The only added property is `indicatorSide` on the check/radio buttons
  (default 14, the indicator side length); everything else inherits the native
  Controls properties (`editable`/`model`/`currentIndex`/`from`/`to`/
  `stepSize`/`placeholderText`/`validator`/...).
- They also work outside ribbon panels (they are ordinary Controls items).
- Radio exclusivity: `ButtonGroup { id: g }` +
  `RibbonRadioButton { ButtonGroup.group: g }`.
- Canvas indicators (arrows) whose color follows a theme token must observe the
  leaf repaint rule "color change triggers `requestPaint`", or the arrow keeps
  stale colors after a theme switch.

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

## Title-Row Containers & Application Button

- **RibbonQuickAccessBar / RibbonButtonGroup** share the `RibbonButtonRowHost`
  base (buttons register through itemChange, rows flow by sizeHint, rowWidth
  published); there is no panel engine here — the row host IS the layout
  authority for its children. The bar places the former after the application
  button and the latter right-aligned against the bar's 8px right margin; a
  frameless host declares its system button area through
  `RibbonBar.systemButtonStripWidth` (default 0 — a native frame reserves
  nothing), which shifts the right group and the tab row's right boundary
  left of the reservation; the width feeds `TitleRectInput.hasQuickAccessBar`.
- **Application button, three modes** (click priority: application window >
  menu > signal only):
  - `RibbonApplicationWindow` (widgets ApplicationWidget parity): a custom
    content item declared as a bar child; the leaf hosts it through a lazy
    Popup presented as an **office-backstage overlay** — anchored on the
    window's `Overlay.overlay`, covering the window by `coverageRatio`
    (0.05..1.0; 1.0 fullscreen, 2/3 or any custom fraction), full height,
    with a selectable slide/fade enter/exit animation (`animation` +
    `animationDuration`). The content is **explicitly reparented into the
    popup's contentItem at aboutToShow** — assigning `Popup.contentItem`
    directly does NOT adopt an item that already has a visual parent
    (QQuickControl only adopts parentless items; the content kept rendering
    bare inside the bar — the "no background" bug). Esc / outside click /
    the top-right cross (`showCloseButton`, mandated for fullscreen
    coverage) close it; the inner `close()` routes closeRequested -> bar ->
    requestApplicationWindowClose -> the leaf popup. The visual reparent
    moves only the parentItem; the bar keeps the registration while the
    QObject parent stays.
  - `applicationMenuItems` + `applicationMenuTriggered` (widgets menu-mode
    parity): **the popup must be created lazily** — the bar leaf is created
    during the bar's componentComplete (the scene window is not realized
    yet), and instantiating a Popup at that point yields a stray native
    window and a blank main window (round 4 observation); create it on first
    open through `Loader { active: false }`.

## Visual Leaf Rules

- Colors/sizes MUST bind to the RibbonTheme/RibbonMetrics singletons; a literal
  color value in a leaf is a review-reject (project-specific enhancement).
- **Binding-shape iron rule (round 8)**: host-property bindings use the
  single-dependency shape `cppHost ? cppHost.yyy : fallback`; the two-dependency
  short-circuit shape `cppHost && cppHost.xxx ? cppHost.yyy : fallback` is
  FORBIDDEN — it corrupts the V4 heap at QQmlData teardown on Qt 6.7.3 debug
  builds (NOTES B48, the optionAction crash root cause). The functional
  equivalent: disabled paths carry an empty value published BY THE HOST, not
  a binding condition.
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
