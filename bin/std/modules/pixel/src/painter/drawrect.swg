#global public
using Core

impl Painter
{
    // Draw an empty rectangle
    #[Swag.Overload]
    func drawRect(using self, left, top, width, height: f32, color: Color, penSize: f32 = 1)
    {
        if width == 0 or height == 0 or color.a == 0 or penSize == 0
            return
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawRect(left, top, width, height, sharedSolidPen)
    }

    #[Swag.Overload]
    func drawRect(using self, rect: Math.Rectangle, color: Color, penSize: f32 = 1)
    {
        drawRect(rect.x, rect.y, rect.width, rect.height, color, penSize)
    }

    #[Swag.Overload]
    func drawRect(using self, left, top, width, height: f32, pen: Pen)
    {
        if width == 0 or height == 0 or pen.brush.color.a == 0 or pen.size == 0
            return

        var l = Math.round(left)
        var t = Math.round(top)
        var w = Math.round(width)
        var h = Math.round(height)

        let aa = curState.paintParams & .Antialiased

        #[Swag.Complete]
        switch pen.borderPos
        {
        case None:
            let o = 0.5
            l += o
            t += o
            w -= 2*o
            h -= 2*o
        case Inside:
            let o = 0.5 + pen.size * 0.5
            l += o
            t += o
            w -= 2*o
            h -= 2*o
        }

        if aa
        {
            l -= 0.5
            t -= 0.5
            w += 1.0
            h += 1.0
        }

        sharedLinePath.setRectRaw(l, t, w, h)
        drawPath(&sharedLinePath, pen)
    }

    #[Swag.Overload]
    func drawRect(using self, rect: Math.Rectangle, pen: Pen)
    {
        drawRect(rect.x, rect.y, rect.width, rect.height, pen)
    }

    // Draw a filled rectangle
    #[Swag.Overload]
    func fillRect(using self, left, top, width, height: f32, brush: Brush)
    {
        if width == 0 or height == 0 or brush.color.a == 0
            return

        var l = Math.round(left)
        var t = Math.round(top)
        var w = Math.round(width)
        var h = Math.round(height)

        let aa = curState.paintParams & .Antialiased
        if aa
        {
            l -= 0.5
            t -= 0.5
            w += 1
            h += 1
        }

        sharedLinePath.setRectRaw(l, t, w, h)
        fillPath(&sharedLinePath, brush)
    }

    #[Swag.Overload]
    func fillRect(using self, rect: Math.Rectangle, brush: Brush)
    {
        fillRect(rect.x, rect.y, rect.width, rect.height, brush)
    }

    #[Swag.Overload]
    func fillRect(using self, left, top, width, height: f32, color: Color)
    {
        if width == 0 or height == 0 or color.a == 0
            return
        sharedSolidBrush.color = color
        sharedSolidBrush.type = .SolidColor
        fillRect(left, top, width, height, sharedSolidBrush)
    }

    #[Swag.Overload]
    func fillRect(using self, rect: Math.Rectangle, color: Color)
    {
        fillRect(rect.x, rect.y, rect.width, rect.height, color)
    }

    // Draw an empty round rectangle
    #[Swag.Overload]
    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, color: Color, penSize: f32 = 1)
    {
        if width == 0 or height == 0 or color.a == 0 or penSize == 0
            return
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawRoundRect(left, top, width, height, radiusX, radiusY, sharedSolidPen)
    }

    #[Swag.Overload]
    func drawRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, pen: Pen)
    {
        if width == 0 or height == 0 or pen.brush.color.a == 0 or pen.size == 0
            return

        var l = Math.round(left)
        var t = Math.round(top)
        var w = Math.round(width)
        var h = Math.round(height)

        let aa = curState.paintParams & .Antialiased

        #[Swag.Complete]
        switch pen.borderPos
        {
        case None:
            let o = 0.5
            l += o
            t += o
            w -= 2*o
            h -= 2*o
        case Inside:
            let o = 0.5 + pen.size * 0.5
            l += o
            t += o
            w -= 2*o
            h -= 2*o
        }

        if aa
        {
            l -= 0.5
            t -= 0.5
            w += 1.0
            h += 1.0
        }

        sharedLinePath.setRoundRect(l, t, w, h, radiusX, radiusY)
        drawPath(&sharedLinePath, pen)
    }

    #[Swag.Overload]
    func drawRoundRect(using self, rect: Math.Rectangle, radiusX, radiusY: f32, pen: Pen)
    {
        drawRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, pen)
    }

    #[Swag.Overload]
    func drawRoundRect(using self, rect: Math.Rectangle, radiusX, radiusY: f32, color: Color, penSize: f32 = 1)
    {
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, sharedSolidPen)
    }

    // Draw a filled round rectangle
    #[Swag.Overload]
    func fillRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, brush: Brush)
    {
        if width == 0 or height == 0 or brush.color.a == 0
            return
        sharedLinePath.setRoundRect(left, top, width, height, radiusX, radiusY)
        fillPath(&sharedLinePath, brush)
    }

    #[Swag.Overload]
    func fillRoundRect(using self, rect: Math.Rectangle, radiusX, radiusY: f32, brush: Brush)
    {
        if rect.width == 0 or rect.height == 0 or brush.color.a == 0
            return
        sharedLinePath.setRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY)
        fillPath(&sharedLinePath, brush)
    }

    #[Swag.Overload]
    func fillRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32, color: Color)
    {
        if width == 0 or height == 0 or color.a == 0
            return
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        fillRoundRect(left, top, width, height, radiusX, radiusY, sharedSolidBrush)
    }

    #[Swag.Overload]
    func fillRoundRect(using self, rect: Math.Rectangle, radiusX, radiusY: f32, color: Color)
    {
        fillRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, color)
    }
}