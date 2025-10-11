#global public
using Core, Pixel

impl Theme
{
    #[Swag.Overload]
    mtd drawSubRect(painter: *Painter, dstRect: Math.Rectangle, subRect: *ThemeImageRect, color: Color, center = true, interpol = InterpolationMode.Linear)
    {
        drawSubRect(painter, .res.imgWidgets, dstRect, subRect, color, center, interpol)
    }

    #[Swag.Overload]
    func drawSubRect(painter: *Painter, texture: Texture, dstRect: Math.Rectangle, subRect: *ThemeImageRect, color: Color, center = true, interpol = InterpolationMode.Linear)
    {
        if subRect == null or subRect.rect.isEmpty() or dstRect.isEmpty() or color.a == 0 do
            return

        painter.setInterpolationMode(interpol)
        defer painter.setInterpolationMode(.Linear)

        if subRect.corner == 0
        {
            painter.drawTexture(dstRect, subRect.rect, texture, color)
            return
        }

        let cx, cy = subRect.corner
        var cxd, cyd = subRect.corner
        if cxd * 2 > dstRect.width do
            cxd = dstRect.width / 2
        if cyd * 2 > dstRect.height do
            cyd = dstRect.height / 2

        if subRect.keepBordersRatio
        {
            cxd = Math.min(cxd, cyd)
            cyd = cxd
        }

        var d = dstRect
        var s = subRect.rect
        for i in 9
        {
            switch i
            {
            case 0:
                d.width  = cxd
                d.height = cyd
                s.width  = cx
                s.height = cy
                painter.drawTexture(d, s, texture, color)

            case 1, 7:
                d.x += cxd
                s.x += cx
                d.width = dstRect.width - (cxd * 2)
                s.width = subRect.rect.width - (cx * 2)
                painter.drawTexture(d, s, texture, color)

            case 2, 8:
                d.x     = dstRect.x + dstRect.width - cxd
                s.x     = subRect.rect.x + subRect.rect.width - cx
                d.width = cxd
                s.width = cx
                painter.drawTexture(d, s, texture, color)

            case 3:
                d.x      = dstRect.x
                d.y      = dstRect.y + cyd
                s.x      = subRect.rect.x
                s.y      = subRect.rect.y + cy
                d.width  = cxd
                s.width  = cx
                d.height = dstRect.height - (cyd * 2)
                s.height = subRect.rect.height - (cy * 2)
                painter.drawTexture(d, s, texture, color)

            case 4:
                d.x += cxd
                s.x += cx
                d.width  = dstRect.width - (cxd * 2)
                d.height = dstRect.height - (cyd * 2)
                s.width  = subRect.rect.width - (cx * 2)
                s.height = subRect.rect.height - (cy * 2)
                if center do
                    painter.drawTexture(d, s, texture, color)

            case 5:
                d.x += d.width
                s.x += s.width
                d.width = cxd
                s.width = cx
                painter.drawTexture(d, s, texture, color)

            case 6:
                d.x      = dstRect.x
                d.y      = dstRect.y + dstRect.height - cyd
                s.x      = subRect.rect.x
                s.y      = subRect.rect.y + subRect.rect.height - cy
                d.width  = cxd
                d.height = cyd
                s.width  = cx
                s.height = cy
                painter.drawTexture(d, s, texture, color)
            }
        }
    }

    func drawIconText(painter: *Painter, position: Math.Rectangle, icon: Icon, name: *RichString, stringFormat: RichStringFormat, iconMargin = 4'f32)
    {
        let count = @countof(dref name)
        if count == 0 and !icon.isValid() do
            return

        var pos       = position
        let iconColor = icon.color.a ? icon.color : stringFormat.palette[0]

        // Icon alone
        if count == 0
        {
            pos.x = pos.horzCenter() - icon.sizeX * 0.5
            icon.paint(painter, pos.x, pos.vertCenter() - (icon.sizeY * 0.5), iconColor)
            return
        }

        // Text + icon
        if icon.isValid() do
            pos.moveLeft(icon.sizeX + iconMargin)

        // Text
        painter.drawRichString(pos, name, stringFormat)

        // Icon
        if icon.isValid() do
            icon.paint(painter, name.boundRect.x - icon.sizeX - iconMargin, name.boundRect.vertCenter() - (icon.sizeY * 0.5), iconColor)
    }

    mtd setTextColors(fmt: *RichStringFormat, mainColor: Color)
    {
        fmt.palette[0] = mainColor
        fmt.palette[1] = .colors.richColor1
        fmt.palette[2] = .colors.richColor2
        fmt.palette[3] = .colors.richColor3
    }
}
