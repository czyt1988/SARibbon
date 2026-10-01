import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import SARibbon 3.0

// Mirrors the widgets MainWindowExample main scene: three categories with
// mixed large/small buttons (icons!), theme switching and click feedback.
// Tabs are auto-generated from the category titles (addCategoryPage parity).
ApplicationWindow {
    id: window
    width: 1300
    height: 460
    visible: true
    title: "SARibbon QML Example"

    RibbonBar {
        id: ribbonBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        applicationLabel: "File"
        onApplicationButtonClicked: feedback.text = qsTr("application button clicked")

        // ---- Home ----
        RibbonCategory {
            title: "Home"

            RibbonPanel {
                panelTitle: "Clipboard"
                RibbonToolButton {
                    text: "Paste"
                    iconSource: "qrc:/icon/icon/folder-checkmark.svg"
                    proportion: Ribbon.Large
                    onClicked: feedback.text = qsTr("Paste clicked")
                }
                RibbonToolButton {
                    text: "Cut"
                    iconSource: "qrc:/icon/icon/delete.svg"
                    proportion: Ribbon.Small
                    onClicked: feedback.text = qsTr("Cut clicked")
                }
                RibbonToolButton {
                    text: "Copy"
                    iconSource: "qrc:/icon/icon/item.svg"
                    proportion: Ribbon.Small
                    onClicked: feedback.text = qsTr("Copy clicked")
                }
            }

            RibbonPanel {
                panelTitle: "Font"
                RibbonToolButton {
                    text: "Bold"
                    iconSource: "qrc:/icon/icon/bold.svg"
                    proportion: Ribbon.Medium
                    checkable: true
                    onClicked: feedback.text = qsTr("Bold toggled: %1").arg(checked)
                }
                RibbonToolButton {
                    text: "Italic"
                    iconSource: "qrc:/icon/icon/Italic.svg"
                    proportion: Ribbon.Small
                    checkable: true
                    onClicked: feedback.text = qsTr("Italic toggled: %1").arg(checked)
                }
                RibbonToolButton {
                    text: "Underline"
                    iconSource: "qrc:/icon/icon/Underline.svg"
                    proportion: Ribbon.Small
                    checkable: true
                    onClicked: feedback.text = qsTr("Underline toggled: %1").arg(checked)
                }
            }

            RibbonPanel {
                panelTitle: "Edit"
                RibbonToolButton {
                    text: "Undo"
                    iconSource: "qrc:/icon/icon/undo.svg"
                    proportion: Ribbon.Large
                    onClicked: feedback.text = qsTr("Undo clicked")
                }
                RibbonToolButton {
                    text: "Redo"
                    iconSource: "qrc:/icon/icon/redo.svg"
                    proportion: Ribbon.Medium
                    onClicked: feedback.text = qsTr("Redo clicked")
                }
            }

            // Mirrors the widgets "button states" demo: every large button
            // state (normal / checked / disabled / checkable / long text /
            // very short text) side by side.
            RibbonPanel {
                panelTitle: "button states"

                RibbonToolButton {
                    text: "Normal"
                    iconSource: "qrc:/icon/icon/file.svg"
                    proportion: Ribbon.Large
                    onClicked: feedback.text = qsTr("Normal clicked")
                }
                RibbonToolButton {
                    text: "Checked"
                    iconSource: "qrc:/icon/icon/enableTest.svg"
                    proportion: Ribbon.Large
                    checkable: true
                    checked: true
                    onClicked: feedback.text = qsTr("Checked toggled: %1").arg(checked)
                }
                RibbonToolButton {
                    id: disableButton
                    text: "Disabled"
                    iconSource: "qrc:/icon/icon/disable.svg"
                    proportion: Ribbon.Large
                    enabled: false
                    onClicked: feedback.text = qsTr("this click must never fire")
                }
                RibbonToolButton {
                    text: "unlock"
                    iconSource: "qrc:/icon/icon/unlock.svg"
                    proportion: Ribbon.Large
                    onClicked: {
                        disableButton.enabled = true;
                        disableButton.text = "Enabled";
                        feedback.text = qsTr("disabled button unlocked");
                    }
                }
                RibbonToolButton {
                    text: "very long text in a button, balabalabala etc"
                    iconSource: "qrc:/icon/icon/long-text.svg"
                    proportion: Ribbon.Large
                    onClicked: feedback.text = qsTr("long text clicked")
                }
                RibbonToolButton {
                    text: "1"
                    iconSource: "qrc:/icon/icon/setText.svg"
                    proportion: Ribbon.Large
                    toolTip: "very short string"
                    onClicked: feedback.text = qsTr("short text clicked")
                }
            }

            // Mirrors the widgets "sa ribbon toolbutton style" panel: the
            // three popup modes (MenuButtonPopup splits action/menu zones,
            // InstantPopup is menu-only, DelayedPopup opens on press-hold),
            // checkable variants and a disabled button with a menu.
            RibbonPanel {
                panelTitle: "toolbutton style"

                RibbonToolButton {
                    text: "test 1"
                    iconSource: "qrc:/icon/icon/test1.svg"
                    proportion: Ribbon.Small
                    toolTip: "use MenuButtonPopup mode: the trailing arrow opens the menu, the icon clicks"
                    popupMode: Ribbon.MenuButtonPopup
                    menuItems: [
                        RibbonMenuItem { text: "item 1"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { text: "item 2"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { text: "item 3"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { separator: true },
                        RibbonMenuItem { text: "item 4"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { text: "item 5"; iconSource: "qrc:/icon/icon/item.svg" }
                    ]
                    onClicked: feedback.text = qsTr("test 1 action zone clicked")
                    onMenuTriggered: feedback.text = qsTr("test 1 menu: %1").arg(item.text)
                }
                RibbonToolButton {
                    text: "test 2"
                    iconSource: "qrc:/icon/icon/test2.svg"
                    proportion: Ribbon.Small
                    toolTip: "use InstantPopup mode: the whole button opens the menu"
                    popupMode: Ribbon.InstantPopup
                    menuItems: [
                        RibbonMenuItem { text: "item 1"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { text: "item 2"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { separator: true },
                        RibbonMenuItem { text: "item 3"; iconSource: "qrc:/icon/icon/item.svg" }
                    ]
                    onMenuTriggered: feedback.text = qsTr("test 2 menu: %1").arg(item.text)
                }
                RibbonToolButton {
                    text: "Delayed\nPopup"
                    iconSource: "qrc:/icon/icon/folder-cog.svg"
                    proportion: Ribbon.Large
                    popupMode: Ribbon.DelayedPopup
                    menuItems: [
                        RibbonMenuItem { text: "item 1"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { text: "item 2"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { text: "item 3"; iconSource: "qrc:/icon/icon/item.svg" }
                    ]
                    onClicked: feedback.text = qsTr("Delayed Popup clicked (press and hold opens the menu)")
                    onMenuTriggered: feedback.text = qsTr("Delayed Popup menu: %1").arg(item.text)
                }
                RibbonToolButton {
                    text: "Menu Button Popup"
                    iconSource: "qrc:/icon/icon/folder-star.svg"
                    proportion: Ribbon.Large
                    popupMode: Ribbon.MenuButtonPopup
                    menuItems: [
                        RibbonMenuItem { text: "item 1"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { text: "item 2"; iconSource: "qrc:/icon/icon/item.svg" }
                    ]
                    onClicked: feedback.text = qsTr("Menu Button Popup action zone clicked")
                    onMenuTriggered: feedback.text = qsTr("Menu Button Popup menu: %1").arg(item.text)
                }
                RibbonToolButton {
                    text: "Instant Popup"
                    iconSource: "qrc:/icon/icon/folder-stats.svg"
                    proportion: Ribbon.Large
                    popupMode: Ribbon.InstantPopup
                    menuItems: [
                        RibbonMenuItem { text: "item 1"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { text: "item 2"; iconSource: "qrc:/icon/icon/item.svg" }
                    ]
                    onMenuTriggered: feedback.text = qsTr("Instant Popup menu: %1").arg(item.text)
                }
                RibbonToolButton {
                    text: "Delayed Popup checkable"
                    iconSource: "qrc:/icon/icon/folder-table.svg"
                    proportion: Ribbon.Large
                    checkable: true
                    popupMode: Ribbon.DelayedPopup
                    menuItems: [
                        RibbonMenuItem { text: "item 1"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { text: "item 2"; iconSource: "qrc:/icon/icon/item.svg" }
                    ]
                    onClicked: feedback.text = qsTr("Delayed Popup checkable toggled: %1").arg(checked)
                    onMenuTriggered: feedback.text = qsTr("Delayed Popup checkable menu: %1").arg(item.text)
                }
                RibbonToolButton {
                    text: "Menu Button Popup checkable"
                    iconSource: "qrc:/icon/icon/folder-checkmark.svg"
                    proportion: Ribbon.Large
                    checkable: true
                    popupMode: Ribbon.MenuButtonPopup
                    menuItems: [
                        RibbonMenuItem { text: "item 1"; iconSource: "qrc:/icon/icon/item.svg" },
                        RibbonMenuItem { text: "item 2"; iconSource: "qrc:/icon/icon/item.svg" }
                    ]
                    onClicked: feedback.text = qsTr("Menu Button Popup checkable toggled: %1").arg(checked)
                    onMenuTriggered: feedback.text = qsTr("Menu Button Popup checkable menu: %1").arg(item.text)
                }
            }

            // Mirrors the widgets "widget test" panel: arbitrary controls
            // embedded into the ribbon panel through RibbonControlContainer.
            RibbonPanel {
                panelTitle: "widget test"

                RibbonControlContainer {
                    text: "ComboBox:"
                    proportion: Ribbon.Small
                    control: ComboBox {
                        editable: true
                        model: [
                            "testItem 1", "testItem 2", "testItem 3", "testItem 4", "testItem 5",
                            "testItem 6", "testItem 7", "testItem 8", "testItem 9", "testItem 10"
                        ]
                        onActivated: feedback.text = qsTr("ComboBox selected: %1").arg(currentText)
                    }
                }
                RibbonControlContainer {
                    text: "ComboBox2:"
                    proportion: Ribbon.Small
                    control: ComboBox {
                        model: [ "option 1", "option 2", "option 3" ]
                        onActivated: feedback.text = qsTr("ComboBox2 selected: %1").arg(currentText)
                    }
                }
                RibbonControlContainer {
                    text: "Line Edit:"
                    proportion: Ribbon.Small
                    control: TextField {
                        placeholderText: qsTr("type and press Enter")
                        onEditingFinished: feedback.text = qsTr("Line Edit: %1").arg(text)
                    }
                }
                RibbonControlContainer {
                    text: "CheckBox:"
                    proportion: Ribbon.Small
                    control: CheckBox {
                        onToggled: feedback.text = qsTr("CheckBox toggled: %1").arg(checked)
                    }
                }
                RibbonControlContainer {
                    text: "SpinBox:"
                    proportion: Ribbon.Small
                    control: SpinBox {
                        onValueModified: feedback.text = qsTr("SpinBox value: %1").arg(value)
                    }
                }
            }
        }

        // ---- Insert ----
        RibbonCategory {
            title: "Insert"

            RibbonPanel {
                panelTitle: "File"
                RibbonToolButton {
                    text: "New File"
                    iconSource: "qrc:/icon/icon/file.svg"
                    proportion: Ribbon.Large
                    onClicked: feedback.text = qsTr("New File clicked")
                }
                RibbonToolButton {
                    text: "Open"
                    iconSource: "qrc:/icon/icon/chinese-char.svg"
                    proportion: Ribbon.Medium
                    onClicked: feedback.text = qsTr("Open clicked")
                }
            }

            RibbonPanel {
                panelTitle: "Layout"
                RibbonToolButton {
                    text: "Layout"
                    iconSource: "qrc:/icon/icon/layout.svg"
                    proportion: Ribbon.Large
                    onClicked: feedback.text = qsTr("Layout clicked")
                }
            }
        }

        // ---- Design ----
        RibbonCategory {
            title: "Design"

            RibbonPanel {
                panelTitle: "Save"
                RibbonToolButton {
                    text: "Save"
                    iconSource: "qrc:/icon/icon/save.svg"
                    proportion: Ribbon.Large
                    onClicked: feedback.text = qsTr("Save clicked")
                }
            }

            // Mirrors the widgets "Context Category" panel: checkable toggles
            // for the two context categories declared below (active drives
            // the colored tabs + band, NOT the item visible)
            RibbonPanel {
                panelTitle: "Context Category"

                RibbonToolButton {
                    text: "Context Category 1"
                    iconSource: "qrc:/icon/icon/ContextCategory.svg"
                    proportion: Ribbon.Large
                    checkable: true
                    // implicit parameter injection keeps Qt 5.12 compatibility
                    // (Qt 6.5+ only deprecates it with a warning)
                    onToggled: {
                        contextCategory1.active = checked;
                        feedback.text = qsTr("context category 1 active: %1").arg(checked);
                    }
                }
                RibbonToolButton {
                    text: "Context Category 2"
                    iconSource: "qrc:/icon/icon/ContextCategory.svg"
                    proportion: Ribbon.Large
                    checkable: true
                    onToggled: {
                        contextCategory2.active = checked;
                        feedback.text = qsTr("context category 2 active: %1").arg(checked);
                    }
                }
            }
        }

        // ---- Other ----
        RibbonCategory {
            title: "Other"

            // Mirrors the widgets "panel one" gallery: two groups (Files +
            // Apps), stretchFactor lets the gallery absorb the panel's extra
            // width (core panel engine weighted distribution)
            RibbonPanel {
                panelTitle: "gallery"

                RibbonGallery {
                    id: gallery
                    stretchFactor: 1

                    RibbonGalleryGroup {
                        groupTitle: "Files"
                        RibbonGalleryItem { text: "Document File"; iconSource: "qrc:/icon/icon/gallery/Document-File.svg" }
                        RibbonGalleryItem { text: "Download File"; iconSource: "qrc:/icon/icon/gallery/Download-File.svg" }
                        RibbonGalleryItem { text: "Drive File Four Word"; iconSource: "qrc:/icon/icon/gallery/Drive-File.svg" }
                        RibbonGalleryItem { text: "Dropbox File"; iconSource: "qrc:/icon/icon/gallery/Dropbox-File.svg" }
                        RibbonGalleryItem { text: "Email File"; iconSource: "qrc:/icon/icon/gallery/Email-File.svg" }
                        RibbonGalleryItem { text: "Encode File"; iconSource: "qrc:/icon/icon/gallery/Encode-File.svg" }
                        RibbonGalleryItem { text: "Favorit File"; iconSource: "qrc:/icon/icon/gallery/Favorit-File.svg" }
                        RibbonGalleryItem { text: "File Error"; iconSource: "qrc:/icon/icon/gallery/File-Error.svg" }
                        RibbonGalleryItem { text: "File Read Only"; iconSource: "qrc:/icon/icon/gallery/File-Readonly.svg" }
                        RibbonGalleryItem { text: "File Settings"; iconSource: "qrc:/icon/icon/gallery/File-Settings.svg" }
                        RibbonGalleryItem { text: "Presentation File"; iconSource: "qrc:/icon/icon/gallery/Presentation-File.svg" }
                    }
                    RibbonGalleryGroup {
                        groupTitle: "Apps"
                        RibbonGalleryItem { text: "Photoshop"; iconSource: "qrc:/icon/icon/gallery/Photoshop.svg" }
                        RibbonGalleryItem { text: "Internet Explorer"; iconSource: "qrc:/icon/icon/gallery/Internet-Explorer.svg" }
                        RibbonGalleryItem { text: "Illustrator"; iconSource: "qrc:/icon/icon/gallery/Illustrator.svg" }
                        RibbonGalleryItem { text: "Google Maps"; iconSource: "qrc:/icon/icon/gallery/Google-Maps.svg" }
                        RibbonGalleryItem { text: "Adobe"; iconSource: "qrc:/icon/icon/gallery/Adobe.svg" }
                        RibbonGalleryItem { text: "Word"; iconSource: "qrc:/icon/icon/gallery/Word.svg" }
                    }
                    onTriggered: feedback.text = qsTr("gallery: %1 triggered").arg(item.text)
                }
            }

            RibbonPanel {
                panelTitle: "gallery controls"

                RibbonToolButton {
                    text: "Switch Group"
                    iconSource: "qrc:/icon/icon/item.svg"
                    proportion: Ribbon.Small
                    onClicked: {
                        gallery.currentGroupIndex = (gallery.currentGroupIndex + 1) % 2;
                        feedback.text = qsTr("gallery group switched: %1").arg(gallery.currentGroupIndex);
                    }
                }
                RibbonToolButton {
                    text: "Scroll"
                    iconSource: "qrc:/icon/icon/redo.svg"
                    proportion: Ribbon.Small
                    onClicked: {
                        gallery.scrollDown();
                        feedback.text = qsTr("gallery scrolled to row %1").arg(gallery.scrollRow);
                    }
                }
            }
        }

        // ---- Context Category 1 (mirrors the widgets context demo) ----
        RibbonContextCategory {
            id: contextCategory1
            contextTitle: "context"
            contextColor: "#2d7d9a"
            active: false

            RibbonCategory {
                title: "context Page1"

                RibbonPanel {
                    panelTitle: "show and hide test"

                    RibbonToolButton {
                        text: "Disable"
                        iconSource: "qrc:/icon/icon/enableTest.svg"
                        proportion: Ribbon.Large
                        enabled: false
                        onClicked: feedback.text = qsTr("never fires while disabled")
                    }
                    RibbonToolButton {
                        text: "unlock"
                        iconSource: "qrc:/icon/icon/unlock.svg"
                        proportion: Ribbon.Large
                        onClicked: feedback.text = qsTr("unlock clicked")
                    }
                    RibbonToolButton {
                        text: "1"
                        iconSource: "qrc:/icon/icon/setText.svg"
                        proportion: Ribbon.Large
                        toolTip: "very short string"
                        onClicked: feedback.text = qsTr("short text clicked")
                    }
                }

                RibbonPanel {
                    panelTitle: "widget"

                    RibbonControlContainer {
                        text: "spinbox:"
                        control: SpinBox {
                            onValueModified: feedback.text = qsTr("context spinbox: %1").arg(value)
                        }
                    }
                    RibbonControlContainer {
                        text: "linedit:"
                        control: TextField {
                            placeholderText: qsTr("context line edit")
                            onEditingFinished: feedback.text = qsTr("context line edit: %1").arg(text)
                        }
                    }
                }
            }

            RibbonCategory {
                title: "context Page2"

                RibbonPanel {
                    panelTitle: "popup zoo"

                    RibbonToolButton {
                        text: "Instant Popup"
                        iconSource: "qrc:/icon/icon/folder-stats.svg"
                        proportion: Ribbon.Large
                        popupMode: Ribbon.InstantPopup
                        menuItems: [
                            RibbonMenuItem { text: "ctx item 1"; iconSource: "qrc:/icon/icon/item.svg" },
                            RibbonMenuItem { text: "ctx item 2"; iconSource: "qrc:/icon/icon/item.svg" }
                        ]
                        onMenuTriggered: feedback.text = qsTr("context menu: %1").arg(item.text)
                    }
                    RibbonToolButton {
                        text: "Menu Button Popup"
                        iconSource: "qrc:/icon/icon/folder-star.svg"
                        proportion: Ribbon.Large
                        popupMode: Ribbon.MenuButtonPopup
                        menuItems: [
                            RibbonMenuItem { text: "ctx item 1"; iconSource: "qrc:/icon/icon/item.svg" }
                        ]
                        onMenuTriggered: feedback.text = qsTr("context menu: %1").arg(item.text)
                    }
                }
            }
        }

        // ---- Context Category 2 (multi-page structure demo) ----
        RibbonContextCategory {
            id: contextCategory2
            contextTitle: "context2"
            contextColor: "#217346"
            active: false

            RibbonCategory {
                title: "context2 Page1"

                RibbonPanel {
                    panelTitle: "page one"

                    RibbonToolButton {
                        text: "Hello"
                        iconSource: "qrc:/icon/icon/showContext.svg"
                        proportion: Ribbon.Large
                        onClicked: feedback.text = qsTr("context2 page one clicked")
                    }
                }
            }

            RibbonCategory {
                title: "context2 Page2"

                RibbonPanel {
                    panelTitle: "page two"

                    RibbonToolButton {
                        text: "World"
                        iconSource: "qrc:/icon/icon/showContext.svg"
                        proportion: Ribbon.Large
                        onClicked: feedback.text = qsTr("context2 page two clicked")
                    }
                }
            }
        }
    }

    // interaction feedback area (proves buttons/tabs are alive)
    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 12

            Label {
                id: feedback
                text: qsTr("click a button or switch a tab")
            }
            Item { Layout.fillWidth: true }
            Button {
                text: qsTr("Office 2021 Blue")
                onClicked: RibbonTheme.currentTheme = Ribbon.RibbonThemeOffice2021Blue
            }
            Button {
                text: qsTr("Office 2016 Blue")
                onClicked: RibbonTheme.currentTheme = Ribbon.RibbonThemeOffice2016Blue
            }
            Button {
                text: qsTr("Dark")
                onClicked: RibbonTheme.currentTheme = Ribbon.RibbonThemeDark
            }
        }
    }
}
