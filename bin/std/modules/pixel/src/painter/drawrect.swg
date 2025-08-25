#global public
using Core

impl Painter
{
    // Draw an empty rectangle
    #[Swag.Overload]
    mtd drawRect(left, top, width, height: f32, color: Color, penSize: f32 = 1)
    {
        if width == 0 or height == 0 or color.a == 0 or penSize == 0:
            return
        me.sharedSolidPen.brush.type  = .SolidColor
        me.sharedSolidPen.brush.color = color
        me.sharedSolidPen.size        = penSize
        me.drawRect(left, top, width, height, me.sharedSolidPen)
    }

    #[Swag.Overload]
    mtd drawRect(rect: Math.Rectangle, color: Color, penSize: f32 = 1)
    {
        me.drawRect(rect.x, rect.y, rect.width, rect.height, color, penSize)
    }

    #[Swag.Overload]
    mtd drawRect(left, top, width, height: f32, pen: Pen)
    {
        if width == 0 or height == 0 or pen.brush.color.a == 0 or pen.size == 0:
            return

        var l = Math.round(left)
        var t = Math.round(top)
        var w = Math.round(width)
        var h = Math.round(height)

        let aa = me.curState.paintParams & .Antialiased

        #[Swag.Complete]
        switch pen.borderPos
        {
        case None:
            let o = 0.5
            l += o
            t += o
            w -= 2 * o
            h -= 2 * o
        case Inside:
            let o = 0.5 + pen.size * 0.5
            l += o
            t += o
            w -= 2 * o
            h -= 2 * o
        }

        if aa
        {
            l -= 0.5
            t -= 0.5
            w += 1.0
            h += 1.0
        }

        me.sharedLinePath.setRectRaw(l, t, w, h)
        me.drawPath(&me.sharedLinePath, pen)
    }

    #[Swag.Overload]
    mtd drawRect(rect: Math.Rectangle, pen: Pen)
    {
        me.drawRect(rect.x, rect.y, rect.width, rect.height, pen)
    }

    // Draw a filled rectangle
    #[Swag.Overload]
    mtd fillRect(left, top, width, height: f32, brush: Brush)
    {
        if width == 0 or height == 0 or brush.color.a == 0:
            return

        var l = Math.round(left)
        var t = Math.round(top)
        var w = Math.round(width)
        var h = Math.round(height)

        let aa = me.curState.paintParams & .Antialiased
        if aa
        {
            l -= 0.5
            t -= 0.5
            w += 1
            h += 1
        }

        me.sharedLinePath.setRectRaw(l, t, w, h)
        me.fillPath(&me.sharedLinePath, brush)
    }

    #[Swag.Overload]
    mtd fillRect(rect: Math.Rectangle, brush: Brush)
    {
        me.fillRect(rect.x, rect.y, rect.width, rect.height, brush)
    }

    #[Swag.Overload]
    mtd fillRect(left, top, width, height: f32, color: Color)
    {
        if width == 0 or height == 0 or color.a == 0:
            return
        me.sharedSolidBrush.color = color
        me.sharedSolidBrush.type  = .SolidColor
        me.fillRect(left, top, width, height, me.sharedSolidBrush)
    }

    #[Swag.Overload]
    mtd fillRect(rect: Math.Rectangle, color: Color)
    {
        me.fillRect(rect.x, rect.y, rect.width, rect.height, color)
    }

    // Draw an empty round rectangle
    #[Swag.Overload]
    mtd drawRoundRect(left, top, width, height: f32, radiusX, radiusY: f32, color: Color, penSize: f32 = 1)
    {
        if width == 0 or height == 0 or color.a == 0 or penSize == 0:
            return
        me.sharedSolidPen.brush.type  = .SolidColor
        me.sharedSolidPen.brush.color = color
        me.sharedSolidPen.size        = penSize
        me.drawRoundRect(left, top, width, height, radiusX, radiusY, me.sharedSolidPen)
    }

    #[Swag.Overload]
    mtd drawRoundRect(left, top, width, height: f32, radiusX, radiusY: f32, pen: Pen)
    {
        if width == 0 or height == 0 or pen.brush.color.a == 0 or pen.size == 0:
            return

        var l = Math.round(left)
        var t = Math.round(top)
        var w = Math.round(width)
        var h = Math.round(height)

        let aa = me.curState.paintParams & .Antialiased

        #[Swag.Complete]
        switch pen.borderPos
        {
        case None:
            let o = 0.5
            l += o
            t += o
            w -= 2 * o
            h -= 2 * o
        case Inside:
            let o = 0.5 + pen.size * 0.5
            l += o
            t += o
            w -= 2 * o
            h -= 2 * o
        }

        if aa
        {
            l -= 0.5
            t -= 0.5
            w += 1.0
            h += 1.0
        }

        me.sharedLinePath.setRoundRect(l, t, w, h, radiusX, radiusY)
        me.drawPath(&me.sharedLinePath, pen)
    }

    #[Swag.Overload]
    mtd drawRoundRect(rect: Math.Rectangle, radiusX, radiusY: f32, pen: Pen)
    {
        me.drawRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, pen)
    }

    #[Swag.Overload]
    mtd drawRoundRect(rect: Math.Rectangle, radiusX, radiusY: f32, color: Color, penSize: f32 = 1)
    {
        me.sharedSolidPen.brush.type  = .SolidColor
        me.sharedSolidPen.brush.color = color
        me.sharedSolidPen.size        = penSize
        me.drawRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, me.sharedSolidPen)
    }

    // Draw a filled round rectangle
    #[Swag.Overload]
    mtd fillRoundRect(left, top, width, height: f32, radiusX, radiusY: f32, brush: Brush)
    {
        if width == 0 or height == 0 or brush.color.a == 0:
            return
        me.sharedLinePath.setRoundRect(left, top, width, height, radiusX, radiusY)
        me.fillPath(&me.sharedLinePath, brush)
    }

    #[Swag.Overload]
    mtd fillRoundRect(rect: Math.Rectangle, radiusX, radiusY: f32, brush: Brush)
    {
        if rect.width == 0 or rect.height == 0 or brush.color.a == 0:
            return
        me.sharedLinePath.setRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY)
        me.fillPath(&me.sharedLinePath, brush)
    }

    #[Swag.Overload]
    mtd fillRoundRect(left, top, width, height: f32, radiusX, radiusY: f32, color: Color)
    {
        if width == 0 or height == 0 or color.a == 0:
            return
        me.sharedSolidBrush.type  = .SolidColor
        me.sharedSolidBrush.color = color
        me.fillRoundRect(left, top, width, height, radiusX, radiusY, me.sharedSolidBrush)
    }

    #[Swag.Overload]
    mtd fillRoundRect(rect: Math.Rectangle, radiusX, radiusY: f32, color: Color)
    {
        me.fillRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, color)
    }
}
