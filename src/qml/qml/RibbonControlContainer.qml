import QtQuick 2.12
import SARibbon 3.0

// RibbonControlContainer default visual leaf: the leading label strip (icon +
// text) rendered inside the host-computed labelWidth; the embedded control is
// positioned by the C++ host after the strip (geometry authority stays in
// C++ — the leaf only paints the label). Flat like the widgets
// SARibbonCtrlContainer (no background, subtitle-colored text).
Item {
    id: root

    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    readonly property string label: cppHost ? cppHost.text : ""
    readonly property string icon: cppHost ? cppHost.iconSource : ""
    readonly property real stripWidth: cppHost ? cppHost.labelWidth : 0

    anchors.fill: parent

    Image {
        id: iconItem
        visible: root.icon.length > 0
        x: 0
        width: 20
        height: 20
        y: (root.height - height) / 2
        source: root.icon
        fillMode: Image.PreserveAspectFit
    }
    Text {
        id: labelItem
        visible: root.label.length > 0
        x: root.icon.length > 0 ? iconItem.width + 3 : 0
        width: Math.max(root.stripWidth - x, 0)
        y: (root.height - height) / 2
        text: root.label
        elide: Text.ElideRight
        color: RibbonTheme.subtitle
    }
}
