#global public
using Core.Drawing, Core.Math

impl Painter
{
    func drawSolidRect(using self, left, top, width, height: f32, color: Color)
    {
        right := left + width
        bottom := top + height
        startTriangles(Vector2.Zero)
        prim1 := pushTriangle(@{left, top}, @{right, top}, @{right, bottom}, color)
        prim2 := pushTriangle(@{left, top}, @{right, bottom}, @{left, bottom}, color)
    }

    func drawEmptyRect(using self, left, top, width, height: f32, pen: Pen)
    {
        right := left + width
        bottom := top + height

        drawingPath.clear()
        drawingPath.moveTo(@{left, top})
        drawingPath.lineTo(@{right, top})
        drawingPath.lineTo(@{right, bottom})
        drawingPath.lineTo(@{left, bottom})
        drawingPath.lineTo(@{left, top})
        drawingPath.close()

        drawPath(Vector2.Zero, drawingPath, pen)
    }
}