#global public
using Core

impl Painter
{
    // Draw a line
    #[Swag.Overload]
    mtd drawLine(start, end: Math.Vector2, pen: Pen)
    {
        me.drawLine(start.x, start.y, end.x, end.y, pen)
    }

    #[Swag.Overload]
    mtd drawLine(startX, startY, endX, endY: f32, color: Color, penSize: f32 = 1)
    {
        if (startX == endX and startY == endY) or color.a == 0 or penSize == 0:
            return
        me.sharedSolidPen.brush.type  = .SolidColor
        me.sharedSolidPen.brush.color = color
        me.sharedSolidPen.size        = penSize
        me.drawLine(startX, startY, endX, endY, me.sharedSolidPen)
    }

    #[Swag.Overload]
    mtd drawLine(startX, startY, endX, endY: f32, pen: Pen)
    {
        if (startX == endX and startY == endY) or pen.brush.color.a == 0 or pen.size == 0:
            return
        me.sharedLinePath.start(startX, startY)
        me.sharedLinePath.lineTo(endX, endY)
        me.drawPath(&me.sharedLinePath, pen)
    }

    #[Swag.Overload]
    mtd drawBezier(startX, startY, endX, endY: f32, ptX, ptY: f32, pen: Pen)
    {
        if (startX == endX and startY == endY) or pen.brush.color.a == 0 or pen.size == 0:
            return
        me.sharedLinePath.start(startX, startY)
        me.sharedLinePath.bezierTo(endX, endY, ptX, ptY)
        me.drawPath(&me.sharedLinePath, pen)
    }
}
