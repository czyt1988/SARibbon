import QtQuick 2.12
import SARibbon 3.0

// RibbonBar default visual leaf: office-2021 flavored. Geometry authority
// stays in the C++ host — the leaf only renders what barCpp publishes
// (categoryRowY / applicationButtonRect) and the accent/content zones.
// Colors via RibbonTheme tokens only (plan-04 S3 theme rule).
Rectangle {
    id: root

    property QtObject barCpp: null
    onBarCppChanged: if (barCpp) barCpp.barQmlItem = root

    readonly property int categoryRowY: barCpp ? barCpp.categoryRowY : 0
    readonly property rect appRect: barCpp && barCpp.applicationButtonRect.width > 0 ? barCpp.applicationButtonRect : Qt.rect(0, 0, 0, 0)

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
            text: root.barCpp ? root.barCpp.applicationLabel : ""
            color: RibbonTheme.textColor
        }
        MouseArea {
            id: appMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: if (root.barCpp) root.barCpp.applicationButtonClicked()
        }
    }
}
