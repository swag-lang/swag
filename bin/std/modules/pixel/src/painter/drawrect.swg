#global public
using Core.Math

impl Painter
{
    // Draw an empty rectangle
    func drawRect(using self, left, top, width, height: f32, color: Color, penSize: f32 = 1)
    {
        if width == 0 or height == 0 or color.a == 0 or penSize == 0
            return
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawRect(left, top, width, height, sharedSolidPen)
    }

    func drawRect(using self, rect: Math.Rectangle, color: Color, penSize: f32 = 1)
    {
        drawRect(rect.x, rect.y, rect.width, rect.height, color, penSize)
    }

    func drawRect(using self, left, top, width, height: f32, pen: Pen)
    {
        if width == 0 or height == 0 or pen.brush.color.a == 0 or pen.size == 0
            return
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
        if width == 0 or height == 0 or brush.color.a == 0
            return
        sharedLinePath.setRect(left, top, width, height)
        fillPath(&sharedLinePath, brush)
    }

    func fillRect(using self, rect: Rectangle, brush: Brush)
    {
        fillRect(rect.x, rect.y, rect.width, rect.height, brush)
    }

    func fillRect(using self, left, top, width, height: f32, color: Color)
    {
        if width == 0 or height == 0 or color.a == 0
            return
        sharedSolidBrush.color = color
        sharedSolidBrush.type = .SolidColor
        fillRect(left, top, width, height, sharedSolidBrush)
    }

    func fillRect(using self, rect: Math.Rectangle, color: Color)
    {
        fillRect(rect.x, rect.y, rect.width, rect.height, color)
    }

    // Draw an empty round rectangle
    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, color: Color, penSize: f32 = 1)
    {
        if width == 0 or height == 0 or color.a == 0 or penSize == 0
            return
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawRoundRect(left, top, width, height, radiusX, radiusY, sharedSolidPen)
    }

    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, pen: Pen)
    {
        if width == 0 or height == 0 or pen.brush.color.a == 0 or pen.size == 0
            return
        sharedLinePath.setRoundRect(left, top, width, height, radiusX, radiusY)
        drawPath(&sharedLinePath, pen)
    }

    func drawRoundRect(using self, rect: Rectangle, radiusX, radiusY: f32, pen: Pen)
    {
        drawRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, pen)
    }

    func drawRoundRect(using self, rect: Rectangle, radiusX, radiusY: f32, color: Color, penSize: f32 = 1)
    {
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, sharedSolidPen)
    }

    // Draw a filled round rectangle
    func fillRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, brush: Brush)
    {
        if width == 0 or height == 0 or brush.color.a == 0
            return
        sharedLinePath.setRoundRect(left, top, width, height, radiusX, radiusY)
        fillPath(&sharedLinePath, brush)
    }

    func fillRoundRect(using self, rect: Math.Rectangle, radiusX, radiusY: f32, brush: Brush)
    {
        if rect.width == 0 or rect.height == 0 or brush.color.a == 0
            return
        sharedLinePath.setRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY)
        fillPath(&sharedLinePath, brush)
    }

    func fillRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, color: Color)
    {
        if width == 0 or height == 0 or color.a == 0
            return
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        fillRoundRect(left, top, width, height, radiusX, radiusY, sharedSolidBrush)
    }

    func fillRoundRect(using self, rect: Math.Rectangle, radiusX, radiusY: f32, color: Color)
    {
        fillRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, color)
    }
}