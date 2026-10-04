import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Templates 2.12 as T
import SARibbon 3.0

// RibbonCheckBox: a compact theme-following CheckBox for ribbon panels.
// Registered from this file as an importable type of the SARibbon module
// (qmlRegisterType by URL — no C++ host: an embedded control's geometry
// authority belongs to RibbonControlContainer, which stretches the control
// to the engine-assigned row height and squeezes its padding). The root is
// the unstyled template (not the running style's CheckBox) so the ribbon
// look does not depend on the application's QQuickStyle — this file is the
// style. The stock Basic/Default indicator (28x28) is taller than a ribbon
// row, so the indicator is redrawn at `indicatorSide` (default 14). Visual
// parity follows the widgets QSS `SARibbonPanel > QCheckBox`
// specialization: flat (transparent) background, text-color label. The
// white checkmark stroke is a fixed contrast color on the inputFocus fill
// — same policy as RibbonColorNoneMark's fixed marks, deliberately not a
// theme token (which also means the Canvas never needs repainting on theme
// switches). The font follows RibbonMetrics so the label scales with the
// ribbon's font.
T.CheckBox {
    id: control

    // Side length of the check square; 14px keeps the control inside the
    // single-line rows the panel engine derives from the category height
    property real indicatorSide: 14

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    padding: 2
    spacing: 4
    hoverEnabled: true

    font.pointSize: RibbonMetrics.fontPointSize

    indicator: Rectangle {
        objectName: "ribbonCheckBoxIndicator"
        implicitWidth: control.indicatorSide
        implicitHeight: control.indicatorSide

        x: control.text ? (control.mirrored ? control.width - width - control.rightPadding : control.leftPadding)
                        : control.leftPadding + (control.availableWidth - width) / 2
        y: control.topPadding + (control.availableHeight - height) / 2

        radius: 2

        // checked fills with the focus token; hover announces with the border
        // (the input-control hover convention shared with the other controls)
        color: control.checkState === Qt.Checked ? RibbonTheme.inputFocus : "transparent"
        border.width: 1
        border.color: !control.enabled ? RibbonTheme.borderColor
                       : (control.checkState !== Qt.Unchecked || control.hovered) ? RibbonTheme.inputFocus
                       : RibbonTheme.subtitle

        // checkmark stroke — fixed white on the inputFocus fill
        Canvas {
            anchors.centerIn: parent
            width: Math.max(parent.width * 0.72, 6)
            height: Math.max(parent.height * 0.5, 4)
            visible: control.checkState === Qt.Checked
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = "#ffffff";
                ctx.lineWidth = Math.max(parent.width * 0.12, 1.2);
                ctx.beginPath();
                ctx.moveTo(1, height * 0.55);
                ctx.lineTo(width * 0.38, height - 1);
                ctx.lineTo(width - 1, 1);
                ctx.stroke();
            }
        }

        // partially checked (tristate): horizontal dash
        Rectangle {
            anchors.centerIn: parent
            width: Math.max(parent.width * 0.55, 4)
            height: Math.max(2, parent.height * 0.14)
            visible: control.checkState === Qt.PartiallyChecked
            color: "#ffffff"
        }
    }

    contentItem: Text {
        objectName: "ribbonCheckBoxLabel"
        leftPadding: control.indicator && !control.mirrored ? control.indicator.width + control.spacing : 0
        rightPadding: control.indicator && control.mirrored ? control.indicator.width + control.spacing : 0

        text: control.text
        font: control.font
        color: control.enabled ? RibbonTheme.textColor : RibbonTheme.subtitle
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }
}
