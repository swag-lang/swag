#global marked
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
        var pos = me.surfaceToLocal(evt.surfacePos)
        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false
            return

        case MousePressed:
            if evt.button == .Left and me.hotChunk:
                me.sigUrlClicked.call(me, me.hotChunk.slice)
            else:
                evt.accepted = false

        case MouseMove:
            let chunk = me.name.getUrlChunkAt(pos)
            if chunk != me.hotChunk
            {
                me.invalidate()
                if chunk
                {
                    chunk.isHotUrl = true
                    me.getApp().setFrameCursor(Cursor.from(.Hand))
                }
                elif me.hotChunk
                {
                    me.hotChunk.isHotUrl = false
                }

                me.hotChunk = chunk
            }

        case MouseLeave:
            if me.hotChunk
            {
                me.hotChunk.isHotUrl = false
                me.hotChunk          = null
                me.invalidate()
            }
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let theme   = me.getTheme()
        let painter = evt.bc.painter

        var clientRect = me.getClientRect()
        var pos        = clientRect
        pos.applyPadding(me.padding)

        // Icon
        if me.icon.isValid()
        {
            var posIcon = pos
            posIcon.applyPadding(me.paddingIcon)

            switch me.horzAlignmentIcon
            {
            case Right:
                posIcon.x = posIcon.right() - me.icon.sizeX
            case Center:
                posIcon.x = posIcon.horzCenter() - (me.icon.sizeX * 0.5)
            }

            switch me.vertAlignmentIcon
            {
            case Bottom:
                posIcon.y = posIcon.bottom() - me.icon.sizeY
            case Center:
                posIcon.y = posIcon.vertCenter() - (me.icon.sizeY * 0.5)
            }

            // Icon
            var color = me.iconColor
            if !color.a:
                color = (me.wndFlags & .Disabled) ? theme.colors.label_DisabledIcon : theme.colors.label_Icon
            me.icon.paint(painter, posIcon.x, posIcon.y, color)
        }

        // Text
        if !me.name.isEmpty()
        {
            var posText = pos
            posText.applyPadding(me.paddingText)

            var sf = me.getStringFormat(bc.isDisabled)
            painter.drawRichString(posText, &me.name, sf)
        }
    }
}

impl Label
{
    internal mtd getStringFormat(isDisabled: bool)->RichStringFormat
    {
        let theme = me.getTheme()

        var sf: retval
        sf.flags |= .Clip
        if me.labelFlags.has(.WordWrap):
            sf.flags.add(.WordWrap)
        if me.labelFlags.has(.DontAdaptSingleLineHeight):
            sf.flags.add(.DontAdaptSingleLineHeight)
        sf.font          = me.getFont()
        sf.horzAlignment = me.horzAlignmentText
        sf.vertAlignment = me.vertAlignmentText

        if me.textColor.a
        {
            sf.palette[0]  = me.textColor
            sf.palette[1]  = me.textColor
            sf.palette[2]  = me.textColor
            sf.palette[3]  = me.textColor
            sf.colorUrl    = me.textColor
            sf.colorHotUrl = me.textColor
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
            if me.labelFlags.has(.LightText):
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
    func create(parent: *Wnd, name: #null string, position: Math.Rectangle = {}, id: WndId = null, flags: LabelFlags = .Zero)->*Label
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
    mtd setText(text: #null string)
    {
        me.name = text
        me.invalidate()
        me.autoHeight()
    }

    // Get the text
    mtd getText()->string
    {
        return me.name.raw
    }

    // Recompute widget height.
    // The widget must have the flag 'AutoHeight'.
    mtd autoHeight()
    {
        if me.labelFlags.has(.AutoHeight)
        {
            var sf = me.getStringFormat(false)
            me.name.compute()
            Painter.computeLayout(me.position, &me.name, sf)
            me.position.height = me.name.boundRect.height
        }
    }
}
