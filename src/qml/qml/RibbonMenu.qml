import QtQuick 2.12
import QtQuick.Controls 2.12
import SARibbon 3.0

// Shared ribbon popup menu leaf. Extracted from RibbonToolButton.qml so the
// button popup, the bar application menu and every nested submenu render the
// same way — the widgets front end gets that for free from SARibbonMenu/QMenu.
// Rendering only: the owning host publishes the RibbonMenuItem list, this file
// publishes back an index path per activation and the host resolves it
// (activateMenuItemPath / activateApplicationMenuItemPath). Submenus nest by
// instantiating this very type at runtime, which is the QML counterpart of the
// recursive SARibbonMenu::addRibbonMenu structure.
// NOTE: no inline `component` syntax here — the module targets Qt 5.12.
Popup {
    id: menuRoot

    // RibbonMenuItem list published by the owning host (QQmlListProperty)
    property var menuModel: []
    // objectName stem: rows come out as "<prefix>Row" / "<prefix>Separator".
    // Repeater delegates carry no QObject parent, so tests walk the visual tree
    // by name and both existing names ("menu", "appMenu") must survive
    property string namePrefix: "menu"
    property int rowHeight: 24
    property int minRowWidth: 140

    // Index path of the activated entry: [row] at the top level, one more
    // element per nesting level below it. Declared as var (a JS array), so
    // handlers read it positionally through arguments[0]
    signal itemActivated(var indexPath)

    // Rows holding an open submenu, so hovering a sibling closes it (QMenu
    // behaviour). A plain array on purpose — nothing binds to it, and the
    // nested popups are created on demand rather than declared, so they cannot
    // be enumerated through the item tree
    property var subRowStack: []

    function pushSubRow(r)
    {
        if (subRowStack.indexOf(r) < 0) {
            subRowStack.push(r);
        }
    }
    function popSubRow(r)
    {
        var i = subRowStack.indexOf(r);
        if (i >= 0) {
            subRowStack.splice(i, 1);
        }
    }
    function closeOtherSubmenus(exceptRow)
    {
        var rows = subRowStack.slice();
        for (var i = 0; i < rows.length; ++i) {
            if (rows[i] !== exceptRow) {
                rows[i].closeSubmenu();
            }
        }
    }
    function closeAllSubmenus()
    {
        var rows = subRowStack.slice();
        for (var i = 0; i < rows.length; ++i) {
            rows[i].closeSubmenu();
        }
    }
    // QMenu reserves the mark column for EVERY row as soon as one entry is
    // checkable, which is what keeps captions aligned in a mixed menu. Only
    // re-evaluated when the model itself is reassigned: flipping checkable at
    // runtime does not re-reserve the column
    function anyCheckable(m)
    {
        if (!m) {
            return false;
        }
        var n = m.length;
        for (var i = 0; i < n; ++i) {
            var e = m[i];
            if (e && e.checkable) {
                return true;
            }
        }
        return false;
    }
    readonly property bool reserveMarkColumn: menuRoot.anyCheckable(menuRoot.menuModel)

    x: 0
    y: 0
    width: menuColumn.implicitWidth + 2
    height: menuColumn.implicitHeight + 2
    padding: 1
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background: Rectangle {
        color: RibbonTheme.contentBg
        border.color: RibbonTheme.menuBorder
        radius: 4
    }
    onClosed: menuRoot.closeAllSubmenus()

    contentItem: Column {
        id: menuColumn

        Repeater {
            model: menuRoot.menuModel
            Item {
                id: row

                // transiently null while the Repeater swaps models; guard every
                // read and hold the results as locals so the item bindings
                // below never re-read it (NOTES B48 single-dependency shape).
                // The owning menu is read the same way: when a popup is torn
                // down its bindings re-evaluate against a null id, and an
                // unguarded read would surface as a TypeError at close time
                readonly property var menu: menuRoot
                readonly property string nameStem: menu ? menu.namePrefix : ""
                readonly property int rowH: menu ? menu.rowHeight : 24
                readonly property int minW: menu ? menu.minRowWidth : 140
                readonly property bool markColumn: menu ? menu.reserveMarkColumn : false

                readonly property var entry: modelData
                readonly property bool isSeparatorEntry: entry ? entry.separator : false
                readonly property bool entryEnabled: entry ? entry.enabled : false
                readonly property bool hasSubmenu: entry ? entry.hasSubmenu : false
                readonly property bool entryCheckable: entry ? entry.checkable : false
                readonly property bool entryChecked: entry ? entry.checked : false
                readonly property string entryText: entry ? entry.text : ""
                readonly property string entryIcon: entry ? entry.iconSource : ""
                readonly property string entryShortcut: entry ? entry.shortcut : ""
                readonly property bool showMark: entryCheckable && entryChecked
                readonly property real contentOpacity: entryEnabled ? 1.0 : 0.45

                objectName: row.isSeparatorEntry ? (row.nameStem + "Separator") : (row.nameStem + "Row")
                width: Math.max(row.minW, rowLeft.implicitWidth + row.rightWidth + 14)
                height: row.isSeparatorEntry ? 9 : row.rowH
                enabled: row.entryEnabled

                // right column: shortcut caption and/or submenu chevron
                readonly property int rightWidth: (entryShortcut.length > 0 || hasSubmenu) ? 26 : 0

                // ---- nested submenu ----
                // Created on demand, by URL: a composite type may not
                // instantiate itself statically (Qt rejects it with "Type
                // RibbonMenu is instantiated recursively", even inside a
                // Component), so Qt.createComponent resolves the sibling file
                // at runtime instead. Building it lazily also means a row
                // without a submenu allocates nothing.
                // Once opened it stays open until an entry is picked, a sibling
                // row is hovered, or this menu closes — closing it the instant
                // the pointer leaves the row would destroy it before the pointer
                // arrives, since the submenu is a separate overlay
                property QtObject subMenu: null
                function prefixedPath(childPath)
                {
                    var out = [index];
                    for (var i = 0; i < childPath.length; ++i) {
                        out.push(childPath[i]);
                    }
                    return out;
                }
                function createSubmenu()
                {
                    var comp = Qt.createComponent("RibbonMenu.qml", Component.PreferSynchronous, menuRoot);
                    if (!comp) {
                        return null;
                    }
                    if (comp.status !== Component.Ready) {
                        comp.destroy();
                        return null;
                    }
                    var m = comp.createObject(row);
                    comp.destroy();
                    if (!m) {
                        return null;
                    }
                    m.menuModel = Qt.binding(function() {
                        return row.entry ? row.entry.submenu : [];
                    });
                    m.namePrefix = menuRoot.namePrefix;
                    m.rowHeight = menuRoot.rowHeight;
                    m.minRowWidth = menuRoot.minRowWidth;
                    m.x = Qt.binding(function() {
                        return row.width;
                    });
                    m.y = 0;
                    m.itemActivated.connect(function(childPath) {
                        menuRoot.itemActivated(row.prefixedPath(childPath));
                    });
                    m.closed.connect(function() {
                        row.forgetSubmenu();
                    });
                    return m;
                }
                function openSubmenu()
                {
                    if (!row.entryEnabled) {
                        return;
                    }
                    if (!row.subMenu) {
                        row.subMenu = row.createSubmenu();
                        if (row.subMenu) {
                            menuRoot.pushSubRow(row);
                        }
                    }
                    if (row.subMenu && !row.subMenu.visible) {
                        row.subMenu.open();
                    }
                }
                function closeSubmenu()
                {
                    if (row.subMenu && row.subMenu.visible) {
                        // forgetSubmenu runs through the closed signal
                        row.subMenu.close();
                    }
                }
                function forgetSubmenu()
                {
                    menuRoot.popSubRow(row);
                    if (row.subMenu) {
                        row.subMenu.destroy();
                        row.subMenu = null;
                    }
                }

                // separator entry
                Rectangle {
                    visible: row.isSeparatorEntry
                    anchors.centerIn: parent
                    width: parent.width - 8
                    height: 1
                    color: RibbonTheme.separator
                }
                // hover / pressed background
                Rectangle {
                    visible: !row.isSeparatorEntry
                    anchors.fill: parent
                    anchors.margins: 1
                    radius: 3
                    color: rowMouse.pressed ? RibbonTheme.contentPressedBg
                           : (rowMouse.containsMouse ? RibbonTheme.contentHoverBg : RibbonTheme.contentBg)
                }

                // ---- left run: mark column, icon, caption ----
                Row {
                    id: rowLeft
                    visible: !row.isSeparatorEntry
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 6
                    spacing: 4

                    // check mark column; hidden (not zero-width) so the Row
                    // drops it from the run entirely for non-checkable menus
                    Item {
                        visible: row.markColumn
                        anchors.verticalCenter: parent.verticalCenter
                        width: 16
                        height: 16
                        Canvas {
                            visible: row.showMark
                            anchors.centerIn: parent
                            width: 12
                            height: 12
                            property color markColor: RibbonTheme.textColor
                            onMarkColorChanged: requestPaint()
                            opacity: row.contentOpacity
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                if (width <= 1 || height <= 1) {
                                    return;
                                }
                                var size = Math.min(width, height);
                                var sq = 2 * Math.floor(size / 2);
                                ctx.translate((width - size) / 2, (height - size) / 2);
                                ctx.strokeStyle = markColor;
                                ctx.lineWidth = 1.6;
                                ctx.beginPath();
                                // stroked tick, the same construction the button
                                // indicator chevron uses (no glyph dependency)
                                ctx.moveTo(Math.floor(sq * 0.18), Math.floor(sq * 0.52));
                                ctx.lineTo(Math.floor(sq * 0.40), Math.floor(sq * 0.74));
                                ctx.lineTo(Math.floor(sq * 0.84), Math.floor(sq * 0.24));
                                ctx.stroke();
                            }
                        }
                    }
                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        width: row.entryIcon.length > 0 ? 16 : 0
                        height: 16
                        source: row.entryIcon
                        fillMode: Image.PreserveAspectFit
                        opacity: row.contentOpacity
                    }
                    Text {
                        id: rowText
                        anchors.verticalCenter: parent.verticalCenter
                        text: row.entryText
                        color: RibbonTheme.textColor
                        opacity: row.contentOpacity
                    }
                }

                // ---- right run: shortcut caption, submenu chevron ----
                Text {
                    visible: row.entryShortcut.length > 0
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: row.hasSubmenu ? 24 : 8
                    text: row.entryShortcut
                    color: RibbonTheme.textColor
                    opacity: row.contentOpacity * 0.75
                }
                Canvas {
                    visible: row.hasSubmenu
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 6
                    width: 12
                    height: 12
                    property color arrowColor: RibbonTheme.textColor
                    onArrowColorChanged: requestPaint()
                    opacity: row.contentOpacity
                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.reset();
                        if (width <= 1 || height <= 1) {
                            return;
                        }
                        var size = Math.min(width, height);
                        var border = Math.floor(size / 4);
                        var sqsize = 2 * Math.floor(size / 2);
                        ctx.translate((width - size) / 2, (height - size) / 2);
                        ctx.strokeStyle = arrowColor;
                        ctx.lineWidth = 1.4;
                        ctx.beginPath();
                        // right-pointing chevron: the button indicator glyph
                        // rotated a quarter turn
                        ctx.moveTo(border, border);
                        ctx.lineTo(sqsize - border, sqsize / 2);
                        ctx.lineTo(border, sqsize - border);
                        ctx.stroke();
                    }
                }

                MouseArea {
                    id: rowMouse
                    visible: !row.isSeparatorEntry
                    anchors.fill: parent
                    hoverEnabled: true
                    enabled: row.entryEnabled
                    onClicked: {
                        if (row.hasSubmenu) {
                            row.openSubmenu();
                            return;
                        }
                        menuRoot.itemActivated([index]);
                    }
                    onContainsMouseChanged: {
                        if (!containsMouse) {
                            return;
                        }
                        menuRoot.closeOtherSubmenus(row);
                        if (row.hasSubmenu) {
                            row.openSubmenu();
                        }
                    }
                }
            }
        }
    }
}
