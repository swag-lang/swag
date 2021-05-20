#global public
using core, mthx, drawing

impl DrawingContext
{
    func drawSolidLine(using self, start, end: Vector2, size = 1.0, color: Color = Argb.Black)
    {
        drawingPath.clear()
        drawingPath.moveTo(start)
        drawingPath.lineTo(end)

        var pen: Pen
        pen.size = size
        //pen.color = Argb.Black
        drawPath(drawingPath, pen)
    }
}