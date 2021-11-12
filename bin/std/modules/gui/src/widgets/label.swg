#global public
using Core, Pixel

struct Label
{
    using widget:   Widget
    icon:           Icon
}

impl IWnd for Label
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        theme := evt.bc.theme
        pos := position
        applyPadding(&pos, padding)

        // Icon
        posIcon := pos
        posIcon.x = pos.horzCenter() - (icon.size * 0.5)
        posIcon.y = pos.vertCenter() - (icon.size * 0.5)
        icon.paint(evt.bc.painter, posIcon.x, posIcon.y, Argb.White)

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
    func create(parent: *Wnd, name: string, position: Math.Rectangle)->*Label
    {
        res := Wnd.create'Label(parent, name, position)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }
}