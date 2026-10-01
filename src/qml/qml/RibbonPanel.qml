import QtQuick 2.12
import SARibbon 3.0

// RibbonPanel default visual leaf: content background + the title caption
// rendered inside the engine-reserved strip (cppHost.titleGeometry — leaf
// renders, the C++ host computes). QSS parity: subtitle color, centered,
// pixelSize = panelTitleHeight * 0.8 (SARibbonPanel::resetTitleLabelFont).
// The option action renders the diagonal-arrow button inside the
// engine-reserved square at the title strip's right end (explicit x/y
// coordinates — anchor resets during teardown re-evaluate bindings on the
// dying host; see NOTES B44 for the round-4 crash investigation).
Rectangle {
    id: root

    // contract face (plan-04 leaf organization rule)
    property QtObject cppHost: null
    onCppHostChanged: if (cppHost) cppHost.qmlLeaf = root

    // state reads always null-guarded
    readonly property string title: cppHost ? cppHost.panelTitle : ""
    readonly property rect titleRect: cppHost ? cppHost.titleGeometry : Qt.rect(0, 0, 0, 0)
    readonly property bool hasOption: cppHost ? cppHost.hasOptionAction : false
    // NOTE (round 8, NOTES B48): keep this a SINGLE-dependency binding, the
    // exact shape of titleRect. The original two-dependency ternary
    // (`cppHost && cppHost.hasOptionAction ? ... : Qt.rect(...)`) triggered a
    // use-after-free at QQmlData teardown on Qt 6.7.3 DEBUG builds (V4 code
    // path corruption from the short-circuit + double dependency), while the
    // single-dependency shape is stable; a disabled host publishes a null
    // rect, so the behavior is identical.
    readonly property rect optionRect: cppHost ? cppHost.optionButtonRect : Qt.rect(0, 0, 0, 0)

    anchors.fill: parent
    color: RibbonTheme.contentBg
    border.width: 0
    radius: 0

    // title caption rendered inside the engine-reserved strip at the panel bottom
    Text {
        x: root.titleRect.x
        y: root.titleRect.y
        width: root.titleRect.width
        height: root.titleRect.height
        text: root.title
        visible: root.title.length > 0 && width > 0 && height > 0
        font.pixelSize: Math.round(RibbonMetrics.panelTitleHeight * 0.8)
        color: RibbonTheme.subtitle
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    // option action: the diagonal-arrow button the engine reserves at the
    // panel's bottom-right corner (widgets SARibbonPanelOptionButton parity).
    // Renders once the host publishes the reserved geometry (deferred on Qt
    // 6.7.3 debug — NOTES B46); the block stays connected so publication is
    // a one-line host change when that lands.
    Item {
        visible: root.hasOption && root.optionRect.width > 0
        x: root.optionRect.x
        y: root.optionRect.y
        width: root.optionRect.width
        height: root.optionRect.height
        Rectangle {
            anchors.fill: parent
            radius: 2
            color: optMouse.pressed ? RibbonTheme.contentPressedBg
                   : (optMouse.containsMouse ? RibbonTheme.contentHoverBg : "transparent")
        }
        Canvas {
            width: 7
            height: 7
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            property color arrowColor: RibbonTheme.subtitle
            onArrowColorChanged: requestPaint()
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = arrowColor;
                ctx.lineWidth = 1.4;
                ctx.beginPath();
                ctx.moveTo(1, height - 2);
                ctx.lineTo(width - 2, 1);
                ctx.moveTo(width - 4, 1);
                ctx.lineTo(width - 2, 1);
                ctx.lineTo(width - 2, 3);
                ctx.stroke();
            }
        }
        MouseArea {
            id: optMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: if (root.cppHost) root.cppHost.triggerOptionAction()
        }
    }
}
