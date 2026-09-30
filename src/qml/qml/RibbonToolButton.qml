import QtQuick 2.12
import SARibbon 3.0

// RibbonToolButton default visual leaf. Layout mirrors the widgets
// SARibbonToolButton two modes: Large = icon above centered text (bottom,
// two-line wrap), Small/Medium = icon left + elided text right. States
// follow the office-2021 QSS (radius 4): normal content-bg, hover
// content-hover-bg, pressed/checked content-pressed-bg (checked adds a
// 1px text-color border). Interaction: the MouseArea calls the host's
// click() invokable which emits clicked()/toggles checked (logic stays
// in the C++ host, rendering here).
Rectangle {
    id: root

    property QtObject buttonCpp: null
    onButtonCppChanged: if (buttonCpp) buttonCpp.buttonQmlItem = root

    readonly property string label: buttonCpp ? buttonCpp.text : ""
    readonly property string icon: buttonCpp ? buttonCpp.iconSource : ""
    readonly property bool large: buttonCpp && buttonCpp.proportion === Ribbon.Large
    readonly property bool checked: buttonCpp ? buttonCpp.checked : false

    // office-2021 state colors (widgets QSS SARibbonToolButton)
    readonly property color stateBg: mouse.pressed ? RibbonTheme.contentPressedBg
                                     : (mouse.containsMouse ? RibbonTheme.contentHoverBg
                                        : (root.checked ? RibbonTheme.contentPressedBg : RibbonTheme.contentBg))
    readonly property color stateText: (mouse.containsMouse && !root.checked && !mouse.pressed)
                                       ? RibbonTheme.contentPressedBg : RibbonTheme.textColor

    anchors.fill: parent
    radius: 4
    color: root.stateBg
    border.width: root.checked ? 1 : 0
    border.color: RibbonTheme.textColor

    // icon sizes (widgets parity: 32 large / 20 small)
    readonly property int iconSide: root.large ? 32 : 20

    // ---- large button: icon centered above the bottom-anchored text ----
    Text {
        id: largeText
        visible: root.large
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 1
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 2
        text: root.label
        wrapMode: Text.WordWrap
        maximumLineCount: 2
        elide: Text.ElideRight
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignTop
        color: root.stateText
    }
    Image {
        visible: root.large
        anchors.top: parent.top
        anchors.topMargin: 2
        anchors.bottom: largeText.top
        anchors.bottomMargin: 2
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 4
        source: root.icon
        sourceSize.width: root.iconSide
        sourceSize.height: root.iconSide
        fillMode: Image.PreserveAspectFit
    }

    // ---- small/medium button: icon left, text right ----
    Image {
        id: smallIcon
        visible: !root.large
        anchors.left: parent.left
        anchors.leftMargin: 1
        anchors.verticalCenter: parent.verticalCenter
        width: root.iconSide
        height: root.iconSide
        source: root.icon
        sourceSize.width: root.iconSide
        sourceSize.height: root.iconSide
        fillMode: Image.PreserveAspectFit
    }
    Text {
        visible: !root.large
        anchors.left: smallIcon.right
        anchors.leftMargin: 2
        anchors.right: parent.right
        anchors.rightMargin: 2
        anchors.verticalCenter: parent.verticalCenter
        text: root.label
        elide: Text.ElideRight
        color: root.stateText
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: if (root.buttonCpp) root.buttonCpp.click()
    }
}
