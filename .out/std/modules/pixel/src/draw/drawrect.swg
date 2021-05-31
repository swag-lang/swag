#global public
using Core.Drawing, Core.Math

impl Painter
{
    private func setRectPath(using self, left, top, width, height: f32)
    {
        right := left + width
        bottom := top + height
        sharedLinePath.start(left, top)
        sharedLinePath.lineTo(right, top)
        sharedLinePath.lineTo(right, bottom)
        sharedLinePath.lineTo(left, bottom)
        sharedLinePath.lineTo(left, top)
        sharedLinePath.close()
    }

    private func setRoundRectPath(using self, left, top, width, height: f32, radiusX, radiusY: f32)
    {
        right := left + width
        bottom := top + height
        sharedLinePath.start(left, top + radiusY)
        sharedLinePath.arcTo(left + radiusX, top + radiusY, radiusX, radiusY, -90, 0)
        sharedLinePath.lineTo(right - radiusX, top)
        sharedLinePath.arcTo(right - radiusX, top + radiusY, radiusX, radiusY, 0, 90)
        sharedLinePath.lineTo(right, bottom - radiusY)
        sharedLinePath.arcTo(right - radiusX, bottom - radiusY, radiusX, radiusY, 90, 180)
        sharedLinePath.lineTo(left + radiusX, bottom)
        sharedLinePath.arcTo(left + radiusX, bottom - radiusY, radiusX, radiusY, 180, 270)
        sharedLinePath.close()
    }

    // Draw an empty rectangle
    func drawRect(using self, left, top, width, height: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.color = color
        sharedSolidPen.size = penSize
        drawRect(left, top, width, height, sharedSolidPen)
    }

    // Draw an empty rectangle
    func drawRect(using self, left, top, width, height: f32, pen: Pen)
    {
        setRectPath(left, top, width, height)
        drawPath(&sharedLinePath, pen)
    }

    // Draw a filled rectangle
    func fillRect(using self, left, top, width, height: f32, color: Color)
    {
        setRectPath(left, top, width, height)
        sharedSolidBrush.color = color
        sharedSolidBrush.textureType = .None
        fillPath(&sharedLinePath, sharedSolidBrush)
    }

    // Draw a filled rectangle
    func fillRect(using self, left, top, width, height: f32, brush: Brush)
    {
        setRectPath(left, top, width, height)
        fillPath(&sharedLinePath, brush)
    }

    // Draw an empty round rectangle
    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.color = color
        sharedSolidPen.size = penSize
        drawRoundRect(left, top, width, height, radiusX, radiusY, sharedSolidPen)
    }

    // Draw an empty round rectangle
    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, pen: Pen)
    {
        setRoundRectPath(left, top, width, height, radiusX, radiusY)
        drawPath(&sharedLinePath, pen)
    }

    // Draw a filled round rectangle
    func fillRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, brush: Brush)
    {
        setRoundRectPath(left, top, width, height, radiusX, radiusY)
        fillPath(&sharedLinePath, brush)
    }
}