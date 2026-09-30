import QtQuick 2.15

// RibbonCategory default visual leaf (plan-04 S4)
Rectangle {
    id: root

    property QtObject categoryCpp: null
    onCategoryCppChanged: if (categoryCpp) categoryCpp.categoryQmlItem = root

    color: "transparent"
}
