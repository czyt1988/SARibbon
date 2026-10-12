# SARibbon QML integration skill

A cookbook for embedding the SARibbon QML module (`import SARibbon 3.0`) into a
Qt Quick application. Everything here is a verified recipe or a real failure
mode encountered in production use — the pitfalls section is the reason this
file exists. The widgets front end (SARibbonWidgets) is out of scope.

- Working reference app: `examples/qml/QmlMainWindowExample`
- C++ host surface: `src/qml/SARibbonQmlTypes.cpp` (the imperative
  registration), headers next to it per class
- Both Qt 5.15 and Qt 6 are first-class; Qt 5 has a dedicated section below —
  read it before touching anything if your consumer app still builds on 5.15

## 1. CMake wiring

```bash
cmake -B build -DSARIBBON_BUILD_QML=ON ...
```

- `SARIBBON_BUILD_QML` is **OFF by default**. Turning it ON auto-enables
  `SARIBBON_USE_FRAMELESS_LIB` — the QML module hard-requires QWindowKit
  (`QWindowKit::Quick`); there is no frameless-less configuration.
- Link the alias `SARibbon::Qml` (export name `Qml`). Consumers never link
  SARibbonCore directly; the Qml target carries it.
- Building from the source tree pulls the nested submodule chain
  `3rdparty/qwindowkit → qmsetup → syscmdline`. On an offline machine clone
  them from an existing local checkout — note the `-c submodule.<name>.url`
  key takes the **name from .gitmodules, not the path**, and modern git also
  needs `protocol.file.allow`:

```bash
# inside the SARibbon checkout, $LOCAL = a populated SARibbon checkout
git -c protocol.file.allow=always \
    -c submodule.3rdparty/qwindowkit.url=$LOCAL/3rdparty/qwindowkit \
    submodule update --init 3rdparty/qwindowkit
cd 3rdparty/qwindowkit
git -c protocol.file.allow=always \
    -c submodule.qmsetup.url=$LOCAL/3rdparty/qwindowkit/qmsetup \
    submodule update --init qmsetup
cd qmsetup
git -c protocol.file.allow=always \
    -c submodule.syscmdline.url=$LOCAL/3rdparty/qwindowkit/qmsetup/src/syscmdline \
    submodule update --init src/syscmdline
```

Getting a name wrong (e.g. `submodule.src/syscmdline.url`) fails with a
confusing "repository does not exist" pointing at a *relative* URL — that is
the misspelled-key symptom, not a network problem.

## 2. C++ host: register types before engine.load()

The module has no qmldir plugin; registration is imperative and must run
before any QML that says `import SARibbon 3.0` is loaded:

```cpp
#include <SARibbonQml/SARibbonQmlGlobal.h>

int main(int argc, char *argv[])
{
    // ... QApplication / QQuickStyle setup ...
    QQmlApplicationEngine engine;
    saRibbonRegisterQmlTypes(&engine);   // idempotent, static-once inside
    engine.load(...);
}
```

- Static builds: the function performs the `Q_INIT_RESOURCE(saribbon_qml)`
  dance internally; do not attempt it yourself (it must run from a function
  in the same translation unit as the resource, which is why the library owns
  it).
- No `addImportPath` is needed for SARibbon itself — `qmlRegisterType` puts
  the types in the global registry. (Your own qrc-hosted QML modules may
  still need one on Qt 5; that is a consumer concern, not a SARibbon one.)
- Every ribbon-facing type name is CamelCase (`RibbonBar`, `RibbonToolButton`
  …); the enums hang off the `Ribbon` namespace object in QML:
  `Ribbon.IconOnly`, `Ribbon.MenuButtonPopup`,
  `Ribbon.RibbonStyleCompactSingleRow`, `Ribbon.GalleryIconWithWordWrapText`…

## 3. Composition skeleton

```
ApplicationWindow
└─ RibbonBar (anchors: top/left/right)          ← the frameless title bar too
   ├─ RibbonCategory  (tab #1; tabs are auto-generated per category)
   │  ├─ RibbonPanel  (a bordered group with a bottom title)
   │  │  ├─ RibbonToolButton
   │  │  ├─ RibbonControlContainer   ← embeds foreign QML controls
   │  │  ├─ RibbonGallery            ← full-height expanding grid
   │  │  └─ RibbonSeparator
   │  └─ ...
   └─ RibbonContextCategory           ← programmatic show/hide pages
```

