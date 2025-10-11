#global public
using Core

impl Painter
{
    // Draw a line
    #[Swag.Overload]
    mtd drawLine(start, end: Math.Vector2, pen: Pen)
    {
        .drawLine(start.x, start.y, end.x, end.y, pen)
    }

    #[Swag.Overload]
    mtd drawLine(startX, startY, endX, endY: f32, color: Color, penSize: f32 = 1)
    {
        if (startX == endX and startY == endY) or color.a == 0 or penSize == 0 do
            return

        let pen = Pen.createSolid(color, penSize)
        .drawLine(startX, startY, endX, endY, pen)
    }

    #[Swag.Overload]
    mtd drawLine(startX, startY, endX, endY: f32, pen: Pen)
    {
        if (startX == endX and startY == endY) or pen.brush.color.a == 0 or pen.size == 0 do
            return
        .sharedLinePath.start(startX, startY)
        .sharedLinePath.lineTo(endX, endY)
        .drawPath(&.sharedLinePath, pen)
    }

    #[Swag.Overload]
    mtd drawBezier(startX, startY, endX, endY: f32, ptX, ptY: f32, pen: Pen)
    {
        if (startX == endX and startY == endY) or pen.brush.color.a == 0 or pen.size == 0 do
            return
        .sharedLinePath.start(startX, startY)
        .sharedLinePath.bezierTo(endX, endY, ptX, ptY)
        .drawPath(&.sharedLinePath, pen)
    }
}
