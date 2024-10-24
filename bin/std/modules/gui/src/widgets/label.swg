#global public
using Core, Pixel

#[Swag.EnumFlags]
enum LabelFlags
{
    Zero                      = 0
    WordWrap
    LightText
    AutoHeight
    DontAdaptSingleLineHeight
}

struct Label
{
    using wnd:            Wnd

    sigUrlClicked:        SigArray'func||(*Label, string)

    icon:                 Icon
    iconColor:            Color
    textColor:            Color
    labelFlags            = LabelFlags.Zero
    horzAlignmentIcon     = StringHorzAlignment.Center
    vertAlignmentIcon     = StringVertAlignment.Center
    horzAlignmentText     = StringHorzAlignment.Center
    vertAlignmentText     = StringVertAlignment.Center
    paddingText:          Math.Vector4
    paddingIcon:          Math.Vector4

    hotChunk:             *RichChunk
}

impl IWnd for Label
{
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        var pos = surfaceToLocal(evt.surfacePos)
        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false
            return

        case MousePressed:
            if evt.button == .Left and hotChunk:
                sigUrlClicked.call(self, hotChunk.slice)
            else:
                evt.accepted = false

        case MouseMove:
            let chunk = name.getUrlChunkAt(pos)
            if chunk != hotChunk
            {
                invalidate()
                if chunk
                {
                    chunk.isHotUrl = true
                    getApp().pushCursor(Cursor.from(.Hand))
                }
                elif hotChunk
                {
                    hotChunk.isHotUrl = false
                    getApp().popCursor()
                }

                hotChunk = chunk
            }

        case MouseLeave:
            if hotChunk
            {
                hotChunk.isHotUrl = false
                hotChunk          = null
                getApp().popCursor()
                invalidate()
            }
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let theme   = getTheme()
        let painter = evt.bc.painter

        var clientRect = getClientRect()
        var pos        = clientRect
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
            if !color.a:
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
    internal mtd getStringFormat(isDisabled: bool)->RichStringFormat
    {
        let theme = getTheme()

        var sf: retval
        sf.flags |= .Clip
        if labelFlags.has(.WordWrap):
            sf.flags.add(.WordWrap)
        if labelFlags.has(.DontAdaptSingleLineHeight):
            sf.flags.add(.DontAdaptSingleLineHeight)
        sf.font          = getFont()
        sf.horzAlignment = horzAlignmentText
        sf.vertAlignment = vertAlignmentText

        if textColor.a
        {
            sf.palette[0]  = textColor
            sf.palette[1]  = textColor
            sf.palette[2]  = textColor
            sf.palette[3]  = textColor
            sf.colorUrl    = textColor
            sf.colorHotUrl = textColor
        }
        elif isDisabled
        {
            sf.palette[0]  = theme.colors.label_DisabledText
            sf.palette[1]  = theme.colors.label_DisabledText
            sf.palette[2]  = theme.colors.label_DisabledText
            sf.palette[3]  = theme.colors.label_DisabledText
            sf.colorUrl    = theme.colors.label_DisabledText
            sf.colorHotUrl = theme.colors.label_DisabledText
        }
        else
        {
            if labelFlags.has(.LightText):
                sf.palette[0] = theme.colors.label_TextLight
            else:
                sf.palette[0] = theme.colors.label_Text
            sf.palette[1]  = theme.colors.label_Text1
            sf.palette[2]  = theme.colors.label_Text2
            sf.palette[3]  = theme.colors.label_Text3
            sf.colorUrl    = theme.colors.url_Text
            sf.colorHotUrl = theme.colors.url_HotText
        }

        return sf
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, name: nl string, position: Math.Rectangle = {}, id: WndId = null, flags: LabelFlags = .Zero)->*Label
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0:
            pos.height = parent.getThemeMetrics().label_Height
        let lbl = Wnd.create'Label(parent, "", pos, id)
        lbl.labelFlags = flags
        lbl.setText(name)
        return lbl
    }

    // Change the text
    mtd setText(text: nl string)
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

    // Recompute widget height.
    // The widget must have the flag 'AutoHeight'.
    mtd autoHeight()
    {
        if labelFlags.has(.AutoHeight)
        {
            var painter: Painter{fake: true}
            var sf       = getStringFormat(false)
            name.compute()
            painter.computeLayout(position, &name, sf)
            position.height = name.boundRect.height
        }
    }
}
