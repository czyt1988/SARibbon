import QtQuick 2.15
import SARibbon 3.0

// RibbonBar default visual leaf (plan-04 S4): background bound to the
// RibbonTheme singleton; geometry comes from the host (anchors.fill).
Rectangle {
    id: root

    property QtObject barCpp: null
    onBarCppChanged: if (barCpp) barCpp.barQmlItem = root

    anchors.fill: parent
    color: RibbonTheme.tokenColor("content-bg")
}
