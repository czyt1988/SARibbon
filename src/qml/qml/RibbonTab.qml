import QtQuick 2.12
import SARibbon 3.0

// RibbonTab default visual leaf (office-2021): no background, no radius; the
// selected tab carries a 4px tab-accent underline, hover a tab-accent-hover
// one (widgets QSS SARibbonTabBar::tab). Click handling stays in the C++ host
// (SARibbonQml::RibbonTab::mousePressEvent); the HoverHandler here only
// drives the underline and never touches mouse events.
Rectangle {
    id: root

    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    readonly property string label: cppHost ? cppHost.text : ""
    readonly property bool current: cppHost ? cppHost.current : false

    anchors.fill: parent
    color: "transparent"

    // widgets parity: context tabs keep the SAME text/underline colors as
    // normal tabs (QSS SARibbonTabBar::tab uses text-color/tab-accent for
    // every tab). Painting the text in the context color would drop it
    // invisibly onto the same-colored band behind the tab row; the context
    // identity is carried by the band itself, which shows through the
    // transparent tab background.
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
