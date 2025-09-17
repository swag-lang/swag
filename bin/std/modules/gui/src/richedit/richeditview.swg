using Core, Pixel

#[Swag.Opaque]
public struct RichEditView
{
    using wnd:            Wnd

    ed:                   *RichEditCtrl
    visibleCaret:         bool
    selMode:              bool
    timerCaret:           *Timer
    firstLineSelMode:     u64
    lastClientRect:       Math.Rectangle
}

impl IWnd for RichEditView
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if me.ed and me.ed.richEdFlags & .WordWrap:
            me.ed.recomputeScroll = true
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = false
        switch evt.kind
        {
        case .MouseDoubleClick:
            if evt.button == .Left
            {
                me.ed.selectWord()
                evt.accepted = true
            }

        case .MousePressed:
            if evt.button == .Left
            {
                evt.accepted = true
                let pt = me.surfaceToLocal(evt.surfacePos) + me.scrollPos
                let cp = me.ed.getCursorPosFromPoint(pt)
                me.firstLineSelMode = cp.lineIndex
                me.ed.setCursorPos(cp)
                if !(evt.modifiers & .Shift):
                    me.ed.selBeg = cp
                me.selMode = true
                me.ed.setRectangularSelection(evt.modifiers & .Alt ? true : false)
                me.ed.ensureCaretIsVisible()
                me.captureMouse()
            }

        case .MouseReleased:
            if me.selMode
            {
                evt.accepted = true
                me.selMode   = false
                me.releaseMouse()
            }

        case .MouseMove:
            if me.selMode
            {
                evt.accepted = true
                let pt = me.surfaceToLocal(evt.surfacePos) + me.scrollPos
                let cp = me.ed.getCursorPosFromPoint(pt)
                if me.ed.cursorPos.lineIndex != cp.lineIndex or me.ed.cursorPos.charPosInLine != cp.charPosInLine
                {
                    if me.ed.hasSelRect
                    {
                        if cp.charPosInLine >= me.ed.selBeg.charPosInLine:
                            me.ed.setCursorPos(cp)
                        me.ed.checkSelection()
                    }
                    else
                    {
                        me.ed.setCursorPos(cp)
                    }

                    me.ed.ensureCaretIsVisible()
                }
            }
        }
    }

    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed
        {
            // Lose focus with escape or enter
            if me.ed.richEdFlags & .AutoLoseFocus
            {
                if evt.key == .Escape or (evt.key == .Enter and evt.modifiers.equals(.Zero))
                {
                    evt.accepted = false
                    me.getTopView().setFocus()
                    return
                }
            }

            evt.accepted = true
            me.ed.keyPressed(evt.key, evt.modifiers)
            me.forceVisibleCaret()
        }
        elif evt.kind == .Rune
        {
            evt.accepted = true
            me.ed.charPressed(evt.key, evt.char, evt.modifiers)
            me.forceVisibleCaret()
        }
    }

    mtd impl onFocusEvent(evt: *FocusEvent)
    {
        switch evt.kind
        {
        case SetFocus:
            me.timerCaret   = me.surface.app.addTimer(500'ms, me)
            me.visibleCaret = true
            me.invalidate()

        case KillFocus:
            me.surface.app.deleteTimer(me.timerCaret)
            me.timerCaret   = null
            me.visibleCaret = false
            me.invalidate()
        }
    }

    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        me.visibleCaret = !me.visibleCaret
        me.invalidate()
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter = evt.bc.painter
        let colors  = me.getThemeColors()

        let marginLeft = me.ed.getMarginLeft()
        var x          = 0'f32
        var y          = 0'f32
        let size       = me.ed.getXExtent()

        // Force visible caret
        if me.ed.forceCaretVisible:
            me.ed.ensureCaretIsVisibleInternal(me.ed.forceCaretVisibleCenter)

        // Start to the first visible line, if known
        var i: u64
        if me.ed.dirtyFirstLineView or me.ed.recomputeScroll
        {
            i                          = 0
            me.ed.firstVisibleLineView = Swag.U64.Max
        }
        else
        {
            i = me.ed.firstVisibleLineView
            y = me.ed.lines[i].drawingRect.y
        }

        if me.ed.recomputeScroll:
            me.ed.countVisibleLines = 0
        me.ed.dirtyFirstLineView = false

        let clientRect  = me.getClientRect()
        let firstHidden = true
        var maxx        = 0'f32

        let sizeChanged = me.lastClientRect != clientRect
        me.lastClientRect = clientRect

        // All lines
        while i < me.ed.lines.count
        {
            // Is the line visible ?
            var bvis = true
            if y + size.y < me.scrollPos.y or y - me.scrollPos.y > clientRect.bottom():
                bvis = false

            // Store the first visible line index
            if bvis and me.ed.firstVisibleLineView == Swag.U64.Max and (me.ed.lines[i].isVisible() or firstHidden):
                me.ed.firstVisibleLineView = i

            if me.ed.recomputeScroll:
                me.ed.countVisibleLines += 1
            me.ed.lastVisibleLineView = i

            let l = me.ed.lines[i]

            l.drawingRect.x      = marginLeft
            l.drawingRect.y      = y
            l.drawingRect.width  = x - marginLeft
            l.drawingRect.height = size.y

            // Current line
            if bvis and me.ed.isSelectionEmpty() and i == me.ed.cursorPos.lineIndex and me.ed.richEdFlags & .CurLineBorder
            {
                var fullRect = l.drawingRect
                fullRect.y -= me.scrollPos.y
                fullRect.setRight(clientRect.right() - me.ed.getMarginRight())
                fullRect.inflate(-1)
                painter.drawRect(fullRect, colors.richEdit_CurLineBorder, 1)
            }

            // Lex the line
            if !l.wordWrap and (l.dirtyStyle or sizeChanged or me.ed.recomputeScroll)
            {
                me.ed.unwrap(i)

                if me.ed.lexer
                {
                    let prevLine = i ? me.ed.lines[i - 1] : null
                    let nextLine = i != me.ed.lines.count - 1 ? me.ed.lines[i + 1] : null
                    me.ed.lexer.compute(prevLine, l, nextLine)
                }

                me.ed.wrap(i)
                l.dirtyStyle = false
            }

            if bvis or me.ed.recomputeScroll
            {
                x = me.paintLine(evt.bc, i, y - me.scrollPos.y, bvis)
            }

            maxx = Math.max(maxx, x)
            y += size.y
            i += 1
            if !me.ed.recomputeScroll and y >= me.scrollPos.y + clientRect.bottom():
                break
        }

        // Caret
        if me.visibleCaret
        {
            let colorCaret = me.getThemeColors().richEdit_Caret
            if me.ed.hasSelRect
            {
                let selBeg = me.ed.getBegSelection()
                let selEnd = me.ed.getEndSelection()
                for caretI in selBeg.lineIndex to selEnd.lineIndex
                {
                    var cp = me.ed.cursorPos
                    cp.lineIndex = caretI
                    if cp.charPosInLine > me.ed.lines[cp.lineIndex].length():
                        continue
                    var posCaret = me.ed.getCaretPos(cp)
                    posCaret.x -= me.scrollPos.x
                    posCaret.y -= me.scrollPos.y
                    painter.fillRect(posCaret, colorCaret)
                }
            }
            else
            {
                var posCaret = me.ed.getCaretPos(me.ed.cursorPos)
                posCaret.x -= me.scrollPos.x
                posCaret.y -= me.scrollPos.y
                painter.fillRect(posCaret, colorCaret)
            }
        }

        // Scroll size
        y = cast(f32) (me.ed.countVisibleLines * size.y)
        if me.ed.richEdFlags & .WordWrap:
            maxx = 0
        elif !me.ed.recomputeScroll:
            maxx = me.ed.scrollWnd.scrollSize.x
        else
        {
            let posCaret = me.ed.getCaretPos(me.ed.cursorPos)
            maxx += posCaret.width + 32
        }

        me.ed.recomputeScroll = false
        me.ed.scrollWnd.setScrollSize(maxx, y)

        // Force visible caret
        if me.ed.forceCaretVisible
        {
            me.ed.forceCaretVisible = false
            me.ed.ensureCaretIsVisibleInternal(me.ed.forceCaretVisibleCenter)
        }
    }
}

