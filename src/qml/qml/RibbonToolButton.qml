import QtQuick 2.15

// RibbonToolButton default visual leaf (plan-04 S5)
Rectangle {
    id: root

    property QtObject buttonCpp: null
    onButtonCppChanged: if (buttonCpp) buttonCpp.buttonQmlItem = root

    readonly property string label: buttonCpp ? buttonCpp.text : ""
    readonly property string icon: buttonCpp ? buttonCpp.iconSource : ""

    color: "transparent"
    radius: 2

    Text {
        anchors.centerIn: parent
        text: root.label
        // font size via metrics singleton binding is applied by consumers
        elide: Text.ElideRight
        width: parent.width - 4
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
