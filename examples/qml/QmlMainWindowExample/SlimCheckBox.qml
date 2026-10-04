import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0

// CheckBox tuned to a ribbon row: the Basic style indicator (28x28) is taller
// than the row height the panel engine assigns an embedded control, so the
// box is replaced by a 14px square with a painted check mark.
CheckBox {
    id: control

    indicator: Rectangle {
        implicitWidth: 14
        implicitHeight: 14
        radius: 2
        x: control.leftPadding
        y: (control.height - height) / 2
        color: "transparent"
        border.color: control.checked ? RibbonTheme.inputFocus : RibbonTheme.subtitle
        Canvas {
            width: 10
            height: 8
            anchors.centerIn: parent
            visible: control.checked
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = RibbonTheme.inputFocus;
                ctx.lineWidth = 1.6;
                ctx.beginPath();
                ctx.moveTo(1, 4);
                ctx.lineTo(4, 7);
                ctx.lineTo(9, 1);
                ctx.stroke();
            }
        }
    }
}
