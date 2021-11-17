using Core, Pixel

public struct EditBox
{
    using scrollwnd:    ScrollWnd
    note:               RichString
    icon:               Icon

    timerCaret:         *Timer
    visibleCaret:       bool
}

impl IWnd for EditBox
{
    mtd onFocusEvent(evt: *FocusEvent)
    {
        if evt.kind == .SetFocus
            timerCaret = surface.app.addTimer(500, self)
        else
            surface.app.deleteTimer(timerCaret)
        invalidate()
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        painter := evt.bc.painter

        clientRect := getClientRect()
        colors := &bc.theme.colors
        rects  := &bc.theme.rects

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorText:      Color = ?

        subRectBk = &rects.edit_SquareBk
        subRectBorder = &rects.edit_SquareBorder

        if bc.isDisabled
            colorBorder = colors.edit_DisabledBorder
        if hasFocus()
            colorBorder = colors.edit_FocusBorder
        else
            colorBorder = colors.edit_Border

        colorBk = colors.edit_SquareBk

        bc.theme.drawSubRect(painter, clientRect, subRectBk, colorBk)
        bc.theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder)

        clientRect.applyPadding(padding)

        // Note
        if !note.isEmpty()
        {
            var format: RichStringFormat
            colorText = colors.edit_Note
            format.palette[0] = colorText
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = bc.theme.res.fontDefault
            Theme.drawIconText(bc.painter, clientRect, icon, &note, format, bc.theme.metrics.iconText_Margin)
        }

        if visibleCaret and hasFocus()
        {
            pos := getCaretPos()
            painter.fillRect(pos, Argb.White)
        }
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MousePressed:
            if evt.button == .Left
                setFocus()
            evt.accepted = true
        }
    }

    mtd onKeyEvent(evt: *KeyEvent)
    {
    }

    mtd onTimerEvent(evt: *TimerEvent)
    {
        visibleCaret = !visibleCaret
        pos := getCaretPos()
        pos.inflate(1)
        pos.offset(-getScrollPos())
        invalidateRect(pos)
    }
}

public impl EditBox
{
    func create(parent: *Wnd, content: string, position: Math.Rectangle, id: string = null)->*EditBox
    {
        pos := position
        if pos.height == 0 pos.height = parent.getThemeMetrics().btnPush_Height
        if pos.width == 0  pos.width  = parent.getThemeMetrics().btnPush_Width
        res := Wnd.create'EditBox(parent, content, pos, id)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }

    mtd getCaretPos()->Math.Rectangle
    {
        pos := getClientRect()
        pos.width = 4
        return pos
    }
}