```qml
import QtQuick
import SARibbon 3.0

ApplicationWindow {
    minimumWidth: ribbonBar.minimumWidth   // engine-measured; don't guess
    visible: true

    RibbonBar {
        id: ribbonBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        windowAgent: RibbonWindowAgent { buttonWidth: 35 }  // frameless
        applicationLabel: qsTr("File")

        RibbonCategory {
            title: qsTr("Home")
            RibbonPanel {
                panelTitle: qsTr("Clipboard")
                RibbonToolButton {
                    text: qsTr("Paste")
                    toolButtonStyle: Ribbon.IconOnly
                    iconSource: "qrc:/icons/paste.svg"
                    onClicked: ...
                }
            }
        }
    }
}
```

Geometry authority lives in the C++ hosts — never place or size categories,
panels, tabs or the application button yourself; declare content and let the
engine lay it out. Two numbers you *should* bind to:

- `RibbonBar.minimumWidth` → window `minimumWidth` (measured from the title
  row, tab row, quick-access bar and system strip, with screen-ratio rules
  applied).
- The bar's `implicitHeight` → whatever hosts the bar. See pitfall P1.

## 4. Buttons, actions, menus

- `RibbonToolButton` supports `toolButtonStyle`
  (`Ribbon.IconOnly` / `Ribbon.TextOnly` / `Ribbon.TextBesideIcon` /
  `Ribbon.TextUnderIcon`), `popupMode` (`Ribbon.DelayedPopup` /
  `Ribbon.MenuButtonPopup` — splits a menu arrow — / `Ribbon.InstantPopup`),
  `checkable`, and `menuActions: [RibbonAction { … }]` for dropdown menus.
- `RibbonAction` is the QAction-derived command object; gallery cells can
  bind one via the group's `addAction(QAction*)`.
- **A `checkable` button must have an observable state.** Bind `checked` to
  something that changes when the world changes (a property with NOTIFY, a
  facade signal). A state that only exists as a *function call result* (e.g.
  "what is under the text cursor") cannot drive a binding — the toggle will
  flip on click and then lie forever. This exact defect shipped once: the
  buttons toggled, nothing followed the cursor, users read it as broken.
  Rule of thumb: no query side, no checkable.
- Buttons in the bar's **right button group** must be `RibbonToolButton`s —
  a search field or combo box cannot go there. Put foreign controls in a
  panel via `RibbonControlContainer` (next section).

## 5. Embedding foreign QML controls

`RibbonControlContainer` adopts any `QQuickItem` (`control:`), with an
optional leading label (`text`, `iconSource`, collapsible via
`enableShowIcon` / `enableShowTitle`) and trailing `suffixText`. It is the
only sanctioned way to put a TextField/ComboBox/SpinBox into a panel.

- Default `rowProportion` is Small: the control is squeezed to padding 1 and
  **clipped to one ribbon row**. That is correct for a combo or a line edit.
- It is wrong for anything tall or self-drawn: a custom panel taller than one
  row gets visually clipped to nothing — the symptom is "the container is
  there but I see no content". If the content is a grid of selectable cells,
  that is a `RibbonGallery`, not a container. For other tall content,
  restructure the panel or host it in the gallery's expanded viewport
  (`RibbonGalleryViewportWidget` sections).

## 6. Galleries

`RibbonGallery` is a full-height (Large proportion) horizontally expanding
grid: per-group paging, scroll buttons, and a "more" button that pops the
expanded viewport listing every group at the same cell metrics (with a
size-grip). `captionStyle` picks the cell caption band
(`Ribbon.GalleryIconOnly` / `Ribbon.GalleryIconWithText` /
`Ribbon.GalleryIconWithWordWrapText`).

Two filling modes:

- **Declarative** — `groups: [RibbonGalleryGroup { groupTitle: …; items: [
  RibbonGalleryItem { text; iconSource; toolTip } ] }]`. The list properties
  are static QML documents; there is no runtime append on the declarative
  face. Use it for fixed content.
- **Dynamic (user data)** — the Q_INVOKABLE API, for content that changes at
  runtime. The rebuild pattern is `clear()` → `addGroup(title)` → per-row
  `group.addItem(text, iconSource, toolTip, userData)`:

