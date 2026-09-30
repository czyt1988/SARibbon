import QtQuick 2.15

// RibbonBar default visual leaf (plan-04 S4): background + title strip bound
// to the RibbonTheme singleton by consumers; geometry comes from the host.
Rectangle {
    id: root

    property QtObject barCpp: null
    onBarCppChanged: if (barCpp) barCpp.barQmlItem = root

    color: "transparent"
}
