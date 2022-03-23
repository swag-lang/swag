using Core, Pixel

#[Swag.Opaque]
public struct RichEditView
{
    using wnd:  Wnd

    ed:                 *RichEdit
    visibleCaret:       bool
    selMode:            bool
    timerCaret:         *Timer
    firstLineSelMode:   uint
}

impl IWnd for RichEditView
{
    mtd onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = false
        switch evt.kind
        {
        case .MouseDoubleClick:
            if evt.button == .Left
            {
                ed.selectWord()
                evt.accepted = true
            }

        case .MousePressed:
            if evt.button == .Left
            {
                evt.accepted = true
                pt := surfaceToLocal(evt.surfacePos) + scrollPos
                cp := ed.getCursorPosFromPoint(pt)
                firstLineSelMode = cp.lineIndex
                ed.setCursorPos(cp)
                ed.selBeg  = cp
                selMode = true
                ed.setRectangularSelection(evt.modifiers & .Alt ? true : false)
                ed.ensureCaretIsVisible()
                captureMouse()
            }

        case .MouseReleased:
            if selMode
            {
                evt.accepted = true
                selMode = false
                releaseMouse()
            }

        case .MouseMove:
            if selMode
            {
                evt.accepted = true
                pt := surfaceToLocal(evt.surfacePos) + scrollPos
                cp := ed.getCursorPosFromPoint(pt)
                if ed.cursorPos.lineIndex != cp.lineIndex or ed.cursorPos.charPosInLine != cp.charPosInLine
                {
                    if ed.hasSelRect
                    {
                        if cp.charPosInLine >= ed.selBeg.charPosInLine
                            ed.setCursorPos(cp)
                        ed.checkSelection()
                    }
                    else
                    {
                        ed.setCursorPos(cp)
                    }

                    ed.ensureCaretIsVisible()
                }
            }
        }
    }

    mtd onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed
        {
            evt.accepted = ed.keyPressed(evt.key, evt.modifiers)
            if evt.accepted forceVisibleCaret()
        }
        elif evt.kind == .Rune
        {
            evt.accepted = ed.charPressed(evt.key, evt.char, evt.modifiers)
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
        invalidate()
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc.painter
        colors  := getThemeColors()

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
            l.drawingRect.width = x - marginLeft
            l.drawingRect.height = size.y

            // Current line
            if ed.richEdFlags & .CurLineBorder
            {
                if bvis and ed.isSelectionEmpty() and i == ed.cursorPos.lineIndex
                {
                    fullRect := l.drawingRect
                    fullRect.y -= scrollPos.y
                    fullRect.setRight(pos.right())
                    painter.drawRect(fullRect, colors.richEdit_CurLineBorder, 1)
                }
            }

            maxx = Math.max(maxx, x)
            y += size.y
            i += 1
            if !ed.recomputeScroll and y >= scrollPos.y + pos.bottom() and !dirtyFold
                break
        }

        // Caret
        if visibleCaret
        {
            colorCaret := getThemeColors().richEdit_Caret
            if ed.hasSelRect
            {
                selBeg := ed.getBegSelection()
                selEnd := ed.getEndSelection()
                loop caretI: selBeg.lineIndex..selEnd.lineIndex
                {
                    cp := selEnd
                    cp.lineIndex = caretI
                    if cp.charPosInLine > ed.lines[cp.lineIndex].length()
                        continue
                    posCaret := ed.getCaretPos(cp)
                    posCaret.x -= scrollPos.x
                    posCaret.y -= scrollPos.y
                    painter.fillRect(posCaret, colorCaret)
                }
            }
            else
            {
                posCaret := ed.getCaretPos(ed.cursorPos)
                posCaret.x -= scrollPos.x
                posCaret.y -= scrollPos.y
                painter.fillRect(posCaret, colorCaret)
            }
        }

        // Scroll size
        y = cast(f32) (ed.countVisibleLines * size.y)
        if !ed.recomputeScroll
            maxx = ed.scrollWnd.scrollSize.x
        else
        {
            posCaret := ed.getCaretPos(ed.cursorPos)
            maxx += posCaret.width + 32
        }

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

    mtd paintString(bc: *PaintContext, x, y: f32, str: String, strStyle: RichEditRuneStyle, visible: bool)->f32
    {
        painter := bc.painter

        font := ed.fontFamily.getFont(strStyle.fontMode)
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

        var oldStyle, newStyle: RichEditRuneStyle
        oldStyle = ed.styles[0]
        x := ed.getMarginLeft() - scrollPos.x

        sel0 := ed.getBegSelection()
        sel1 := ed.getEndSelection()
        size := ed.getXExtent()

        i := 0
        while i < count
        {
            defer { i += 1; pChar += 1; pStyle += 1; }
            newStyle = ed.styles[dref pStyle]

            selected := false
            if ed.hasSelRect
            {
                var cp1: RichEditCursor
                cp1.charPosInLine = i
                cp1.lineIndex = lineIndex
                charPos := ed.getCursorDisplayPos(cp1)
                charPos.y -= size.y * 0.5
                if ed.selRectangle.contains(charPos)
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
                if hasFocus()
                    newStyle = ed.stylesSelect[0]
                else if ed.richEdFlags & .ShowFocusSel
                    newStyle = ed.stylesSelect[1]
                if !newStyle.colFg.a
                    newStyle.colFg = ed.styles[dref pStyle].colFg
                newStyle.underline = ed.styles[dref pStyle].underline
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

            cr := dref pChar
            if cr == "\t"'rune
            {
                x1 := x
                x += ed.getTabOffset(cast(s32) x)

                // Background color for tabulations
                if visible and oldStyle.colBk.a
                    bc.painter.fillRect(@{x1, y, x - x1, size.y}, oldStyle.colBk)
            }
            else
            {
                str += cr
            }
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
