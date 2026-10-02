import QtQuick 2.12
import SARibbon 3.0

// Scroll arrow overlay leaf. It is NOT the category's background leaf: panels are
// z=0 siblings of that leaf (z=-1), so arrows drawn there would be covered exactly
// when they are needed. The C++ host creates this item separately and raises it to
// z=1. Geometry authority stays in C++ — the host publishes the rectangles computed
// by the core pure function SARibbon::Core::scrollButtonRects (12px, full height,
// swapped under RTL) and the visibility flags from the engine's Result::scrollFlags.
// The root is a bare Item with no mouse or hover handler of its own, so presses
// fall through to the panels underneath; only the two arrow strips take input.
Item {
    id: root
    objectName: "categoryScrollOverlay"

    property QtObject cppHost: null

    readonly property var flags: cppHost ? cppHost.scrollButtonFlags : null
    readonly property var geo: cppHost ? cppHost.scrollButtonGeometry : null
    readonly property bool leftOn: flags ? flags.left : false
    readonly property bool rightOn: flags ? flags.right : false
    readonly property real btnWidth: geo ? geo.width : 0
    readonly property real btnHeight: geo ? geo.height : 0
    readonly property real btnLeftX: geo ? geo.leftX : 0
    readonly property real btnRightX: geo ? geo.rightX : 0

    anchors.fill: parent

    // ---- left arrow (Qt 5.12 has no inline components, so both are spelled out) ----
    Item {
        id: leftBtn
        objectName: "categoryScrollLeftButton"
        visible: root.leftOn
        x: root.btnLeftX
        y: 0
        width: root.btnWidth
        height: root.btnHeight

        Rectangle {
            anchors.fill: parent
            color: leftMouse.pressed ? RibbonTheme.contentPressedBg
                                     : (leftMouse.containsMouse ? RibbonTheme.contentHoverBg : RibbonTheme.contentBg)
        }
        Rectangle {
            // inner hairline, the side facing the panels
            x: parent.width - 1
            y: 0
            width: 1
            height: parent.height
            color: RibbonTheme.separator
        }
        Canvas {
            width: 5
            height: 9
            anchors.centerIn: parent
            property color arrowColor: RibbonTheme.textColor
            onArrowColorChanged: requestPaint()
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.fillStyle = arrowColor;
                ctx.beginPath();
                ctx.moveTo(width, 0);
                ctx.lineTo(width, height);
                ctx.lineTo(0, height / 2);
                ctx.closePath();
                ctx.fill();
            }
        }
        MouseArea {
            id: leftMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                if (root.cppHost) {
                    root.cppHost.scrollByButton(true);
                }
            }
        }
    }

    // ---- right arrow ----
    Item {
        id: rightBtn
        objectName: "categoryScrollRightButton"
        visible: root.rightOn
        x: root.btnRightX
        y: 0
        width: root.btnWidth
        height: root.btnHeight

        Rectangle {
            anchors.fill: parent
            color: rightMouse.pressed ? RibbonTheme.contentPressedBg
                                      : (rightMouse.containsMouse ? RibbonTheme.contentHoverBg : RibbonTheme.contentBg)
        }
        Rectangle {
            x: 0
            y: 0
            width: 1
            height: parent.height
            color: RibbonTheme.separator
        }
        Canvas {
            width: 5
            height: 9
            anchors.centerIn: parent
            property color arrowColor: RibbonTheme.textColor
            onArrowColorChanged: requestPaint()
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.fillStyle = arrowColor;
                ctx.beginPath();
                ctx.moveTo(0, 0);
                ctx.lineTo(0, height);
                ctx.lineTo(width, height / 2);
                ctx.closePath();
                ctx.fill();
            }
        }
        MouseArea {
            id: rightMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                if (root.cppHost) {
                    root.cppHost.scrollByButton(false);
                }
            }
        }
    }
}