impl RichEditView
{
    mtd forceVisibleCaret()
    {
        if me.timerCaret
        {
            me.visibleCaret = true
            me.timerCaret.restart()
            me.invalidate()
        }
    }

    mtd paintString(bc: *PaintContext, x, y: f32, str: String, strStyle: RichEditRuneStyle, visible: bool)->f32
    {
        let painter = bc.painter

        let fontR = me.ed.getFont().getFont(strStyle.fontMode)
        if !fontR:
            return 0

        let self = Painter.measureString(str, fontR)

        // Background rectangle
        if visible and strStyle.colBk.a
        {
            var rect: Math.Rectangle
            rect.x      = x
            rect.y      = y
            rect.width  = self.width
            rect.height = me.ed.getXExtent().y
            painter.fillRect(rect, strStyle.colBk)
        }

        painter.drawString(x, y, str, fontR, strStyle.colFg, strStyle.underline)
        return x + self.width
    }

    mtd paintLine(bc: *PaintContext, lineIndex: u64, py: f32, visible: bool)->f32
    {
        let line = me.ed.lines[lineIndex]
        let y    = py

        // Dummy character for empty lines, so that a selected line display something
        var dummyChar  = ' ''rune
        var dummyStyle = 0'u8
        var count      = line.length()
        var pChar      = line.chars.buffer
        var pStyle     = line.styles.buffer
        if !count
        {
            pChar  = cast([*] rune) &dummyChar
            pStyle = cast([*] u8) &dummyStyle
            count  = 1
        }

        var str: String{allocator: @getcontext().tempAllocator}
        str.reserve(line.length())

        var oldStyle, newStyle: RichEditRuneStyle
        oldStyle = me.ed.styles[0]
        var x = me.ed.getMarginLeft() - me.scrollPos.x

        let sel0 = me.ed.getBegSelection()
        let sel1 = me.ed.getEndSelection()
        let size = me.ed.getXExtent()

        var i = 0
        while i < count
        {
            defer
            {
                i += 1
                pChar += 1
                pStyle += 1
            }
            newStyle = me.ed.styles[dref pStyle]

            var cr           = dref pChar
            let isTabulation = cr == '\t'

            var selected = false
            if me.ed.hasSelRect
            {
                var cp1: RichEditCursor
                cp1.charPosInLine = i
                cp1.lineIndex     = lineIndex
                var charPos = me.ed.getCursorDisplayPos(cp1)
                charPos.y -= size.y * 0.5
                if me.ed.selRectangle.contains(charPos):
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
                if me.hasFocus():
                    newStyle = me.ed.stylesSelect[0]
                elif me.ed.richEdFlags & .ShowFocusSel:
                    newStyle = me.ed.stylesSelect[1]
                if !newStyle.colFg.a:
                    newStyle.colFg = me.ed.styles[dref pStyle].colFg
                newStyle.underline = me.ed.styles[dref pStyle].underline
            }

            if me.ed.richEdFlags & .DrawBlanks
            {
                if cr == ' ' and pChar != &dummyChar
                {
                    newStyle.colFg = me.getThemeColors().richEdit_TextSpecial
                    cr             = '.'
                }
                elif isTabulation
                {
                    newStyle.colFg = me.getThemeColors().richEdit_TextSpecial
                }
            }

            // Paint accumulated string if we have a change of style
            let styleChange = newStyle != oldStyle
            if styleChange or isTabulation
            {
                x        = me.paintString(bc, x, y, str, oldStyle, visible)
                oldStyle = newStyle
                str.clear()
            }

            if isTabulation
            {
                let x1 = x
                x += me.ed.getTabOffset(cast(s32) x)

                // Background color for tabulations
                if visible and oldStyle.colBk.a:
                    bc.painter.fillRect({x1, y, x - x1, size.y}, oldStyle.colBk)
            }
            else
            {
                str += cr
            }
        }

        // Remaining string
        if !str.isEmpty()
        {
            let x1 = me.paintString(bc, x, y, str, oldStyle, visible)
            x = x1
        }

        return x + me.scrollPos.x
    }
}
