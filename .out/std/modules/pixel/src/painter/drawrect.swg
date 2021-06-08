#global public
using Core.Drawing, Core.Math

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

    // Draw an empty rectangle
    func drawRect(using self, left, top, width, height: f32, pen: Pen)
    {
        sharedLinePath.setRect(left, top, width, height)
        drawPath(&sharedLinePath, pen)
    }

    // Draw a filled rectangle
    func fillRect(using self, left, top, width, height: f32, color: Color)
    {
        sharedLinePath.setRect(left, top, width, height)
        sharedSolidBrush.color = color
        sharedSolidBrush.type = .SolidColor
        fillPath(&sharedLinePath, sharedSolidBrush)
    }

    // Draw a filled rectangle
    func fillRect(using self, left, top, width, height: f32, brush: Brush)
    {
        sharedLinePath.setRect(left, top, width, height)
        fillPath(&sharedLinePath, brush)
    }

    // Draw an empty round rectangle
    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawRoundRect(left, top, width, height, radiusX, radiusY, sharedSolidPen)
    }

    // Draw an empty round rectangle
    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, pen: Pen)
    {
        sharedLinePath.setRoundRect(left, top, width, height, radiusX, radiusY)
        drawPath(&sharedLinePath, pen)
    }

    // Draw a filled round rectangle
    func fillRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, brush: Brush)
    {
        sharedLinePath.setRoundRect(left, top, width, height, radiusX, radiusY)
        fillPath(&sharedLinePath, brush)
    }

    // Draw a filled round rectangle
    func fillRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, color: Color)
    {
        sharedLinePath.setRoundRect(left, top, width, height, radiusX, radiusY)
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        fillPath(&sharedLinePath, sharedSolidBrush)
    }
}