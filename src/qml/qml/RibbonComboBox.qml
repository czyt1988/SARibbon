import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Templates 2.12 as T
import SARibbon 3.0

// RibbonComboBox: a compact theme-following ComboBox for ribbon panels.
// Registered from this file as an importable type of the SARibbon module
// (qmlRegisterType by URL — no C++ host; embedded geometry belongs to
// RibbonControlContainer). The root is the unstyled template so the look
// does not depend on the application's QQuickStyle. Visual parity follows
// the widgets QSS `SARibbonPanel > QComboBox` specialization: 1px
// inputBorder frame over contentBg, the border switching to inputFocus on
// hover/focus, and the selection painted with the selection-bg token. The
// dropdown is themed too (contentBg + menuBorder + selectionBg) — a stock
// style popup would stay white-on-black in dark themes. The chevron is a
// Canvas redraw of the stock double-arrow (oversized for a ribbon row);
// because its color follows a theme token, the RibbonToolButton-leaf
// repaint rule applies: a color change must trigger requestPaint, or the
// arrow keeps stale colors after a theme switch. The contentItem TextField
// mirrors the stock template wiring (editText/displayText, readOnly while
// the popup is down) with squeezed paddings so the text survives the
// row-height squeeze; a template TextField carries no background of its
// own, so the combo's frame is painted exactly once.
T.ComboBox {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    leftPadding: padding + (!control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)
    rightPadding: padding + (control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)

    spacing: 4
    hoverEnabled: true

    font.pointSize: RibbonMetrics.fontPointSize

    TextMetrics {
        id: comboMetrics
        font: control.font
        text: "Ag"
    }

    delegate: T.ItemDelegate {
        id: popupRow
        objectName: "ribbonComboBoxPopupRow"

        // B54 trap 4: delegate bindings re-evaluate during popup teardown,
        // when file-scope ids are already null — route them through guarded
        // local properties instead of binding straight onto `control`
        readonly property var combo: control
        readonly property var list: popupList
        readonly property bool isCurrent: combo ? combo.currentIndex === index : false
        readonly property bool isHighlighted: combo ? combo.highlightedIndex === index : false

        width: list ? list.width : 0
        height: Math.max(24, rowLabel.implicitHeight + 8)
        hoverEnabled: combo ? combo.hoverEnabled : false
        highlighted: popupRow.isHighlighted

        text: {
            var c = combo
            if (!c) {
                return ""
            }
            return c.textRole ? (Array.isArray(c.model) ? modelData[c.textRole] : model[c.textRole]) : modelData
        }
        font.weight: popupRow.isCurrent ? Font.DemiBold : Font.Normal

        background: Rectangle {
            visible: popupRow.enabled
            color: popupRow.isCurrent ? RibbonTheme.selectionBg
                   : (popupRow.hovered ? RibbonTheme.contentHoverBg : "transparent")
        }

        contentItem: Text {
            id: rowLabel
            leftPadding: 6
            rightPadding: 6
            text: popupRow.text
            font: popupRow.font
            color: popupRow.enabled ? RibbonTheme.textColor : RibbonTheme.subtitle
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
    }

    indicator: Canvas {
        id: comboArrow
        objectName: "ribbonComboBoxIndicator"
        x: control.mirrored ? 2 : control.width - width - 2
        y: (control.height - height) / 2
        width: 12
        height: 12
        // theme-token color: repaint on change or the arrow goes stale
        // (RibbonToolButton leaf's indicator rule)
        property color arrowColor: control.enabled ? RibbonTheme.textColor : RibbonTheme.subtitle
        onArrowColorChanged: requestPaint()
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            ctx.strokeStyle = arrowColor;
            ctx.lineWidth = 1.4;
            ctx.beginPath();
            ctx.moveTo(2, 4);
            ctx.lineTo(6, 8);
            ctx.lineTo(10, 4);
            ctx.stroke();
        }
    }

    contentItem: T.TextField {
        id: comboField
        objectName: "ribbonComboBoxContent"
        leftPadding: 3
        rightPadding: 3
        topPadding: 1
        bottomPadding: 1

        text: control.editable ? control.editText : control.displayText
        enabled: control.editable
        autoScroll: control.editable
        readOnly: control.down
        inputMethodHints: control.inputMethodHints
        validator: control.validator
        selectByMouse: true

        font: control.font
        color: control.enabled ? RibbonTheme.textColor : RibbonTheme.subtitle
        selectionColor: RibbonTheme.selectionBg
        selectedTextColor: RibbonTheme.textColor
        verticalAlignment: TextInput.AlignVCenter
    }

    background: Rectangle {
        objectName: "ribbonComboBoxBackground"
        // font-derived compact height: the panel engine fixes the row
        // height, so the stock 40px implicit height would only clip
        implicitWidth: 100
        implicitHeight: Math.ceil(comboMetrics.height) + 6
        color: RibbonTheme.contentBg
        border.width: 1
        border.color: !control.enabled ? RibbonTheme.borderColor
                       : (control.hovered || control.activeFocus
                          || (control.popup && control.popup.visible)) ? RibbonTheme.inputFocus
                       : RibbonTheme.inputBorder
    }

    popup: T.Popup {
        id: comboPopup
        objectName: "ribbonComboBoxPopup"
        y: control.height
        width: control.width
        height: Math.min(contentItem.implicitHeight, control.Window.height - topMargin - bottomMargin)
        topMargin: 6
        bottomMargin: 6
        padding: 1

        background: Rectangle {
            objectName: "ribbonComboBoxPopupBackground"
            color: RibbonTheme.contentBg
            border.width: 1
            border.color: RibbonTheme.menuBorder
        }

        contentItem: ListView {
            id: popupList
            objectName: "ribbonComboBoxPopupList"
            clip: true
            implicitHeight: contentHeight
            model: control.delegateModel
            currentIndex: control.highlightedIndex
            highlightMoveDuration: 0

            T.ScrollIndicator.vertical: ScrollIndicator { }
        }
    }
}
