import QtQuick 2.12
import SARibbon 3.0

// RibbonBar default visual leaf: office-2021 flavored. Geometry authority
// stays in the C++ host — the leaf only renders what barCpp publishes
// (categoryRowY / applicationButtonRect) and the accent/content zones.
// Colors via RibbonTheme tokens only (plan-04 S3 theme rule).
Rectangle {
    id: root

    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    readonly property int categoryRowY: cppHost ? cppHost.categoryRowY : 0
    readonly property int titleBarHeight: cppHost ? cppHost.titleBarHeight : 0
    readonly property rect appRect: cppHost && cppHost.applicationButtonRect.width > 0 ? cppHost.applicationButtonRect : Qt.rect(0, 0, 0, 0)
    readonly property var bands: cppHost ? cppHost.contextBands : []

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
            onClicked: if (root.cppHost) root.cppHost.applicationButtonClicked()
        }
    }
}
