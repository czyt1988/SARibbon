import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0

// RibbonBar default visual leaf: office-2021 flavored. Geometry authority
// stays in the C++ host — the leaf only renders what barCpp publishes
// (categoryRowY / applicationButtonRect) and the accent/content zones.
// Colors via RibbonTheme tokens only (plan-04 S3 theme rule).
Rectangle {
    id: root

    property QtObject cppHost: null
    onCppHostChanged: {
        if (cppHost) {
            cppHost.qmlLeaf = root;
            // hand the host to the bottom border once (the border's own
            // Component.onCompleted runs before this handshake injects cppHost)
            bottomBorder.host = cppHost;
        }
    }

    readonly property int categoryRowY: cppHost ? cppHost.categoryRowY : 0
    readonly property int titleBarHeight: cppHost ? cppHost.titleBarHeight : 0
    readonly property rect appRect: cppHost && cppHost.applicationButtonRect.width > 0 ? cppHost.applicationButtonRect : Qt.rect(0, 0, 0, 0)
    readonly property var bands: cppHost ? cppHost.contextBands : []
    readonly property bool hasAppMenu: cppHost ? cppHost.hasApplicationMenu : false
    readonly property bool hasAppWindow: cppHost ? cppHost.hasApplicationWindow : false
    readonly property Item appWindowItem: cppHost ? cppHost.applicationWindowItem : null
    // frameless state mirrors (published by the C++ host on agent changes)
    readonly property bool framelessActive: cppHost ? cppHost.framelessActive : false
    readonly property string windowTitle: cppHost ? cppHost.windowTitle : ""
    readonly property int systemStripWidth: cppHost ? cppHost.systemButtonStripWidth : 0
    readonly property rect titleRect: {
        if (cppHost) {
            var tr = cppHost.titleRect;
            if (tr !== undefined && tr.width > 0) {
                return tr;
            }
        }
        return Qt.rect(0, 0, 0, 0);
    }

    // ---- entry points (app menu / app window) ----
    function openAppMenu()
    {
        if (!appMenuLoader.active) {
            appMenuLoader.active = true;  // lazy creation (see note below)
        }
        if (appMenuLoader.item && !appMenuLoader.item.visible) {
            appMenuLoader.item.open();
        }
    }
    function openApplicationWindow()
    {
        if (!appWindowLoader.active) {
            appWindowLoader.active = true;  // lazy creation (popup-in-leaf rule)
        }
        if (appWindowLoader.item) {
            appWindowLoader.item.open();
        }
    }
    function closeApplicationWindow()
    {
        if (appWindowLoader.item) {
            appWindowLoader.item.close();
        }
    }

    anchors.fill: parent
    color: RibbonTheme.accent

    // category content zone below the tab row (office-2021: content-bg)
    Rectangle {
        x: 0
        y: root.categoryRowY
        width: parent.width
        height: Math.max(parent.height - root.categoryRowY, 0)
        color: RibbonTheme.contentBg
    }

    // bottom edge line: the ribbon is a fixed-height band (the host publishes
    // its implicit height from the mode metrics), and without a visible lower
    // border the content zone melts into the window background below it.
    // Leaves ride at z=-1 (background layer), so a line kept inside this leaf
    // would be overpainted by the category / panel leaves that reach exactly
    // to the bar's bottom edge — the line therefore hangs on the host itself,
    // one layer above the structural children.
    // Bindings deliberately reference the C++ host only (never this leaf):
    // the line outlives the leaf root when the bar tears down, and a binding
    // into the dying leaf root is exactly the teardown crash family NOTES B44
    // recorded for this module.
    Rectangle {
        id: bottomBorder
        // injected by the leaf root's onCppHostChanged (this item's own
        // Component.onCompleted runs before the handshake); from then on no
        // binding references the leaf root, so the line survives the leaf's
        // teardown untouched (NOTES B44 crash family)
        property QtObject host: null
        parent: host
        z: 1
        x: 0
        y: host ? Math.max(host.height - 1, 0) : 0
        width: host ? host.width : 0
        height: 1
        visible: host !== null
        color: RibbonTheme.borderColor
    }

    // context category bands: span their tabs from the bar top through the
    // tab row (widgets paintContextCategoryTab parity: base color + 5px
    // vibrant highlight + centered title). This leaf sits at z=-1, so the
    // structural tab items render on top of the band.
    Repeater {
        model: root.bands
        Item {
            x: modelData.x
            y: 0
            width: modelData.width
            height: Math.max(root.categoryRowY - 1, 0)
            Rectangle {
                anchors.fill: parent
                color: modelData.color
            }
            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 5
                color: modelData.highlight
            }
            Text {
                x: 0
                y: 5
                width: parent.width
                height: Math.max(root.titleBarHeight - 5, 0)
                text: modelData.title
                visible: text.length > 0 && height > 0
                color: modelData.textColor
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    // window title (widgets SARibbonBar::paintWindowTitle parity): painted
    // into the core-engine title free rect, center-aligned by default, with
    // the theme text color. Only rendered under the frameless decoration —
    // the native frame carries its own caption otherwise.
    Text {
        visible: root.framelessActive && root.titleRect.width > 0
                 && root.windowTitle.length > 0
        x: root.titleRect.x
        y: root.titleRect.y
        width: root.titleRect.width
        height: root.titleRect.height
        text: root.windowTitle
        color: RibbonTheme.textColor
        elide: Text.ElideRight
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font: RibbonMetrics.font
    }

    // frameless system button row (widgets SARibbonSystemButtonBar parity):
    // pinned to the bar's top-right corner inside the reserved strip; the
    // C++ host already subtracts systemButtonStripWidth from every right
    // host's anchor, so this row never overlaps the right button group
    Loader {
        id: sysButtonRowLoader
        objectName: "sysButtonRow"
        active: root.framelessActive
        source: "qrc:/SARibbon/RibbonWindowButtonRow.qml"
        anchors.top: parent.top
        anchors.right: parent.right
        height: root.titleBarHeight
        width: root.systemStripWidth
        onLoaded: {
            item.cppHost = Qt.binding(function() { return root.cppHost; });
        }
    }

    // application button (widgets: vertically expanding, spans title+tab row;
    // office-2021: transparent, radius 2, hover/pressed 5px bottom underline)
    Item {
        objectName: "appButtonArea"
        visible: root.appRect.width > 0
        x: root.appRect.x
        y: root.appRect.y
        width: root.appRect.width
        height: root.appRect.height

        Rectangle {
            id: appBackground
            anchors.fill: parent
            radius: 2
            color: appMouse.pressed ? RibbonTheme.accentPressed
                   : (appMouse.containsMouse ? RibbonTheme.accentHover : "transparent")
        }
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 5
            visible: appMouse.containsMouse || appMouse.pressed
            color: appMouse.pressed ? RibbonTheme.accent : RibbonTheme.accentHover
        }
        Text {
            anchors.centerIn: parent
            text: root.cppHost ? root.cppHost.applicationLabel : ""
            color: RibbonTheme.textColor
        }
        MouseArea {
            id: appMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                // widgets ApplicationWidget mode first (the example default),
                // then the menu mode (USE_APPLICATION_NORMAL_MENU); the
                // clicked signal always fires
                if (root.hasAppWindow) {
                    root.openApplicationWindow();
                } else if (root.hasAppMenu) {
                    root.openAppMenu();
                }
                if (root.cppHost) {
                    root.cppHost.applicationButtonClicked();
                }
            }
        }
    }

    // ---- application menu (styled popup over theme tokens) ----
    // NOTE: the popup is created LAZILY on first open. A Popup instantiated
    // eagerly inside a bar leaf (during the bar's componentComplete, before
    // the scene window is realized) becomes a standalone native window and
    // breaks the main window's scene — observed as a blank 1316x499 main
    // window plus a stray 336x179 popup window (round 4 bisect).
    Loader {
        id: appMenuLoader
        active: false
        sourceComponent: appMenuComponent
    }

    // ---- application window (widgets ApplicationWidget mode) ----
    // Same lazy-creation rule as the app menu. The presentation is an
    // office-backstage overlay: a popup covering the window (width follows
    // the declared coverageRatio — 1.0 fullscreen, 2/3, custom; always full
    // height) with a selectable slide/fade enter/exit animation, closing on
    // Esc, outside press and the top-right cross. The user-declared
    // RibbonApplicationWindow rides in as a child of the popup's content
    // item — reparented explicitly in aboutToShow, because assigning
    // Popup.contentItem directly does NOT adopt an item that already has a
    // visual parent (QQuickControl only reparents parentless items, so the
    // content kept rendering inside the bar with no background behind it —
    // the no-background bug this presentation fixes).
    Loader {
        id: appWindowLoader
        active: false
        sourceComponent: appWindowComponent
    }
    Component {
        id: appWindowComponent
        Popup {
            id: appWinPopup
            objectName: "appWinPopup"

            // declared window content + presentation knobs (leaf-side
            // mirrors of the RibbonApplicationWindow properties)
            readonly property Item appWin: root.appWindowItem
            readonly property real winWidth: Overlay.overlay ? Overlay.overlay.width : 0
            readonly property real winHeight: Overlay.overlay ? Overlay.overlay.height : 0
            readonly property real coverage: appWin ? appWin.coverageRatio : 1.0
            readonly property int animEffect: appWin ? appWin.animation : RibbonApplicationWindow.NoAnimation
            readonly property int animDuration: appWin ? appWin.animationDuration : 250

            // window coordinates: parented to the window overlay so x/y and
            // the coverage sizing address the whole window, not the bar band
            parent: Overlay.overlay
            x: 0
            y: 0
            width: Math.max(winWidth * coverage, 1)
            height: winHeight
            padding: 0
            focus: true
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

            // ---- enter/exit animations, selected by the declared effect ----
            // the PropertyAction keeps the slides fully opaque even when the
            // previous session ended on a fade (Qt < 5.15.3 does not restore
            // opacity/scale after the exit transition)
            property Transition enterSlideLeft: Transition {
                PropertyAction { property: "opacity"; value: 1.0 }
                NumberAnimation { property: "x"; from: -appWinPopup.width; to: 0;
                                  duration: appWinPopup.animDuration; easing.type: Easing.OutCubic }
            }
            property Transition exitSlideLeft: Transition {
                NumberAnimation { property: "x"; to: -appWinPopup.width;
                                  duration: appWinPopup.animDuration; easing.type: Easing.InCubic }
            }
            property Transition enterSlideRight: Transition {
                PropertyAction { property: "opacity"; value: 1.0 }
                NumberAnimation { property: "x"; from: appWinPopup.winWidth; to: 0;
                                  duration: appWinPopup.animDuration; easing.type: Easing.OutCubic }
            }
            property Transition exitSlideRight: Transition {
                NumberAnimation { property: "x"; to: appWinPopup.winWidth;
                                  duration: appWinPopup.animDuration; easing.type: Easing.InCubic }
            }
            property Transition enterFade: Transition {
                NumberAnimation { property: "opacity"; from: 0.0; to: 1.0;
                                  duration: appWinPopup.animDuration; easing.type: Easing.OutQuad }
            }
            property Transition exitFade: Transition {
                NumberAnimation { property: "opacity"; from: 1.0; to: 0.0;
                                  duration: appWinPopup.animDuration; easing.type: Easing.InQuad }
            }

            enter: animEffect === RibbonApplicationWindow.SlideFromLeft ? enterSlideLeft
                 : animEffect === RibbonApplicationWindow.SlideFromRight ? enterSlideRight
                 : animEffect === RibbonApplicationWindow.Fade ? enterFade
                 : null
            exit: animEffect === RibbonApplicationWindow.SlideFromLeft ? exitSlideLeft
                : animEffect === RibbonApplicationWindow.SlideFromRight ? exitSlideRight
                : animEffect === RibbonApplicationWindow.Fade ? exitFade
                : null

            // backstage panel chrome: theme content background; a separating
            // right edge when the coverage leaves part of the window visible
            // (fullscreen coverage is flush with the window border)
            background: Rectangle {
                color: RibbonTheme.contentBg
                Rectangle {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    width: 1
                    visible: appWinPopup.coverage < 1.0
                    color: RibbonTheme.borderColor
                }
            }

            // declared-content host; the close cross rides above the content
            // through z (fullscreen coverage has no outside area to click,
            // so the cross is the mandated exit next to Esc)
            contentItem: Item {
                id: appWinContentHost
                objectName: "appWinContentHost"

                Rectangle {
                    id: appWinCloseButton
                    objectName: "appWinCloseButton"
                    z: 1000
                    width: 40
                    height: root.titleBarHeight
                    anchors.top: parent.top
                    anchors.right: parent.right
                    visible: appWin ? appWin.showCloseButton : false
                    color: appWinCloseMouse.pressed ? RibbonTheme.sysButtonPressed
                           : (appWinCloseMouse.containsMouse ? RibbonTheme.sysButtonHover : "transparent")
                    Text {
                        anchors.centerIn: parent
                        text: "\u2715"
                        color: RibbonTheme.textColor
                        font: RibbonMetrics.font
                    }
                    MouseArea {
                        id: appWinCloseMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: appWinPopup.close()
                    }
                }
            }

            onAboutToShow: {
                // The exit animations drag the popup item beyond the window
                // edge, and the popup positioner syncs that back into the
                // POPUP's own x/y (its authoritative origin: reposition
                // rebuilds the item geometry from it). Reset the POPUP
                // coordinates here — resetting only the item's x is undone
                // by the very next reposition. Without this, a fade or
                // no-animation reopen parks the panel at the leftover
                // offscreen origin (slides masked it: their `to: 0` x
                // animation pulled the panel back by itself)
                appWinPopup.x = 0;
                appWinPopup.y = 0;
                // fade exits can leave the item transparent on Qt < 5.15.3
                // (no opacity/scale restore after exit transitions there)
                if (appWinContentHost.parent) {
                    appWinContentHost.parent.opacity = 1.0;
                }
                // the bar is the frameless draggable title bar and QWK's hit
                // test is purely geometric (it knows nothing of the overlay
                // stacking): without this exemption every click inside the
                // popup area that overlaps the bar band is swallowed as a
                // window drag — the close cross went dead at partial
                // coverage exactly this way (at full coverage it only kept
                // working because it happened to sit inside the registered
                // system-button strip)
                var agent = root.cppHost ? root.cppHost.windowAgent : null;
                if (agent && appWinContentHost.parent) {
                    agent.setHitTestVisible(appWinContentHost.parent, true);
                }
                // adopt the declared item explicitly (a direct contentItem
                // assignment cannot — see the note above); the content fills
                // the host so the coverage geometry drives its size
                if (root.appWindowItem) {
                    root.appWindowItem.parent = appWinContentHost;
                    root.appWindowItem.anchors.fill = appWinContentHost;
                    root.appWindowItem.popupVisible = true;
                }
            }
            onClosed: {
                // hand the covered area back to the title-bar drag
                var agent = root.cppHost ? root.cppHost.windowAgent : null;
                if (agent && appWinContentHost.parent) {
                    agent.setHitTestVisible(appWinContentHost.parent, false);
                }
                if (root.appWindowItem) {
                    root.appWindowItem.popupVisible = false;
                }
            }
            Component.onDestruction: {
                // the declared item is a QObject child of the bar host and
                // outlives this popup: detach it instead of leaving a visual
                // parent and anchors pointing into the dying content host
                if (root.appWindowItem && root.appWindowItem.parent === appWinContentHost) {
                    root.appWindowItem.anchors.fill = null;
                    root.appWindowItem.parent = null;
                }
            }
        }
    }
    // ---- application menu (shared RibbonMenu leaf) ----
    // Same rendering as the button popup; rows are a touch taller/wider to
    // match the widgets application menu. The popup stays inside the lazy
    // Loader and closes itself after an activation reaches the host.
    Component {
        id: appMenuComponent
        RibbonMenu {
            id: appMenu
            x: root.appRect.x
            y: root.appRect.y + root.appRect.height
            menuModel: root.cppHost ? root.cppHost.applicationMenuModel : []
            namePrefix: "appMenu"
            rowHeight: 26
            minRowWidth: 160
            onItemActivated: {
                // positional read: a var signal parameter is not injected as a
                // named handler argument
                root.cppHost.activateApplicationMenuItemPath(arguments[0]);
                appMenu.close();
            }
        }
    }
}
