#global public
using Core, Pixel

struct LabelPaintStyle
{
    stringFormat:   RichStringFormat
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

        var sf: RichStringFormat
        sf.flags |= .WordWrap
        sf.font = theme.res.fontDefault
        sf.horzAlignment = .Justify
        sf.palette[0] = Argb.White
        evt.bc.painter.drawRichString(pos, &name, sf)
    }
}

impl Label
{
    func paint(bc: *PaintContext, position: Math.Rectangle, name: *RichString, icon: Icon, style: LabelPaintStyle)
    {
        count := @countof(dref name)
        if count == 0 and !icon.isValid()
            return

        painter := bc.painter
        pos     := position

        // Icon alone
        if count == 0
        {
            pos.x = pos.horzCenter() - icon.size * 0.5
            icon.paint(bc.painter, pos.x, pos.vertCenter() - (icon.size * 0.5), style.stringFormat.palette[0])
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

        bc.painter.drawRichString(pos, name, style.stringFormat)

        if icon.isValid()
        {
            icon.paint(bc.painter, name.boundRect.x - icon.size - style.iconMargin, name.boundRect.vertCenter() - (icon.size * 0.5), style.stringFormat.palette[0])
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