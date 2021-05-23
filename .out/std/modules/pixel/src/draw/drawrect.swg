#global public
using Core.Drawing, Core.Math

impl Painter
{
    func fillRect(using self, left, top, width, height: f32, color: Color)
    {
        right := left + width
        bottom := top + height
        prim1 := pushTriangle(@{left, top}, @{right, top}, @{right, bottom}, color)
        prim2 := pushTriangle(@{left, top}, @{right, bottom}, @{left, bottom}, color)
    }

    private func setRectPath(using self, left, top, width, height: f32)
    {
        right := left + width
        bottom := top + height
        sharedLinePath.clear()
        sharedLinePath.moveTo(@{left, top})
        sharedLinePath.lineTo(@{right, top})
        sharedLinePath.lineTo(@{right, bottom})
        sharedLinePath.lineTo(@{left, bottom})
        sharedLinePath.lineTo(@{left, top})
        sharedLinePath.close()
    }

    func drawRect(using self, left, top, width, height: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.color = color
        sharedSolidPen.size = penSize
        drawRect(left, top, width, height, sharedSolidPen)
    }

    func drawRect(using self, left, top, width, height: f32, pen: Pen)
    {
        setRectPath(left, top, width, height)
        drawPath(sharedLinePath, pen)
    }

    private func setRoundRectPath(using self, left, top, width, height: f32, radiusX, radiusY: f32)
    {
        right := left + width
        bottom := top + height
        sharedLinePath.clear()
        sharedLinePath.moveTo(@{left, top + radiusY})
        sharedLinePath.arcTo(@{left + radiusX, top + radiusY}, radiusX, radiusY, -90, 0)
        sharedLinePath.lineTo(@{right - radiusX, top})
        sharedLinePath.arcTo(@{right - radiusX, top + radiusY}, radiusX, radiusY, 0, 90)
        sharedLinePath.lineTo(@{right, bottom - radiusY})
        sharedLinePath.arcTo(@{right - radiusX, bottom - radiusY}, radiusX, radiusY, 90, 180)
        sharedLinePath.lineTo(@{left + radiusX, bottom})
        sharedLinePath.arcTo(@{left + radiusX, bottom - radiusY}, radiusX, radiusY, 180, 270)
        sharedLinePath.close()
    }

    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.color = color
        sharedSolidPen.size = penSize
        drawRoundRect(left, top, width, height, radiusX, radiusY, sharedSolidPen)
    }

    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, pen: Pen)
    {
        setRoundRectPath(left, top, width, height, radiusX, radiusY)
        drawPath(sharedLinePath, pen)
    }
}