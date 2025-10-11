#global public
using Core

impl Painter
{
    // Draw an empty rectangle
    #[Swag.Overload]
    mtd drawRect(left, top, width, height: f32, color: Color, penSize: f32 = 1)
    {
        if width == 0 or height == 0 or color.a == 0 or penSize == 0 do
            return

        let pen = Pen.createSolid(color, penSize)
        .drawRect(left, top, width, height, pen)
    }

    #[Swag.Overload]
    mtd drawRect(rect: Math.Rectangle, color: Color, penSize: f32 = 1)
    {
        .drawRect(rect.x, rect.y, rect.width, rect.height, color, penSize)
    }

    #[Swag.Overload]
    mtd drawRect(left, top, width, height: f32, pen: Pen)
    {
        if width == 0 or height == 0 or pen.brush.color.a == 0 or pen.size == 0 do
            return

        var l = Math.round(left)
        var t = Math.round(top)
        var w = Math.round(width)
        var h = Math.round(height)

        let aa = .curState.paintParams & .Antialiased

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

        .sharedLinePath.setRectRaw(l, t, w, h)
        .drawPath(&.sharedLinePath, pen)
    }

    #[Swag.Overload]
    mtd drawRect(rect: Math.Rectangle, pen: Pen)
    {
        .drawRect(rect.x, rect.y, rect.width, rect.height, pen)
    }

    // Draw a filled rectangle
    #[Swag.Overload]
    mtd fillRect(left, top, width, height: f32, brush: Brush)
    {
        if width == 0 or height == 0 or brush.color.a == 0 do
            return

        var l = Math.round(left)
        var t = Math.round(top)
        var w = Math.round(width)
        var h = Math.round(height)

        let aa = .curState.paintParams & .Antialiased
        if aa
        {
            l -= 0.5
            t -= 0.5
            w += 1
            h += 1
        }

        .sharedLinePath.setRectRaw(l, t, w, h)
        .fillPath(&.sharedLinePath, brush)
    }

    #[Swag.Overload]
    mtd fillRect(rect: Math.Rectangle, brush: Brush)
    {
        .fillRect(rect.x, rect.y, rect.width, rect.height, brush)
    }

    #[Swag.Overload]
    mtd fillRect(left, top, width, height: f32, color: Color)
    {
        if width == 0 or height == 0 or color.a == 0 do
            return

        let brush = Brush.createSolid(color)
        .fillRect(left, top, width, height, brush)
    }

    #[Swag.Overload]
    mtd fillRect(rect: Math.Rectangle, color: Color)
    {
        .fillRect(rect.x, rect.y, rect.width, rect.height, color)
    }

    // Draw an empty round rectangle
    #[Swag.Overload]
    mtd drawRoundRect(left, top, width, height: f32, radiusX, radiusY: f32, color: Color, penSize: f32 = 1)
    {
        if width == 0 or height == 0 or color.a == 0 or penSize == 0 do
            return

        let pen = Pen.createSolid(color, penSize)
        .drawRoundRect(left, top, width, height, radiusX, radiusY, pen)
    }

    #[Swag.Overload]
    mtd drawRoundRect(left, top, width, height: f32, radiusX, radiusY: f32, pen: Pen)
    {
        if width == 0 or height == 0 or pen.brush.color.a == 0 or pen.size == 0 do
            return

        var l = Math.round(left)
        var t = Math.round(top)
        var w = Math.round(width)
        var h = Math.round(height)

        let aa = .curState.paintParams & .Antialiased

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

        .sharedLinePath.setRoundRect(l, t, w, h, radiusX, radiusY)
        .drawPath(&.sharedLinePath, pen)
    }

    #[Swag.Overload]
    mtd drawRoundRect(rect: Math.Rectangle, radiusX, radiusY: f32, pen: Pen)
    {
        .drawRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, pen)
    }

    #[Swag.Overload]
    mtd drawRoundRect(rect: Math.Rectangle, radiusX, radiusY: f32, color: Color, penSize: f32 = 1)
    {
        let pen = Pen.createSolid(color, penSize)
        .drawRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, pen)
    }

    // Draw a filled round rectangle
    #[Swag.Overload]
    mtd fillRoundRect(left, top, width, height: f32, radiusX, radiusY: f32, brush: Brush)
    {
        if width == 0 or height == 0 or brush.color.a == 0 do
            return
        .sharedLinePath.setRoundRect(left, top, width, height, radiusX, radiusY)
        .fillPath(&.sharedLinePath, brush)
    }

    #[Swag.Overload]
    mtd fillRoundRect(rect: Math.Rectangle, radiusX, radiusY: f32, brush: Brush)
    {
        if rect.width == 0 or rect.height == 0 or brush.color.a == 0 do
            return
        .sharedLinePath.setRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY)
        .fillPath(&.sharedLinePath, brush)
    }

    #[Swag.Overload]
    mtd fillRoundRect(left, top, width, height: f32, radiusX, radiusY: f32, color: Color)
    {
        if width == 0 or height == 0 or color.a == 0 do
            return

        let brush = Brush.createSolid(color)
        .fillRoundRect(left, top, width, height, radiusX, radiusY, brush)
    }

    #[Swag.Overload]
    mtd fillRoundRect(rect: Math.Rectangle, radiusX, radiusY: f32, color: Color)
    {
        .fillRoundRect(rect.x, rect.y, rect.width, rect.height, radiusX, radiusY, color)
    }
}
