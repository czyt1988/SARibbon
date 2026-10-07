import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0

// RibbonToolButton default visual leaf. Layout mirrors the widgets
// SARibbonToolButton two modes: Large = icon above centered text (bottom,
// two-line wrap), Small/Medium = icon left + elided text right. Popup modes
// follow the host-published hit zones (geometry authority in C++):
// - MenuButtonPopup: action zone (icon) + menu zone (text strip / arrow
//   strip) as two separately highlighted MouseAreas
// - InstantPopup: the whole button opens the styled popup
// - DelayedPopup: the whole button clicks; press-and-hold opens the popup
// States follow the office-2021 QSS (radius 4): normal content-bg, hover
// content-hover-bg, pressed/checked content-pressed-bg (checked adds a 1px
// text-color border). A disabled host greys the content (opacity) and both
// zones swallow input. The popup itself is the shared RibbonMenu leaf: it
// renders RibbonMenuItem rows (check marks, shortcuts, nested submenus) and
// publishes an index path per activation, which this leaf forwards to the
// host's activateMenuItemPath invokable (logic stays in the C++ host,
// rendering in RibbonMenu.qml).
Rectangle {
    id: root

    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    readonly property string label: cppHost ? cppHost.text : ""
    readonly property string icon: cppHost ? cppHost.iconSource : ""
    readonly property string tip: cppHost ? cppHost.toolTip : ""
    // large-type verdict comes from the host (core effectiveButtonType parity);
    // single-dependency binding shape is mandatory (NOTES B48)
    readonly property bool large: cppHost ? cppHost.largeType : false
    readonly property bool wordWrap: cppHost ? cppHost.wordWrap : true
    readonly property bool checked: cppHost ? cppHost.checked : false
    readonly property bool disabled: !cppHost || !cppHost.enabled
    readonly property bool hasMenu: cppHost ? cppHost.hasMenu : false
    readonly property int popupMode: cppHost ? cppHost.popupMode : Ribbon.DelayedPopup
    // two-zone split (MenuButtonPopup with entries); InstantPopup uses the
    // whole button as the menu zone
    readonly property bool split: root.hasMenu && root.popupMode === Ribbon.MenuButtonPopup
    readonly property bool instant: root.hasMenu && root.popupMode === Ribbon.InstantPopup
    readonly property rect hitAction: cppHost ? cppHost.actionRect : Qt.rect(0, 0, 0, 0)
    readonly property rect hitMenu: cppHost ? cppHost.menuRect : Qt.rect(0, 0, 0, 0)
    // ---- host-published draw geometry (core SARibbonToolButtonLayout) ----
    readonly property rect iconBox: cppHost ? cppHost.iconGeometry : Qt.rect(0, 0, 0, 0)
    readonly property rect textBox: cppHost ? cppHost.textGeometry : Qt.rect(0, 0, 0, 0)
    readonly property rect indBox: cppHost ? cppHost.indicatorGeometry : Qt.rect(0, 0, 0, 0)
    readonly property string caption: cppHost ? cppHost.displayText : ""
    // true when the caption box is the two-line top-aligned budget
    readonly property bool wrapCaption: cppHost ? cppHost.textWordWrap : false

    // long-press (DelayedPopup) suppresses the click that follows the hold
    property bool heldForMenu: false

    // title-row rendering context (host-published): the quick access bar and
    // the right button group sit flat on the title row — widgets theme-base
    // QSS `SARibbonButtonGroupWidget > QToolButton` paints them without
    // normal-state background and border, only hover/pressed/checked tint
    readonly property bool flat: cppHost ? cppHost.flat : false

    // office-2021 state colors (widgets QSS SARibbonToolButton): the caption
    // keeps the theme text color in EVERY state — a hover/pressed background
    // is always a light tint of the content background, so recoloring the text
    // with a background token made it invisible on hover
    readonly property color stateBg: mouse.pressed ? RibbonTheme.contentPressedBg
                                     : (mouse.containsMouse ? RibbonTheme.contentHoverBg
                                        : (root.checked ? RibbonTheme.contentPressedBg
                                           : (root.flat ? "transparent" : RibbonTheme.contentBg)))
    readonly property color stateText: RibbonTheme.textColor

    // ---- entry points the C++ host invokes (openMenu/closeMenu) ----
    function openMenu()
    {
        if (!popupMenu.visible) {
            popupMenu.open();
        }
    }
    function closeMenu()
    {
        if (popupMenu.visible) {
            popupMenu.close();
        }
    }

    anchors.fill: parent
    radius: 4
    // whole-button background only when there is no split (single zone)
    color: root.split ? RibbonTheme.contentBg : root.stateBg
    // 1px border on checked or on the keyboard focus (plan-05 S7 baseline:
    // the tab focus must be visible; it reuses the checked border weight)
    border.width: (root.checked && !root.split) || root.activeFocus ? 1 : 0
    border.color: RibbonTheme.textColor

    // ---- plan-05 S7: keyboard + accessibility baseline ----
    // tab focus lands here (the leaf is the visual item; the host stays the
    // layout/logic authority), Space/Enter/Return trigger the host click
    // path — a disabled host swallows the key exactly like a mouse click
    activeFocusOnTab: true
    Keys.onPressed: {
        if ((event.key === Qt.Key_Space || event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
                && cppHost && cppHost.enabled) {
            event.accepted = true;
            cppHost.click();
        }
    }
    // public attached properties (the QQuickAccessibleAttached face is
    // private in C++ on Qt <= 6.7; its QML form is the public route). The
    // subset is kept to the members Qt 5.12 already had — both lanes share
    // this leaf verbatim
    Accessible.role: Accessible.Button
    Accessible.name: root.label
    Accessible.description: root.tip
    Accessible.checked: root.checked
    Accessible.checkable: cppHost ? cppHost.checkable : false

    // ---- split-zone backgrounds (MenuButtonPopup) ----
    Rectangle {
        visible: root.split
        x: root.hitAction.x
        y: root.hitAction.y
        width: root.hitAction.width
        height: root.hitAction.height
        radius: 4
        color: !root.disabled && mouse.pressed ? RibbonTheme.contentPressedBg
               : (!root.disabled && mouse.containsMouse ? RibbonTheme.contentHoverBg
                  : (root.checked ? RibbonTheme.contentPressedBg
                     : (root.flat ? "transparent" : RibbonTheme.contentBg)))
        border.width: root.checked ? 1 : 0
        border.color: RibbonTheme.textColor
    }
    Rectangle {
        visible: root.split
        x: root.hitMenu.x
        y: root.hitMenu.y
        width: root.hitMenu.width
        height: root.hitMenu.height
        radius: 4
        color: !root.disabled && menuMouse.pressed ? RibbonTheme.contentPressedBg
               : (!root.disabled && menuMouse.containsMouse ? RibbonTheme.contentHoverBg
                  : (root.flat ? "transparent" : RibbonTheme.contentBg))
    }

    // disabled content: opacity carries the grey (no literal colors)
    readonly property real contentOpacity: root.disabled ? 0.45 : 1.0

    // ---- content, positioned by the host-published draw rects ----
    // One icon item and one caption item serve both button types: the core
    // layout already decided where they go (large = icon box above the bottom
    // text box, small = icon box left of the text box), so the leaf never
    // re-derives geometry (iron rule: rendering here, layout in core).
    Image {
        id: contentIcon
        visible: root.iconBox.width > 0 && root.iconBox.height > 0 && root.icon.length > 0
        // The icon height IS the host-published icon box height, which the core
        // layout derives from the panel's large-button height (mode precomputed:
        // three-row / two-row / single-row each fix the box, hence the icon),
        // and the width follows the source aspect ratio, clamped to the box
        // width. Every large button of a mode therefore draws the same icon
        // height with a proportionally scaled width, and the text box below /
        // beside it keeps its fixed budget.
        // The natural aspect is captured once from the loaded source (before
        // sourceSize is pinned to the draw size, which would otherwise feed
        // back into itself); until then a square source is assumed.
        property real natW: 0
        property real natH: 0
        readonly property real aspect: (natW > 0 && natH > 0) ? natW / natH : 1
        readonly property real drawW: Math.min(root.iconBox.height * aspect, root.iconBox.width)
        readonly property real drawH: drawW / aspect
        width: drawW
        height: drawH
        x: root.iconBox.x + (root.iconBox.width - width) / 2
        y: root.iconBox.y + (root.iconBox.height - height) / 2
        source: root.icon
        fillMode: Image.PreserveAspectFit
        opacity: root.contentOpacity
        onStatusChanged: {
            if (status === Image.Ready && natW <= 0 && sourceSize.width > 0) {
                natW = sourceSize.width;
                natH = sourceSize.height;
            }
        }
        onWidthChanged: if (natW > 0) sourceSize = Qt.size(Math.max(width, 1), Math.max(height, 1))
        onHeightChanged: if (natW > 0) sourceSize = Qt.size(Math.max(width, 1), Math.max(height, 1))
        Component.onCompleted: if (natW > 0) sourceSize = Qt.size(Math.max(width, 1), Math.max(height, 1))
    }
    Text {
        id: contentText
        visible: root.caption.length > 0 && root.textBox.width > 0
        x: root.textBox.x
        y: root.textBox.y
        width: root.textBox.width
        height: root.textBox.height
        text: root.caption
        color: root.stateText
        opacity: root.contentOpacity
        // wrapped large caption: two-line budget, top aligned, centered
        // (widgets TextWordWrap | AlignTop | AlignHCenter); everything else is
        // a single centered line already elided by the host (AlignCenter)
        wrapMode: root.wrapCaption ? Text.WordWrap : Text.NoWrap
        maximumLineCount: root.wrapCaption ? 2 : 1
        elide: root.wrapCaption ? Text.ElideNone : Text.ElideRight
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: root.wrapCaption ? Text.AlignTop : Text.AlignVCenter
    }

    // menu indicator arrow: the widgets proxy style polyline (a stroked,
    // bottom-pointing chevron centered in the indicator rect), not a filled
    // triangle — both front ends must draw the same glyph
    Canvas {
        id: indicator
        property color arrowColor: root.stateText
        onArrowColorChanged: requestPaint()
        visible: root.indBox.width > 1 && root.indBox.height > 1
        x: root.indBox.x
        y: root.indBox.y
        width: root.indBox.width
        height: root.indBox.height
        opacity: root.contentOpacity
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            if (width <= 1 || height <= 1) {
                return;
            }
            var size = Math.min(width, height);
            var border = Math.floor(size / 4);
            var sqsize = 2 * Math.floor(size / 2);
            var ax = [border, sqsize / 2, sqsize - border];
            var ay = [sqsize / 2, sqsize - border, sqsize / 2];
            var minX = Math.min(ax[0], ax[1], ax[2]);
            var maxX = Math.max(ax[0], ax[1], ax[2]);
            var minY = Math.min(ay[0], ay[1], ay[2]);
            var maxY = Math.max(ay[0], ay[1], ay[2]);
            // QPolygon::boundingRect center (integer division like QRect)
            var cx = minX + Math.floor((maxX - minX + 1) / 2);
            var cy = minY + Math.floor((maxY - minY + 1) / 2);
            var sx = sqsize / 2 - cx - 1;
            var sy = sqsize / 2 - cy - 1;
            ctx.translate((width - size) / 2 + sx, (height - size) / 2 + sy);
            ctx.strokeStyle = arrowColor;
            ctx.lineWidth = 1.4;
            ctx.beginPath();
            ctx.moveTo(ax[0], ay[0]);
            ctx.lineTo(ax[1], ay[1]);
            ctx.lineTo(ax[2], ay[2]);
            ctx.stroke();
        }
    }

    // ---- interaction zones ----
    MouseArea {
        id: mouse
        // single-zone mode: the whole button (split mode keeps this area on
        // the action zone so hover/press states keep working)
        x: root.split ? root.hitAction.x : 0
        y: root.split ? root.hitAction.y : 0
        width: root.split ? root.hitAction.width : parent.width
        height: root.split ? root.hitAction.height : parent.height
        hoverEnabled: true
        enabled: !root.disabled
        onPressAndHold: {
            if (root.hasMenu && root.popupMode === Ribbon.DelayedPopup) {
                root.heldForMenu = true;
                root.cppHost.openMenu();
            }
        }
        onClicked: {
            if (root.instant) {
                root.cppHost.openMenu();
                return;
            }
            if (root.heldForMenu) {
                root.heldForMenu = false;
                return;
            }
            root.cppHost.click();
        }
    }
    MouseArea {
        id: menuMouse
        visible: root.split
        x: root.hitMenu.x
        y: root.hitMenu.y
        width: root.hitMenu.width
        height: root.hitMenu.height
        hoverEnabled: true
        enabled: !root.disabled
        onClicked: root.cppHost.openMenu()
    }

    // ---- styled popup (shared RibbonMenu leaf, theme tokens only) ----
    // The menu rendering lives in RibbonMenu.qml so the bar application menu
    // and every nested submenu look identical; this leaf keeps the open/close
    // entry points the C++ host invokes and mirrors the visibility back so
    // menuVisible stays the single source of truth for tests.
    RibbonMenu {
        id: popupMenu
        x: 0
        y: root.height
        menuModel: root.cppHost ? root.cppHost.menuItems : []
        namePrefix: "menu"
        rowHeight: 24
        minRowWidth: 140
        // the path arrives as a var signal parameter, which Qt does not inject
        // as a named handler argument — read it positionally instead
        onItemActivated: root.cppHost.activateMenuItemPath(arguments[0])
        onOpened: if (root.cppHost) root.cppHost.menuVisible = true
        onClosed: if (root.cppHost) root.cppHost.menuVisible = false
    }

    HoverHandler {
        id: hover
        enabled: !root.disabled
    }
    // toolbar parity: an icon-only button that hid its caption surfaces the
    // caption through the tooltip when the user set none (a QToolBar shows
    // the QAction text the same way); the fallback only fires while the
    // caption is genuinely not rendered (empty text box)
    readonly property bool captionHidden: root.textBox.width <= 0 && root.label.length > 0
    ToolTip.visible: hover.hovered && (root.tip.length > 0 || root.captionHidden)
    ToolTip.text: root.tip.length > 0 ? root.tip : root.label
    ToolTip.delay: 400
}
