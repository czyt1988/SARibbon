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
    readonly property bool large: cppHost && cppHost.proportion === Ribbon.Large
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

    // icon sizes (widgets parity: 32 large / 20 small)
    readonly property int iconSide: root.large ? 32 : 20
    // disabled content: opacity carries the grey (no literal colors)
    readonly property real contentOpacity: root.disabled ? 0.45 : 1.0

    // ---- large button: icon centered in the action zone, text + arrow in
    // the bottom strip (the menu zone when split, else the bottom area) ----
    Text {
        id: largeText
        visible: root.large
        x: root.split ? (root.hitMenu.x + 2) : 2
        y: root.split ? root.hitMenu.y : parent.height - height - 1
        width: (root.split ? root.hitMenu.width : parent.width) - (root.split ? (indicator.width + 4) : 2)
        height: root.split ? root.hitMenu.height : implicitHeight
        text: root.label
        wrapMode: Text.WordWrap
        maximumLineCount: 2
        elide: Text.ElideRight
        horizontalAlignment: root.split ? Text.AlignLeft : Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: root.stateText
        opacity: root.contentOpacity
    }
    Image {
        id: largeIcon
        visible: root.large
        x: root.hitAction.x + 2
        y: root.hitAction.y + 2
        width: root.hitAction.width - 4
        height: root.hitAction.height - 4
        source: root.icon
        sourceSize.width: root.iconSide
        sourceSize.height: root.iconSide
        fillMode: Image.PreserveAspectFit
        opacity: root.contentOpacity
    }

    // menu indicator arrow (filled triangle, theme-colored)
    Canvas {
        id: indicator
        property color arrowColor: root.stateText
        onArrowColorChanged: requestPaint()
        visible: root.split && root.large
        width: 8
        height: 5
        x: root.hitMenu.x + root.hitMenu.width - width - 3
        y: root.hitMenu.y + root.hitMenu.height / 2 - height / 2
        opacity: root.contentOpacity
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            ctx.fillStyle = arrowColor;
            ctx.beginPath();
            ctx.moveTo(0, 0);
            ctx.lineTo(width, 0);
            ctx.lineTo(width / 2, height);
            ctx.closePath();
            ctx.fill();
        }
    }

    // ---- small/medium button: icon left, text right, arrow strip trailing ----
    Image {
        id: smallIcon
        visible: !root.large
        x: 1
        anchors.verticalCenter: parent.verticalCenter
        width: root.iconSide
        height: root.iconSide
        source: root.icon
        sourceSize.width: root.iconSide
        sourceSize.height: root.iconSide
        fillMode: Image.PreserveAspectFit
        opacity: root.contentOpacity
    }
    Text {
        id: smallText
        visible: !root.large
        anchors.left: smallIcon.right
        anchors.leftMargin: 2
        anchors.right: root.split ? indicatorSmall.left : parent.right
        anchors.rightMargin: root.split ? 0 : 2
        anchors.verticalCenter: parent.verticalCenter
        text: root.label
        elide: Text.ElideRight
        color: root.stateText
        opacity: root.contentOpacity
    }
    Canvas {
        id: indicatorSmall
        property color arrowColor: root.stateText
        onArrowColorChanged: requestPaint()
        visible: root.split && !root.large
        width: 8
        height: 5
        x: root.hitMenu.x + root.hitMenu.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        opacity: root.contentOpacity
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            ctx.fillStyle = arrowColor;
            ctx.beginPath();
            ctx.moveTo(0, 0);
            ctx.lineTo(width, 0);
            ctx.lineTo(width / 2, height);
            ctx.closePath();
            ctx.fill();
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
