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
    mtd onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed
        {
            evt.accepted = ed.keyPressed(evt.key, evt.modifiers)
            if evt.accepted forceVisibleCaret()
        }
        elif evt.kind == .Char
        {
            evt.accepted = ed.charPressed(evt.char, evt.modifiers)
            if evt.accepted forceVisibleCaret()

        }
    }

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

        marginLeft := ed.getMarginLeft()
        x    := 0'f32
        y    := 0'f32
        size := ed.getXExtent()

        // Start to the first visible line, if known
        var i: uint
        if ed.dirtyFirstLineView or ed.recomputeScroll
        {
            i = 0
            ed.firstVisibleLineView = Swag.UInt.Max
        }
        else
        {
            i = ed.firstVisibleLineView
            y = ed.lines[i].drawingRect.y
        }

        if ed.recomputeScroll
            ed.countVisibleLines = 0
        ed.dirtyFirstLineView = false

        pos := getClientRect()
        firstHidden := true
        dirtyFold := false
        maxx := 0'f32

        // All lines
        while i < ed.lines.count
        {
            // Is the line visible ?
            bvis := true
            if y + size.y < scrollPos.y or y - scrollPos.y > pos.bottom()
                bvis = false

            // Store the first visible line index
            if bvis and ed.firstVisibleLineView == Swag.UInt.Max and (ed.lines[i].isVisible() or firstHidden)
                ed.firstVisibleLineView = i

            if ed.recomputeScroll
                ed.countVisibleLines += 1
            ed.lastVisibleLineView = i

            l := ed.lines[i]
            if bvis or l.dirtyStyle or l.dirtyFold
                x = paintLine(evt.bc, i, y - scrollPos.y, true)

            l.drawingRect.x = marginLeft
            l.drawingRect.y = y
            l.drawingRect.width = 1 - x
            l.drawingRect.height = size.y

            maxx = Math.max(maxx, x)
            y += size.y
            i += 1
            if !ed.recomputeScroll and y >= scrollPos.y + pos.bottom() and !dirtyFold
                break
        }

        // Caret
        posCaret := ed.getCaretPos()
        if visibleCaret
        {
            posCaret.x -= scrollPos.x
            posCaret.y -= scrollPos.y
            painter.fillRect(posCaret, getThemeColors().richEdit_Caret)
        }

        // Scroll size
        y = cast(f32) (ed.countVisibleLines * size.y)
        if !ed.recomputeScroll
            maxx = ed.scrollWnd.scrollSize.x
        ed.recomputeScroll = false
        ed.scrollWnd.setScrollSize(maxx, y)

        // Force visible caret
        if ed.forceCaretVisible
        {
            ed.forceCaretVisible = false
            ed.ensureCaretIsVisibleInternal(ed.forceCaretVisibleCenter)
        }
    }
}

impl RichEditView
{
    mtd forceVisibleCaret()
    {
        if timerCaret
        {
            visibleCaret = true
            timerCaret.restart()
            invalidate()
        }
    }

    mtd paintString(bc: *PaintContext, x, y: f32, str: String, strStyle: RichEditCharStyle, visible: bool)->f32
    {
        painter := bc.painter

        font := getTheme().res.fontDefault.getFont(strStyle.fontMode)
        me := Painter.measureString(str, font)

        // Background rectangle
        if visible and strStyle.colBk.a
        {
            var rect: Math.Rectangle
            rect.x = x
            rect.y = y
            rect.width = me.width
            rect.height = ed.getXExtent().y
            painter.fillRect(rect, strStyle.colBk)
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
        x := ed.getMarginLeft() - scrollPos.x

        sel0 := ed.getBegSelection()
        sel1 := ed.getEndSelection()

        i := 0
        while i < count
        {
            defer { i += 1; pChar += 1; pStyle += 1; }
            newStyle = ed.styles[dref pStyle]

            selected := false
            if ed.hasSelRect
            {
                 // and m_SelRect.IsInRect(charpos))
                selected = true
            }
            else
            {
                if (lineIndex > sel0.lineIndex and lineIndex < sel1.lineIndex) or
                   (lineIndex != sel1.lineIndex and lineIndex == sel0.lineIndex and i >= sel0.charPosInLine) or
                   (lineIndex != sel0.lineIndex and lineIndex == sel1.lineIndex and i < sel1.charPosInLine) or
                   (lineIndex == sel0.lineIndex and sel0.lineIndex == sel1.lineIndex and i >= sel0.charPosInLine and i < sel1.charPosInLine)
                {
                    selected = true
                }
            }

            if selected
            {
                newStyle = hasFocus() ? ed.stylesSelect[0] : ed.stylesSelect[1]
                if !newStyle.colFg.a
                    newStyle.colFg = ed.styles[dref pStyle].colFg
            }

            // Paint accumulated string if we have a change of style
            styleChange := newStyle != oldStyle
            if styleChange
            {
                x1 := paintString(bc, x, y, str, oldStyle, visible)
                x = x1
                oldStyle = newStyle
                str.clear()
            }

            str += dref pChar
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
