#global public
using Core, Drawing, Core.Math

impl Painter
{
    // Draw a line
    func drawLine(using self, start, end: Vector2, pen: Pen)
    {
        drawLine(start.x, start.y, end.x, end.y, pen)
    }

    // Draw a line
    func drawLine(using self, startX, startY, endX, endY: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.brush.textureType = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawLine(startX, startY, endX, endY, sharedSolidPen)
    }

    // Draw a line
    func drawLine(using self, startX, startY, endX, endY: f32, pen: Pen)
    {
        sharedLinePath.start(startX, startY)
        sharedLinePath.lineTo(endX, endY)
        drawPath(&sharedLinePath, pen)
    }
}