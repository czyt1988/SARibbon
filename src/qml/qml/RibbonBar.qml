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
    readonly property Item appWindowItem: cppHost && cppHost.hasApplicationWindow ? cppHost.applicationWindowItem : null

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

    // application button (widgets: vertically expanding, spans title+tab row;
    // office-2021: transparent, radius 2, hover/pressed 5px bottom underline)
    Item {
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
    // Same lazy-creation rule as the app menu. The user-declared
    // RibbonApplicationWindow rides in as the popup contentItem; its own
    // implicit size drives the popup size. Esc / outside click close
    // (Popup semantics); inner close() routes through the host back here.
    Loader {
        id: appWindowLoader
        active: false
        sourceComponent: appWindowComponent
    }
    Component {
        id: appWindowComponent
        Popup {
            id: appWinPopup
            x: root.appRect.x
            y: root.appRect.y + root.appRect.height
            width: (root.appWindowItem ? Math.max(root.appWindowItem.implicitWidth, root.appWindowItem.width) : 200) + 2
            height: (root.appWindowItem ? Math.max(root.appWindowItem.implicitHeight, root.appWindowItem.height) : 200) + 2
            padding: 1
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
            background: Rectangle {
                color: RibbonTheme.contentBg
                border.color: RibbonTheme.menuBorder
                radius: 4
            }
            contentItem: root.appWindowItem
            onOpened: if (root.appWindowItem) root.appWindowItem.popupVisible = true
            onClosed: if (root.appWindowItem) root.appWindowItem.popupVisible = false
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
            menuModel: root.cppHost ? root.cppHost.applicationMenuItems : []
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
