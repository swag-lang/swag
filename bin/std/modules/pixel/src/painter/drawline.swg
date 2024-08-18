#global public
using Core

impl Painter
{
    // Draw a line
    #[Swag.Overload]
    mtd drawLine(start, end: Math.Vector2, pen: Pen)
    {
        drawLine(start.x, start.y, end.x, end.y, pen)
    }

    #[Swag.Overload]
    mtd drawLine(startX, startY, endX, endY: f32, color: Color, penSize: f32 = 1)
    {
        if (startX == endX and startY == endY) or color.a == 0 or penSize == 0:
            return
        sharedSolidPen.brush.type  = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size        = penSize
        drawLine(startX, startY, endX, endY, sharedSolidPen)
    }

    #[Swag.Overload]
    mtd drawLine(startX, startY, endX, endY: f32, pen: Pen)
    {
        if (startX == endX and startY == endY) or pen.brush.color.a == 0 or pen.size == 0:
            return
        sharedLinePath.start(startX, startY)
        sharedLinePath.lineTo(endX, endY)
        drawPath(&sharedLinePath, pen)
    }
}
