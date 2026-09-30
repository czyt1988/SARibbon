import QtQuick 2.15

// RibbonTab default visual leaf (plan-04 S4)
Rectangle {
    id: root

    property QtObject tabCpp: null
    onTabCppChanged: if (tabCpp) tabCpp.tabQmlItem = root

    readonly property string label: tabCpp ? tabCpp.text : ""

    color: "transparent"
    radius: 3

    Text {
        anchors.centerIn: parent
        text: root.label
    }
}
