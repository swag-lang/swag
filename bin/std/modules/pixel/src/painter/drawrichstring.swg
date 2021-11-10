using Core, Math

public struct RichStringFormat
{
    font:       FontFamily
    palette:    [4] Color
}

impl Painter
{
    // Recompute the layout for a given rectangle
    mtd computeLayout(pos: Math.Rectangle, text: *RichString)
    {
        text.layoutRect = pos

        x := text.layoutRect.x
        visit *c: text.chunks
        {
            switch c.fontStyle
            {
            case Regular:
                c.font = text.format.font.regular
            case Bold:
                c.font = text.format.font.bold
            case Italic:
                c.font = text.format.font.italic
            case BoldItalic:
                c.font = text.format.font.boldItalic
            }

            c.pos.x = x; x += measureString(c.slice, c.font).width
            c.pos.y = text.layoutRect.y
            c.color = text.format.palette[c.colorIdx]
        }
    }

    // Draw the string. The layout will be computed at the first call, and each
    // time 'pos' has changed
    public mtd drawRichString(pos: Rectangle, text: *RichString)
    {
        if text.layoutRect != pos
            computeLayout(pos, text)

        visit *c: text.chunks
        {
            if c.isSpace continue
            drawString(c.pos.x, c.pos.y, c.slice, c.font, c.color, c.under)
        }
    }
}