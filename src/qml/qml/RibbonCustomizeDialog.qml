import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import SARibbon 3.0

// Customize picker dialog (plan-04 WS-C3).
//
// The QML counterpart of the widgets SARibbonCustomizeDialog, written against
// the same vocabulary but not as a translation of its layout: the left pane is
// the command catalogue (tag filter + search + list), the middle pane adds and
// removes, the right pane is the ribbon tree preview with the scope radios and
// the structural editors. objectNames keep the widgets control names
// (pushButtonAdd, treeViewResult, ...) so a test written against one front end
// reads against the other.
//
// Editing never touches the ribbon: every button appends a
// Core::SARibbonCustomizeRecord to the bound RibbonCustomizer, the tree model
// replays those records into a shadow copy for the preview, and only OK calls
// apply(). Cancel drops the pending list. That is the widgets
// mCustomizeDatasCache / applyCustomize split.
//
// Palette rather than per-item colors: the popup publishes RibbonTheme tokens
// through its palette and the Controls items below inherit them, so the dialog
// re-themes with the ribbon and no literal color appears in this file.
//
// NOTE for tests (NOTES B59): this dialog owns popups, so it must be declared
// inside a container item, never as the QML root object.
Popup {
    id: dialog

    objectName: "customizeDialog"

    // == public API ==
    // The ribbon being customized. autoRegister runs against it on open
    property var bar: null
    // Command catalogue. Defaults to an internal instance so the dialog is
    // usable standalone; assign a shared registry to keep keys across dialogs
    property RibbonActionRegistry registry: defaultRegistry
    // Record producer/applier. Defaults to an internal instance
    property RibbonCustomizer customizer: defaultCustomizer
    // Tree scope, one of Ribbon.ShowAllCategory / ShowMainCategory /
    // ShowQuickAccessBar (widgets RibbonTreeShowType)
    property int showType: Ribbon.ShowAllCategory
    // Catalogue filter; Ribbon.UnknowActionTag lists every tag
    property int filterTag: Ribbon.UnknowActionTag
    // Row proportion a newly added command gets (widgets comboBoxActionProportion)
    property int proportion: Ribbon.Medium

    // Selected tree row, -1 when nothing is selected
    readonly property int selectedRow: treeViewResult.currentIndex
    // Selected catalogue row, -1 when nothing is selected
    readonly property int selectedActionRow: listViewSelect.currentIndex
    // Pending (not yet applied) record count, for the status caption
    readonly property int pendingCount: dialog.customizer ? dialog.customizer.recordCount : 0

    // Emitted after OK applied the records, with the apply() result
    signal acceptedWithResult(bool applied)
    // Emitted after Cancel dropped the pending records
    signal discarded()
    // Emitted when a record could not be produced (mirrors the widgets dialog
    // silently refusing an operation on an uncustomizable row)
    signal operationRefused(string reason)

    width: 820
    height: 560
    modal: true
    closePolicy: Popup.CloseOnEscape
    // centered in the declaring item rather than in Overlay.overlay: the dialog
    // also has to work inside a plain Item (tests run it offscreen with no
    // Controls overlay attached)
    x: dialog.parent ? Math.max(0, (dialog.parent.width - dialog.width) / 2) : 0
    y: dialog.parent ? Math.max(0, (dialog.parent.height - dialog.height) / 2) : 0

    palette.window: RibbonTheme.contentBg
    palette.windowText: RibbonTheme.textColor
    palette.base: RibbonTheme.contentBg
    palette.text: RibbonTheme.textColor
    palette.button: RibbonTheme.contentBg
    palette.buttonText: RibbonTheme.textColor
    palette.highlight: RibbonTheme.selectionBg
    palette.highlightedText: RibbonTheme.textColor
    palette.mid: RibbonTheme.borderColor
    palette.dark: RibbonTheme.borderColor
    palette.placeholderText: RibbonTheme.subtitle
    palette.toolTipBase: RibbonTheme.contentBg
    palette.toolTipText: RibbonTheme.textColor

    background: Rectangle {
        color: RibbonTheme.contentBg
        border.color: RibbonTheme.menuBorder
        radius: 4
    }

    // == internal defaults (overridable through the properties above) ==
    RibbonActionRegistry {
        id: defaultRegistry
    }
    RibbonCustomizer {
        id: defaultCustomizer
        bar: dialog.bar
        registry: dialog.registry
    }

    // == models ==
    RibbonActionRegistryModel {
        id: actionModel
        registry: dialog.registry
        filterTag: dialog.filterTag
        searchText: lineEditSearchAction.text
    }
    RibbonCustomizeTreeModel {
        id: treeModel
        bar: dialog.bar
        registry: dialog.registry
        customizer: dialog.customizer
        showType: dialog.showType
    }

    // == helpers ==
    // One RibbonMenu row description. Every key RibbonMenu.qml reads is
    // present, so a row never sees undefined (NOTES B60)
    function menuEntry(text, checkable, checked)
    {
        return {
            "text": text,
            "enabled": true,
            "checkable": checkable,
            "checked": checked,
            "separator": false,
            "hasSubmenu": false,
            "iconSource": "",
            "shortcut": "",
            "submenu": []
        };
    }
    // Tag filter rows: "Please Select" (all tags) then one row per registered
    // tag, in the order tagInfoList reports
    function tagMenuModel()
    {
        var out = [dialog.menuEntry(qsTr("Please Select"), true, dialog.filterTag === Ribbon.UnknowActionTag)];
        if (dialog.registry) {
            var list = dialog.registry.tagInfoList();
            for (var i = 0; i < list.length; ++i) {
                out.push(dialog.menuEntry(list[i].name + " (" + list[i].count + ")", true, dialog.filterTag === list[i].tag));
            }
        }
        return out;
    }
    function tagMenuTags()
    {
        var out = [Ribbon.UnknowActionTag];
        if (dialog.registry) {
            var list = dialog.registry.tagInfoList();
            for (var i = 0; i < list.length; ++i) {
                out.push(list[i].tag);
            }
        }
        return out;
    }
    function tagMenuCaption()
    {
        if (dialog.filterTag === Ribbon.UnknowActionTag) {
            return qsTr("Please Select");
        }
        var list = dialog.registry ? dialog.registry.tagInfoList() : [];
        for (var i = 0; i < list.length; ++i) {
            if (list[i].tag === dialog.filterTag) {
                return list[i].name;
            }
        }
        return qsTr("Please Select");
    }
    function proportionMenuModel()
    {
        return [dialog.menuEntry(qsTr("Large"), true, dialog.proportion === Ribbon.Large),
                dialog.menuEntry(qsTr("Medium"), true, dialog.proportion === Ribbon.Medium),
                dialog.menuEntry(qsTr("Small"), true, dialog.proportion === Ribbon.Small)];
    }
    function proportionCaption()
    {
        if (dialog.proportion === Ribbon.Large) {
            return qsTr("Large");
        }
        if (dialog.proportion === Ribbon.Small) {
            return qsTr("Small");
        }
        return qsTr("Medium");
    }
    // Address of the selected tree row; a full-key map even with no selection
    function selectedInfo()
    {
        // Two dependency reads, both load bearing. QML captures binding
        // dependencies dynamically, so a property read inside a called function
        // still counts, and every enabled binding below goes through here.
        // `revision` covers a model reset that kept the row count (a rename or a
        // visibility toggle), `count` covers one that changed it.
        var rev = treeModel.revision;
        var n = treeViewResult.count;
        return treeModel.infoAt(n < 0 ? -1 : treeViewResult.currentIndex);
    }
    // Structural editor availability, following the widgets level rules
    function canAdd()
    {
        if (listViewSelect.currentIndex < 0) {
            return false;
        }
        var info = selectedInfo();
        var nt = info.nodeType;
        return nt !== Ribbon.CategoryNode && nt !== Ribbon.QuickAccessNode;
    }
    function canRemove()
    {
        var info = selectedInfo();
        return info.canCustomize && info.nodeType !== Ribbon.QuickAccessNode;
    }
    function canRename()
    {
        var info = selectedInfo();
        return info.canCustomize
                && (info.nodeType === Ribbon.CategoryNode || info.nodeType === Ribbon.PanelNode);
    }
    function canMove(delta)
    {
        var info = selectedInfo();
        if (info.nodeType === Ribbon.QuickAccessNode) {
            return false;
        }
        return delta < 0 ? info.indexInParent > 0 : info.indexInParent < info.siblingCount - 1;
    }
    function canToggleVisible()
    {
        var info = selectedInfo();
        return info.nodeType === Ribbon.CategoryNode && !info.contextCategory;
    }
    // New group needs a category or a group target (widgets
    // onPushButtonNewPanelClicked refuses every other level)
    function canNewPanel()
    {
        var nt = selectedInfo().nodeType;
        return nt === Ribbon.CategoryNode || nt === Ribbon.PanelNode;
    }
    function refuse(reason)
    {
        dialog.operationRefused(reason);
        return false;
    }
    // Sequential title for a new node, the widgets
    // "new category[customize]%1" shape (String has no arg() in JS)
    function newCategoryTitle()
    {
        dialog.categorySerial += 1;
        return qsTr("new category[customize]%1").replace("%1", dialog.categorySerial);
    }
    function newPanelTitle()
    {
        dialog.panelSerial += 1;
        return qsTr("new panel[customize]%1").replace("%1", dialog.panelSerial);
    }

    // Title serials (widgets mCustomizeCategoryCount / mCustomizePanelCount)
    property int categorySerial: 0
    property int panelSerial: 0

    // == operations ==
    // Register the ribbon and refresh both panes (widgets setupRibbonBar)
    function setup()
    {
        if (dialog.bar && dialog.registry) {
            // the C++ autoRegister returns a pointer map QML cannot hold, so the
            // invokable wrapper reports the catalogue size instead
            dialog.registry.autoRegisterBar(dialog.bar);
        }
        if (dialog.customizer) {
            dialog.customizer.bar = dialog.bar;
            dialog.customizer.registry = dialog.registry;
        }
        actionModel.update();
        treeModel.update();
    }
    // Add the selected catalogue command at the selected tree row
    function addSelected()
    {
        var row = listViewSelect.currentIndex;
        if (row < 0 || !dialog.customizer) {
            return refuse("no command selected");
        }
        var key = actionModel.keyAt(row);
        if (key.length === 0) {
            return refuse("empty command key");
        }
        var info = selectedInfo();
        var nt = info.nodeType;
        if (nt === Ribbon.QuickAccessNode || nt === Ribbon.QuickAccessActionNode) {
            // issue #67 parity: the quick access scope appends to the title row
            return dialog.keepSelection(info, dialog.customizer.addQuickAction(key, -1));
        }
        if (nt === Ribbon.CategoryNode) {
            // widgets refuses a category target: a command lives in a panel
            return refuse("select a group or a command");
        }
        return dialog.keepSelection(info,
                                    dialog.customizer.addAction(key,
                                                                dialog.proportion,
                                                                info.categoryObjName,
                                                                info.panelObjName));
    }
    // Put the tree selection back on the row carrying this address and hand
    // through `ok`. Every record resets the preview model, and a ListView that
    // survives a reset with its old row number ends up pointing at whatever
    // moved into that slot, so the address is what has to be followed.
    function keepSelection(info, ok)
    {
        if (!ok) {
            return false;
        }
        var row = -1;
        if (info.nodeType === Ribbon.CategoryNode) {
            row = treeModel.rowOfCategory(info.categoryObjName);
        } else if (info.nodeType === Ribbon.PanelNode) {
            row = treeModel.rowOfPanel(info.categoryObjName, info.panelObjName);
        } else if (info.nodeType === Ribbon.ActionNode) {
            row = treeModel.rowOfAction(info.categoryObjName, info.panelObjName, info.key);
        } else if (info.nodeType === Ribbon.QuickAccessActionNode) {
            row = treeModel.rowOfQuickAction(info.key);
        }
        if (row >= 0) {
            treeViewResult.currentIndex = row;
            treeViewResult.positionViewAtIndex(row, ListView.Contain);
        }
        return true;
    }
    // Remove the selected tree row
    function removeSelected()
    {
        if (!dialog.customizer) {
            return refuse("no customizer");
        }
        var info = selectedInfo();
        if (!info.canCustomize) {
            return refuse("row is not customizable");
        }
        var nt = info.nodeType;
        if (nt === Ribbon.CategoryNode) {
            return dialog.customizer.removeCategory(info.categoryObjName);
        }
        if (nt === Ribbon.PanelNode) {
            return dialog.customizer.removePanel(info.categoryObjName, info.panelObjName);
        }
        if (nt === Ribbon.ActionNode) {
            return dialog.customizer.removeAction(info.categoryObjName, info.panelObjName, info.key);
        }
        if (nt === Ribbon.QuickAccessActionNode) {
            return dialog.customizer.removeQuickAction(info.key);
        }
        return refuse("row cannot be removed");
    }
    // New category after the selected top level row, else appended
    function newCategory()
    {
        if (!dialog.customizer) {
            return refuse("no customizer");
        }
        var info = selectedInfo();
        var index = info.nodeType === Ribbon.CategoryNode ? info.indexInParent + 1 : -1;
        return dialog.customizer.addCategory(newCategoryTitle(), index, "");
    }
    // New group in the selected category, or after the selected group
    function newPanel()
    {
        if (!dialog.customizer) {
            return refuse("no customizer");
        }
        var info = selectedInfo();
        if (info.nodeType === Ribbon.CategoryNode) {
            return dialog.customizer.addPanel(newPanelTitle(), -1, info.categoryObjName, "");
        }
        if (info.nodeType === Ribbon.PanelNode) {
            return dialog.customizer.addPanel(newPanelTitle(),
                                              info.indexInParent + 1,
                                              info.categoryObjName,
                                              "");
        }
        return refuse("select a category or a group");
    }
    // Rename a category or a group (widgets refuses actions)
    function renameSelected(newName)
    {
        if (!dialog.customizer) {
            return refuse("no customizer");
        }
        if (!newName || newName.length === 0) {
            return refuse("empty name");
        }
        var info = selectedInfo();
        if (info.nodeType === Ribbon.CategoryNode) {
            return dialog.customizer.renameCategory(newName, info.categoryObjName);
        }
        if (info.nodeType === Ribbon.PanelNode) {
            return dialog.customizer.renamePanel(newName, info.categoryObjName, info.panelObjName);
        }
        return refuse("only a category or a group can be renamed");
    }
    // Relative move within the parent, refused at the ends (no clamping)
    function moveSelected(delta)
    {
        if (!dialog.customizer) {
            return refuse("no customizer");
        }
        var info = selectedInfo();
        var nt = info.nodeType;
        if (nt === Ribbon.CategoryNode) {
            return dialog.keepSelection(info, dialog.customizer.changeCategoryOrder(info.categoryObjName, delta));
        }
        if (nt === Ribbon.PanelNode) {
            return dialog.keepSelection(info,
                                        dialog.customizer.changePanelOrder(info.categoryObjName,
                                                                           info.panelObjName,
                                                                           delta));
        }
        if (nt === Ribbon.ActionNode) {
            return dialog.keepSelection(info,
                                        dialog.customizer.changeActionOrder(info.categoryObjName,
                                                                            info.panelObjName,
                                                                            info.key,
                                                                            delta));
        }
        if (nt === Ribbon.QuickAccessActionNode) {
            return dialog.keepSelection(info, dialog.customizer.changeQuickActionOrder(info.key, delta));
        }
        return refuse("row cannot be moved");
    }
    // Show/hide a main category (widgets checkbox on the tree item)
    function toggleSelectedVisible()
    {
        if (!dialog.customizer) {
            return refuse("no customizer");
        }
        var info = selectedInfo();
        if (info.nodeType !== Ribbon.CategoryNode || info.contextCategory) {
            return refuse("only a main category can be hidden");
        }
        return dialog.customizer.visibleCategory(info.categoryObjName, !info.nodeVisible);
    }
    // Drop every pending record (widgets pushButtonReset)
    function resetAll()
    {
        if (dialog.customizer) {
            dialog.customizer.clearRecords();
        }
    }
    // OK: apply and close
    function accept()
    {
        var applied = dialog.customizer ? dialog.customizer.apply() : false;
        dialog.close();
        dialog.acceptedWithResult(applied);
        return applied;
    }
    // Cancel: drop the pending records and close
    function reject()
    {
        resetAll();
        dialog.close();
        dialog.discarded();
    }

    // Re-read the ribbon when the dialog is shown, so a tree changed elsewhere
    // in the meantime shows up (widgets updateModel on show)
    onAboutToShow: {
        dialog.setup();
        tagMenuButton.text = qsTr("Please Select") + ": " + dialog.tagMenuCaption();
    }

    // == rename prompt (the QInputDialog counterpart) ==
    Popup {
        id: renamePopup
        objectName: "renamePopup"
        width: 300
        height: 96
        modal: true
        parent: dialog.contentItem
        x: Math.max(0, (parent.width - width) / 2)
        y: Math.max(0, (parent.height - height) / 2)
        palette.window: RibbonTheme.contentBg
        palette.base: RibbonTheme.contentBg
        palette.text: RibbonTheme.textColor
        palette.button: RibbonTheme.contentBg
        palette.buttonText: RibbonTheme.textColor
        background: Rectangle {
            color: RibbonTheme.contentBg
            border.color: RibbonTheme.menuBorder
            radius: 4
        }
        property string originalText: ""
        // Open the prompt over the selected row (widgets onPushButtonRenameClicked)
        function prompt()
        {
            if (!dialog.canRename()) {
                dialog.refuse("only a category or a group can be renamed");
                return;
            }
            var info = dialog.selectedInfo();
            renameField.text = info.title;
            originalText = info.title;
            open();
            renameField.forceActiveFocus();
        }
        contentItem: ColumnLayout {
            spacing: 8
            Text {
                text: qsTr("name:")
                color: RibbonTheme.textColor
            }
            TextField {
                id: renameField
                objectName: "renameField"
                Layout.fillWidth: true
                selectByMouse: true
                onAccepted: renamePopup.commit()
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                Item {
                    Layout.fillWidth: true
                }
                Button {
                    objectName: "renameOkButton"
                    text: qsTr("OK")
                    onClicked: renamePopup.commit()
                }
                Button {
                    objectName: "renameCancelButton"
                    text: qsTr("Cancel")
                    onClicked: renamePopup.close()
                }
            }
        }
        function commit()
        {
            var t = renameField.text;
            close();
            if (t.length > 0 && t !== originalText) {
                dialog.renameSelected(t);
            }
        }
    }

    // == tag filter menu ==
    RibbonMenu {
        id: tagMenu
        objectName: "tagMenu"
        namePrefix: "tagMenu"
        minRowWidth: 180
        parent: tagMenuButton
        y: tagMenuButton.height
        onAboutToShow: menuModel = dialog.tagMenuModel()
        onItemActivated: {
            var tags = dialog.tagMenuTags();
            var i = indexPath[0];
            if (i >= 0 && i < tags.length) {
                dialog.filterTag = tags[i];
                tagMenuButton.text = qsTr("Please Select") + ": " + dialog.tagMenuCaption();
            }
            close();
        }
    }

    // == proportion menu ==
    RibbonMenu {
        id: proportionMenu
        objectName: "proportionMenu"
        namePrefix: "proportionMenu"
        minRowWidth: 120
        parent: proportionButton
        x: proportionButton.width - width
        y: proportionButton.height
        onAboutToShow: menuModel = dialog.proportionMenuModel()
        onItemActivated: {
            var values = [Ribbon.Large, Ribbon.Medium, Ribbon.Small];
            var i = indexPath[0];
            if (i >= 0 && i < values.length) {
                dialog.proportion = values[i];
                proportionButton.text = qsTr("proportion:") + " " + dialog.proportionCaption();
            }
            close();
        }
    }

    contentItem: ColumnLayout {
        spacing: 8

        // ---- title ----
        Text {
            objectName: "dialogTitle"
            text: qsTr("Customize Dialog")
            color: RibbonTheme.textColor
            font.pixelSize: 14
            font.bold: true
        }

        // ---- three panes ----
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8

            // == left: command catalogue ==
            ColumnLayout {
                Layout.preferredWidth: 240
                Layout.fillHeight: true
                spacing: 6

                Button {
                    id: tagMenuButton
                    objectName: "comboBoxActionIndex"
                    Layout.fillWidth: true
                    text: qsTr("Please Select")
                    onClicked: tagMenu.open()
                }
                TextField {
                    id: lineEditSearchAction
                    objectName: "lineEditSearchAction"
                    Layout.fillWidth: true
                    placeholderText: qsTr("search")
                    selectByMouse: true
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: RibbonTheme.contentBg
                    border.color: RibbonTheme.inputBorder
                    ListView {
                        id: listViewSelect
                        objectName: "listViewSelect"
                        anchors.fill: parent
                        anchors.margins: 1
                        clip: true
                        model: actionModel
                        currentIndex: -1
                        delegate: Rectangle {
                            id: actionRow
                            objectName: "actionRow"
                            // B48 single-dependency shape: hold the list view
                            // once so the bindings below never re-read a null id
                            readonly property var view: ListView.view
                            readonly property int rowH: 22
                            width: actionRow.view ? actionRow.view.width : 100
                            height: actionRow.rowH
                            color: ListView.isCurrentItem ? RibbonTheme.selectionBg
                                                            : (actionMouse.containsMouse ? RibbonTheme.contentHoverBg
                                                                                         : RibbonTheme.contentBg)
                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 6
                                anchors.rightMargin: 6
                                spacing: 4
                                Image {
                                    Layout.preferredWidth: 16
                                    Layout.preferredHeight: 16
                                    source: iconSource
                                    fillMode: Image.PreserveAspectFit
                                    visible: iconSource.length > 0
                                }
                                Text {
                                    objectName: "actionRowText"
                                    Layout.fillWidth: true
                                    text: model.text
                                    color: RibbonTheme.textColor
                                    elide: Text.ElideRight
                                    font.pixelSize: 12
                                }
                            }
                            MouseArea {
                                id: actionMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: listViewSelect.currentIndex = index
                            }
                        }
                    }
                }
            }

            // == middle: add / remove / proportion ==
            ColumnLayout {
                Layout.preferredWidth: 96
                Layout.fillHeight: true
                spacing: 6
                Item {
                    Layout.preferredHeight: 60
                }
                Button {
                    id: pushButtonAdd
                    objectName: "pushButtonAdd"
                    Layout.fillWidth: true
                    text: qsTr("Add >>")
                    enabled: dialog.canAdd()
                    onClicked: dialog.addSelected()
                }
                Button {
                    id: pushButtonDelete
                    objectName: "pushButtonDelete"
                    Layout.fillWidth: true
                    text: qsTr("<< Remove")
                    enabled: dialog.canRemove()
                    onClicked: dialog.removeSelected()
                }
                Item {
                    Layout.preferredHeight: 12
                }
                Button {
                    id: proportionButton
                    objectName: "comboBoxActionProportion"
                    Layout.fillWidth: true
                    text: qsTr("proportion:") + " " + dialog.proportionCaption()
                    onClicked: proportionMenu.open()
                }
                Item {
                    Layout.fillHeight: true
                }
            }

            // == right: ribbon tree ==
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 6

                // scope radios (widgets radioButtonMainCategory / AllCategory /
                // QuickAccessBar); plain checkable buttons, exclusivity comes
                // from the single showType property they all write
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6
                    Button {
                        id: radioButtonMainCategory
                        objectName: "radioButtonMainCategory"
                        text: qsTr("Main Category")
                        checkable: true
                        checked: dialog.showType === Ribbon.ShowMainCategory
                        onClicked: dialog.showType = Ribbon.ShowMainCategory
                    }
                    Button {
                        id: radioButtonAllCategory
                        objectName: "radioButtonAllCategory"
                        text: qsTr("All Category")
                        checkable: true
                        checked: dialog.showType === Ribbon.ShowAllCategory
                        onClicked: dialog.showType = Ribbon.ShowAllCategory
                    }
                    Button {
                        id: radioButtonQuickAccessBar
                        objectName: "radioButtonQuickAccessBar"
                        text: qsTr("Quick Access Bar")
                        checkable: true
                        checked: dialog.showType === Ribbon.ShowQuickAccessBar
                        onClicked: dialog.showType = Ribbon.ShowQuickAccessBar
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: RibbonTheme.contentBg
                    border.color: RibbonTheme.inputBorder
                    ListView {
                        id: treeViewResult
                        objectName: "treeViewResult"
                        anchors.fill: parent
                        anchors.margins: 1
                        clip: true
                        model: treeModel
                        currentIndex: -1
                        // A model reset drops the selection; keep it only when
                        // the row still exists
                        onCountChanged: {
                            if (currentIndex >= count) {
                                currentIndex = -1;
                            }
                        }
                        delegate: Rectangle {
                            id: treeRow
                            objectName: "treeRow"
                            readonly property var view: ListView.view
                            readonly property int indent: 8 + depth * 16
                            width: treeRow.view ? treeRow.view.width : 200
                            height: 22
                            color: ListView.isCurrentItem ? RibbonTheme.selectionBg
                                                            : (treeMouse.containsMouse ? RibbonTheme.contentHoverBg
                                                                                       : RibbonTheme.contentBg)
                            // dim a category a pending record hid, so the preview
                            // reads as the post-apply state
                            opacity: nodeVisible ? 1.0 : 0.45
                            // icon + caption, anchored rather than laid out in a
                            // Row: a Row positions children from their width, so
                            // a caption whose width derives from its own x would
                            // close the loop
                            Image {
                                id: treeRowIcon
                                anchors.left: parent.left
                                anchors.leftMargin: treeRow.indent
                                anchors.verticalCenter: parent.verticalCenter
                                width: iconSource.length > 0 ? 16 : 0
                                height: 16
                                source: iconSource
                                fillMode: Image.PreserveAspectFit
                            }
                            Text {
                                id: treeRowText
                                objectName: "treeRowText"
                                anchors.left: treeRowIcon.right
                                anchors.leftMargin: 4
                                anchors.right: parent.right
                                anchors.rightMargin: 6 + (pending ? 34 : 0)
                                                     + (nodeType === Ribbon.CategoryNode && !contextCategory ? 20 : 0)
                                anchors.verticalCenter: parent.verticalCenter
                                text: title
                                color: RibbonTheme.textColor
                                elide: Text.ElideRight
                                font.pixelSize: 12
                                font.bold: depth === 0
                            }
                            // "new" marker: a row a pending record created
                            Rectangle {
                                visible: pending
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.right: parent.right
                                anchors.rightMargin: 6
                                width: pendingText.width + 6
                                height: 14
                                radius: 2
                                color: RibbonTheme.contentHoverBg
                                Text {
                                    id: pendingText
                                    anchors.centerIn: parent
                                    text: qsTr("new")
                                    color: RibbonTheme.subtitle
                                    font.pixelSize: 10
                                }
                            }
                            // visibility checkbox on main categories only
                            Rectangle {
                                id: visibleBox
                                // above treeMouse, which is declared last and
                                // would otherwise swallow the click
                                z: 2
                                visible: nodeType === Ribbon.CategoryNode && !contextCategory
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.right: parent.right
                                anchors.rightMargin: pending ? 40 : 6
                                width: 12
                                height: 12
                                radius: 2
                                color: RibbonTheme.contentBg
                                border.color: RibbonTheme.inputBorder
                                border.width: 1
                                Rectangle {
                                    visible: nodeVisible
                                    anchors.centerIn: parent
                                    width: 6
                                    height: 6
                                    color: RibbonTheme.accent
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    anchors.margins: -3
                                    onClicked: {
                                        treeViewResult.currentIndex = index;
                                        dialog.toggleSelectedVisible();
                                    }
                                }
                            }
                            MouseArea {
                                id: treeMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: treeViewResult.currentIndex = index
                            }
                        }
                    }
                }

                // structural editors
                GridLayout {
                    Layout.fillWidth: true
                    columns: 4
                    rowSpacing: 4
                    columnSpacing: 4
                    Button {
                        id: pushButtonNewCategory
                        objectName: "pushButtonNewCategory"
                        Layout.fillWidth: true
                        text: qsTr("New Category")
                        onClicked: dialog.newCategory()
                    }
                    Button {
                        id: pushButtonNewPanel
                        objectName: "pushButtonNewPanel"
                        Layout.fillWidth: true
                        text: qsTr("New Group")
                        enabled: dialog.canNewPanel()
                        onClicked: dialog.newPanel()
                    }
                    Button {
                        id: pushButtonRename
                        objectName: "pushButtonRename"
                        Layout.fillWidth: true
                        text: qsTr("Rename")
                        enabled: dialog.canRename()
                        onClicked: renamePopup.prompt()
                    }
                    Button {
                        id: pushButtonReset
                        objectName: "pushButtonReset"
                        Layout.fillWidth: true
                        text: qsTr("reset")
                        enabled: dialog.pendingCount > 0
                        onClicked: dialog.resetAll()
                    }
                    Button {
                        id: pushButtonUp
                        objectName: "pushButtonUp"
                        Layout.fillWidth: true
                        text: qsTr("Up")
                        enabled: dialog.canMove(-1)
                        onClicked: dialog.moveSelected(-1)
                    }
                    Button {
                        id: pushButtonDown
                        objectName: "pushButtonDown"
                        Layout.fillWidth: true
                        text: qsTr("Down")
                        enabled: dialog.canMove(1)
                        onClicked: dialog.moveSelected(1)
                    }
                    Button {
                        id: pushButtonHide
                        objectName: "pushButtonHide"
                        Layout.fillWidth: true
                        text: dialog.selectedInfo().nodeVisible ? qsTr("Hide") : qsTr("Show")
                        enabled: dialog.canToggleVisible()
                        onClicked: dialog.toggleSelectedVisible()
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                }
            }
        }

        // ---- footer ----
        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Text {
                id: pendingCaption
                objectName: "pendingCaption"
                Layout.fillWidth: true
                text: qsTr("pending changes: %1").replace("%1", dialog.pendingCount)
                color: RibbonTheme.subtitle
                font.pixelSize: 11
            }
            Button {
                id: pushButtonOk
                objectName: "pushButtonOk"
                text: qsTr("OK")
                enabled: dialog.pendingCount > 0
                onClicked: dialog.accept()
            }
            Button {
                id: pushButtonCancel
                objectName: "pushButtonCancel"
                text: qsTr("Cancel")
                onClicked: dialog.reject()
            }
        }
    }
}
