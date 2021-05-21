#global public
using Core, Drawing, Core.Math

impl Painter
{
    func drawSolidLine(using self, start, end: Vector2, pen: Pen)
    {
        drawingPath.clear()
        drawingPath.moveTo(start)
        drawingPath.lineTo(end)
        drawPath(drawingPath, pen)
    }
}