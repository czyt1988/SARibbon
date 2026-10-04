import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Templates 2.12 as T
import SARibbon 3.0

// RibbonRadioButton: a compact theme-following RadioButton for ribbon
// panels. Same registration and embedding model as RibbonCheckBox (no C++
// host; RibbonControlContainer owns the geometry; template root so the
// look does not depend on the application's QQuickStyle). Visual parity
// follows the widgets QSS `SARibbonPanel > QRadioButton` specialization:
// flat (transparent) background, text-color label. Radio groups get their
// exclusivity from the Controls2 ButtonGroup attached property, the QML
// counterpart of what the widgets example wires with QButtonGroup.
T.RadioButton {
    id: control

    // Ring diameter; 14px keeps the control inside one panel row
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
        objectName: "ribbonRadioButtonIndicator"
        implicitWidth: control.indicatorSide
        implicitHeight: control.indicatorSide

        x: control.text ? (control.mirrored ? control.width - width - control.rightPadding : control.leftPadding)
                        : control.leftPadding + (control.availableWidth - width) / 2
        y: control.topPadding + (control.availableHeight - height) / 2

        radius: width / 2
        color: "transparent"
        border.width: 1
        border.color: !control.enabled ? RibbonTheme.borderColor
                       : (control.checked || control.hovered) ? RibbonTheme.inputFocus
                       : RibbonTheme.subtitle

        // center dot appears on the ring's inputFocus border
        Rectangle {
            anchors.centerIn: parent
            width: Math.max(parent.width * 0.55, 4)
            height: width
            radius: width / 2
            visible: control.checked
            color: RibbonTheme.inputFocus
        }
    }

    contentItem: Text {
        objectName: "ribbonRadioButtonLabel"
        leftPadding: control.indicator && !control.mirrored ? control.indicator.width + control.spacing : 0
        rightPadding: control.indicator && control.mirrored ? control.indicator.width + control.spacing : 0

        text: control.text
        font: control.font
        color: control.enabled ? RibbonTheme.textColor : RibbonTheme.subtitle
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }
}
