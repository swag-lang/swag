#global public
using Core, Pixel

#[Swag.EnumFlags]
enum LabelFlags
{
    Zero = 0
    WordWrap
    LightText
    AutoHeight
}

struct Label
{
    using wnd: Wnd

    icon:               Icon
    iconColor:          Color
    labelFlags          = LabelFlags.Zero
    horzAlignmentIcon   = StringHorzAlignment.Center
    vertAlignmentIcon   = StringVertAlignment.Center
    horzAlignmentText   = StringHorzAlignment.Center
    vertAlignmentText   = StringVertAlignment.Center
    paddingText:        Math.Vector4
    paddingIcon:        Math.Vector4
}

impl IWnd for Label
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc = evt.bc
        let theme = getTheme()
        let painter = evt.bc.painter

        var clientRect = getClientRect()
        var pos = clientRect
        pos.applyPadding(padding)

        // Icon
        if icon.isValid()
        {
            var posIcon = pos
            posIcon.applyPadding(paddingIcon)

            switch horzAlignmentIcon
            {
            case Right:
                posIcon.x = posIcon.right() - icon.sizeX
            case Center:
                posIcon.x = posIcon.horzCenter() - (icon.sizeX * 0.5)
            }

            switch vertAlignmentIcon
            {
            case Bottom:
                posIcon.y = posIcon.bottom() - icon.sizeY
            case Center:
                posIcon.y = posIcon.vertCenter() - (icon.sizeY * 0.5)
            }

            // Icon
            var color = iconColor
            if !color.a
                color = (wndFlags & .Disabled) ? theme.colors.label_DisabledIcon : theme.colors.label_Icon
            icon.paint(painter, posIcon.x, posIcon.y, color)
        }

        // Text
        if !name.isEmpty()
        {
            var posText = pos
            posText.applyPadding(paddingText)

            var sf = getStringFormat(bc.isDisabled)
            painter.drawRichString(posText, &name, sf)
        }
    }
}

impl Label
{
    private mtd getStringFormat(isDisabled: bool)->RichStringFormat
    {
        let theme = getTheme()

        var sf: retval
        sf.flags |= .Clip
        if labelFlags.has(.WordWrap)
            sf.flags.add(.WordWrap)
        sf.font = getFont()
        sf.horzAlignment = horzAlignmentText
        sf.vertAlignment = vertAlignmentText

        if isDisabled
        {
            sf.palette[0] = theme.colors.label_DisabledText
            sf.palette[1] = theme.colors.label_DisabledText
            sf.palette[2] = theme.colors.label_DisabledText
            sf.palette[3] = theme.colors.label_DisabledText
        }
        else
        {
            if labelFlags.has(.LightText)
                sf.palette[0] = theme.colors.label_TextLight
            else
                sf.palette[0] = theme.colors.label_Text
            sf.palette[1] = theme.colors.label_Text1
            sf.palette[2] = theme.colors.label_Text2
            sf.palette[3] = theme.colors.label_Text3
        }

        return sf
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, name: string, position: Math.Rectangle = {}, id: WndId = null)->*Label
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0
            pos.height = parent.getThemeMetrics().label_Height
        let lbl = Wnd.create'Label(parent, name, pos, id)
        return lbl
    }

    private mtd autoHeight()
    {
        // Recompute height
        if labelFlags.has(.AutoHeight)
        {
            var painter: Painter{fake = true}
            var sf = getStringFormat(false)
            name.compute()
            painter.computeLayout(position, &name, sf)
            position.height = name.boundRect.height
        }
    }

    // Change the text
    mtd setText(text: string)
    {
        name = text
        invalidate()
        autoHeight()
    }

	// Get the text
	mtd getText()->string
	{
		return name.raw
	}
}