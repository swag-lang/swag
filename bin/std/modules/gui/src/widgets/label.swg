#global public
using Core, Pixel

#[Swag.EnumFlags]
enum LabelPaintFlags
{
    Zero = 0
}

struct LabelPaintStyle
{
    flags:          LabelPaintFlags = Zero
    stringFormat:   StringFormat
    font:           *Font
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
        bc.painter.drawString(position, name, style.font, style.colorText, style.stringFormat)
    }

    func create(parent: *Wnd, name: string, position: Math.Rectangle)->*Label
    {
        res := Wnd.create'Label(parent, name, position)

        evt := CreateEvent{}
        res.sendEvent(&evt)

        return res
    }
}