```qml
RibbonGallery {
    id: gallery
    property var rows: model.rows           // some reactive list
    onRowsChanged: rebuild()
    Component.onCompleted: rebuild()
    function rebuild() {
        clear()
        const group = addGroup(qsTr("Templates"))
        for (let i = 0; i < rows.length; ++i)
            group.addItem(rows[i].title, "", rows[i].title, rows[i].id)
        currentGroupIndex = 0
    }
    onTriggered: function (item, index) {
        insertById(item.userData)           // userData survives the rebuild
    }
}
```

- `userData` is an opaque consumer payload handed back verbatim on
  `triggered(item, index)` — the mapping from cell back to model row needs no
  parallel array in QML.
- `selectable` gates whether a cell may become *current* (selection mark);
  activation fires regardless. `enabled` gates both, with the standard
  45%-opacity look.
- `activateItem(index)` is the programmatic activation entry (same terminal
  as a cell click) — tests should drive it instead of synthesizing clicks on
  leaf cells.
- Cells created by `addItem()`/`addAction()` are owned by the group;
  `RibbonGalleryGroup::clear()` deletes them. Do not call `clear()` on a
  group whose items were declared in a QML document.

## 7. Theme bridging

The ribbon has its own design system (`RibbonTheme` singleton). The bridge
between an app theme and the ribbon is **one-way**: push, never bind
two-way.

```js
function applyBridgeTheme() {
    RibbonTheme.currentTheme = appTheme.variant === "dark"
            ? Ribbon.RibbonThemeOffice2021Dark
            : Ribbon.RibbonThemeOffice2021Blue
    RibbonTheme.setAccentColor(appTheme.chromeBg)   // ← chrome, not accent!
    RibbonTheme.setContentBgColor(appTheme.surfaceBg)
    RibbonTheme.setTextColor(appTheme.textPrimary)
}
```

- **`accent` in SARibbon is the title/tab row background**, not an emphasis
  color. Pushing the app's highlight color here paints the whole tab strip in
  it and visually detaches the ribbon from the window chrome. Push the app's
  chrome/toolBar background instead; content backgrounds and text go to their
  own setters.
- Push once per theme change (`Theme.changed`-style signal), not per frame.
- A "follow app theme" toggle is a consumer feature: when off, simply stop
  pushing (the ribbon keeps its last palette); do not attempt to sync
  backwards.
- Icons have no tinting pipeline. `iconSource` renders as-is, which breaks
  monochrome SVGs on dark themes. The portable recipe is re-encoding the SVG
  per palette as a data URL with the color baked into the glyph fill:
  `"data:image/svg+xml;utf8," + encodeURIComponent(svgWithColor)` — works on
  both engine generations, no effects module needed.

## 8. Collapsed (minimum) mode and the embedding contract

`minimumMode: true` folds the bar to its title + tab row. Single-clicking a
tab raises the current category as a floating **preview** overlaying the
central content (Qt::Popup parity: it drops on an outside press, never
reflows the layout); double-click toggles the mode itself
(`tabDoubleClickToMinimumMode`, default on).

**Embedding contract — the single most common "it's broken" report.** The
preview paints past the bar's bottom edge and raises the bar's z *among its
own siblings*. That is enough when the bar is a direct child of the window's
content item (the recommended hosting, as in the example). When the bar sits
deeper — inside a `Loader`, a layout, any wrapper Item whose siblings come
after it — z does not cross the extra ancestor levels, and the preview
renders **under** the central content. The user sees "clicking the tab does
nothing".

The consumer must lift the host while the preview is up:

```qml
Loader {
    id: commandBar
    source: "RibbonCommandBar.qml"          // hosts the RibbonBar
    z: item && item.categoryPreviewUp ? 20 : 0
}
```

Expose `categoryPreviewUp: ribbonBar.minimumModeExpanded` from the wrapper
(and give toolbar fallbacks a constant `false` of the same name, so the
binding is uniform). `RibbonBar` documents this contract next to
`isMinimumModeExpanded()`.

## 9. Frameless windows

`windowAgent: RibbonWindowAgent { … }` wires QWindowKit's Quick route: the
bar becomes the draggable title bar, the system button row renders at the
bar's right edge (`systemButtonStripWidth` is already subtracted from every
right-side reservation), and the DWM frame follows the ribbon theme's
dark/light mode. Windows-specific niceties (snap layout, system shadow) come
with QWK — see its docs.

