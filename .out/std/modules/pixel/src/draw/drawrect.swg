#global public
using Core.Drawing, Core.Math

impl Painter
{
    func drawSolidRect(using self, left, top, width, height: f32, color: Color)
    {
        right := left + width
        bottom := top + height
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

        drawPath(drawingPath, pen)
    }

    func drawEmptyRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, pen: Pen)
    {
        right := left + width
        bottom := top + height

        drawingPath.clear()
        drawingPath.moveTo(@{left, top + radiusY})
        drawingPath.arcTo(@{left + radiusX, top + radiusY}, radiusX, radiusY, -90, 0)
        drawingPath.lineTo(@{right - radiusX, top})
        drawingPath.arcTo(@{right - radiusX, top + radiusY}, radiusX, radiusY, 0, 90)
        drawingPath.lineTo(@{right, bottom - radiusY})
        drawingPath.arcTo(@{right - radiusX, bottom - radiusY}, radiusX, radiusY, 90, 180)
        drawingPath.lineTo(@{left + radiusX, bottom})
        drawingPath.arcTo(@{left + radiusX, bottom - radiusY}, radiusX, radiusY, 180, 270)
        drawingPath.close()

        drawPath(drawingPath, pen)
    }
}