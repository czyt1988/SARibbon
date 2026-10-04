import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0

// RibbonColorMenu default visual leaf: a popup holding the title, the theme
// palette (standard row + shade rows), the optional "no color" row, the custom
// color row and the recorded custom colors — the QML counterpart of the widgets
// SAColorMenu, whose entries appear in that very order (enableNoneColorAction
// inserts the "no color" action before the custom color one).
// Every number comes from the C++ host: the color lists, the shade column count,
// the swatch box, the mark inset and the captions. The leaf only arranges three
// RibbonColorGrid instances and two text rows, so swatch geometry stays the
// WS-D2 core derivation instead of being recomputed here.
// The popup chrome is QML-native rather than QMenu's, a deliberate departure
// documented on the host class.
// NOTE: no inline `component` syntax here — the module targets Qt 5.12.
Item {
    id: root

    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    anchors.fill: parent

    // single-dependency reads of the host, hoisted (NOTES B48): a chained
    // `cppHost && cppHost.x ? cppHost.y : f` shape makes the V4 compiler drop
    // the dependency and the binding silently freezes
    readonly property var standardColors: cppHost ? cppHost.standardColors : []
    readonly property var shadeColors: cppHost ? cppHost.paletteColors : []
    readonly property int shadeColumns: cppHost ? cppHost.paletteColumns : 10
    readonly property var recordedColors: cppHost ? cppHost.customColors : []
    readonly property int customCapacity: cppHost ? cppHost.maxCustomColorCount : 10
    readonly property size swatchSize: cppHost ? cppHost.colorIconSize : Qt.size(10, 10)
    readonly property bool noneEnabled: cppHost ? cppHost.noneColorEnabled : false
    readonly property int noneMarkSide: cppHost ? cppHost.noneMarkSide : 16
    readonly property int noneSlashInset: cppHost ? cppHost.noneMarkSlashInset : 0
    readonly property string titleText: cppHost ? cppHost.themeColorsTitle : ""
    readonly property string customText: cppHost ? cppHost.customColorText : ""
    readonly property string noneText: cppHost ? cppHost.noneColorText : ""
    readonly property int rowHeight: cppHost ? cppHost.actionRowHeight : 24
    readonly property int paletteGap: cppHost ? cppHost.paletteSpacing : 8

    // Host-driven popup control: RibbonColorMenu::openMenu/closeMenu reach these
    // two by name through QMetaObject::invokeMethod and publish menuVisible
    // themselves when there is no rendered leaf, so both names must survive
    function openMenu()
    {
        if (!colorPopup.visible) {
            colorPopup.open();
        }
    }
    function closeMenu()
    {
        if (colorPopup.visible) {
            colorPopup.close();
        }
    }
    function publishVisible(on)
    {
        if (cppHost) {
            cppHost.menuVisible = on;
        }
    }

    // Report a picked color. The two palette grids clear each other's checked
    // state first, mirroring SAColorPaletteGridWidget::onMainColorClicked /
    // onPaletteColorClicked; that is inert while both stay non-checkable (as the
    // widgets menu builds them) and becomes correct the moment they are not
    function pickMainColor(c)
    {
        shadeGrid.clearCheckedState();
        pickColor(c);
    }
    function pickShadeColor(c)
    {
        mainGrid.clearCheckedState();
        pickColor(c);
    }
    function pickColor(c)
    {
        if (cppHost) {
            cppHost.emitSelectedColor(c);
        }
    }
    function pickNoneColor()
    {
        if (cppHost) {
            cppHost.selectNoneColor();
        }
    }
    function requestCustomColor()
    {
        if (cppHost) {
            cppHost.requestCustomColor();
        }
    }

    // the custom grid's row minimum follows the swatch box (see the grid note)
    onSwatchSizeChanged: customGrid.applyRowMinimum()

    Popup {
        id: colorPopup

        parent: root
        x: 0
        y: root.height
        padding: 4
        width: menuColumn.implicitWidth + 2 * padding + 2
        height: menuColumn.implicitHeight + 2 * padding + 2
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle {
            color: RibbonTheme.contentBg
            border.color: RibbonTheme.menuBorder
            radius: 4
        }
        onOpened: root.publishVisible(true)
        onClosed: root.publishVisible(false)

        contentItem: Column {
            id: menuColumn

            // Row width anchor: the two action rows span the widest
            // FIXED-size sibling, never menuColumn.width — a Column's
            // implicitWidth derives from its children's widths, so a row
            // reading menuColumn.width (which the popup derives from
            // menuColumn.implicitWidth) closes a layout cycle and ends in
            // the "Column called polish() inside updatePolish()" loop
            readonly property real contentRowWidth: Math.max(paletteColumn.implicitWidth, menuTitle.implicitWidth)

            spacing: 4

            Text {
                id: menuTitle

                visible: root.titleText.length > 0
                leftPadding: 4
                text: root.titleText
                color: RibbonTheme.subtitle
            }

            // SAColorPaletteGridWidget: one standard row above the shade rows,
            // both inside a layout with 1px contents margins and spacing 8, and
            // the shade rows packed with no gap between them
            Column {
                id: paletteColumn

                leftPadding: 1
                rightPadding: 1
                topPadding: 1
                bottomPadding: 1
                spacing: root.paletteGap

                RibbonColorGrid {
                    id: mainGrid

                    objectName: "colorMenuMainGrid"
                    colorList: root.standardColors
                    columnCount: 0
                    colorIconSize: root.swatchSize
                    colorCheckable: false
                    width: implicitWidth
                    height: implicitHeight
                    onColorClicked: root.pickMainColor(arguments[0])
                }
                RibbonColorGrid {
                    id: shadeGrid

                    objectName: "colorMenuShadeGrid"
                    colorList: root.shadeColors
                    columnCount: root.shadeColumns
                    verticalSpacing: 0
                    colorIconSize: root.swatchSize
                    colorCheckable: false
                    width: implicitWidth
                    height: implicitHeight
                    onColorClicked: root.pickShadeColor(arguments[0])
                }
            }

            // "no color" row: the widgets QAction whose icon is
            // SAColorToolButton::paintNoneColor, drawn here by the same shared
            // mark a grid cell uses
            Item {
                id: noneRow

                visible: root.noneEnabled
                objectName: "colorMenuNoneRow"
                width: menuColumn.contentRowWidth
                height: root.rowHeight

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 1
                    radius: 3
                    color: noneRowMouse.pressed ? RibbonTheme.contentPressedBg
                                                : (noneRowMouse.containsMouse ? RibbonTheme.contentHoverBg : "transparent")
                }
                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 6
                    spacing: 4

                    RibbonColorNoneMark {
                        anchors.verticalCenter: parent.verticalCenter
                        width: root.noneMarkSide
                        height: root.noneMarkSide
                        slashInset: root.noneSlashInset
                        markMargin: 1
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: root.noneText
                        color: RibbonTheme.textColor
                    }
                }
                MouseArea {
                    id: noneRowMouse

                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.pickNoneColor()
                }
            }

            // custom color row: raises customColorRequested() instead of running
            // a QColorDialog (host class note)
            Item {
                id: customRow

                objectName: "colorMenuCustomRow"
                width: menuColumn.contentRowWidth
                height: root.rowHeight

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 1
                    radius: 3
                    color: customRowMouse.pressed ? RibbonTheme.contentPressedBg
                                                  : (customRowMouse.containsMouse ? RibbonTheme.contentHoverBg : "transparent")
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    text: root.customText
                    color: RibbonTheme.textColor
                }
                MouseArea {
                    id: customRowMouse

                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.requestCustomColor()
                }
            }

            // recorded custom colors: widgets sizes this grid to the swatch box,
            // gives it a trailing expanding spring and caps the columns at the
            // record capacity. The row minimum the widgets menu sets is smaller
            // than a cell, so it never grows a row — the call is kept anyway so
            // the parity stays visible if the swatch box is ever shrunk
            RibbonColorGrid {
                id: customGrid

                objectName: "colorMenuCustomGrid"
                colorList: root.recordedColors
                columnCount: root.customCapacity
                colorIconSize: root.swatchSize
                colorCheckable: false
                horizontalSpacerToRight: true
                width: implicitWidth
                height: implicitHeight
                onColorClicked: root.pickColor(arguments[0])

                function applyRowMinimum()
                {
                    setRowMinimumHeight(0, root.swatchSize.height);
                }
                Component.onCompleted: applyRowMinimum()
            }
        }
    }
}
