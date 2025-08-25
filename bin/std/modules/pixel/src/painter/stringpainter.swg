#global marked
#global public
using Core

struct StringPainter
{
    str:     RichString
    fmt:     RichStringFormat
}

impl StringPainter
{
    // Set font
    mtd font(font: *Font)
    {
        me.fmt.font.regular = font
    }

    // Set text color
    mtd color(col: Color)
    {
        me.fmt.palette[0] = col
    }

    // Set horizontal alignment
    mtd horzAlign(align: StringHorzAlignment)
    {
        me.fmt.horzAlignment = align
    }

    // Set vertical alignment
    mtd vertAlign(align: StringVertAlignment)
    {
        me.fmt.vertAlignment = align
    }

    // Set drawing flags
    mtd flags(f: StringFormatFlags)
    {
        me.fmt.flags = f
    }

    // Draw string
    mtd draw(painter: *Painter, rect: Math.Rectangle, what: string)
    {
        me.str.set(what)
        painter.drawRichString(rect, &me.str, &me.fmt)
    }
}
