import QtQuick 2.15
import SARibbon 3.0

// RibbonCategory default visual leaf (plan-04 S4): the bar background shows
// through; geometry comes from the host (anchors.fill).
Rectangle {
    id: root

    property QtObject categoryCpp: null
    onCategoryCppChanged: if (categoryCpp) categoryCpp.categoryQmlItem = root

    anchors.fill: parent
    color: "transparent"
}
