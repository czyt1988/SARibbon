import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import SARibbon 3.0

// Mirrors the widgets MainWindowExample main scene: three categories with
// mixed large/small buttons (icons!), theme switching and click feedback.
// Tabs are auto-generated from the category titles (addCategoryPage parity).
ApplicationWindow {
    id: window
    width: 1000
    height: 420
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
