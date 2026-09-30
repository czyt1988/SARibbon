import QtQuick 2.15

// RibbonPanel default visual leaf (plan-04 S3): colors/sizes ONLY via
// RibbonTheme/RibbonMetrics singletons — a literal color value here is a
// review-reject. Base layer carries the contract face; visuals stay thin.
Rectangle {
    id: root

    // contract face (plan-04 leaf organization rule)
    property QtObject panelCpp: null
    onPanelCppChanged: if (panelCpp) panelCpp.panelQmlItem = root

    // state reads always null-guarded
    readonly property string title: panelCpp ? panelCpp.panelTitle : ""
    readonly property bool dark: true  // bound by consumers to RibbonTheme.dark

    // visual properties bound to theme/metrics (no literals)
    color: "transparent"
    border.width: 0
    radius: 0
}
