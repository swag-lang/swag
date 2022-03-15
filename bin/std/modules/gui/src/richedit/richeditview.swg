#global public
using Core, Pixel

struct RichEditView
{
    using wnd:  Wnd

    ed:             *RichEdit
    visibleCaret:   bool
    timerCaret:     *Timer
}

impl IWnd for RichEditView
{
    mtd onFocusEvent(evt: *FocusEvent)
    {
        if evt.kind == .SetFocus
        {
            timerCaret = surface.app.addTimer(500'ms, self)
            visibleCaret = true
        }
        else
        {
            surface.app.deleteTimer(timerCaret)
            timerCaret = null
            visibleCaret = false
        }

        invalidate()
    }

    mtd onTimerEvent(evt: *TimerEvent)
    {
        visibleCaret = !visibleCaret
        pos := ed.getCaretPos()
        pos.inflate(1)
        invalidateRect(pos)
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc.painter

        x    := ed.getMarginLeft()
        y    := ed.padding.y
        size := ed.getXExtent()

        visit l, i: ed.lines
        {
            x1 := paintLine(evt.bc, i, y, true)

            l.drawingRect.x = x
            l.drawingRect.y = y
            l.drawingRect.width = x1 - x
            l.drawingRect.height = size.y

            y += l.drawingRect.height
        }

        // Caret
        posCaret := ed.getCaretPos()
        if visibleCaret
            painter.fillRect(posCaret, getThemeColors().richEdit_Caret)
    }
}

impl RichEditView
{
    mtd paintString(bc: *PaintContext, x, y: f32, str: String, strStyle: RichEditCharStyle, visible: bool)->f32
    {
        painter := bc.painter

        font := getTheme().res.fontDefault.getFont(strStyle.fontMode)
        me := Painter.measureString(str, font)

        if visible and strStyle.colBk.a
        {
        }

        painter.drawString(x, y, str, font, strStyle.colFg, strStyle.underline)
        return x + me.width
    }

    mtd paintLine(bc: *PaintContext, lineIndex: uint, y: f32, visible: bool)->f32
    {
        line := ed.lines[lineIndex]

        // Dummy character for empty lines, so that a selected line display something
        dummyChar  := " "'rune
        dummyStyle := 0'u8
        count  := line.length()
        pChar  := line.chars.buffer
        pStyle := line.styles.buffer
        if !count
        {
            pChar  = &dummyChar
            pStyle = &dummyStyle
            count   = 1
        }

        var str: String{allocator: @getcontext().tempAllocator}
        str.reserve(line.length())

        var oldStyle, newStyle: RichEditCharStyle
        oldStyle = ed.styles[0]
        x := ed.getMarginLeft()

        i := 0
        while i < count
        {
            defer { i += 1; pChar += 1; pStyle += 1; }

            newStyle = ed.styles[dref pStyle]

            // Paint accumulated string if we have a change of style
            styleChange := newStyle != oldStyle
            if styleChange
            {
                x1 := paintString(bc, x, y, str, oldStyle, visible)
                x = x1
                oldStyle = newStyle
                str.clear()
            }

            str += line.chars[i]
        }

        // Remaining string
        if !str.isEmpty()
        {
            x1 := paintString(bc, x, y, str, oldStyle, visible)
            x = x1
        }

        return x
    }
}
