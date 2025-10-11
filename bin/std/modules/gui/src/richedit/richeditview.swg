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
        if .ed and .ed.richEdFlags & .WordWrap do
            .ed.recomputeScroll = true
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = false
        switch evt.kind
        {
        case .MouseDoubleClick:
            if evt.button == .Left
            {
                .ed.selectWord()
                evt.accepted = true
            }

        case .MousePressed:
            if evt.button == .Left
            {
                evt.accepted = true
                let pt = .surfaceToLocal(evt.surfacePos) + .scrollPos
                let cp = .ed.getCursorPosFromPoint(pt)
                .firstLineSelMode = cp.lineIndex
                .ed.setCursorPos(cp)
                if !(evt.modifiers & .Shift) do
                    .ed.selBeg = cp
                .selMode = true
                .ed.setRectangularSelection(evt.modifiers & .Alt ? true : false)
                .ed.ensureCaretIsVisible()
                .captureMouse()
            }

        case .MouseReleased:
            if .selMode
            {
                evt.accepted = true
                .selMode     = false
                .releaseMouse()
            }

        case .MouseMove:
            if .selMode
            {
                evt.accepted = true
                let pt = .surfaceToLocal(evt.surfacePos) + .scrollPos
                let cp = .ed.getCursorPosFromPoint(pt)
                if .ed.cursorPos.lineIndex != cp.lineIndex or .ed.cursorPos.charPosInLine != cp.charPosInLine
                {
                    if .ed.hasSelRect
                    {
                        if cp.charPosInLine >= .ed.selBeg.charPosInLine do
                            .ed.setCursorPos(cp)
                        .ed.checkSelection()
                    }
                    else
                    {
                        .ed.setCursorPos(cp)
                    }

                    .ed.ensureCaretIsVisible()
                }
            }
        }
    }

    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed
        {
            // Lose focus with escape or enter
            if .ed.richEdFlags & .AutoLoseFocus
            {
                if evt.key == .Escape or (evt.key == .Enter and evt.modifiers.equals(.Zero))
                {
                    evt.accepted = false
                    .getTopView().setFocus()
                    return
                }
            }

            evt.accepted = true
            .ed.keyPressed(evt.key, evt.modifiers)
            .forceVisibleCaret()
        }
        elif evt.kind == .Rune
        {
            evt.accepted = true
            .ed.charPressed(evt.key, evt.char, evt.modifiers)
            .forceVisibleCaret()
        }
    }

    mtd impl onFocusEvent(evt: *FocusEvent)
    {
        switch evt.kind
        {
        case SetFocus:
            .timerCaret   = .surface.app.addTimer(500'ms, me)
            .visibleCaret = true
            .invalidate()

        case KillFocus:
            .surface.app.deleteTimer(.timerCaret)
            .timerCaret   = null
            .visibleCaret = false
            .invalidate()
        }
    }

    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        .visibleCaret = !.visibleCaret
        .invalidate()
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter = evt.bc.painter
        let colors  = .getThemeColors()

        let marginLeft = .ed.getMarginLeft()
        var x          = 0'f32
        var y          = 0'f32
        let size       = .ed.getXExtent()

        // Force visible caret
        if .ed.forceCaretVisible do
            .ed.ensureCaretIsVisibleInternal(.ed.forceCaretVisibleCenter)

        // Start to the first visible line, if known
        var i: u64
        if .ed.dirtyFirstLineView or .ed.recomputeScroll
        {
            i                        = 0
            .ed.firstVisibleLineView = Swag.U64.Max
        }
        else
        {
            i = .ed.firstVisibleLineView
            y = .ed.lines[i].drawingRect.y
        }

        if .ed.recomputeScroll do
            .ed.countVisibleLines = 0
        .ed.dirtyFirstLineView = false

        let clientRect  = .getClientRect()
        let firstHidden = true
        var maxx        = 0'f32

        let sizeChanged = .lastClientRect != clientRect
        .lastClientRect = clientRect

        // All lines
        while i < .ed.lines.count
        {
            // Is the line visible ?
            var bvis = true
            if y + size.y < .scrollPos.y or y - .scrollPos.y > clientRect.bottom() do
                bvis = false

            // Store the first visible line index
            if bvis and .ed.firstVisibleLineView == Swag.U64.Max and (.ed.lines[i].isVisible() or firstHidden) do
                .ed.firstVisibleLineView = i

            if .ed.recomputeScroll do
                .ed.countVisibleLines += 1
            .ed.lastVisibleLineView = i

            let l = .ed.lines[i]

            l.drawingRect.x      = marginLeft
            l.drawingRect.y      = y
            l.drawingRect.width  = x - marginLeft
            l.drawingRect.height = size.y

            // Current line
            if bvis and .ed.isSelectionEmpty() and i == .ed.cursorPos.lineIndex and .ed.richEdFlags & .CurLineBorder
            {
                var fullRect = l.drawingRect
                fullRect.y -= .scrollPos.y
                fullRect.setRight(clientRect.right() - .ed.getMarginRight())
                fullRect.inflate(-1)
                painter.drawRect(fullRect, colors.richEdit_CurLineBorder, 1)
            }

            // Lex the line
            if !l.wordWrap and (l.dirtyStyle or sizeChanged or .ed.recomputeScroll)
            {
                .ed.unwrap(i)

                if .ed.lexer
                {
                    let prevLine = i ? .ed.lines[i - 1] : null
                    let nextLine = i != .ed.lines.count - 1 ? .ed.lines[i + 1] : null
                    .ed.lexer.compute(prevLine, l, nextLine)
                }

                .ed.wrap(i)
                l.dirtyStyle = false
            }

            if bvis or .ed.recomputeScroll
            {
                x = .paintLine(evt.bc, i, y - .scrollPos.y, bvis)
            }

            maxx = Math.max(maxx, x)
            y += size.y
            i += 1
            if !.ed.recomputeScroll and y >= .scrollPos.y + clientRect.bottom() do
                break
        }

        // Caret
        if .visibleCaret
        {
            let colorCaret = .getThemeColors().richEdit_Caret
            if .ed.hasSelRect
            {
                let selBeg = .ed.getBegSelection()
                let selEnd = .ed.getEndSelection()
                for caretI in selBeg.lineIndex to selEnd.lineIndex
                {
                    var cp = .ed.cursorPos
                    cp.lineIndex = caretI
                    if cp.charPosInLine > .ed.lines[cp.lineIndex].length() do
                        continue
                    var posCaret = .ed.getCaretPos(cp)
                    posCaret.x -= .scrollPos.x
                    posCaret.y -= .scrollPos.y
                    painter.fillRect(posCaret, colorCaret)
                }
            }
            else
            {
                var posCaret = .ed.getCaretPos(.ed.cursorPos)
                posCaret.x -= .scrollPos.x
                posCaret.y -= .scrollPos.y
                painter.fillRect(posCaret, colorCaret)
            }
        }

        // Scroll size
        y = cast(f32) (.ed.countVisibleLines * size.y)
        if .ed.richEdFlags & .WordWrap do
            maxx = 0
        elif !.ed.recomputeScroll do
            maxx = .ed.scrollWnd.scrollSize.x
        else
        {
            let posCaret = .ed.getCaretPos(.ed.cursorPos)
            maxx += posCaret.width + 32
        }

        .ed.recomputeScroll = false
        .ed.scrollWnd.setScrollSize(maxx, y)

        // Force visible caret
        if .ed.forceCaretVisible
        {
            .ed.forceCaretVisible = false
            .ed.ensureCaretIsVisibleInternal(.ed.forceCaretVisibleCenter)
        }
    }
}

