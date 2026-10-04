import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Templates 2.12 as T
import SARibbon 3.0

// RibbonSpinBox: a compact theme-following SpinBox for ribbon panels.
// Registered from this file as an importable type of the SARibbon module
// (qmlRegisterType by URL — no C++ host; embedded geometry belongs to
// RibbonControlContainer; template root so the look does not depend on the
// application's QQuickStyle). The widgets QSS has no SpinBox
// specialization (panels embed a native QSpinBox), so this control adopts
// the input-control convention of the other ribbon inputs: 1px inputBorder
// frame over contentBg, switching to inputFocus on hover/focus, selection
// in selection-bg. The steppers are a Windows-style column on the trailing
// edge (up on the top half, down on the bottom half) instead of the stock
// Basic layout (full-height buttons on both sides) — that is what
// QSpinBox looks like inside a ribbon row. The chevrons are Canvas items
// whose color follows theme tokens, so they follow the
// repaint-on-color-change rule. All native properties (from, to, value,
// stepSize, prefix, suffix, decimals, editable, wrap) are inherited
// unchanged.
T.SpinBox {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    // the trailing stepper column is part of the text budget (official
    // templates compute the paddings from the indicators the same way)
    leftPadding: padding + (control.mirrored && up.indicator && up.indicator.visible ? up.indicator.width : 0)
    rightPadding: padding + (!control.mirrored && up.indicator && up.indicator.visible ? up.indicator.width : 0)

    padding: 2
    hoverEnabled: true

    font.pointSize: RibbonMetrics.fontPointSize

    validator: IntValidator {
        locale: control.locale.name
        bottom: Math.min(control.from, control.to)
        top: Math.max(control.from, control.to)
    }

    TextMetrics {
        id: spinMetrics
        font: control.font
        text: "Ag"
    }

    contentItem: TextInput {
        objectName: "ribbonSpinBoxContent"
        z: 2
        text: control.displayText
        clip: width < implicitWidth

        font: control.font
        color: control.enabled ? RibbonTheme.textColor : RibbonTheme.subtitle
        selectionColor: RibbonTheme.selectionBg
        selectedTextColor: RibbonTheme.textColor
        verticalAlignment: TextInput.AlignVCenter

        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: control.inputMethodHints
    }

    up.indicator: Rectangle {
        objectName: "ribbonSpinBoxUpIndicator"
        x: control.mirrored ? 0 : control.width - width
        y: 0
        width: 18
        height: control.height / 2
        color: control.up.pressed ? RibbonTheme.contentPressedBg
               : (control.up.hovered ? RibbonTheme.contentHoverBg : "transparent")
        border.width: 1
        border.color: !control.enabled ? RibbonTheme.borderColor : RibbonTheme.inputBorder

        Canvas {
            anchors.centerIn: parent
            width: 10
            height: 6
            // theme-token color: repaint on change or the chevron goes stale
            property color arrowColor: control.up.enabled
                                        ? ((control.up.pressed || control.up.hovered) ? RibbonTheme.inputFocus
                                                                                     : RibbonTheme.textColor)
                                        : RibbonTheme.subtitle
            onArrowColorChanged: requestPaint()
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = arrowColor;
                ctx.lineWidth = 1.4;
                ctx.beginPath();
                ctx.moveTo(2, 4);
                ctx.lineTo(5, 1);
                ctx.lineTo(8, 4);
                ctx.stroke();
            }
        }
    }

    down.indicator: Rectangle {
        objectName: "ribbonSpinBoxDownIndicator"
        x: control.mirrored ? 0 : control.width - width
        y: control.height - height
        width: 18
        height: control.height / 2
        color: control.down.pressed ? RibbonTheme.contentPressedBg
               : (control.down.hovered ? RibbonTheme.contentHoverBg : "transparent")
        border.width: 1
        border.color: !control.enabled ? RibbonTheme.borderColor : RibbonTheme.inputBorder

        Canvas {
            anchors.centerIn: parent
            width: 10
            height: 6
            property color arrowColor: control.down.enabled
                                        ? ((control.down.pressed || control.down.hovered) ? RibbonTheme.inputFocus
                                                                                       : RibbonTheme.textColor)
                                        : RibbonTheme.subtitle
            onArrowColorChanged: requestPaint()
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = arrowColor;
                ctx.lineWidth = 1.4;
                ctx.beginPath();
                ctx.moveTo(2, 1);
                ctx.lineTo(5, 4);
                ctx.lineTo(8, 1);
                ctx.stroke();
            }
        }
    }

    background: Rectangle {
        objectName: "ribbonSpinBoxBackground"
        implicitWidth: 70
        implicitHeight: Math.ceil(spinMetrics.height) + 6
        color: RibbonTheme.contentBg
        border.width: 1
        border.color: !control.enabled ? RibbonTheme.borderColor
                       : (control.hovered || control.activeFocus) ? RibbonTheme.inputFocus
                       : RibbonTheme.inputBorder
    }
}