One interaction to know: QWK's hit test is purely geometric and knows
nothing of overlay stacking. If you pop a QML `Popup` that overlaps the bar
band, every click inside that area is swallowed as a window drag unless you
exempt the popup item: `windowAgent.setHitTestVisible(popupItem, true)` while
open, `false` on close.

## 10. Pitfall checklist (production-scarred)

Each item below is a real defect class, not a hypothetical:

- **P1 — Wrapper height.** If the bar is hosted by a wrapper Item/Loader,
  its `implicitHeight` must be `ribbonBar.implicitHeight`. A wrapper that
  sizes itself differently lets the category row paint past the host and
  overlap the content below — the page "gains a second windowed look".
- **P2 — Collapsed preview under content.** Section 8. Symptom: folded
  ribbon, tab click, nothing appears.
- **P3 — Container-clipped custom panels.** Section 5. Symptom: gallery-ish
  content invisible inside a `RibbonControlContainer`.
- **P4 — Lying checkables.** Section 4. Symptom: toggle flips but never
  tracks the document/cursor state.
- **P5 — Accent misuse.** Section 7. Symptom: tab strip painted in the
  highlight color, ribbon detached from the chrome.
- **P6 — Right group rejects non-buttons.** Section 4. Symptom: search box
  "cannot be assigned" or silently absent.
- **P7 — Clobbering engine geometry.** Manually positioning categories,
  panels, tabs or the application button fights the C++ layout engine
  (metrics come from `RibbonMetrics` and are re-derived on style/theme
  changes). Declare structure only.
- **P8 — Off-line submodule names.** Section 1. Symptom: clone failures
  citing relative URLs.

## 11. Qt 5.15 lane

The module compiles and behaves on 5.15.16 (the validation baseline), but
the consumer side carries extra rules:

- **Versioned library imports are mandatory.** Qt 5's QML compiler rejects
  unversioned imports ("Library import requires a version") — a QML file
  written in Qt 6 style (`import QtQuick`, `import SARibbon 3.0` is already
  fine) fails to load wholesale. Either keep every ribbon-facing file on
  versioned imports (`import QtQuick 2.15`, `import QtQuick.Controls 2.15`,
  …) or run a configure-time rewrite into the build directory (the AgentWorkbench
  project ships a working `awb_internal_qt5_rewritten` implementation in its
  `cmake/` layer, worth copying).
- **`import SARibbon 3.0` itself needs no extra path** — imperative
  registration covers Qt 5, including the enum metatype registrations the
  engine requires there (handled inside `saRibbonRegisterQmlTypes`; without
  them every enum-typed property is an opaque "unregistered datatype" on
  Qt 5).
- General Qt 5 QML traps that keep biting ribbon code: `ObjectModel` does
  not exist (use JS arrays as models); unqualified document-root ids inside
  compiled delegates/handlers throw `ReferenceError` (pass the scope in as a
  property); `QtQuick.Window` must be imported explicitly for `Window`.
- `RibbonCheckBox`, `RibbonRadioButton`, `RibbonComboBox`, `RibbonSpinBox`
  and `RibbonTextField` are QML-file types (registered from `qrc:/SARibbon/`)
  — usable on both versions; prefer them over raw Controls inside panels so
  the padding/height harmonization applies.

## 12. Testing the ribbon from C++

The patterns that work (see a full harness in any consumer's ribbonui test):

- Find the bar through the QObject tree:
  `page->findChild<SARibbonQml::RibbonBar *>()` — the QML leaf items are
  reachable the same way (`RibbonTab`, `RibbonCategory`, `RibbonGallery`).
- Assert geometry, not pixels: `bar->implicitHeight() > bar->categoryRowY()
  + 8` proves the category row laid out; mapping the visible category's
  bottom to scene coordinates proves the collapsed preview overlays the
  content below the command bar.
- Drive real interactions with `QTest::mouseClick` on tab/grip items mapped
  to scene coordinates, and programmatic ones through the invokables
  (`activateItem`, `setMinimumMode`).
- Install a message handler for the whole load; a ribbon page that loads
  with zero QML warnings is the contract. `ReferenceError`s from scope
  mistakes surface exactly here.
