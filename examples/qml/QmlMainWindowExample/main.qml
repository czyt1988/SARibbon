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
        onApplicationButtonClicked: log(qsTr("application button clicked"))
        // ApplicationWidget mode (widgets example default): a custom panel
        // below the File button; the menu entries stay declared — click
        // priority: window > menu > signal only (widgets parity)
        RibbonApplicationWindow {
            id: appWindow
            width: 300
            height: 210
            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 8
                Label {
                    text: qsTr("Application Window")
                    font.bold: true
                }
                ListView {
                    id: appWindowList
                    width: parent.width
                    height: 100
                    clip: true
                    model: [ qsTr("item 1"), qsTr("item 2"), qsTr("item 3"),
                             qsTr("item 4"), qsTr("item 5"), qsTr("item 6") ]
                    delegate: ItemDelegate {
                        width: appWindowList.width
                        text: modelData
                        onClicked: log(qsTr("application window: %1").arg(modelData))
                    }
                    ScrollBar.vertical: ScrollBar { }
                }
                Label { text: qsTr("Press the Esc key to exit the window.") }
                Row {
                    spacing: 8
                    Button {
                        text: qsTr("Cancel")
                        onClicked: appWindow.close()
                    }
                    Button {
                        flat: true
                        width: 36
                        onClicked: appWindow.close()
                        Text {
                            anchors.centerIn: parent
                            text: "\u2715"
                            color: RibbonTheme.textColor
                        }
                    }
                }
            }
        }
        applicationMenuItems: [
            RibbonMenuItem { text: qsTr("test 1"); iconSource: "qrc:/icon/icon/action.svg" },
            RibbonMenuItem { text: qsTr("test 2"); iconSource: "qrc:/icon/icon/action2.svg" },
            RibbonMenuItem { separator: true },
            RibbonMenuItem { text: qsTr("Auto save"); checkable: true; checked: true; shortcut: "Ctrl+Shift+S" },
            RibbonMenuItem { text: qsTr("Save as..."); shortcut: "Ctrl+Shift+P" },
            RibbonMenuItem {
                text: qsTr("Recent files")
                submenu: [
                    RibbonMenuItem { text: qsTr("demo-1.saribbon"); iconSource: "qrc:/icon/icon/file.svg" },
                    RibbonMenuItem { text: qsTr("demo-2.saribbon"); iconSource: "qrc:/icon/icon/file.svg" },
                    RibbonMenuItem { separator: true },
                    RibbonMenuItem { text: qsTr("Read only"); checkable: true }
                ]
            },
            RibbonMenuItem { separator: true },
            RibbonMenuItem { text: qsTr("test 3"); iconSource: "qrc:/icon/icon/action3.svg" }
        ]
        onApplicationMenuTriggered: function(item) {
            log(qsTr("application menu: %1%2")
                .arg(item.text)
                .arg(item.checkable ? (item.checked ? " [on]" : " [off]") : ""))
        }

        // quick access bar (widgets quick access parity): small buttons on
        // the title row after the application button
        RibbonQuickAccessBar {
            // single-choice group: the row host implements the QActionGroup
            // behavior widgets gets from the action bridge (the two view
            // buttons below can never be checked at the same time)
            exclusive: true
            RibbonToolButton {
                text: qsTr("Save")
                iconSource: "qrc:/icon/icon/save.svg"
                proportion: Ribbon.Small
                onClicked: log(qsTr("quick access: Save clicked"))
            }
            RibbonToolButton {
                text: qsTr("Undo")
                iconSource: "qrc:/icon/icon/undo.svg"
                proportion: Ribbon.Small
                onClicked: log(qsTr("quick access: Undo clicked"))
            }
            RibbonToolButton {
                text: qsTr("Icons")
                proportion: Ribbon.Small
                checkable: true
                checked: true
                onToggled: function(checked) { if (checked) log(qsTr("quick access view: icons")) }
            }
            RibbonToolButton {
                text: qsTr("Details")
                proportion: Ribbon.Small
                checkable: true
                onToggled: function(checked) { if (checked) log(qsTr("quick access view: details")) }
            }            RibbonToolButton {
                text: qsTr("Redo")
                iconSource: "qrc:/icon/icon/redo.svg"
                proportion: Ribbon.Small
                onClicked: log(qsTr("quick access: Redo clicked"))
            }
            RibbonToolButton {
                text: qsTr("Presentation File 1")
                iconSource: "qrc:/icon/icon/file.svg"
                proportion: Ribbon.Small
                popupMode: Ribbon.InstantPopup
                menuItems: [
                    RibbonMenuItem { text: qsTr("file 1-1"); iconSource: "qrc:/icon/icon/item.svg" },
                    RibbonMenuItem { text: qsTr("file 1-2"); iconSource: "qrc:/icon/icon/item.svg" },
                    RibbonMenuItem {
                        text: qsTr("Open in")
                        submenu: [
                            RibbonMenuItem { text: qsTr("New window"); shortcut: "Ctrl+N" },
                            RibbonMenuItem { text: qsTr("Preview pane"); checkable: true }
                        ]
                    }
                ]
                    onMenuTriggered: function(item) { log(qsTr("quick access menu: %1").arg(item.text)) }
            }
        }

        // right button group (widgets right bar parity): help + toggle
        RibbonButtonGroup {
            RibbonToolButton {
                text: qsTr("Help")
                iconSource: "qrc:/icon/icon/help.svg"
                proportion: Ribbon.Small
                onClicked: log(qsTr("help clicked (widgets shows the version message box)"))
            }
            RibbonToolButton {
                text: qsTr("Visible")
                iconSource: "qrc:/icon/icon/showContext.svg"
                proportion: Ribbon.Small
                checkable: true
                checked: true
                onToggled: function(checked) { log(qsTr("right group visible toggle: %1").arg(checked)) }
            }
        }

        // ---- Home ----
        RibbonCategory {
            title: "Home"

            // Mirrors the widgets "ribbon style" panel: six exclusive style
            // radios (RibbonStyle* flag values), a theme combobox and the
            // font size buttons, all embedded through RibbonControlContainer
            RibbonPanel {
                panelTitle: "ribbon style"

                ButtonGroup { id: styleGroup }

                RibbonControlContainer {
                    text: ""
                    control: SlimRadioButton {
                        ButtonGroup.group: styleGroup
                        text: qsTr("office style")
                        checked: true
                        onToggled: function(checked) {
                            if (checked) {
                                ribbonBar.ribbonStyle = Ribbon.RibbonStyleLooseThreeRow;
                                log(qsTr("LooseThreeRow: tabs below title, 3 rows, word wrap on"));
                            }
                        }
                    }
                }
                RibbonControlContainer {
                    text: ""
                    control: SlimRadioButton {
                        ButtonGroup.group: styleGroup
                        text: qsTr("wps style")
                        onToggled: function(checked) {
                            if (checked) {
                                ribbonBar.ribbonStyle = Ribbon.RibbonStyleCompactThreeRow;
                                log(qsTr("CompactThreeRow: tabs on title, 3 rows"));
                            }
                        }
                    }
                }
                RibbonControlContainer {
                    text: ""
                    control: SlimRadioButton {
                        ButtonGroup.group: styleGroup
                        text: qsTr("office 2 row")
                        onToggled: function(checked) {
                            if (checked) {
                                ribbonBar.ribbonStyle = Ribbon.RibbonStyleLooseTwoRow;
                                log(qsTr("LooseTwoRow: 2 rows, word wrap off"));
                            }
                        }
                    }
                }
                RibbonControlContainer {
                    text: ""
                    control: SlimRadioButton {
                        ButtonGroup.group: styleGroup
                        text: qsTr("wps 2 row")
                        onToggled: function(checked) {
                            if (checked) {
                                ribbonBar.ribbonStyle = Ribbon.RibbonStyleCompactTwoRow;
                                log(qsTr("CompactTwoRow: tabs on title, 2 rows"));
                            }
                        }
                    }
                }
                RibbonControlContainer {
                    text: ""
                    control: SlimRadioButton {
                        ButtonGroup.group: styleGroup
                        text: qsTr("loose single row")
                        onToggled: function(checked) {
                            if (checked) {
                                ribbonBar.ribbonStyle = Ribbon.RibbonStyleLooseSingleRow;
                                log(qsTr("LooseSingleRow: 1 row, panel titles hidden, icon-right text"));
                            }
                        }
                    }
                }
                RibbonControlContainer {
                    text: ""
                    control: SlimRadioButton {
                        ButtonGroup.group: styleGroup
                        text: qsTr("compact single row")
                        onToggled: function(checked) {
                            if (checked) {
                                ribbonBar.ribbonStyle = Ribbon.RibbonStyleCompactSingleRow;
                                log(qsTr("CompactSingleRow: 1 row + tabs on title"));
                            }
                        }
                    }
                }

                RibbonSeparator { }

                RibbonControlContainer {
                    text: "Theme:"
                    control: SlimComboBox {
                        // index maps onto the RibbonEnums::Theme values below
                        property var themeValues: [
                            Ribbon.RibbonThemeWindows7,
                            Ribbon.RibbonThemeOffice2013,
                            Ribbon.RibbonThemeOffice2016Blue,
                            Ribbon.RibbonThemeOffice2021Blue,
                            Ribbon.RibbonThemeOffice2021Green,
                            Ribbon.RibbonThemeOffice2021Dark,
                            Ribbon.RibbonThemeDark,
                            Ribbon.RibbonThemeDark2
                        ]
                        model: [
                            qsTr("Windows 7"), qsTr("Office 2013"), qsTr("Office 2016 Blue"),
                            qsTr("Office 2021 Blue"), qsTr("Office 2021 Green"), qsTr("Office 2021 Dark"),
                            qsTr("Dark"), qsTr("Dark 2")
                        ]
                        onActivated: function(index) {
                            RibbonTheme.currentTheme = themeValues[index];
                            log(qsTr("theme switched: %1").arg(currentText));
                        }
                    }
                }

                RibbonToolButton {
                    text: "Larger"
                    iconSource: "qrc:/icon/icon/largerFont.svg"
                    proportion: Ribbon.Small
                    onClicked: {
                        RibbonMetrics.fontPointSize = RibbonMetrics.fontPointSize + 1;
                        log(qsTr("font point size: %1").arg(RibbonMetrics.fontPointSize));
                    }
                }
                RibbonToolButton {
                    text: "Smaller"
                    iconSource: "qrc:/icon/icon/smallFont.svg"
                    proportion: Ribbon.Small
                    onClicked: {
                        RibbonMetrics.fontPointSize = Math.max(RibbonMetrics.fontPointSize - 1, 6);
                        log(qsTr("font point size: %1").arg(RibbonMetrics.fontPointSize));
                    }
                }

                // widgets "Switch to RTL" parity: flips the application
                // layout direction; the core engines mirror through saIsRTL()
                RibbonToolButton {
                    text: qsTr("Switch to RTL")
                    iconSource: "qrc:/icon/icon/layout.svg"
                    proportion: Ribbon.Small
                    onClicked: {
                        RibbonTheme.rtl = !RibbonTheme.rtl;
                        text = RibbonTheme.rtl ? qsTr("Switch to LTR") : qsTr("Switch to RTL");
                        log(qsTr("layout direction: %1").arg(RibbonTheme.rtl ? "RTL" : "LTR"));
                    }
                }

                // theme customization entry: overriding the accent key color
                // re-derives every dependent token, so the whole ribbon
                // repaints without touching a single literal color
                RibbonToolButton {
                    text: qsTr("Accent")
                    iconSource: "qrc:/icon/icon/setText.svg"
                    proportion: Ribbon.Small
                    onClicked: {
                        accentPicker.current = RibbonTheme.accent;
                        accentPicker.open();
                    }
                }

                // widgets "select font" + QFontComboBox parity: family change
                // rebuilds the metrics and relayouts every host
                RibbonControlContainer {
                    text: qsTr("Font:")
                    control: SlimComboBox {
                        id: fontFamilyCombo
                        model: RibbonMetrics.commonFontFamilies()
                        onActivated: {
                            RibbonMetrics.fontFamily = currentText;
                            log(qsTr("font family: %1").arg(currentText));
                        }
                        Component.onCompleted: {
                            const idx = model.indexOf(RibbonMetrics.fontFamily);
                            if (idx >= 0) {
                                currentIndex = idx;
                            }
                        }
                    }
                }

                // widgets "Alignment Center" parity: tab row alignment
                // inside the free strip (left/center/right)
                RibbonControlContainer {
                    text: qsTr("Align:")
                    control: SlimComboBox {
                        model: [ qsTr("Left"), qsTr("Center"), qsTr("Right") ]
                        currentIndex: 0
                        onActivated: function(index) {
                            ribbonBar.tabAlignment = index;  // 0/1/2 = AlignLeft/Center/Right
                            log(qsTr("tab alignment: %1").arg(currentText));
                        }
                    }
                }
            }

            RibbonPanel {
                panelTitle: "Clipboard"
                RibbonToolButton {
                    text: "Paste"
                    iconSource: "qrc:/icon/icon/folder-checkmark.svg"
                    proportion: Ribbon.Large
                    onClicked: log(qsTr("Paste clicked"))
                }
                RibbonToolButton {
                    text: "Cut"
                    iconSource: "qrc:/icon/icon/delete.svg"
                    proportion: Ribbon.Small
                    onClicked: log(qsTr("Cut clicked"))
                }
                RibbonToolButton {
                    text: "Copy"
                    iconSource: "qrc:/icon/icon/item.svg"
                    proportion: Ribbon.Small
                    onClicked: log(qsTr("Copy clicked"))
                }
            }

            RibbonPanel {
                panelTitle: "Font"
                RibbonToolButton {
                    text: "Bold"
                    iconSource: "qrc:/icon/icon/bold.svg"
                    proportion: Ribbon.Medium
                    checkable: true
                    onClicked: log(qsTr("Bold toggled: %1").arg(checked))
                }
                RibbonToolButton {
                    text: "Italic"
                    iconSource: "qrc:/icon/icon/Italic.svg"
                    proportion: Ribbon.Small
                    checkable: true
                    onClicked: log(qsTr("Italic toggled: %1").arg(checked))
                }
                RibbonToolButton {
                    text: "Underline"
                    iconSource: "qrc:/icon/icon/Underline.svg"
                    proportion: Ribbon.Small
                    checkable: true
                    onClicked: log(qsTr("Underline toggled: %1").arg(checked))
                }
            }

            RibbonPanel {
                panelTitle: "Edit"
                RibbonToolButton {
                    text: "Undo"
                    iconSource: "qrc:/icon/icon/undo.svg"
                    proportion: Ribbon.Large
                    onClicked: log(qsTr("Undo clicked"))
                }
                RibbonToolButton {
                    text: "Redo"
                    iconSource: "qrc:/icon/icon/redo.svg"
                    proportion: Ribbon.Medium
                    onClicked: log(qsTr("Redo clicked"))
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
                    onClicked: log(qsTr("Normal clicked"))
                }
                RibbonToolButton {
                    text: "Checked"
                    iconSource: "qrc:/icon/icon/enableTest.svg"
                    proportion: Ribbon.Large
                    checkable: true
                    checked: true
                    onClicked: log(qsTr("Checked toggled: %1").arg(checked))
                }
                RibbonToolButton {
                    id: disableButton
                    text: "Disabled"
                    iconSource: "qrc:/icon/icon/disable.svg"
                    proportion: Ribbon.Large
                    enabled: false
                    onClicked: log(qsTr("this click must never fire"))
                }
                RibbonToolButton {
                    text: "unlock"
                    iconSource: "qrc:/icon/icon/unlock.svg"
                    proportion: Ribbon.Large
                    onClicked: {
                        disableButton.enabled = true;
                        disableButton.text = "Enabled";
                        log(qsTr("disabled button unlocked"));
                    }
                }
                RibbonToolButton {
                    text: "very long text in a button, balabalabala etc"
                    iconSource: "qrc:/icon/icon/long-text.svg"
                    proportion: Ribbon.Large
                    onClicked: log(qsTr("long text clicked"))
                }
                RibbonToolButton {
                    text: "1"
                    iconSource: "qrc:/icon/icon/setText.svg"
                    proportion: Ribbon.Large
                    toolTip: "very short string"
                    onClicked: log(qsTr("short text clicked"))
                }
            }

            // Mirrors the widgets "sa ribbon toolbutton style" panel: the
            // three popup modes (MenuButtonPopup splits action/menu zones,
            // InstantPopup is menu-only, DelayedPopup opens on press-hold),
            // checkable variants and a disabled button with a menu.
            RibbonPanel {
                panelTitle: "toolbutton style"
                // option action: the diagonal button at the panel's bottom-right
                hasOptionAction: true
                onOptionActionTriggered: log(qsTr("option action triggered (widgets shows a message box)"))

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
                    onClicked: log(qsTr("test 1 action zone clicked"))
                    onMenuTriggered: function(item) { log(qsTr("test 1 menu: %1").arg(item.text)) }
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
                    onMenuTriggered: function(item) { log(qsTr("test 2 menu: %1").arg(item.text)) }
                }
                // mirrors the widgets panel's separator between the small
                // tests and the large popup buttons
                RibbonSeparator { }
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
                    onClicked: log(qsTr("Delayed Popup clicked (press and hold opens the menu)"))
                    onMenuTriggered: function(item) { log(qsTr("Delayed Popup menu: %1").arg(item.text)) }
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
                    onClicked: log(qsTr("Menu Button Popup action zone clicked"))
                    onMenuTriggered: function(item) { log(qsTr("Menu Button Popup menu: %1").arg(item.text)) }
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
                    onMenuTriggered: function(item) { log(qsTr("Instant Popup menu: %1").arg(item.text)) }
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
                    onClicked: log(qsTr("Delayed Popup checkable toggled: %1").arg(checked))
                    onMenuTriggered: function(item) { log(qsTr("Delayed Popup checkable menu: %1").arg(item.text)) }
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
                    onClicked: log(qsTr("Menu Button Popup checkable toggled: %1").arg(checked))
                    onMenuTriggered: function(item) { log(qsTr("Menu Button Popup checkable menu: %1").arg(item.text)) }
                }
            }

            // Mirrors the widgets "widget test" panel: arbitrary controls
            // embedded into the ribbon panel through RibbonControlContainer.
            RibbonPanel {
                panelTitle: "widget test"

                RibbonControlContainer {
                    id: comboContainer
                    text: "ComboBox:"
                    iconSource: "qrc:/icon/icon/setText.svg"
                    proportion: Ribbon.Small
                    control: SlimComboBox {
                        editable: true
                        model: [
                            "testItem 1", "testItem 2", "testItem 3", "testItem 4", "testItem 5",
                            "testItem 6", "testItem 7", "testItem 8", "testItem 9", "testItem 10"
                        ]
                        onActivated: log(qsTr("ComboBox selected: %1").arg(currentText))
                    }
                }
                RibbonControlContainer {
                    text: "ComboBox2:"
                    proportion: Ribbon.Small
                    control: SlimComboBox {
                        model: [ "option 1", "option 2", "option 3" ]
                        onActivated: log(qsTr("ComboBox2 selected: %1").arg(currentText))
                    }
                }
                RibbonControlContainer {
                    text: "Line Edit:"
                    proportion: Ribbon.Small
                    control: TextField {
                        placeholderText: qsTr("type and press Enter")
                        onEditingFinished: log(qsTr("Line Edit: %1").arg(text))
                    }
                }
                RibbonControlContainer {
                    text: "CheckBox:"
                    proportion: Ribbon.Small
                    control: SlimCheckBox {
                        onToggled: function(checked) { log(qsTr("CheckBox toggled: %1").arg(checked)) }
                    }
                }
                RibbonControlContainer {
                    text: "SpinBox:"
                    // trailing label after the control (widgets
                    // SARibbonLineWidgetContainer::setSuffix parity): the strip
                    // is carved out of the container, the control keeps its width
                    suffixText: "px"
                    proportion: Ribbon.Small
                    control: SpinBox {
                        onValueModified: log(qsTr("SpinBox value: %1").arg(value))
                    }
                }
                RibbonToolButton {
                    text: "Label"
                    iconSource: "qrc:/icon/icon/long-text.svg"
                    proportion: Ribbon.Small
                    toolTip: "cycle the ComboBox container label strip: icon + text / icon only / text only / none"
                    onClicked: {
                        var showIcon = comboContainer.enableShowIcon;
                        var showTitle = comboContainer.enableShowTitle;
                        if (showIcon && showTitle) {
                            showTitle = false;
                        } else if (showIcon) {
                            showIcon = false;
                            showTitle = true;
                        } else {
                            showIcon = true;
                        }
                        comboContainer.enableShowIcon = showIcon;
                        comboContainer.enableShowTitle = showTitle;
                        log(qsTr("container label strip: icon %1 / title %2 (label width %3)")
                            .arg(showIcon).arg(showTitle).arg(comboContainer.labelWidth));
                    }
                }

                // Color widgets (the widgets example's "color" panel). Both
                // buttons own a RibbonColorMenu with the standard row and the
                // shade rows; the font color one keeps the widgets
                // ColorUnderIcon shape (icon on top, color band underneath) and
                // the fill one paints the picked color into the icon box.
                // The menu's custom color row is forwarded as the button's own
                // customColorRequested(); customColorPicker stands in for the
                // QColorDialog the widgets menu owns.
                RibbonColorToolButton {
                    id: fontColorButton
                    text: "Font Color"
                    iconSource: "qrc:/icon/icon/setText.svg"
                    proportion: Ribbon.Large
                    colorStyle: Ribbon.ColorUnderIcon
                    color: "#c0392b"
                    toolTip: "ColorUnderIcon: click for the color, click the arrow for the palette"
                    onColorClicked: log(qsTr("font color: %1 (checked %2)").arg(color).arg(checked))
                    onCustomColorRequested: customColorPicker.openFor(fontColorButton.colorMenu)
                }
                RibbonColorToolButton {
                    id: fillColorButton
                    text: "Fill Color"
                    iconSource: "qrc:/icon/icon/Italic.svg"
                    proportion: Ribbon.Small
                    colorStyle: Ribbon.ColorFillToIcon
                    color: "#2e6da4"
                    toolTip: "ColorFillToIcon: the picked color fills the icon box"
                    onColorClicked: log(qsTr("fill color: %1 (checked %2)").arg(color).arg(checked))
                    onCustomColorRequested: customColorPicker.openFor(fillColorButton.colorMenu)
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
                    onClicked: log(qsTr("New File clicked"))
                }
                RibbonToolButton {
                    text: "Open"
                    iconSource: "qrc:/icon/icon/chinese-char.svg"
                    proportion: Ribbon.Medium
                    onClicked: log(qsTr("Open clicked"))
                }
            }

            RibbonPanel {
                panelTitle: "Layout"
                RibbonToolButton {
                    text: "Layout"
                    iconSource: "qrc:/icon/icon/layout.svg"
                    proportion: Ribbon.Large
                    onClicked: log(qsTr("Layout clicked"))
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
                    onClicked: log(qsTr("Save clicked"))
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
                    onToggled: function(checked) {
                        contextCategory1.active = checked;
                        log(qsTr("context category 1 active: %1").arg(checked));
                    }
                }
                RibbonToolButton {
                    text: "Context Category 2"
                    iconSource: "qrc:/icon/icon/ContextCategory.svg"
                    proportion: Ribbon.Large
                    checkable: true
                    onToggled: function(checked) {
                        contextCategory2.active = checked;
                        log(qsTr("context category 2 active: %1").arg(checked));
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
                    onTriggered: function(item) { log(qsTr("gallery: %1 triggered").arg(item.text)) }
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
                        log(qsTr("gallery group switched: %1").arg(gallery.currentGroupIndex));
                    }
                }
                RibbonToolButton {
                    text: "Scroll"
                    iconSource: "qrc:/icon/icon/redo.svg"
                    proportion: Ribbon.Small
                    onClicked: {
                        gallery.scrollDown();
                        log(qsTr("gallery scrolled to row %1").arg(gallery.scrollRow));
                    }
                }
                // cycles the three core caption styles; the cell metrics (icon
                // box + caption height) come from SA::calcGalleryCellMetrics
                RibbonToolButton {
                    text: "Caption Style"
                    iconSource: "qrc:/icon/icon/item.svg"
                    proportion: Ribbon.Small
                    toolTip: "cycle the gallery cell caption style: icon only / icon with text / icon with word wrap text"
                    onClicked: {
                        gallery.captionStyle = (gallery.captionStyle + 1) % 3;
                        var names = ["icon only", "icon with text", "icon with word wrap text"];
                        log(qsTr("gallery caption style: %1 (caption height %2)")
                            .arg(names[gallery.captionStyle]).arg(gallery.captionHeight));
                    }
                }
            }

            // The customize picker (widgets SARibbonCustomizeDialog parity):
            // a command catalogue on the left, the ribbon tree preview on the
            // right, every edit staged as a core customize record that only OK
            // applies and Cancel drops
            RibbonPanel {
                panelTitle: "customize"

                RibbonToolButton {
                    text: "Customize"
                    iconSource: "qrc:/icon/icon/item.svg"
                    proportion: Ribbon.Large
                    toolTip: "open the ribbon customize dialog (edits are staged until OK)"
                    onClicked: window.openCustomizeDialog()
                }
            }
        }

        // ---- Delete (dynamic panel add/remove, widgets Delete category parity) ----
        RibbonCategory {
            title: "Delete"

            // dynamic panels ride a ListModel + Repeater: insertions and
            // removals flow through the bar/category registration chain
            ListModel {
                id: dynamicPanels
                ListElement { name: "panel 1" }
                ListElement { name: "panel 2" }
            }

            RibbonPanel {
                panelTitle: "panel 1"

                RibbonToolButton {
                    text: "remove panel"
                    iconSource: "qrc:/icon/icon/remove.svg"
                    proportion: Ribbon.Large
                    onClicked: {
                        if (dynamicPanels.count > 0) {
                            dynamicPanels.remove(dynamicPanels.count - 1);
                            log(qsTr("removed the last dynamic panel (%1 left)").arg(dynamicPanels.count));
                        } else {
                            log(qsTr("no dynamic panel left to remove"));
                        }
                    }
                }
            }

            RibbonPanel {
                panelTitle: "insert panel test"

                RibbonToolButton {
                    text: "insert at 0"
                    iconSource: "qrc:/icon/icon/test1.svg"
                    proportion: Ribbon.Large
                    onClicked: {
                        dynamicPanels.insert(0, { name: qsTr("panel@0-%1").arg(dynamicPanels.count) });
                        log(qsTr("inserted a panel at 0"));
                    }
                }
                RibbonToolButton {
                    text: "insert at end"
                    iconSource: "qrc:/icon/icon/test2.svg"
                    proportion: Ribbon.Large
                    onClicked: {
                        dynamicPanels.append({ name: qsTr("panel@end-%1").arg(dynamicPanels.count) });
                        log(qsTr("inserted a panel at the end"));
                    }
                }
                RibbonToolButton {
                    text: "insert at -1"
                    iconSource: "qrc:/icon/icon/item.svg"
                    proportion: Ribbon.Large
                    onClicked: {
                        dynamicPanels.insert(-1, { name: qsTr("panel@-1") });
                        log(qsTr("insert at -1 (edge case, ListModel clamps)"));
                    }
                }
            }

            Repeater {
                model: dynamicPanels
                delegate: RibbonPanel {
                    panelTitle: model.name
                    RibbonToolButton {
                        text: "Text Only"
                        iconSource: "qrc:/icon/icon/setText.svg"
                        proportion: Ribbon.Large
                        onClicked: log(qsTr("%1 button clicked").arg(model.name))
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
                        onClicked: log(qsTr("never fires while disabled"))
                    }
                    RibbonToolButton {
                        text: "unlock"
                        iconSource: "qrc:/icon/icon/unlock.svg"
                        proportion: Ribbon.Large
                        onClicked: log(qsTr("unlock clicked"))
                    }
                    RibbonToolButton {
                        text: "1"
                        iconSource: "qrc:/icon/icon/setText.svg"
                        proportion: Ribbon.Large
                        toolTip: "very short string"
                        onClicked: log(qsTr("short text clicked"))
                    }
                }

                RibbonPanel {
                    panelTitle: "widget"

                    RibbonControlContainer {
                        text: "spinbox:"
                        control: SpinBox {
                            onValueModified: log(qsTr("context spinbox: %1").arg(value))
                        }
                    }
                    RibbonControlContainer {
                        text: "linedit:"
                        control: TextField {
                            placeholderText: qsTr("context line edit")
                            onEditingFinished: log(qsTr("context line edit: %1").arg(text))
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
                        onMenuTriggered: function(item) { log(qsTr("context menu: %1").arg(item.text)) }
                    }
                    RibbonToolButton {
                        text: "Menu Button Popup"
                        iconSource: "qrc:/icon/icon/folder-star.svg"
                        proportion: Ribbon.Large
                        popupMode: Ribbon.MenuButtonPopup
                        menuItems: [
                            RibbonMenuItem { text: "ctx item 1"; iconSource: "qrc:/icon/icon/item.svg" }
                        ]
                        onMenuTriggered: function(item) { log(qsTr("context menu: %1").arg(item.text)) }
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
                        onClicked: log(qsTr("context2 page one clicked"))
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
                        onClicked: log(qsTr("context2 page two clicked"))
                    }
                }
            }
        }
    }

    // Accent picker for the theme customization entry. Deliberately plain
    // QtQuick rather than QtQuick.Dialogs: the module supports Qt 5.12, where
    // the ColorDialog API differs. Picking a swatch calls setAccentColor(),
    // which overwrites the "accent" key color and re-derives every dependent
    // token, so the whole ribbon repaints from the palette alone. Switching
    // the theme back to a built-in one reloads that theme's JSON and clears
    // the override (RibbonTheme.hasCustomPalette turns false again).
    Popup {
        id: accentPicker
        property color current: RibbonTheme.accent
        readonly property var swatches: [
            "#2b579a", "#217346", "#b7472a", "#8764b8", "#c19224",
            "#1f6f78", "#a13d63", "#4a5a6a", "#7a4a1f", "#1d3f73"
        ]
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        anchors.centerIn: Overlay.overlay
        padding: 12

        contentItem: ColumnLayout {
            spacing: 10

            Label {
                text: qsTr("Accent color")
                font.bold: true
                color: RibbonTheme.textColor
            }
            Grid {
                columns: 5
                spacing: 6
                Repeater {
                    model: accentPicker.swatches
                    delegate: Rectangle {
                        width: 34
                        height: 34
                        radius: 4
                        color: modelData
                        border.width: Qt.colorEqual(accentPicker.current, modelData) ? 3 : 1
                        border.color: RibbonTheme.borderColor
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                accentPicker.current = modelData;
                                RibbonTheme.setAccentColor(modelData);
                                log(qsTr("accent overridden: %1 (hasCustomPalette=%2)")
                                    .arg(modelData).arg(RibbonTheme.hasCustomPalette));
                                accentPicker.close();
                            }
                        }
                    }
                }
            }
            RowLayout {
                spacing: 8
                Rectangle {
                    Layout.preferredWidth: 26
                    Layout.preferredHeight: 26
                    radius: 3
                    color: accentPicker.current
                    border.width: 1
                    border.color: RibbonTheme.borderColor
                }
                Label {
                    text: accentPicker.current
                    color: RibbonTheme.textColor
                }
                Item { Layout.fillWidth: true }
                Button {
                    text: qsTr("Close")
                    onClicked: accentPicker.close()
                }
            }
        }
    }

    // Stand-in for the QColorDialog that the widgets SAColorMenu owns. The QML
    // menu deliberately ships without a dialog (it would drag a second toolkit
    // into SARibbonQml), so activating its "More colors" row emits
    // customColorRequested(); this popup answers it and hands the picked color
    // back through addCustomColor(), which records it in the menu's custom row
    // and selects it on the button.
    Popup {
        id: customColorPicker
        property var targetMenu: null
        readonly property var swatches: [
            "#c0392b", "#e67e22", "#f1c40f", "#2ecc71", "#16a085",
            "#2980b9", "#8e44ad", "#d35400", "#7f8c8d", "#2c3e50",
            "#ff9ff3", "#feca57", "#48dbfb", "#1dd1a1", "#5f27cd",
            "#576574", "#a29bfe", "#fab1a0", "#55efc4", "#fd79a8"
        ]
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        anchors.centerIn: Overlay.overlay
        padding: 12

        // Remember which menu asked, then show the popup
        function openFor(menu)
        {
            targetMenu = menu;
            open();
        }

        contentItem: ColumnLayout {
            spacing: 10

            Label {
                text: qsTr("Custom color")
                font.bold: true
                color: RibbonTheme.textColor
            }
            Grid {
                columns: 5
                spacing: 6
                Repeater {
                    model: customColorPicker.swatches
                    delegate: Rectangle {
                        width: 30
                        height: 30
                        radius: 3
                        color: modelData
                        border.width: 1
                        border.color: RibbonTheme.borderColor
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                var menu = customColorPicker.targetMenu;
                                if (menu) {
                                    menu.addCustomColor(modelData);
                                    log(qsTr("custom color added: %1 (recorded %2)")
                                        .arg(modelData).arg(menu.customColors.length));
                                }
                                customColorPicker.close();
                            }
                        }
                    }
                }
            }
            Label {
                // The recorded custom colors of the menu that opened the popup;
                // the menu keeps ten and shifts left once full, exactly like the
                // widgets SAColorMenu
                readonly property var recorded: customColorPicker.targetMenu ? customColorPicker.targetMenu.customColors : []
                text: qsTr("Recorded: %1").arg(recorded.map(function(c) {
                    return String(c);
                }).join(", "))
                color: RibbonTheme.subtitle
                Layout.fillWidth: true
                Layout.maximumWidth: 220
                elide: Text.ElideRight
            }
            Button {
                text: qsTr("Close")
                onClicked: customColorPicker.close()
            }
        }
    }

    // The customize picker ships as a QML-only leaf inside the module resource,
    // so an application reaches it by URL: `import SARibbon 3.0` carries the
    // C++ backed types only. It is created on first use and parented to the
    // window — NOTES B59: an object owning popups must never be the root of its
    // own component, and a Popup is no Loader item either.
    property var customizeDialog: null

    function openCustomizeDialog()
    {
        if (customizeDialog === null) {
            var comp = Qt.createComponent("qrc:/SARibbon/RibbonCustomizeDialog.qml");
            if (comp.status !== Component.Ready) {
                log(qsTr("customize dialog unavailable: %1").arg(comp.errorString()));
                return;
            }
            customizeDialog = comp.createObject(window.contentItem, {
                                                    parent: window.contentItem,
                                                    bar: ribbonBar
                                                });
            customizeDialog.acceptedWithResult.connect(function(applied) {
                log(qsTr("customize applied: %1, %2 categories now")
                    .arg(applied).arg(ribbonBar.categoryCount));
            });
            customizeDialog.discarded.connect(function() {
                log(qsTr("customize discarded, the ribbon is untouched"));
            });
            customizeDialog.operationRefused.connect(function(reason) {
                log(qsTr("customize refused: %1").arg(reason));
            });
        }
        customizeDialog.bar = ribbonBar;
        customizeDialog.open();
    }

    // Event log (the widgets example's central QTextBrowser counterpart):
    // every handler routes through log(); the status line shows the last
    // event, the scrolling area keeps the full history
    function log(msg)
    {
        eventLog.append(msg);
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.margins: 4
            spacing: 12

            Flickable {
                id: logFlick
                Layout.fillWidth: true
                Layout.preferredHeight: 64
                contentWidth: eventLog.width
                contentHeight: eventLog.height
                clip: true
                TextArea {
                    id: eventLog
                    width: logFlick.width
                    readOnly: true
                    wrapMode: TextArea.Wrap
                    text: qsTr("click a button or switch a tab")
                    onHeightChanged: logFlick.contentY = Math.max(height - logFlick.height, 0)
                    function append(msg)
                    {
                        text += "\n" + msg;
                        cursorPosition = text.length;
                    }
                }
                ScrollBar.vertical: ScrollBar { }
            }
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
