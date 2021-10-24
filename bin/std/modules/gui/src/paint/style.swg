#global namespace Style
using Core, Pixel

func drawTexture(painter: *Painter, texture: Texture, dstRect, srcRect: Math.Rectangle, corner: f32, color: Color)
{
    if srcRect.isEmpty() or dstRect.isEmpty() or color.a == 0
        return

    painter.setInterpolationMode(InterpolationMode.Linear)

    if corner == 0
    {
        painter.drawTexture(dstRect, srcRect, texture, color)
        return
    }

    cx, cy := corner
    if cx * 2 > dstRect.width
        cx = dstRect.width / 2
    if cy * 2 > dstRect.height
        cy = dstRect.height / 2

    d := dstRect
    s := srcRect
    loop i: 9
    {
        switch i
        {
        case 0:
            d.width = cx; d.height = cy
            s.width = cx; s.height = cy
            painter.drawTexture(d, s, texture, color)

        case 1, 7:
            d.x += cx; s.x += cx
            d.width = dstRect.width - (cx * 2)
            s.width = srcRect.width - (cx * 2)
            painter.drawTexture(d, s, texture, color)

        case 2, 8:
            d.x = dstRect.x + dstRect.width - cx
            s.x = srcRect.x + srcRect.width - cx
            d.width = cx; s.width = cx
            painter.drawTexture(d, s, texture, color)

        case 3:
            d.x = dstRect.x; d.y = dstRect.y + cx
            s.x = srcRect.x; s.y = srcRect.y + cy
            d.width = cx; s.width = cx;
            d.height = dstRect.height - (cy * 2)
            s.height = srcRect.height - (cy * 2)
            painter.drawTexture(d, s, texture, color)

        case 4:
            d.x += cx; s.x += cx
            d.width = dstRect.width - (cx * 2)
            d.height = dstRect.height - (cy * 2)
            s.width = srcRect.width - (cx * 2)
            s.height = srcRect.height - (cy * 2)
            painter.drawTexture(d, s, texture, color)

        case 5:
            d.x += d.width; s.x += s.width
            d.width = cx; s.width = cy
            painter.drawTexture(d, s, texture, color)

        case 6:
            d.x = dstRect.x; d.y = dstRect.y + dstRect.height - cy
            s.x = srcRect.x; s.y = srcRect.y + srcRect.height - cy
            d.width = cx; d.height = cy
            s.width = cx; s.height = cy
            painter.drawTexture(d, s, texture, color)
        }
    }
}