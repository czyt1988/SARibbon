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
// zones swallow input. The popup is a theme-token-styled Popup with
// RibbonMenuItem rows; activation goes through the host's activateMenuItem
// invokable (logic stays in the C++ host, rendering here).
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
    readonly property int naturalIconSide: cppHost ? cppHost.iconSide : 0

    // long-press (DelayedPopup) suppresses the click that follows the hold
    property bool heldForMenu: false

    // office-2021 state colors (widgets QSS SARibbonToolButton)
    readonly property color stateBg: mouse.pressed ? RibbonTheme.contentPressedBg
                                     : (mouse.containsMouse ? RibbonTheme.contentHoverBg
                                        : (root.checked ? RibbonTheme.contentPressedBg : RibbonTheme.contentBg))
    readonly property color stateText: (mouse.containsMouse && !root.checked && !mouse.pressed)
                                       ? RibbonTheme.contentPressedBg : RibbonTheme.textColor

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
    border.width: root.checked && !root.split ? 1 : 0
    border.color: RibbonTheme.textColor

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
                  : (root.checked ? RibbonTheme.contentPressedBg : RibbonTheme.contentBg))
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
                  : RibbonTheme.contentBg)
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
        visible: root.naturalIconSide > 0 && root.icon.length > 0
        // the icon is painted at its natural size, centered in the icon box
        // (widgets drawItemPixmap(iconRect, Qt::AlignCenter, pixmap))
        width: root.naturalIconSide
        height: root.naturalIconSide
        x: root.iconBox.x + (root.iconBox.width - width) / 2
        y: root.iconBox.y + (root.iconBox.height - height) / 2
        source: root.icon
        sourceSize.width: root.naturalIconSide
        sourceSize.height: root.naturalIconSide
        fillMode: Image.PreserveAspectFit
        opacity: root.contentOpacity
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

    // ---- styled popup (theme tokens only) ----
    Popup {
        id: popupMenu
        y: root.height
        x: 0
        width: menuColumn.implicitWidth + 2
        height: menuColumn.implicitHeight + 2
        padding: 1
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle {
            color: RibbonTheme.contentBg
            border.color: RibbonTheme.menuBorder
            radius: 4
        }
        contentItem: Column {
            id: menuColumn
            Repeater {
                model: root.cppHost ? root.cppHost.menuItems : []
                Item {
                    // test reachability: rows are findable by objectName
                    objectName: modelData.separator ? "menuSeparator" : "menuRow"
                    width: Math.max(140, rowText.implicitWidth + rowIcon.width + 30)
                    height: modelData.separator ? 9 : 24
                    enabled: modelData.enabled
                    // separator entry
                    Rectangle {
                        visible: modelData.separator
                        anchors.centerIn: parent
                        width: parent.width - 8
                        height: 1
                        color: RibbonTheme.separator
                    }
                    // normal entry
                    Rectangle {
                        visible: !modelData.separator
                        anchors.fill: parent
                        anchors.margins: 1
                        radius: 3
                        color: rowMouse.pressed ? RibbonTheme.contentPressedBg
                               : (rowMouse.containsMouse ? RibbonTheme.contentHoverBg : RibbonTheme.contentBg)
                    }
                    Row {
                        visible: !modelData.separator
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 6
                        spacing: 4
                        Image {
                            id: rowIcon
                            anchors.verticalCenter: parent.verticalCenter
                            width: modelData.iconSource ? 16 : 0
                            height: 16
                            source: modelData.iconSource
                            fillMode: Image.PreserveAspectFit
                            opacity: modelData.enabled ? 1.0 : 0.45
                        }
                        Text {
                            id: rowText
                            anchors.verticalCenter: parent.verticalCenter
                            text: modelData.text
                            color: RibbonTheme.textColor
                            opacity: modelData.enabled ? 1.0 : 0.45
                        }
                    }
                    MouseArea {
                        id: rowMouse
                        visible: !modelData.separator
                        anchors.fill: parent
                        hoverEnabled: true
                        enabled: modelData.enabled
                        onClicked: root.cppHost.activateMenuItem(index)
                    }
                }
            }
        }
        onOpened: if (root.cppHost) root.cppHost.menuVisible = true
        onClosed: if (root.cppHost) root.cppHost.menuVisible = false
    }

    HoverHandler {
        id: hover
        enabled: !root.disabled
    }
    ToolTip.visible: hover.hovered && root.tip.length > 0
    ToolTip.text: root.tip
    ToolTip.delay: 400
}
