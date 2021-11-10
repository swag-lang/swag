using Core, Math

public struct RichStringFormat
{
    font: FontFamily
}

impl Painter
{
    public mtd drawRichString(pos: Rectangle, text: *RichString)
    {
        if text.layoutRect != pos
            text.computeLayout(pos)

        visit *c: text.chunks
        {
            drawString(c.pos.x, c.pos.y, c.slice, c.font, c.color, c.under)
        }
    }
}