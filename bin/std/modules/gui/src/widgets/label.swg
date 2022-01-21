#global public
using Core, Pixel

#[Swag.EnumFlags]
enum LabelFlags
{
    None
    WordWrap
}

struct Label
{
    using wnd: Wnd

    icon:               Icon
    labelFlags          = LabelFlags.None
    horzAlignmentIcon   = StringHorzAlignment.Left
    vertAlignmentIcon   = StringVertAlignment.Center
    horzAlignmentText   = StringHorzAlignment.Left
    vertAlignmentText   = StringVertAlignment.Center
    paddingText:        Math.Vector4
    paddingIcon:        Math.Vector4
}

impl IWnd for Label
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        theme   := getTheme()
        painter := evt.bc.painter

        clientRect := getClientRect()
        pos        := clientRect
        pos.applyPadding(padding)

        // Icon
        if icon.isValid()
        {
            posIcon := pos
            posIcon.applyPadding(paddingIcon)

            switch horzAlignmentIcon
            {
            case Right:
                posIcon.x = posIcon.right() - icon.size
            case Center:
                posIcon.x = posIcon.horzCenter() - (icon.size * 0.5)
            }

            switch vertAlignmentIcon
            {
            case Bottom:
                posIcon.y = posIcon.bottom() - icon.size
            case Center:
                posIcon.y = posIcon.vertCenter() - (icon.size * 0.5)
            }

            // Icon
            color := (wndFlags & .Disabled) ? theme.colors.label_DisabledIcon : theme.colors.label_Icon
            icon.paint(painter, posIcon.x, posIcon.y, color)
        }

        // Text
        if !name.isEmpty()
        {
            posText := pos
            posText.applyPadding(paddingText)

            var sf: RichStringFormat
            sf.flags |= .Clip
            if labelFlags & .WordWrap
                sf.flags |= .WordWrap
            sf.font = theme.res.fontDefault
            sf.horzAlignment = horzAlignmentText
            sf.vertAlignment = vertAlignmentText
            sf.palette[0] = bc.isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text
            sf.palette[1] = bc.isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text1
            sf.palette[2] = bc.isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text2
            sf.palette[3] = bc.isDisabled ? theme.colors.label_DisabledText : theme.colors.label_Text3
            painter.drawRichString(posText, &name, sf)
        }
    }
}

impl Label
{
    #[Swag.Discardable]
    func create(parent: *Wnd, name: string, position: Math.Rectangle, id: string = null)->*Label
    {
        Debug.assert(parent != null)
        return Wnd.create'Label(parent, name, position, id)
    }
}