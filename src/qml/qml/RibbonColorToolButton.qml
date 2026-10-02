import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0

// RibbonColorToolButton default visual leaf. Everything structural is the
// RibbonToolButton leaf: the same host-published draw rects, the same two-zone
// MenuButtonPopup split, the same office-2021 state colors, the same chevron
// indicator. The only addition is the swatch, and its rectangle is not derived
// here — the host publishes colorRect from core SA::calcColorUnderIconMetrics
// (ColorUnderIcon) or from the natural icon box inset by the widgets generated
// icon's 1-in-32 border (ColorFillToIcon). An invalid color swaps the filled
// rectangle for the shared RibbonColorNoneMark, so the "no color" glyph is the
// very one the color menu row draws and the slash inset is the one core
// SA::noneColorSlashLine produces.
// There is no RibbonMenu here: the popup is a RibbonColorMenu host the C++ side
// owns, so the leaf's job ends at forwarding the click to cppHost.openMenu().
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
    readonly property bool split: root.hasMenu && root.popupMode === Ribbon.MenuButtonPopup
    readonly property bool instant: root.hasMenu && root.popupMode === Ribbon.InstantPopup
    readonly property rect hitAction: cppHost ? cppHost.actionRect : Qt.rect(0, 0, 0, 0)
    readonly property rect hitMenu: cppHost ? cppHost.menuRect : Qt.rect(0, 0, 0, 0)
    // ---- host-published draw geometry (core SARibbonToolButtonLayout) ----
    readonly property rect iconBox: cppHost ? cppHost.iconGeometry : Qt.rect(0, 0, 0, 0)
    readonly property rect textBox: cppHost ? cppHost.textGeometry : Qt.rect(0, 0, 0, 0)
    readonly property rect indBox: cppHost ? cppHost.indicatorGeometry : Qt.rect(0, 0, 0, 0)
    readonly property string caption: cppHost ? cppHost.displayText : ""
    readonly property bool wrapCaption: cppHost ? cppHost.textWordWrap : false
    readonly property int naturalIconSide: cppHost ? cppHost.iconSide : 0
    // ---- host-published color geometry (core SA color helpers) ----
    readonly property color fillColor: cppHost ? cppHost.color : "transparent"
    readonly property bool hasColor: cppHost ? cppHost.hasValidColor : false
    readonly property rect colorBox: cppHost ? cppHost.colorRect : Qt.rect(0, 0, 0, 0)
    // empty in ColorFillToIcon and without an iconSource: the swatch takes the
    // icon's place in the first case, there is nothing to draw in the second
    readonly property rect iconDrawBox: cppHost ? cppHost.iconDrawRect : Qt.rect(0, 0, 0, 0)
    readonly property int slashInset: cppHost ? cppHost.colorSlashInset : 0

    // long-press (DelayedPopup) suppresses the click that follows the hold
    property bool heldForMenu: false

    // office-2021 state colors (widgets QSS SARibbonToolButton)
    readonly property color stateBg: mouse.pressed ? RibbonTheme.contentPressedBg
                                     : (mouse.containsMouse ? RibbonTheme.contentHoverBg
                                        : (root.checked ? RibbonTheme.contentPressedBg : RibbonTheme.contentBg))
    readonly property color stateText: (mouse.containsMouse && !root.checked && !mouse.pressed)
                                       ? RibbonTheme.contentPressedBg : RibbonTheme.textColor

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

    // ---- icon: only in ColorUnderIcon, only into the host's icon draw rect ----
    // widgets renders QIcon into the reduced icon area with the aspect ratio
    // preserved, hence PreserveAspectFit inside iconDrawBox rather than the
    // natural-size centering the plain tool button leaf uses.
    Image {
        id: contentIcon
        visible: root.iconDrawBox.width > 0 && root.iconDrawBox.height > 0 && root.icon.length > 0
        x: root.iconDrawBox.x
        y: root.iconDrawBox.y
        width: root.iconDrawBox.width
        height: root.iconDrawBox.height
        source: root.icon
        sourceSize.width: root.naturalIconSide
        sourceSize.height: root.naturalIconSide
        fillMode: Image.PreserveAspectFit
        opacity: root.contentOpacity
    }

    // ---- the swatch: a filled rect for a valid color, the shared none mark
    // otherwise. Both sit exactly on the host-published colorRect, so the two
    // rendering styles differ only in which rect the host handed down.
    Rectangle {
        id: colorBand
        visible: root.hasColor && root.colorBox.width > 0 && root.colorBox.height > 0
        x: root.colorBox.x
        y: root.colorBox.y
        width: root.colorBox.width
        height: root.colorBox.height
        color: root.fillColor
        opacity: root.contentOpacity
    }
    RibbonColorNoneMark {
        id: noneMark
        visible: !root.hasColor && root.colorBox.width > 0 && root.colorBox.height > 0
        x: root.colorBox.x
        y: root.colorBox.y
        width: root.colorBox.width
        height: root.colorBox.height
        slashInset: root.slashInset
        markMargin: 0
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
        wrapMode: root.wrapCaption ? Text.WordWrap : Text.NoWrap
        maximumLineCount: root.wrapCaption ? 2 : 1
        elide: root.wrapCaption ? Text.ElideNone : Text.ElideRight
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: root.wrapCaption ? Text.AlignTop : Text.AlignVCenter
    }

    // menu indicator arrow: the widgets proxy style polyline, identical to the
    // plain tool button leaf so both front ends draw the same glyph
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

    HoverHandler {
        id: hover
        enabled: !root.disabled
    }
    ToolTip.visible: hover.hovered && root.tip.length > 0
    ToolTip.text: root.tip
    ToolTip.delay: 400
}
