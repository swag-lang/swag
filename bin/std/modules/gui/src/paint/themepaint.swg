#global public
using Core, Pixel

impl Theme
{
    mtd drawSubRect(painter: *Painter, dstRect: Math.Rectangle, subRect: *ThemeImageRect, color: Color, center = true)
    {
        drawSubRect(painter, res.imgWidgets, dstRect, subRect, color, center)
    }

    func drawSubRect(painter: *Painter, texture: Texture, dstRect: Math.Rectangle, subRect: *ThemeImageRect, color: Color, center = true)
    {
        if subRect == null or subRect.rect.isEmpty() or dstRect.isEmpty() or color.a == 0
            return

        painter.setInterpolationMode(InterpolationMode.Linear)

        if subRect.corner == 0
        {
            painter.drawTexture(dstRect, subRect.rect, texture, color)
            return
        }

        cx, cy := subRect.corner
        cxd, cyd := subRect.corner
        if cxd * 2 > dstRect.width
            cxd = dstRect.width / 2
        if cyd * 2 > dstRect.height
            cyd = dstRect.height / 2

        d := dstRect
        s := subRect.rect
        loop i: 9
        {
            switch i
            {
            case 0:
                d.width = cxd; d.height = cyd
                s.width = cx; s.height = cy
                painter.drawTexture(d, s, texture, color)

            case 1, 7:
                d.x += cxd; s.x += cx
                d.width = dstRect.width - (cxd * 2)
                s.width = subRect.rect.width - (cx * 2)
                painter.drawTexture(d, s, texture, color)

            case 2, 8:
                d.x = dstRect.x + dstRect.width - cxd
                s.x = subRect.rect.x + subRect.rect.width - cx
                d.width = cxd; s.width = cx
                painter.drawTexture(d, s, texture, color)

            case 3:
                d.x = dstRect.x; d.y = dstRect.y + cyd
                s.x = subRect.rect.x; s.y = subRect.rect.y + cy
                d.width = cxd; s.width = cx;
                d.height = dstRect.height - (cyd * 2)
                s.height = subRect.rect.height - (cy * 2)
                painter.drawTexture(d, s, texture, color)

            case 4:
                d.x += cxd; s.x += cx
                d.width = dstRect.width - (cxd * 2)
                d.height = dstRect.height - (cyd * 2)
                s.width = subRect.rect.width - (cx * 2)
                s.height = subRect.rect.height - (cy * 2)
                if center
                    painter.drawTexture(d, s, texture, color)

            case 5:
                d.x += d.width; s.x += s.width
                d.width = cxd; s.width = cx
                painter.drawTexture(d, s, texture, color)

            case 6:
                d.x = dstRect.x; d.y = dstRect.y + dstRect.height - cyd
                s.x = subRect.rect.x; s.y = subRect.rect.y + subRect.rect.height - cy
                d.width = cxd; d.height = cyd
                s.width = cx; s.height = cy
                painter.drawTexture(d, s, texture, color)
            }
        }
    }

    func drawIconText(painter: *Painter, position: Math.Rectangle, icon: Icon, name: *RichString, stringFormat: RichStringFormat, iconMargin = 4'f32)
    {
        count := @countof(dref name)
        if count == 0 and !icon.isValid()
            return

        pos := position

        // Icon alone
        if count == 0
        {
            pos.x = pos.horzCenter() - icon.size * 0.5
            icon.paint(painter, pos.x, pos.vertCenter() - (icon.size * 0.5), stringFormat.palette[0])
            return
        }

        // Text + icon
        if icon.isValid()
        {
            switch stringFormat.horzAlignment
            {
            case Left:
                pos.x += icon.size + iconMargin
            case Center:
                pos.x += (icon.size + iconMargin) * 0.5
            }
        }

        painter.drawRichString(pos, name, stringFormat)

        if icon.isValid()
        {
            icon.paint(painter, name.boundRect.x - icon.size - iconMargin, name.boundRect.vertCenter() - (icon.size * 0.5), stringFormat.palette[0])
        }
    }
}