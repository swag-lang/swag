#global public
using Core, Pixel

struct Label
{
    using widget:   Widget
    icon:           Icon
    isWordWrap:     bool = true
    horzAlignment:  StringHorzAlignment = Left
    vertAlignment:  StringVertAlignment = Center
    iconTextMargin: f32
}

impl IWnd for Label
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        theme := evt.bc.theme
        pos := position
        applyPadding(&pos, padding)

        // Icon
        posIcon, posText := pos
        switch horzAlignment
        {
        case Left:
            posText.x += icon.size;
        case Right:
            posIcon.x = pos.right() - icon.size
        case Center:
            posText.x += (icon.size + iconTextMargin) * 0.5
            posIcon.x = pos.horzCenter() - (icon.size * 0.5)
        }

        switch vertAlignment
        {
        case Bottom:
            posIcon.y = pos.bottom() - icon.size
        case Center:
            posIcon.y = pos.vertCenter() - (icon.size * 0.5)
        }

        // Text
        if !name.isEmpty()
        {
            var sf: RichStringFormat
            sf.flags |= .Clip
            if isWordWrap
                sf.flags |= .WordWrap
            sf.font = theme.res.fontDefault
            sf.horzAlignment = horzAlignment
            sf.vertAlignment = vertAlignment
            sf.palette[0] = isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text
            sf.palette[1] = isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text1
            sf.palette[2] = isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text2
            sf.palette[3] = isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text3
            evt.bc.painter.drawRichString(posText, &name, sf)
        }

        // Icon
        if icon.isValid()
        {
            color := isDisabled ? theme.colors.label_DisabledIcon : theme.colors.label_Icon
            if name.isEmpty()
                icon.paint(evt.bc.painter, posIcon.x, posIcon.y, color)
            else
                icon.paint(evt.bc.painter, name.boundRect.x - icon.size - iconTextMargin, posIcon.y, color)
        }
    }
}

impl Label
{
    func create(parent: *Wnd, name: string, position: Math.Rectangle, id: string = null)->*Label
    {
        res := Wnd.create'Label(parent, name, position, id)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }
}