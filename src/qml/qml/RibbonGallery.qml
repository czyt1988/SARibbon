import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0

// RibbonGallery default visual leaf. Geometry authority stays in the C++
// host: cell size / columns / scroll row all come from the published grid
// metrics (core calcGalleryGridCellSize), the leaf only places cells inside
// them. Cells render the widgets IconWithWordWrapText style (icon above the
// wrapped caption). The trailing strip carries the scroll-up / scroll-down /
// more buttons (widgets gallery button column parity, 15px); the more button
// opens a styled popup viewport listing every group, where a cell click
// switches the group and activates the entry.
// NOTE: no inline `component` syntax here — the module targets Qt 5.12.
Rectangle {
    id: root

    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    readonly property var currentGroup: cppHost ? cppHost.groupAt(cppHost.currentGroupIndex) : null
    readonly property var items: currentGroup ? currentGroup.items : []
    readonly property int columns: cppHost ? cppHost.gridColumns : 1
    readonly property int cellW: cppHost ? cppHost.gridSize.width : 80
    readonly property int cellH: cppHost ? cppHost.gridSize.height : 24
    readonly property int scrollRow: cppHost ? cppHost.scrollRow : 0
    readonly property int displayRow: cppHost ? cppHost.displayRow : 1
    readonly property int stripW: cppHost ? cppHost.buttonStripWidth : 15
    // caption band + icon box come from the host (core calcGalleryCellMetrics,
    // the very same derivation the widgets group feeds setIconSize with) — the
    // leaf must not invent a band from the panel title height
    readonly property int captionH: cppHost ? cppHost.captionHeight : 24
    readonly property int iconW: cppHost ? cppHost.cellIconWidth : 60
    readonly property int iconH: cppHost ? cppHost.cellIconHeight : 40

    // ---- entry points the C++ host invokes ----
    function openViewport()
    {
        if (!viewport.visible) {
            viewport.open();
        }
    }

    anchors.fill: parent
    color: RibbonTheme.contentBg
    border.width: 1
    border.color: RibbonTheme.separator

    // ---- the item grid (left of the button strip) ----
    Item {
        id: gridArea
        x: 1
        y: 1
        width: Math.max(parent.width - root.stripW - 2, 0)
        height: Math.max(parent.height - 2, 0)
        clip: true

        Repeater {
            model: root.items
            Item {
                id: cell
                // transiently null while the Repeater swaps models (group
                // switch); guard every read
                readonly property var entry: modelData
                readonly property int col: index % root.columns
                readonly property int row: Math.floor(index / root.columns)
                // icon box: host-derived (core calcGalleryCellMetrics), clamped to
                // the cell area above the caption band. Held as locals so the
                // Image/Text bindings below never read `parent` — the shape that
                // survives teardown (NOTES B48)
                readonly property int bodyH: Math.max(root.cellH - root.captionH - 2, 0)
                readonly property int iconBoxW: Math.min(root.iconW, Math.max(root.cellW - 2, 0))
                readonly property int iconBoxH: Math.min(root.iconH, cell.bodyH)
                x: cell.col * root.cellW
                y: (cell.row - root.scrollRow) * root.cellH
                width: root.cellW
                height: root.cellH
                visible: cell.row >= root.scrollRow && cell.row < root.scrollRow + root.displayRow
                          && cell.x + cell.width <= gridArea.width
                enabled: cell.entry ? cell.entry.enabled : false

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 1
                    radius: 3
                    color: !cell.entry || !cell.entry.enabled ? "transparent"
                           : (cellMouse.pressed ? RibbonTheme.contentPressedBg
                              : (cellMouse.containsMouse ? RibbonTheme.contentHoverBg : "transparent"))
                }
                Image {
                    id: cellIcon
                    // centered in the cell area above the caption band
                    x: (root.cellW - cell.iconBoxW) / 2
                    y: 1 + (cell.bodyH - cell.iconBoxH) / 2
                    width: cell.iconBoxW
                    height: cell.iconBoxH
                    source: cell.entry ? cell.entry.iconSource : ""
                    fillMode: Image.PreserveAspectFit
                    opacity: !cell.entry || !cell.entry.enabled ? 0.45 : 1.0
                }
                Text {
                    x: 1
                    y: root.cellH - root.captionH
                    width: root.cellW - 2
                    height: root.captionH
                    text: cell.entry ? cell.entry.text : ""
                    wrapMode: Text.WordWrap
                    maximumLineCount: 2
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignTop
                    // the band is two line spacings wide, so half of it minus the
                    // leading is the largest size that still fits both lines
                    font.pixelSize: Math.max(Math.floor(root.captionH / 2) - 2, 8)
                    color: RibbonTheme.textColor
                    opacity: !cell.entry || !cell.entry.enabled ? 0.45 : 1.0
                }
                MouseArea {
                    id: cellMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    enabled: cell.entry && cell.entry.enabled
                    onClicked: root.cppHost.activateItem(index)
                }
                ToolTip.visible: cellMouse.containsMouse && cell.entry && cell.entry.toolTip.length > 0
                ToolTip.text: cell.entry ? cell.entry.toolTip : ""
                ToolTip.delay: 400
            }
        }
    }

    // ---- scroll / more button strip (trailing edge); three plain buttons
    // (Qt 5.12 has no inline components) ----
    Item {
        id: strip
        x: parent.width - root.stripW
        y: 1
        width: root.stripW - 1
        height: Math.max(parent.height - 2, 0)

        // scroll up
        Item {
            id: upBtn
            x: 0
            y: 0
            width: parent.width
            height: parent.height / 3
            enabled: root.scrollRow > 0
            Rectangle {
                anchors.fill: parent
                color: !upBtn.enabled ? "transparent"
                       : (upMouse.pressed ? RibbonTheme.contentPressedBg
                          : (upMouse.containsMouse ? RibbonTheme.contentHoverBg : "transparent"))
            }
            Canvas {
                width: 9
                height: 6
                anchors.centerIn: parent
                property color arrowColor: upBtn.enabled ? RibbonTheme.textColor : RibbonTheme.separator
                onArrowColorChanged: requestPaint()
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.reset();
                    ctx.fillStyle = arrowColor;
                    ctx.beginPath();
                    ctx.moveTo(width / 2, 0);
                    ctx.lineTo(width, height);
                    ctx.lineTo(0, height);
                    ctx.closePath();
                    ctx.fill();
                }
            }
            MouseArea {
                id: upMouse
                anchors.fill: parent
                hoverEnabled: true
                enabled: upBtn.enabled
                onClicked: root.cppHost.scrollUp()
            }
        }
        // scroll down
        Item {
            id: downBtn
            x: 0
            y: upBtn.height
            width: parent.width
            height: parent.height / 3
            enabled: root.scrollRow + root.displayRow < (cppHost ? cppHost.totalRows : 0)
            Rectangle {
                anchors.fill: parent
                color: !downBtn.enabled ? "transparent"
                       : (downMouse.pressed ? RibbonTheme.contentPressedBg
                          : (downMouse.containsMouse ? RibbonTheme.contentHoverBg : "transparent"))
            }
            Canvas {
                width: 9
                height: 6
                anchors.centerIn: parent
                property color arrowColor: downBtn.enabled ? RibbonTheme.textColor : RibbonTheme.separator
                onArrowColorChanged: requestPaint()
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.reset();
                    ctx.fillStyle = arrowColor;
                    ctx.beginPath();
                    ctx.moveTo(0, 0);
                    ctx.lineTo(width, 0);
                    ctx.lineTo(width / 2, height);
                    ctx.closePath();
                    ctx.fill();
                }
            }
            MouseArea {
                id: downMouse
                anchors.fill: parent
                hoverEnabled: true
                enabled: downBtn.enabled
                onClicked: root.cppHost.scrollDown()
            }
        }
        // more: dropdown triangle + underline, opens the viewport
        Item {
            id: moreBtn
            x: 0
            y: upBtn.height + downBtn.height
            width: parent.width
            height: Math.max(parent.height - upBtn.height - downBtn.height, 0)
            Rectangle {
                anchors.fill: parent
                color: moreMouse.pressed ? RibbonTheme.contentPressedBg
                       : (moreMouse.containsMouse ? RibbonTheme.contentHoverBg : "transparent")
            }
            Canvas {
                width: 8
                height: 5
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: -2
                property color arrowColor: RibbonTheme.textColor
                onArrowColorChanged: requestPaint()
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.reset();
                    ctx.fillStyle = arrowColor;
                    ctx.beginPath();
                    ctx.moveTo(0, 0);
                    ctx.lineTo(width, 0);
                    ctx.lineTo(width / 2, height);
                    ctx.closePath();
                    ctx.fill();
                }
            }
            Rectangle {
                width: 8
                height: 1
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 3
                color: RibbonTheme.textColor
            }
            MouseArea {
                id: moreMouse
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.openViewport()
            }
        }
    }

    // ---- popup viewport: every group with its grid ----
    Popup {
        id: viewport
        y: root.height
        x: 0
        width: Math.max(root.width, 300)
        height: Math.min(viewportColumn.implicitHeight + 12, 420)
        padding: 4
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle {
            color: RibbonTheme.contentBg
            border.color: RibbonTheme.menuBorder
            radius: 4
        }
        contentItem: Column {
            id: viewportColumn
            spacing: 2
            Repeater {
                model: root.cppHost ? root.cppHost.groups : []
                // outer delegate: one group (title + mini grid). groupIndex
                // captures the OUTER index — the inner Repeater's index
                // shadows `index` inside its own delegate
                Column {
                    id: vpGroup
                    readonly property int groupIndex: index
                    readonly property var groupModel: modelData
                    width: viewportColumn.width
                    spacing: 1
                    Text {
                        text: vpGroup.groupModel ? vpGroup.groupModel.groupTitle : ""
                        color: RibbonTheme.subtitle
                        elide: Text.ElideRight
                        width: parent.width
                    }
                    Grid {
                        columns: Math.max(Math.floor(vpGroup.width / 74), 1)
                        spacing: 1
                        Repeater {
                            model: vpGroup.groupModel ? vpGroup.groupModel.items : []
                            Item {
                                id: vpCell
                                readonly property var entry: modelData
                                width: 74
                                height: 44
                                enabled: vpCell.entry ? vpCell.entry.enabled : false
                                Rectangle {
                                    anchors.fill: parent
                                    radius: 3
                                    color: !vpCell.entry || !vpCell.entry.enabled ? "transparent"
                                           : (vpMouse.pressed ? RibbonTheme.contentPressedBg
                                              : (vpMouse.containsMouse ? RibbonTheme.contentHoverBg : "transparent"))
                                }
                                Image {
                                    anchors.top: parent.top
                                    anchors.topMargin: 2
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: parent.width - 8
                                    height: 22
                                    source: vpCell.entry ? vpCell.entry.iconSource : ""
                                    fillMode: Image.PreserveAspectFit
                                    opacity: !vpCell.entry || !vpCell.entry.enabled ? 0.45 : 1.0
                                }
                                Text {
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: 1
                                    width: parent.width - 2
                                    height: 16
                                    text: vpCell.entry ? vpCell.entry.text : ""
                                    wrapMode: Text.WordWrap
                                    maximumLineCount: 2
                                    elide: Text.ElideRight
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignTop
                                    font.pixelSize: 9
                                    color: RibbonTheme.textColor
                                    opacity: !vpCell.entry || !vpCell.entry.enabled ? 0.45 : 1.0
                                }
                                MouseArea {
                                    id: vpMouse
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    enabled: vpCell.entry && vpCell.entry.enabled
                                    onClicked: {
                                        // switch the group first, then activate
                                        // (activateItem is current-group scoped)
                                        root.cppHost.currentGroupIndex = vpGroup.groupIndex;
                                        root.cppHost.activateItem(model.index);
                                        viewport.close();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
