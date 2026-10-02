import QtQuick 2.12

// The "no color" mark: a white box, a red round-cap diagonal slash and a black
// outline. Shared by the color grid cells and the color menu's "no color" row so
// the two cannot drift apart.
// Only the slash inset is data: the owning C++ host publishes it, and it comes
// from core SA::noneColorSlashLine — the very call widgets
// SAColorToolButton::paintNoneColor makes — so both front ends draw an identical
// mark instead of each rediscovering the geometry.
// The three colors are the mark's own fixed colors rather than theme tokens:
// widgets paints them as Qt::white / Qt::red / Qt::black literals, and letting a
// theme recolor them would change what the mark means.
// NOTE: no inline `component` syntax here — the module targets Qt 5.12.
Canvas {
    id: mark

    // Horizontal inset of both slash ends, relative to the mark box
    property int slashInset: 0
    // Inset from this item's edge to the mark box. Widgets paints the icon
    // version into a rect adjusted by 1 on every side, so the menu row passes 1
    // and a grid cell, whose canvas already is the swatch, passes 0
    property int markMargin: 0

    onSlashInsetChanged: requestPaint()
    onMarkMarginChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()

    readonly property int boxX: mark.markMargin
    readonly property int boxY: mark.markMargin
    readonly property int boxW: Math.max(mark.width - 2 * mark.markMargin, 0)
    readonly property int boxH: Math.max(mark.height - 2 * mark.markMargin, 0)

    onPaint: {
        var ctx = getContext("2d");
        ctx.reset();
        if (boxW <= 0 || boxH <= 0) {
            return;
        }
        ctx.fillStyle = "#ffffff";
        ctx.fillRect(boxX, boxY, boxW, boxH);
        ctx.lineWidth = 1;
        ctx.lineCap = "round";
        ctx.strokeStyle = "#ff0000";
        ctx.beginPath();
        // the +0.5 offsets centre the 1px stroke on the pixel row QPainter's
        // integer coordinates address
        ctx.moveTo(boxX + slashInset + 0.5, boxY + boxH - 0.5);
        ctx.lineTo(boxX + boxW - 1 - slashInset + 0.5, boxY + 0.5);
        ctx.stroke();
        ctx.strokeStyle = "#000000";
        ctx.strokeRect(boxX + 0.5, boxY + 0.5, boxW - 1, boxH - 1);
    }
}
