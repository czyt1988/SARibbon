import QtQuick 2.15
import SARibbon 3.0

// RibbonPanel default visual leaf (plan-04 S3): colors ONLY via the
// RibbonTheme singleton — a literal color value here is a review-reject.
// The title strip geometry comes from the engine via panelCpp.titleGeometry
// (leaf renders, the C++ host computes — geometry authority stays in C++).
Rectangle {
    id: root

    // contract face (plan-04 leaf organization rule)
    property QtObject panelCpp: null
    onPanelCppChanged: if (panelCpp) panelCpp.panelQmlItem = root

    // state reads always null-guarded
    readonly property string title: panelCpp ? panelCpp.panelTitle : ""
    readonly property rect titleRect: panelCpp ? panelCpp.titleGeometry : Qt.rect(0, 0, 0, 0)

    anchors.fill: parent
    color: "transparent"
    border.width: 0
    radius: 0

    // title caption rendered inside the engine-reserved strip at the panel bottom
    Text {
        x: root.titleRect.x
        y: root.titleRect.y
        width: root.titleRect.width
        height: root.titleRect.height
        text: root.title
        visible: root.title.length > 0 && height > 0
        color: RibbonTheme.tokenColor("text-color")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
