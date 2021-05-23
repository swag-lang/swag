#global public
using Core, Drawing, Core.Math

impl Painter
{
    func drawLine(using self, start, end: Vector2, pen: Pen)
    {
        drawLine(start.x, start.y, end.x, end.y, pen)
    }

    func drawLine(using self, startX, startY, endX, endY: f32, color: Color, size: f32)
    {
        drawingSolidPen.color = color
        drawingSolidPen.size = size
        drawLine(startX, startY, endX, endY, drawingSolidPen)
    }

    func drawLine(using self, startX, startY, endX, endY: f32, pen: Pen)
    {
        drawingPath.clear()
        drawingPath.moveTo(startX, startY)
        drawingPath.lineTo(endX, endY)
        drawPath(drawingPath, pen)
    }
}