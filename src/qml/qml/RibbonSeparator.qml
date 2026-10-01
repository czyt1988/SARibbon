import QtQuick 2.12
import SARibbon 3.0

// RibbonSeparator default visual leaf: a 1px theme-colored vertical line,
// inset from the top/bottom edges (widgets QSS `SARibbonCategory >
// SARibbonSeparatorWidget` margin parity). Geometry authority stays in the
// C++ host (the engine sizes the item); this only paints the line.
Item {
    id: root

    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    anchors.fill: parent

    Rectangle {
        x: (parent.width - width) / 2
        y: 3
        width: 1
        height: Math.max(parent.height - 6, 0)
        color: RibbonTheme.separator
    }
}
