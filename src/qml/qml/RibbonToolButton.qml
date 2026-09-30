import QtQuick 2.15
import SARibbon 3.0

// RibbonToolButton default visual leaf (plan-04 S5): colors via RibbonTheme
// tokens only; geometry from the host (engine-placed button rect).
Rectangle {
    id: root

    property QtObject buttonCpp: null
    onButtonCppChanged: if (buttonCpp) buttonCpp.buttonQmlItem = root

    readonly property string label: buttonCpp ? buttonCpp.text : ""
    readonly property string icon: buttonCpp ? buttonCpp.iconSource : ""

    anchors.fill: parent
    color: "transparent"
    radius: 2

    Text {
        anchors.centerIn: parent
        text: root.label
        color: RibbonTheme.tokenColor("text-color")
        // font size via metrics singleton binding is applied by consumers
        elide: Text.ElideRight
        width: parent.width - 4
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
