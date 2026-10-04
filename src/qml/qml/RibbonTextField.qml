import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Templates 2.12 as T
import SARibbon 3.0

// RibbonTextField: a compact theme-following single-line editor for ribbon
// panels. Registered from this file as an importable type of the SARibbon
// module (qmlRegisterType by URL — no C++ host; embedded geometry belongs
// to RibbonControlContainer; template root so the look does not depend on
// the application's QQuickStyle). Visual parity follows the widgets QSS
// `SARibbonPanel > QLineEdit` specialization: 1px inputBorder frame over
// contentBg switching to inputFocus on focus, text in text-color,
// selection in selection-bg. A template TextField renders no placeholder
// by itself (the stock styles provide that rendering), so this file draws
// the placeholder through a plain Text overlay reading the template's
// placeholderTextColor property. All native properties (placeholderText,
// validator, echoMode, maximumLength, ...) are inherited unchanged.
T.TextField {
    id: control

    implicitWidth: implicitBackgroundWidth + leftInset + rightInset
                   || Math.max(contentWidth, placeholderItem.implicitWidth) + leftPadding + rightPadding
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding,
                             placeholderItem.implicitHeight + topPadding + bottomPadding)

    padding: 2
    leftPadding: padding + 2
    rightPadding: padding + 2
    selectByMouse: true

    font.pointSize: RibbonMetrics.fontPointSize

    color: control.enabled ? RibbonTheme.textColor : RibbonTheme.subtitle
    selectionColor: RibbonTheme.selectionBg
    selectedTextColor: RibbonTheme.textColor
    placeholderTextColor: RibbonTheme.subtitle
    verticalAlignment: TextInput.AlignVCenter

    TextMetrics {
        id: fieldMetrics
        font: control.font
        text: "Ag"
    }

    Text {
        id: placeholderItem
        objectName: "ribbonTextFieldPlaceholder"
        x: control.leftPadding
        y: control.topPadding
        width: control.width - (control.leftPadding + control.rightPadding)
        height: control.height - (control.topPadding + control.bottomPadding)

        text: control.placeholderText
        font: control.font
        color: control.placeholderTextColor
        verticalAlignment: control.verticalAlignment
        visible: !control.length && !control.preeditText
        elide: Text.ElideRight
        renderType: control.renderType
    }

    background: Rectangle {
        objectName: "ribbonTextFieldBackground"
        // font-derived compact height: the panel engine fixes the row
        // height, so the stock 40px implicit height would only clip
        implicitWidth: 100
        implicitHeight: Math.ceil(fieldMetrics.height) + 6
        color: RibbonTheme.contentBg
        border.width: 1
        border.color: control.activeFocus ? RibbonTheme.inputFocus
                      : (control.enabled ? RibbonTheme.inputBorder : RibbonTheme.borderColor)
    }
}
