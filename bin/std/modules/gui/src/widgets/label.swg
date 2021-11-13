#global public
using Core, Pixel

struct Label
{
    using widget:   Widget
    icon:           Icon
    horzAlignment:  StringHorzAlignment = Left
    vertAlignment:  StringVertAlignment = Top
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
        switch horzAlignment
        {
        case Left:      break
        case Right:     posIcon.x = pos.right() - icon.size
        case Center:    posIcon.x = pos.horzCenter() - (icon.size * 0.5)
        }

        switch vertAlignment
        {
        case Top:       break
        case Bottom:    posIcon.y = pos.bottom() - icon.size
        case Center:    posIcon.y = pos.vertCenter() - (icon.size * 0.5)
        }

        color := isDisabled ? theme.colors.label_DisabledIcon : theme.colors.label_Icon
        icon.paint(evt.bc.painter, posIcon.x, posIcon.y, color)

        // Text
        var sf: RichStringFormat
        sf.flags |= .WordWrap | .Clip
        sf.font = theme.res.fontDefault
        sf.horzAlignment = horzAlignment
        sf.vertAlignment = vertAlignment
        sf.palette[0] = isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text
        sf.palette[1] = isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text1
        sf.palette[2] = isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text2
        sf.palette[3] = isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text3
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