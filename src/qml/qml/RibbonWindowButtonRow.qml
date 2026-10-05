import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import SARibbon 3.0

// Frameless system button row (widgets SARibbonSystemButtonBar visual parity).
// Rendered by the RibbonBar leaf's right edge: three flat glyph buttons with
// the office-2021 hover shapes (sys-button-hover/pressed tokens; the close
// button paints the close-bg red family). Clicks drive the attached window
// through ordinary Qt mouse events (client-area semantics, widgets default).
// Set registerSystemButtons to also register each button with the QWK agent
// as a native system button — required for the Windows 11 Snap Layout flyout,
// but it moves the buttons into the non-client area (hover/click then route
// through the native window manager, not Qt), which mirrors the widgets
// SARIBBON_ENABLE_SNAP_LAYOUT gate.
// NOTE: no `component` inline types here — Qt 5.12 has no inline components;
// the three buttons repeat the small template instead.
Row {
    id: root

    // injected by RibbonBar.qml: the C++ RibbonBar host
    property QtObject cppHost: null

    // also register the buttons with QWK as native system buttons (Snap
    // Layout); off by default, matching the widgets snap-layout gate
    property bool registerSystemButtons: false

    // window state drives the maximize glyph (restore vs maximize)
    readonly property QtObject attachedWindow: cppHost && cppHost.framelessActive && cppHost.windowAgent
                                              ? cppHost.windowAgent.window : null
    readonly property bool windowMaximized: {
        if (!attachedWindow) {
            return false;
        }
        return (attachedWindow.visibility === Window.Maximized)
               || (attachedWindow.visibility === Window.FullScreen);
    }

    spacing: 0

    Rectangle {
        id: minButton
        width: 30
        height: root.height
        color: mouseAreaMin.pressed ? RibbonTheme.sysButtonPressed
               : (mouseAreaMin.containsMouse ? RibbonTheme.sysButtonHover : "transparent")
        Canvas {
            // minimize glyph: a single bottom bar
            anchors.centerIn: parent
            width: 10
            height: 10
            Connections {
                target: RibbonTheme
                onPaletteChanged: parent.requestPaint()
            }
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.fillStyle = RibbonTheme.textColor;
                ctx.fillRect(0, 8, 10, 1.4);
            }
        }
        MouseArea {
            id: mouseAreaMin
            anchors.fill: parent
            hoverEnabled: true
            onClicked: if (root.attachedWindow) root.attachedWindow.showMinimized()
        }
        Component.onCompleted: if (root.registerSystemButtons && cppHost) cppHost.setSystemButton("minimize", minButton)
    }

    Rectangle {
        id: maxButton
        width: 30
        height: root.height
        color: mouseAreaMax.pressed ? RibbonTheme.sysButtonPressed
               : (mouseAreaMax.containsMouse ? RibbonTheme.sysButtonHover : "transparent")
        Canvas {
            // maximize glyph: plain square, or two offset rectangles when the
            // window is maximized (restore glyph)
            anchors.centerIn: parent
            width: 10
            height: 10
            property bool maximized: root.windowMaximized
            onMaximizedChanged: requestPaint()
            Connections {
                target: RibbonTheme
                onPaletteChanged: parent.requestPaint()
            }
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = RibbonTheme.textColor;
                if (maximized) {
                    // restore: back square outline + front filled square
                    ctx.strokeRect(2.5, 0.5, 7, 7);
                    ctx.fillRect(0.5, 2.5, 7, 7);
                    ctx.strokeRect(0.5, 2.5, 7, 7);
                } else {
                    ctx.strokeRect(0.5, 0.5, 9, 9);
                }
            }
        }
        MouseArea {
            id: mouseAreaMax
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                if (!root.attachedWindow) {
                    return;
                }
                if (root.windowMaximized) {
                    root.attachedWindow.showNormal();
                } else {
                    root.attachedWindow.showMaximized();
                }
            }
        }
        Component.onCompleted: if (root.registerSystemButtons && cppHost) cppHost.setSystemButton("maximize", maxButton)
    }

    Rectangle {
        id: closeButton
        width: 40
        height: root.height
        color: mouseAreaClose.pressed ? RibbonTheme.tokenColor("close-bg-pressed")
               : (mouseAreaClose.containsMouse ? RibbonTheme.tokenColor("close-bg") : "transparent")
        Canvas {
            // close glyph: an X stroke (white over the red hover background)
            anchors.centerIn: parent
            width: 10
            height: 10
            property bool hovered: mouseAreaClose.containsMouse
            onHoveredChanged: requestPaint()
            Connections {
                target: RibbonTheme
                onPaletteChanged: parent.requestPaint()
            }
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = hovered ? "#ffffff" : RibbonTheme.textColor;
                ctx.lineWidth = 1.2;
                ctx.beginPath();
                ctx.moveTo(0.5, 0.5);
                ctx.lineTo(9.5, 9.5);
                ctx.moveTo(9.5, 0.5);
                ctx.lineTo(0.5, 9.5);
                ctx.stroke();
            }
        }
        MouseArea {
            id: mouseAreaClose
            anchors.fill: parent
            hoverEnabled: true
            onClicked: if (root.attachedWindow) root.attachedWindow.close()
        }
        Component.onCompleted: if (root.registerSystemButtons && cppHost) cppHost.setSystemButton("close", closeButton)
    }
}
