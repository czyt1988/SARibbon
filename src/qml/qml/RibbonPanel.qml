import QtQuick 2.12
import SARibbon 3.0

// RibbonPanel default visual leaf: content background + the title caption
// rendered inside the engine-reserved strip (panelCpp.titleGeometry — leaf
// renders, the C++ host computes). QSS parity: subtitle color, centered,
// pixelSize = panelTitleHeight * 0.8 (SARibbonPanel::resetTitleLabelFont).
Rectangle {
    id: root

    // contract face (plan-04 leaf organization rule)
    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    // state reads always null-guarded
    readonly property string title: cppHost ? cppHost.panelTitle : ""
    readonly property rect titleRect: cppHost ? cppHost.titleGeometry : Qt.rect(0, 0, 0, 0)

    anchors.fill: parent
    color: RibbonTheme.contentBg
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
        font.pixelSize: Math.round(RibbonMetrics.panelTitleHeight * 0.8)
        color: RibbonTheme.subtitle
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
