#global public
using Core, Drawing, Core.Math

impl Painter
{
    func drawLine(using self, start, end: Vector2, pen: Pen)
    {
        drawLine(start.x, start.y, end.x, end.y, pen)
    }

    func drawLine(using self, startX, startY, endX, endY: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.color = color
        sharedSolidPen.size = penSize
        drawLine(startX, startY, endX, endY, sharedSolidPen)
    }

    func drawLine(using self, startX, startY, endX, endY: f32, pen: Pen)
    {
        sharedLinePath.start(startX, startY)
        sharedLinePath.lineTo(endX, endY)
        drawPath(&sharedLinePath, pen)
    }
}