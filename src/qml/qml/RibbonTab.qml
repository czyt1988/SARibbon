import QtQuick 2.15
import SARibbon 3.0

// RibbonTab default visual leaf (plan-04 S4): colors via RibbonTheme tokens
// only (no literals, plan-04 S3 theme rule); geometry from the host.
Rectangle {
    id: root

    property QtObject tabCpp: null
    onTabCppChanged: if (tabCpp) tabCpp.tabQmlItem = root

    readonly property string label: tabCpp ? tabCpp.text : ""
    readonly property bool current: tabCpp ? tabCpp.current : false

    anchors.fill: parent
    color: "transparent"
    radius: 3

    Text {
        anchors.centerIn: parent
        text: root.label
        font.bold: root.current
        color: root.current ? RibbonTheme.tokenColor("accent") : RibbonTheme.tokenColor("text-color")
    }
}
