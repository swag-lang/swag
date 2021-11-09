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

impl IWnd for Label
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        theme := evt.bc.theme
        pos := position
        applyPadding(&pos, padding)

        var sf: StringFormat
        sf.flags |= .WordWrap
        evt.bc.painter.drawString(pos, name, theme.res.fontDefault.regular, Argb.White, sf)
    }
}

impl Label
{
    func paint(bc: *PaintContext, position: Math.Rectangle, name: string, icon: Icon, style: LabelPaintStyle)
    {
        if @countof(name) == 0 and !icon.isValid()
            return

        painter := bc.painter
        pos     := position

        // Icon alone
        if @countof(name) == 0
        {
            pos.x = pos.horzCenter() - icon.size * 0.5
            icon.paint(bc.painter, pos.x, pos.vertCenter() - (icon.size * 0.5), style.colorText)
            return
        }

        // Text + icon
        if icon.isValid()
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

        if icon.isValid()
        {
            icon.paint(bc.painter, outRect.x - icon.size - style.iconMargin, outRect.vertCenter() - (icon.size * 0.5), style.colorText)
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