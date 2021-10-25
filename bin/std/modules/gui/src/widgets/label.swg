#global public
using Core, Pixel

struct LabelPaintStyle
{
    stringFormat:   StringFormat
    font:           *Font
    colorText:      Color
    iconMargin:     f32 = 4
}

struct Label
{
    using base: Widget
}

impl Label
{
    func paint(bc: *PaintContext, position: Math.Rectangle, name: string, icon: Icon, style: LabelPaintStyle)
    {
        painter := bc.painter
        pos := position

        if icon.imageList
        {
            switch style.stringFormat.horzAlignment
            {
            case Left:
                pos.x += icon.size + style.iconMargin
            case Center:
                pos.x += (icon.size + style.iconMargin) * 0.5
            }
        }

        var outRect: Math.Rectangle = ?
        bc.painter.drawString(pos, name, style.font, style.colorText, style.stringFormat, &outRect)

        if icon.imageList
        {
            icon.draw(bc.painter, outRect.x - icon.size - style.iconMargin, outRect.vertCenter() - (icon.size * 0.5), style.colorText)
        }
    }

    func create(parent: *Wnd, name: string, position: Math.Rectangle)->*Label
    {
        res := Wnd.create'Label(parent, name, position)

        evt := CreateEvent{}
        res.sendEvent(&evt)

        return res
    }
}