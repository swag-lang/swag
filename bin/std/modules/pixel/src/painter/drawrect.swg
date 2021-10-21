#global public
using Core.Math

impl Painter
{
    // Draw an empty rectangle
    func drawRect(using self, left, top, width, height: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawRect(left, top, width, height, sharedSolidPen)
    }

    func drawRect(using self, left, top, width, height: f32, pen: Pen)
    {
        sharedLinePath.setRect(left, top, width, height)
        drawPath(&sharedLinePath, pen)
    }

    func drawRect(using self, rect: Rectangle, pen: Pen)
    {
        drawRect(rect.x, rect.y, rect.width, rect.height, pen)
    }

    // Draw a filled rectangle
    func fillRect(using self, left, top, width, height: f32, brush: Brush)
    {
        sharedLinePath.setRect(left, top, width, height)
        fillPath(&sharedLinePath, brush)
    }

    func fillRect(using self, rect: Rectangle, brush: Brush)
    {
        fillRect(rect.x, rect.y, rect.width, rect.height, brush)
    }

    func fillRect(using self, left, top, width, height: f32, color: Color)
    {
        sharedSolidBrush.color = color
        sharedSolidBrush.type = .SolidColor
        fillRect(left, top, width, height, sharedSolidBrush)
    }

    func fillRect(using self, rect: Math.Rectangle, color: Color)
    {
        fillRect(rect.x, rect.y, rect.width, rect.height, color)
    }

    // Draw an empty round rectangle
    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawRoundRect(left, top, width, height, radiusX, radiusY, sharedSolidPen)
    }

    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, pen: Pen)
    {
        sharedLinePath.setRoundRect(left, top, width, height, radiusX, radiusY)
        drawPath(&sharedLinePath, pen)
    }

    func drawRoundRect(using self, rect: Rectangle, radiusX, radiusY: f32, pen: Pen)
    {
        drawRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, pen)
    }

    // Draw a filled round rectangle
    func fillRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, brush: Brush)
    {
        sharedLinePath.setRoundRect(left, top, width, height, radiusX, radiusY)
        fillPath(&sharedLinePath, brush)
    }

    func fillRoundRect(using self, rect: Math.Rectangle, radiusX, radiusY: f32, brush: Brush)
    {
        sharedLinePath.setRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY)
        fillPath(&sharedLinePath, brush)
    }

    func fillRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, color: Color)
    {
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        fillRoundRect(left, top, width, height, radiusX, radiusY, sharedSolidBrush)
    }

    func fillRoundRect(using self, rect: Math.Rectangle, radiusX, radiusY: f32, color: Color)
    {
        fillRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, color)
    }
}