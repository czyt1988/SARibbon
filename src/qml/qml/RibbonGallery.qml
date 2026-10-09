import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.2
import SARibbon 3.0

// RibbonGallery default visual leaf. Geometry authority stays in the C++
// host: cell size / columns / scroll row all come from the published grid
// metrics (core calcGalleryGridCellSize), the leaf only places cells inside
// them. The caption band follows the host's captionStyle — no band (IconOnly),
// one elided line (IconWithText) or two wrapped lines (IconWithWordWrapText) —
// mirroring the three paint paths of the widgets
// SARibbonGalleryGroupItemDelegate. The trailing strip carries the scroll-up /
// scroll-down / more buttons (widgets gallery button column parity, 15px); the
// more button opens a styled popup viewport listing every group at the very
// same cell size the inline grid renders (host metrics — the widgets viewport
// re-shows the same group widgets, never a downscaled copy), scrollable with
// its height refitted on every open, plus the custom widget sections
// (widgets viewport addWidget parity) attached below the groups; a cell click
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

    // ---- popup viewport: every group with its grid + custom sections ----
    // The expanded content renders at the very cell size the inline grid
    // shows: cell / icon box / caption band all come from the host-published
    // metrics (core calcGalleryGridCellSize + calcGalleryCellMetrics), exactly
    // like the widgets side — its viewport pops the very same gallery group
    // widgets, so the expanded view is never a downscaled copy. Tall content
    // scrolls (the widgets viewport is a QScrollArea) and the popup height
    // refits the space below the gallery on every open (widgets
    // showMoreDetail parity). Custom sections (widgets viewport addWidget
    // parity) are host-owned items attached into the slots below the groups.
    // A size grip sits on the bottom-right corner (widgets viewport QSizeGrip
    // parity): a drag sets an explicit user size that wins over the fitted
    // defaults until the popup is reopened, and width/height stay clamped to
    // the window so the grip can never push the popup off-screen.
    Popup {
        id: viewport
        objectName: "viewportPopup"
        y: root.height
        x: 0
        width: Math.max(root.width, 300)
        padding: 4
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle {
            color: RibbonTheme.contentBg
            border.color: RibbonTheme.menuBorder
            radius: 4
        }
        // the fit expression as a named function so the height binding and
        // the reopen rewire below share one body
        function heightFit()
        {
            if (!root) {
                return 420;
            }
            var wanted = viewportColumn.implicitHeight + 12;
            var win = root.Window.window;
            if (!win) {
                return Math.min(wanted, 420);
            }
            var below = win.height - root.mapToItem(win.contentItem, 0, root.height).y - 8;
            return Math.min(wanted, Math.max(below, 200));
        }
        // height: content height clamped to the window space below the
        // gallery. A binding, not an onAboutToShow snapshot: at aboutToShow
        // the content column has not polished yet (implicitHeight still 0),
        // while the binding re-fits on every content change; each
        // re-evaluation takes a fresh mapToItem, and the window height side
        // is reactive (the Window.window attached property notifies)
        height: heightFit()
        // reopen restarts from the fitted defaults — a user size is one-shot,
        // exactly like the widgets viewport (every showMoreDetail sizes from
        // heightHintForWidth before the grip is dragged)
        onAboutToShow: {
            if (grip.userSized) {
                grip.userSized = false;
                // an RTL drag shifted x alongside the width; restore the
                // gallery-aligned origin before the fitted sizes come back
                viewport.x = 0;
                viewport.width = Qt.binding(function() { return Math.max(root.width, 300); });
                viewport.height = Qt.binding(viewport.heightFit);
            }
        }
        contentItem: Flickable {
            id: viewportFlick
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            // not interactive: the flickable must never steal mouse events
            // from the size grip (its drag axis locks, so a vertical resize
            // drag stole the following horizontal one). Scrolling stays with
            // the scrollbar and the wheel — the widgets QScrollArea behaves
            // the same way (no drag-scroll either)
            interactive: false
            contentWidth: width
            contentHeight: viewportColumn.implicitHeight
            ScrollBar.vertical: ScrollBar { }
            // size grip overlaying the content bottom-right corner. Popup
            // routes declared children into the content ITEM — the Flickable
            // — and the Flickable in turn routes ITS declared children into
            // the scrolling content wrapper, where the grip would travel
            // with the content and lose the stacking tie against the
            // scrollbar (created after the wrapper, same z). The explicit
            // parent assignment re-homes the grip onto the Flickable
            // itself: pinned to the visible corner, and its z: 100 now
            // wins against the scrollbar sibling (both Qt lanes route
            // Popup contentData and Flickable children this way — verified
            // in qquickpopup.cpp / qquickflickable.cpp)
            Item {
                id: grip
                objectName: "viewportGrip"
                parent: viewportFlick
                // the user size set by a drag; while true the popup stays at
                // the explicit width/height and ignores the fitted defaults
                property bool userSized: false
                readonly property int gripSize: 16
                // RTL mirrors the grip to the bottom-LEFT corner (widgets
                // viewport parity); the theme singleton is the module's
                // single RTL source (it drives the app-global layout
                // direction and carries rtlChanged)
                readonly property bool rtl: RibbonTheme.rtl
                // position off the popup's own geometry (padding included):
                // the popup applies width/height synchronously while the
                // contentItem — the parent here — re-layouts on polish, so a
                // parent-size-bound grip lagged one layout behind and sat
                // outside the popup bounds right after a shrink (the next
                // press then closed the popup through CloseOnPressOutside)
                x: grip.rtl ? 0 : viewport.width - 2 * viewport.padding - grip.gripSize - 2
                y: viewport.height - 2 * viewport.padding - grip.gripSize - 2
                width: grip.gripSize
                height: grip.gripSize
                // always on (widgets QSizeGrip parity) and above the content:
                // the grip owns its 16x16 corner, exactly like the grip the
                // widgets viewport paints over its scroll view corner
                z: 100
                MouseArea {
                    id: gripMouse
                    anchors.fill: parent
                    // SizeFDiagCursor grows towards bottom-right (the LTR
                    // corner); SizeBDiagCursor grows towards bottom-left (the
                    // RTL corner) — the widgets viewport grip mirrors the
                    // same way on RTL
                    cursorShape: grip.rtl ? Qt.SizeBDiagCursor : Qt.SizeFDiagCursor
                    // swallow the drag so the Flickable underneath never
                    // steals it (press-and-drag on a Flickable child starts
                    // a flick gesture otherwise)
                    preventStealing: true
                    property real startX: 0
                    property real startY: 0
                    property real startW: 0
                    property real startH: 0
                    property real startVx: 0
                    onPressed: {
                        grip.userSized = true;
                        startX = mouseX;
                        startY = mouseY;
                        startW = viewport.width;
                        startH = viewport.height;
                        startVx = viewport.x;
                    }
                    onPositionChanged: {
                        if (!pressed) {
                            return;
                        }
                        // one write breaks the width/height bindings for good
                        // (writes from a mouse handler need no event loop), so
                        // the explicit size wins until the reopen rewires
                        var w = grip.clampW(startW + (grip.rtl ? startX - mouseX : mouseX - startX));
                        var h = grip.clampH(startH + mouseY - startY);
                        // RTL grows leftward: the popup's left edge moves with
                        // the drag, so x shifts by the same delta (right edge
                        // stays put)
                        if (grip.rtl) {
                            viewport.x = startVx - (w - startW);
                        }
                        viewport.width = w;
                        viewport.height = h;
                    }
                }
                // resize clamps stay window-aware live during the drag
                function clampW(w)
                {
                    if (!root) {
                        return Math.max(w, 300);
                    }
                    var win = root.Window.window;
                    if (!win) {
                        return Math.max(w, 300);
                    }
                    // window-space left edge of the popup
                    var winX = root.mapToItem(win.contentItem, 0, 0).x + viewport.x;
                    // LTR: the right edge may reach the window right edge;
                    // RTL: the left edge may reach the window left edge (the
                    // right edge stays put), so the span to it is the max
                    var maxW = grip.rtl ? (winX + viewport.width) : (win.width - winX);
                    return Math.max(300, Math.min(w, Math.max(maxW, 300)));
                }
                function clampH(h)
                {
                    if (!root) {
                        return Math.max(h, 200);
                    }
                    var win = root.Window.window;
                    var below = win ? (win.height - root.mapToItem(win.contentItem, 0, root.height).y - 8) : 4096;
                    return Math.max(200, Math.min(h, Math.max(below, 200)));
                }
                // the grip glyph: three diagonal dents, widgets QSizeGrip
                // style (theme colors, no hardcoded palette)
                Canvas {
                    anchors.fill: parent
                    property color glyphColor: RibbonTheme.textColor
                    onGlyphColorChanged: requestPaint()
                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.reset();
                        if (width <= 2 || height <= 2) {
                            return;
                        }
                        ctx.strokeStyle = glyphColor;
                        ctx.lineWidth = 1;
                        var n = 3;
                        for (var i = 1; i <= n; ++i) {
                            var off = i * 4;
                            ctx.beginPath();
                            ctx.moveTo(width - off, height - 2);
                            ctx.lineTo(width - 2, height - off);
                            ctx.stroke();
                        }
                    }
                }
            }
            Column {
                id: viewportColumn
                width: viewportFlick.width
                spacing: 2
                Repeater {
                    model: root.cppHost ? root.cppHost.groups : []
                    // outer delegate: one group (title + grid). groupIndex
                    // captures the OUTER index — the inner Repeater's index
                    // shadows `index` inside its own delegate
                    Column {
                        id: vpGroup
                        readonly property int groupIndex: index
                        readonly property var groupModel: modelData
                        readonly property int cellW: root ? root.cellW : 80
                        width: viewportColumn.width
                        spacing: 1
                        Text {
                            text: vpGroup.groupModel ? vpGroup.groupModel.groupTitle : ""
                            color: RibbonTheme.subtitle
                            elide: Text.ElideRight
                            width: parent.width
                        }
                        Grid {
                            // same cell width as the inline grid — the expanded
                            // content stays the size the ribbon panel shows
                            columns: Math.max(Math.floor(vpGroup.width / Math.max(vpGroup.cellW, 1)), 1)
                            spacing: 1
                            Repeater {
                                model: vpGroup.groupModel ? vpGroup.groupModel.items : []
                                Item {
                                    id: vpCell
                                    readonly property var entry: modelData
                                    // same guarded-read discipline as the inline
                                    // grid cell (NOTES B48/B54: ids go null
                                    // before the delegates are destroyed on
                                    // teardown)
                                    readonly property int cellW: root ? root.cellW : 0
                                    readonly property int cellH: root ? root.cellH : 0
                                    readonly property int captionH: root ? root.captionH : 0
                                    readonly property int iconW: root ? root.iconW : 0
                                    readonly property int iconH: root ? root.iconH : 0
                                    readonly property bool drawsCaption: root ? root.drawsCaption : false
                                    readonly property bool wrapsCaption: root ? root.wrapsCaption : false
                                    readonly property int bodyH: Math.max(vpCell.cellH - vpCell.captionH - 2, 0)
                                    readonly property int iconBoxW: Math.min(vpCell.iconW, Math.max(vpCell.cellW - 2, 0))
                                    readonly property int iconBoxH: Math.min(vpCell.iconH, vpCell.bodyH)
                                    readonly property bool entryEnabled: vpCell.entry ? vpCell.entry.enabled : false
                                    width: vpCell.cellW
                                    height: vpCell.cellH
                                    enabled: vpCell.entryEnabled
                                    Rectangle {
                                        anchors.fill: parent
                                        radius: 3
                                        color: !vpCell.entryEnabled ? "transparent"
                                               : (vpMouse.pressed ? RibbonTheme.contentPressedBg
                                                  : (vpMouse.containsMouse ? RibbonTheme.contentHoverBg : "transparent"))
                                    }
                                    Image {
                                        // centered exactly like the inline grid
                                        // cell: same icon box, same caption band
                                        x: (vpCell.cellW - vpCell.iconBoxW) / 2
                                        y: 1 + (vpCell.bodyH - vpCell.iconBoxH) / 2
                                        width: vpCell.iconBoxW
                                        height: vpCell.iconBoxH
                                        source: vpCell.entry ? vpCell.entry.iconSource : ""
                                        fillMode: Image.PreserveAspectFit
                                        opacity: vpCell.entryEnabled ? 1.0 : 0.45
                                    }
                                    Text {
                                        visible: vpCell.entryEnabled && vpCell.drawsCaption
                                        x: 1
                                        y: vpCell.cellH - vpCell.captionH
                                        width: vpCell.cellW - 2
                                        height: vpCell.captionH
                                        text: vpCell.entry ? vpCell.entry.text : ""
                                        wrapMode: vpCell.wrapsCaption ? Text.WordWrap : Text.NoWrap
                                        maximumLineCount: vpCell.wrapsCaption ? 2 : 1
                                        elide: Text.ElideRight
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignTop
                                        font.pixelSize: vpCell.wrapsCaption
                                                        ? Math.max(Math.floor(vpCell.captionH / 2) - 2, 8)
                                                        : Math.max(vpCell.captionH - 4, 8)
                                        color: RibbonTheme.textColor
                                    }
                                    MouseArea {
                                        id: vpMouse
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        enabled: vpCell.entryEnabled
                                        onClicked: {
                                            if (!root || !root.cppHost) {
                                                return;
                                            }
                                            // switch the group first, then
                                            // activate (activateItem is
                                            // current-group scoped). Plain
                                            // `index` (Repeater context
                                            // property): `model.index` is only
                                            // valid for QAbstractItemModel
                                            // models — a plain list model
                                            // leaves it undefined
                                            root.cppHost.currentGroupIndex = vpGroup.groupIndex;
                                            root.cppHost.activateItem(index);
                                            viewport.close();
                                        }
                                    }
                                    ToolTip.visible: vpMouse.containsMouse && vpCell.entry && vpCell.entry.toolTip.length > 0
                                    ToolTip.text: vpCell.entry ? vpCell.entry.toolTip : ""
                                    ToolTip.delay: 400
                                }
                            }
                        }
                    }
                }
                // custom sections (widgets viewport addWidget parity):
                // host-owned items the leaf attaches visually into the slot;
                // the slot width binding pushes the rendering context
                // (contract §6), the section content drives the slot height
                // (its implicitHeight follows childrenRect from C++)
                Repeater {
                    model: root.cppHost ? root.cppHost.viewportWidgets : []
                    Column {
                        id: vpSection
                        readonly property var sectionModel: modelData
                        readonly property real contentH: vpSection.sectionModel ? vpSection.sectionModel.implicitHeight : 0
                        width: viewportColumn.width
                        spacing: 1
                        Text {
                            text: vpSection.sectionModel ? vpSection.sectionModel.sectionTitle : ""
                            color: RibbonTheme.subtitle
                            elide: Text.ElideRight
                            width: parent.width
                        }
                        Item {
                            id: sectionSlot
                            width: parent.width
                            height: Math.max(vpSection.contentH, 1)
                            Component.onCompleted: {
                                if (vpSection.sectionModel && root && root.cppHost) {
                                    root.cppHost.attachViewportWidget(vpSection.sectionModel, sectionSlot);
                                    // a section that sized itself keeps its
                                    // width; an implicit-only section receives
                                    // the slot width as rendering context
                                    if (vpSection.sectionModel.width <= 0) {
                                        vpSection.sectionModel.width = Qt.binding(function() { return sectionSlot.width; });
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
