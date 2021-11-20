using Core, Pixel

public struct EditBox
{
    using scrollwnd:    ScrollWnd
    note:               RichString
    icon:               Icon
    textContent:        String
    autoLoseFocus:      bool = true

    sigEnterPressed:    func(*EditBox)
    sigEscapePressed:   func(*EditBox)

    isHot:              bool
    isDragging:         bool
    selBeg, selEnd:     uint
    timerCaret:         *Timer
    visibleCaret:       bool
    posSelBeg:          f32
    posSelEnd:          f32
}

impl IWnd for EditBox
{
    mtd onFocusEvent(evt: *FocusEvent)
    {
        if evt.kind == .SetFocus
        {
            timerCaret = surface.app.addTimer(500, self)
            visibleCaret = true
        }
        else
        {
            surface.app.deleteTimer(timerCaret)
            timerCaret = null
        }

        invalidate()
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        painter := evt.bc.painter

        clientRect := getClientRect()
        theme   := getTheme()
        colors  := getThemeColors()
        metrics := getThemeMetrics()
        rects   := getThemeRects()

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorText:      Color = ?

        subRectBk = &rects.edit_SquareBk
        subRectBorder = &rects.edit_SquareBorder

        if bc.isDisabled
            colorBorder = colors.edit_DisabledBorder
        else if hasFocus()
            colorBorder = colors.edit_FocusBorder
        else if isHot
            colorBorder = colors.edit_HotBorder
        else
            colorBorder = colors.edit_Border

        colorBk = colors.edit_SquareBk

        theme.drawSubRect(painter, clientRect, subRectBk, colorBk)
        theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder)

        clientRect.inflate(-metrics.editBox_SquarePadding)

        // Note
        if !note.isEmpty() and textContent.isEmpty()
        {
            var format: RichStringFormat
            colorText = colors.edit_Note
            format.palette[0] = colorText
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault
            Theme.drawIconText(bc.painter, clientRect, icon, &note, format, metrics.iconText_Margin)
        }

        // Selection
        if selBeg != selEnd and hasFocus()
        {
            pos := clientRect
            pos.x = posSelBeg + theme.metrics.editBox_SquarePadding
            pos.width = posSelEnd - posSelBeg
            bc.painter.fillRect(pos, colors.edit_SelectedBk)
        }

        // Content
        colorText = colors.edit_Text
        bc.painter.drawString(clientRect.x, clientRect.y, textContent, theme.res.fontDefault.regular, colorText)

        // Caret
        if visibleCaret and hasFocus()
        {
            pos := getCaretPos()
            pos.x += theme.metrics.editBox_SquarePadding
            painter.fillRect(pos, Argb.White)
        }
    }

    mtd forceVisibleCaret()
    {
        if timerCaret
        {
            visibleCaret = true
            timerCaret.restart()
        }
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MousePressed:
            if evt.button != .Left
                break

            setFocus()
            x := getByteIndexAtPos(evt.pos.x - getThemeMetrics().editBox_SquarePadding)
            setSelection(x, x)
            forceVisibleCaret()
            evt.accepted = true
            isDragging = true
            captureMouse()

        case MouseReleased:
            if isDragging
                releaseMouse()
            isDragging = false
            evt.accepted = true

        case MouseMove:
            if !isDragging
                break
            evt.accepted = true
            x := getByteIndexAtPos(evt.pos.x - getThemeMetrics().editBox_SquarePadding)
            setSelection(selBeg, x)

        case MouseEnter:
            isHot = true
            invalidate()

        case MouseLeave:
            isHot = false
            invalidate()
        }
    }

    mtd onTimerEvent(evt: *TimerEvent)
    {
        visibleCaret = !visibleCaret
        pos := getCaretPos()
        pos.inflate(1)
        pos.offset(-getScrollPos())
        invalidateRect(pos)
    }

    mtd onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed
        {
            evt.accepted = keyPressed(evt.key, evt.modifiers)
            if evt.accepted
                forceVisibleCaret()
        }
        else if evt.kind == .Char
        {
            if evt.char < 32
                evt.accepted = false
            else
            {
                forceVisibleCaret()
                insertChar(evt.char)
            }
        }
    }
}

public impl EditBox
{
    func create(parent: *Wnd, content: string, position: Math.Rectangle, id: string = null)->*EditBox
    {
        res := Wnd.create'EditBox(parent, content, position, id)
        if res.position.height == 0
            res.position.height = res.getThemeMetrics().btnPush_Height
        if res.position.width == 0
            res.position.width  = res.getThemeMetrics().btnPush_Width

        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }

    // Delete selected text
    mtd deleteSelection()
    {
        if deleteSelectionPrivate()
            notifyChanged()
    }

    // Set current selection
    mtd setSelection(start, end: uint)
    {
        selBeg = start
        selEnd = end
        checkSelection()
        computeCaretPos()
    }

