#global public
using Core, Drawing, Core.Math

impl Painter
{
    func drawLine(using self, start, end: Vector2, pen: Pen)
    {
        drawLine(start.x, start.y, end.x, end.y, pen)
    }

    // Draw a line
    func drawLine(using self, startX, startY, endX, endY: f32, pen: Pen)
    {
        drawingPath.clear()
        drawingPath.moveTo(startX, startY)
        drawingPath.lineTo(endX, endY)
        drawPath(drawingPath, pen)
    }
}