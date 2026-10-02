import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0

// RibbonGallery default visual leaf. Geometry authority stays in the C++
// host: cell size / columns / scroll row all come from the published grid
// metrics (core calcGalleryGridCellSize), the leaf only places cells inside
// them. The caption band follows the host's captionStyle — no band (IconOnly),
// one elided line (IconWithText) or two wrapped lines (IconWithWordWrapText) —
// mirroring the three paint paths of the widgets
// SARibbonGalleryGroupItemDelegate. The trailing strip carries the scroll-up /
// scroll-down / more buttons (widgets gallery button column parity, 15px); the
// more button opens a styled popup viewport listing every group, where a cell
// click switches the group and activates the entry.
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
    // the caption style decides whether the cell draws a caption at all and
    // how many lines it may use (host-published, Ribbon.GalleryCaptionStyle)
    readonly property int captionStyle: cppHost ? cppHost.captionStyle : Ribbon.GalleryIconWithWordWrapText
    readonly property bool drawsCaption: root.captionStyle !== Ribbon.GalleryIconOnly
    readonly property bool wrapsCaption: root.captionStyle === Ribbon.GalleryIconWithWordWrapText
    // current (selected) cell of the current group, -1 for none
    readonly property int currentItemIndex: cppHost ? cppHost.currentItemIndex : -1

    // how many cells currently see the pointer. Qt Quick delivers hover to the
    // topmost hover-enabled item only, so a full-area MouseArea layered over the
    // grid to catch "the pointer left" would swallow every cell hover instead;
    // the cells therefore publish themselves and the host derives the leave from
    // the count dropping back to zero
    property int hoverCount: 0

    // ---- entry points the C++ host invokes ----
    function openViewport()
    {
        if (!viewport.visible) {
            viewport.open();
        }
    }

    // report a cell hover to the host (negative index = left the grid); the
    // host resolves it to an entry and publishes hovered
    function reportHover(index)
    {
        if (root.cppHost) {
            root.cppHost.notifyCellHovered(index);
        }
    }

    // no cell sees the pointer any more: clear any hover preview
    onHoverCountChanged: if (root.hoverCount === 0) root.reportHover(-1)

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
                // Every read of a same-file id is guarded and hoisted into a
                // cell-local: during teardown the ids go null before the
                // Repeater delegates are destroyed, and an unguarded read
                // throws a TypeError into the binding (NOTES B48/B54). The
                // children below therefore never read `root` / `gridArea` /
                // `cellMouse` / `parent` themselves.
                readonly property int columns: root ? root.columns : 1
                readonly property int cellW: root ? root.cellW : 0
                readonly property int cellH: root ? root.cellH : 0
                readonly property int scrollRow: root ? root.scrollRow : 0
                readonly property int displayRow: root ? root.displayRow : 1
                readonly property int gridW: gridArea ? gridArea.width : 0
                readonly property int captionH: root ? root.captionH : 0
                readonly property int currentItemIndex: root ? root.currentItemIndex : -1
                readonly property bool drawsCaption: root ? root.drawsCaption : false
                readonly property bool wrapsCaption: root ? root.wrapsCaption : false
                readonly property bool cellHovered: cellMouse ? cellMouse.containsMouse : false
                readonly property bool cellPressed: cellMouse ? cellMouse.pressed : false
                readonly property int col: index % Math.max(cell.columns, 1)
                readonly property int row: Math.floor(index / Math.max(cell.columns, 1))
                // icon box: host-derived (core calcGalleryCellMetrics), clamped to
                // the cell area above the caption band
                readonly property int bodyH: Math.max(cell.cellH - cell.captionH - 2, 0)
                readonly property int iconBoxW: Math.min(root ? root.iconW : 0, Math.max(cell.cellW - 2, 0))
                readonly property int iconBoxH: Math.min(root ? root.iconH : 0, cell.bodyH)
                readonly property bool isCurrent: index === cell.currentItemIndex
                readonly property bool entryEnabled: cell.entry ? cell.entry.enabled : false
                x: cell.col * cell.cellW
                y: (cell.row - cell.scrollRow) * cell.cellH
                width: cell.cellW
                height: cell.cellH
                visible: cell.row >= cell.scrollRow && cell.row < cell.scrollRow + cell.displayRow
                          && cell.x + cell.width <= cell.gridW
                enabled: cell.entryEnabled

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 1
                    radius: 3
                    // the current cell keeps its selection fill under the hover
                    // tint (widgets PE_PanelItemViewItem State_Selected parity)
                    color: !cell.entryEnabled ? "transparent"
                           : (cell.cellPressed ? RibbonTheme.contentPressedBg
                              : (cell.cellHovered ? RibbonTheme.contentHoverBg
                                 : (cell.isCurrent ? RibbonTheme.selectionBg : "transparent")))
                }
                Image {
                    id: cellIcon
                    // centered in the cell area above the caption band
                    x: (cell.cellW - cell.iconBoxW) / 2
                    y: 1 + (cell.bodyH - cell.iconBoxH) / 2
                    width: cell.iconBoxW
                    height: cell.iconBoxH
                    source: cell.entry ? cell.entry.iconSource : ""
                    fillMode: Image.PreserveAspectFit
                    opacity: cell.entryEnabled ? 1.0 : 0.45
                }
                Text {
                    // IconOnly reserves no band at all (host captionHeight == 0),
                    // IconWithText draws one elided line, IconWithWordWrapText two
                    // wrapped lines — the widgets delegate's three paint paths
                    visible: cell.entryEnabled && cell.drawsCaption
                    x: 1
                    y: cell.cellH - cell.captionH
                    width: cell.cellW - 2
                    height: cell.captionH
                    text: cell.entry ? cell.entry.text : ""
                    wrapMode: cell.wrapsCaption ? Text.WordWrap : Text.NoWrap
                    maximumLineCount: cell.wrapsCaption ? 2 : 1
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignTop
                    // WordWrap: the band is two line spacings wide, so half of it
                    // minus the leading is the largest size that still fits both
                    // lines. SingleLine: the whole band is one line spacing.
                    font.pixelSize: cell.wrapsCaption ? Math.max(Math.floor(cell.captionH / 2) - 2, 8)
                                                      : Math.max(cell.captionH - 4, 8)
                    color: RibbonTheme.textColor
                    opacity: 1.0
                }
                MouseArea {
                    id: cellMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    enabled: cell.entryEnabled
                    onClicked: if (root) root.cppHost.activateItem(index)
                    onContainsMouseChanged: {
                        if (!root) {
                            return;
                        }
                        root.hoverCount = Math.max(root.hoverCount + (containsMouse ? 1 : -1), 0);
                        if (containsMouse) {
                            root.reportHover(index);
                        }
                    }
                }
                ToolTip.visible: cell.cellHovered && cell.entry && cell.entry.toolTip.length > 0
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