    // Select all text
    mtd selectAll()
    {
        setSelection(0, textContent.length)
    }
}

protected impl EditBox
{
    mtd keyPressed(key: Input.Key, mdf: Input.KeyModifiers)->bool
    {
        ctrl  := mdf & .Control
        shift := mdf & .Shift

        switch key
        {
        case A:
            if ctrl and !shift
            {
                setSelection(0, textContent.length)
                return true
            }

        case Return:
            if sigEnterPressed
                sigEnterPressed(self)
            if autoLoseFocus
                getTopView().setFocus()
            return true

        case Escape:
            if sigEscapePressed
                sigEscapePressed(self)
            if autoLoseFocus
                getTopView().setFocus()
            return true

        case Left:
            selEnd = moveCaretLeft(selEnd)
            if shift
                setSelection(selBeg, selEnd)
            else
                setSelection(selEnd, selEnd)
            return true

        case Right:
            selEnd = moveCaretRight(selEnd)
            if shift
                setSelection(selBeg, selEnd)
            else
                setSelection(selEnd, selEnd)
            return true

        case Home:
            if shift
                setSelection(selBeg, 0)
            else
                setSelection(0, 0)
            return true

        case End:
            if shift
                setSelection(selBeg, textContent.length)
            else
                setSelection(textContent.length, textContent.length)
            return true

        case Back:
            if selBeg == selEnd
            {
                if !selBeg
                    return true
                selBeg = moveCaretLeft(selBeg)
            }

            deleteSelection()
            return true

        case Delete:
            if selBeg == selEnd
            {
                selEnd = moveCaretRight(selBeg)
            }

            deleteSelection()
            return true
        }

        return true
    }

    mtd insertChar(r: rune)
    {
        var utf8: [5] u8
        cpt := Utf8.fromUnicode(utf8, @[r])
        if !cpt return
        utf8[cpt] = 0

        deleteSelectionPrivate()
        textContent.insert(selEnd, @mkstring(&utf8[0], cpt))
        setSelection(selEnd + cpt, selEnd + cpt)

        notifyChanged()
    }

    #[Swag.Inline]
    {
        mtd getSelBeg() => selBeg <= selEnd ? selBeg : selEnd
        mtd getSelEnd() => selBeg <= selEnd ? selEnd : selBeg
    }

    #[Swag.Discardable]
    mtd deleteSelectionPrivate()->bool
    {
        if selBeg == selEnd
            return false

        checkSelection()

        selB := getSelBeg()
        selE := getSelEnd()
        if selB == textContent.length
            return false
        textContent.remove(selB, selE - selB)
        setSelection(selB, selB)
        return true
    }

    mtd getCaretPos()->Math.Rectangle
    {
        var result: retval = ?

        clientRect := getClientRect()
        clientRect.applyPadding(padding)

        result = clientRect
        if !textContent.isEmpty()
            result.x += posSelEnd

        font := getTheme().res.fontDefault.regular
        result.height = font.height
        result.y = clientRect.vertCenter() - result.height * 0.5

        result.width = 2
        return result
    }

    mtd getByteIndexAtPos(x: f32)->uint
    {
        if !textContent.length
            return 0

        font := getTheme().res.fontDefault.regular

        // Can be better, of course...
        var painter: Painter{fake: true}
        lastWidth := 0'f32
        loop idx: 1..textContent.length-1
        {
            str := @mkstring(textContent.buffer, idx)
            met := painter.measureString(str, font)
            if x <= met.width
            {
                diff := (met.width - lastWidth) * 0.5
                if x < lastWidth + diff
                    return idx - 1
                return idx
            }

            lastWidth = met.width
        }

        return textContent.length
    }

    mtd computeCaretPos()
    {
        font := getTheme().res.fontDefault.regular

        str := @mkstring(textContent.buffer, selEnd)
        var painter: Painter{fake: true}
        met := painter.measureString(str, font)
        posSelBeg, posSelEnd = met.width

        if selBeg != selEnd
        {
            str = @mkstring(textContent.buffer, selBeg)
            met = painter.measureString(str, font)
            posSelBeg = met.width
        }
    }

    mtd moveCaretLeft(sel: uint)->uint
    {
        if !sel
            return sel
        str := @mkstring(textContent.buffer, sel)
        (?, eat) := Utf8.decodeLastRune(str)
        return sel - eat
    }

    mtd moveCaretRight(sel: uint)->uint
    {
        if sel == textContent.length
            return sel
        str := @mkstring(textContent.buffer + sel, textContent.length - sel)
        (?, eat) := Utf8.decodeRune(str)
        return sel + eat
    }

    mtd checkSelection()
    {
        selBeg = Math.min(selBeg, textContent.length)
        selEnd = Math.min(selEnd, textContent.length)
        invalidate()
    }

    mtd notifyChanged()
    {
        computeCaretPos()
        invalidate()
    }
}