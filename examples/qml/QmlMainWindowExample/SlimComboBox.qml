import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0

// ComboBox tuned to a ribbon row: the Basic style indicator (40x28) is taller
// than the row height the panel engine assigns an embedded control, so the
// drop-down arrow is replaced by a compact chevron painted from the theme
// text color. The container squeezes the padding further; together the combo
// fits one row without clipping.
ComboBox {
    id: control

    // the style's content item pads like a form field; inside a ribbon row the
    // caption must use the whole inner height or it renders half clipped
    Component.onCompleted: {
        if (contentItem && contentItem.hasOwnProperty("padding")) {
            contentItem.padding = 0;
            // the Basic template pins topPadding/bottomPadding on their own,
            // so the blanket padding reset alone leaves the caption clipped
            contentItem.topPadding = 0;
            contentItem.bottomPadding = 0;
            contentItem.leftPadding = 4;
            contentItem.rightPadding = 4;
        }
    }

    indicator: Canvas {
        implicitWidth: 12
        implicitHeight: 12
        x: control.width - width - 2
        y: (control.height - height) / 2
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            ctx.strokeStyle = RibbonTheme.textColor;
            ctx.lineWidth = 1.4;
            ctx.beginPath();
            ctx.moveTo(2, 4);
            ctx.lineTo(6, 8);
            ctx.lineTo(10, 4);
            ctx.stroke();
        }
    }
}
