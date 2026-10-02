import QtQuick 2.12
import SARibbon 3.0

// RibbonControlContainer default visual leaf: the leading label strip (icon +
// text) rendered inside the host-computed labelWidth and the trailing suffix
// strip inside suffixWidth; the embedded control is positioned by the C++ host
// between the two (geometry authority stays in C++ — the leaf only paints the
// labels). Flat like the widgets SARibbonCtrlContainer (no background,
// subtitle-colored text).
Item {
    id: root

    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    readonly property string label: cppHost ? cppHost.text : ""
    readonly property string suffix: cppHost ? cppHost.suffixText : ""
    readonly property string icon: cppHost ? cppHost.iconSource : ""
    readonly property real stripWidth: cppHost ? cppHost.labelWidth : 0
    readonly property real tailWidth: cppHost ? cppHost.suffixWidth : 0
    readonly property bool showsIcon: cppHost ? cppHost.enableShowIcon : true
    readonly property bool showsTitle: cppHost ? cppHost.enableShowTitle : true

    anchors.fill: parent

    Image {
        id: iconItem
        visible: root.showsIcon && root.icon.length > 0
        x: 0
        width: 20
        height: 20
        y: (root.height - height) / 2
        source: root.icon
        fillMode: Image.PreserveAspectFit
    }
    Text {
        id: labelItem
        visible: root.showsTitle && root.label.length > 0
        x: root.showsIcon && root.icon.length > 0 ? iconItem.width + 3 : 0
        width: Math.max(root.stripWidth - x, 0)
        y: (root.height - height) / 2
        text: root.label
        elide: Text.ElideRight
        color: RibbonTheme.subtitle
    }
    Text {
        id: suffixItem
        visible: root.suffix.length > 0
        x: Math.max(root.width - root.tailWidth, 0)
        width: Math.max(root.tailWidth, 0)
        y: (root.height - height) / 2
        text: root.suffix
        elide: Text.ElideRight
        color: RibbonTheme.subtitle
    }
}
