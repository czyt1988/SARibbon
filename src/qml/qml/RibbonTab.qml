import QtQuick 2.12
import SARibbon 3.0

// RibbonTab default visual leaf (office-2021): no background, no radius; the
// selected tab carries a 4px tab-accent underline, hover a tab-accent-hover
// one (widgets QSS SARibbonTabBar::tab). Click handling stays in the C++ host
// (SARibbonQml::RibbonTab::mousePressEvent); the HoverHandler here only
// drives the underline and never touches mouse events.
Rectangle {
    id: root

    property QtObject tabCpp: null
    onTabCppChanged: if (tabCpp) tabCpp.tabQmlItem = root

    readonly property string label: tabCpp ? tabCpp.text : ""
    readonly property bool current: tabCpp ? tabCpp.current : false

    anchors.fill: parent
    color: "transparent"

    Text {
        // keep the optical center of the text above the 4px underline
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -2
        text: root.label
        color: root.current ? RibbonTheme.tabAccent : RibbonTheme.textColor
    }
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 4
        color: root.current ? RibbonTheme.tabAccent : RibbonTheme.tabAccentHover
        visible: root.current || tabHover.hovered
    }
    HoverHandler {
        id: tabHover
    }
}
