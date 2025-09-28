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
        .fmt.font.regular = font
    }

    // Set text color
    mtd color(col: Color)
    {
        .fmt.palette[0] = col
    }

    // Set horizontal alignment
    mtd horzAlign(align: StringHorzAlignment)
    {
        .fmt.horzAlignment = align
    }

    // Set vertical alignment
    mtd vertAlign(align: StringVertAlignment)
    {
        .fmt.vertAlignment = align
    }

    // Set drawing flags
    mtd flags(f: StringFormatFlags)
    {
        .fmt.flags = f
    }

    // Draw string
    mtd draw(painter: *Painter, rect: Math.Rectangle, what: string)
    {
        .str.set(what)
        painter.drawRichString(rect, &.str, &.fmt)
    }
}
