import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0

// RadioButton tuned to a ribbon row: the Basic style indicator (28x28) is
// taller than the row height the panel engine assigns an embedded control,
// so the circle is replaced by a 14px ring with an 8px dot.
RadioButton {
    id: control

    indicator: Rectangle {
        implicitWidth: 14
        implicitHeight: 14
        radius: 7
        x: control.leftPadding
        y: (control.height - height) / 2
        color: "transparent"
        border.color: control.checked ? RibbonTheme.inputFocus : RibbonTheme.subtitle
        Rectangle {
            width: 8
            height: 8
            radius: 4
            anchors.centerIn: parent
            visible: control.checked
            color: RibbonTheme.inputFocus
        }
    }
}
