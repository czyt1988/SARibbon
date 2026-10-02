import QtQuick 2.12
import SARibbon 3.0

// RibbonColorGrid default visual leaf: one swatch per cell of the host's
// published grid. Geometry authority stays in the C++ host — cell rects, the
// "no color" flags and the slash inset all come from the published metrics
// (which reproduce the widgets SAColorGridWidget / SAColorToolButton
// derivation), the leaf only paints the swatch, its hover / pressed / checked
// frame and the "no color" mark.
// NOTE: no inline `component` syntax here — the module targets Qt 5.12.
Item {
    id: root

    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    anchors.fill: parent

    // single-dependency reads of the host, hoisted (NOTES B48): a chained
    // `cppHost && cppHost.x ? cppHost.y : f` shape makes the V4 compiler drop
    // the dependency and the binding silently freezes
    readonly property var rects: cppHost ? cppHost.cellRects : []
    readonly property var noneFlags: cppHost ? cppHost.cellNoneColor : []
    readonly property var colors: cppHost ? cppHost.colorList : []
    readonly property int swatchMargin: cppHost ? cppHost.cellMargin : 4
    readonly property bool checkable: cppHost ? cppHost.colorCheckable : false
    readonly property int checkedIndex: cppHost ? cppHost.checkedIndex : -1
    readonly property int slashInset: cppHost ? cppHost.noneColorSlashInset : 0

    Repeater {
        model: root.rects.length

        delegate: Item {
            id: cell
            objectName: "colorGridCell"

            // delegate-local hoists of every same-file id read (NOTES B48)
            readonly property int cellIndex: index
            readonly property var cellRect: root.rects[index] !== undefined ? root.rects[index] : Qt.rect(0, 0, 0, 0)
            readonly property var cellColor: root.colors[index] !== undefined ? root.colors[index] : "transparent"
            readonly property bool noneColor: root.noneFlags[index] === true
            readonly property int margin: root.swatchMargin
            readonly property int slash: root.slashInset
            readonly property bool isChecked: root.checkable && root.checkedIndex === cell.cellIndex
            readonly property bool cellHovered: cellMouse ? cellMouse.containsMouse : false
            readonly property bool cellPressed: cellMouse ? cellMouse.pressed : false
            readonly property int swatchW: Math.max(cell.cellRect.width - 2 * cell.margin, 0)
            readonly property int swatchH: Math.max(cell.cellRect.height - 2 * cell.margin, 0)

            x: cell.cellRect.x
            y: cell.cellRect.y
            width: cell.cellRect.width
            height: cell.cellRect.height

            // hover / pressed frame of the auto-raise tool button, and the
            // checked frame of the exclusive group
            Rectangle {
                anchors.fill: parent
                radius: 2
                color: cell.cellPressed ? RibbonTheme.contentPressedBg
                                        : (cell.isChecked ? RibbonTheme.selectionBg
                                                          : (cell.cellHovered ? RibbonTheme.contentHoverBg : "transparent"))
                border.width: cell.isChecked ? 1 : 0
                border.color: RibbonTheme.accent
            }

            // the swatch itself: SAColorToolButton::paintColor fills the whole
            // color rect, which for an icon-null button is the button rect
            Rectangle {
                visible: !cell.noneColor
                x: cell.margin
                y: cell.margin
                width: cell.swatchW
                height: cell.swatchH
                color: cell.cellColor
            }

            // "no color" cell: SAColorToolButton::paintNoneColor, drawn by the
            // shared mark the color menu row uses as well. The slash inset is
            // host published (core SA::noneColorSlashLine), so both front ends
            // draw the identical mark
            RibbonColorNoneMark {
                visible: cell.noneColor
                x: cell.margin
                y: cell.margin
                width: cell.swatchW
                height: cell.swatchH
                slashInset: cell.slash
            }

            MouseArea {
                id: cellMouse
                anchors.fill: parent
                hoverEnabled: true
                onPressed: if (root) root.cppHost.notifyCellPressed(index)
                onReleased: if (root) root.cppHost.notifyCellReleased(index)
                onClicked: if (root) root.cppHost.activateCell(index)
            }
        }
    }
}
