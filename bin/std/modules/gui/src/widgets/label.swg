#global public
using Core, Pixel

struct LabelPaintStyle
{
    stringFormat:   StringFormat
    colorText:      Color
}

struct Label
{
    using base: Widget
}

impl Label
{
    func paint(bc: *PaintContext, position: Math.Rectangle, name: string, style: LabelPaintStyle)
    {
        bc.painter.drawString(position, name, bc.theme.fontSmall.bold, style.colorText, style.stringFormat)
    }
}