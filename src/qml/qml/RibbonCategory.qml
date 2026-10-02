import QtQuick 2.12
import SARibbon 3.0

// RibbonCategory default visual leaf: the bar's content zone shows through;
// panel separators are rendered at the x positions the C++ host publishes
// (engine-written resultSeparatorGeometry — geometry authority stays in C++).
// QSS parity: separator color + margin-top/bottom 3px.
// This leaf is the BACKGROUND layer (z=-1). The scroll arrows live in a second
// leaf, RibbonCategoryScroll.qml, raised above the panels by the host; scrolling
// itself is animated host-side (QPropertyAnimation on scrollPosition), so panels
// are re-placed by the engine on every animated step instead of being moved here.
Rectangle {
    id: root

    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    readonly property var sepXs: cppHost ? cppHost.separatorXs : []

    anchors.fill: parent
    color: "transparent"

    Repeater {
        model: root.sepXs
        Rectangle {
            x: modelData
            y: 3
            width: 1
            height: Math.max(root.height - 6, 0)
            color: RibbonTheme.separator
        }
    }
}
