import QtQuick 2.15
import QtQuick.Controls 2.15
import SARibbon 3.0

ApplicationWindow {
    id: window
    width: 1000
    height: 280
    visible: true
    title: "SARibbon QML Example"

    RibbonBar {
        id: ribbonBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        RibbonTab { text: "Home" }
        RibbonTab { text: "Insert" }
        RibbonTab { text: "Design" }

        RibbonCategory {
            title: "Home"
            RibbonPanel {
                panelTitle: "Clipboard"
                RibbonToolButton { text: "Paste"; proportion: Ribbon.Large }
                RibbonToolButton { text: "Cut"; proportion: Ribbon.Small }
                RibbonToolButton { text: "Copy"; proportion: Ribbon.Small }
            }
            RibbonPanel {
                panelTitle: "Font"
                RibbonToolButton { text: "B"; proportion: Ribbon.Medium }
                RibbonToolButton { text: "I"; proportion: Ribbon.Small }
                RibbonToolButton { text: "U"; proportion: Ribbon.Small }
            }
        }
    }
}