impl RichEditView
{
    mtd forceVisibleCaret()
    {
        if .timerCaret
        {
            .visibleCaret = true
            .timerCaret.restart()
            .invalidate()
        }
    }

    mtd paintString(bc: *PaintContext, x, y: f32, str: String, strStyle: RichEditRuneStyle, visible: bool)->f32
    {
        let painter = bc.painter

        let fontR = .ed.getFont().getFont(strStyle.fontMode)
        if !fontR do
            return 0

        let self = Painter.measureString(str, fontR)

        // Background rectangle
        if visible and strStyle.colBk.a
        {
            var rect: Math.Rectangle
            rect.x      = x
            rect.y      = y
            rect.width  = self.width
            rect.height = .ed.getXExtent().y
            painter.fillRect(rect, strStyle.colBk)
        }

        painter.drawString(x, y, str, fontR, strStyle.colFg, strStyle.underline)
        return x + self.width
    }

    mtd paintLine(bc: *PaintContext, lineIndex: u64, py: f32, visible: bool)->f32
    {
        let line = .ed.lines[lineIndex]
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
        oldStyle = .ed.styles[0]
        var x = .ed.getMarginLeft() - .scrollPos.x

        let sel0 = .ed.getBegSelection()
        let sel1 = .ed.getEndSelection()
        let size = .ed.getXExtent()

        var i = 0
        while i < count
        {
            defer
            {
                i += 1
                pChar += 1
                pStyle += 1
            }
            newStyle = .ed.styles[dref pStyle]

            var cr           = dref pChar
            let isTabulation = cr == '\t'

            var selected = false
            if .ed.hasSelRect
            {
                var cp1: RichEditCursor
                cp1.charPosInLine = i
                cp1.lineIndex     = lineIndex
                var charPos = .ed.getCursorDisplayPos(cp1)
                charPos.y -= size.y * 0.5
                if .ed.selRectangle.contains(charPos) do
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
                if .hasFocus() do
                    newStyle = .ed.stylesSelect[0]
                elif .ed.richEdFlags & .ShowFocusSel do
                    newStyle = .ed.stylesSelect[1]
                if !newStyle.colFg.a do
                    newStyle.colFg = .ed.styles[dref pStyle].colFg
                newStyle.underline = .ed.styles[dref pStyle].underline
            }

            if .ed.richEdFlags & .DrawBlanks
            {
                if cr == ' ' and pChar != &dummyChar
                {
                    newStyle.colFg = .getThemeColors().richEdit_TextSpecial
                    cr             = '.'
                }
                elif isTabulation
                {
                    newStyle.colFg = .getThemeColors().richEdit_TextSpecial
                }
            }

            // Paint accumulated string if we have a change of style
            let styleChange = newStyle != oldStyle
            if styleChange or isTabulation
            {
                x        = .paintString(bc, x, y, str, oldStyle, visible)
                oldStyle = newStyle
                str.clear()
            }

            if isTabulation
            {
                let x1 = x
                x += .ed.getTabOffset(cast(s32) x)

                // Background color for tabulations
                if visible and oldStyle.colBk.a do
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
            let x1 = .paintString(bc, x, y, str, oldStyle, visible)
            x = x1
        }

        return x + .scrollPos.x
    }
}
