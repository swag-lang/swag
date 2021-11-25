using Core, Pixel

public enum EditBoxForm
{
    Square
    Round
    Flat
}

public struct EditBox
{
    using wnd:          Wnd
    note:               RichString
    icon:               Icon
    textContent:        String
    form:               EditBoxForm = Square
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
    scrollPosX:         f32
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

        theme   := getTheme()
        colors  := getThemeColors()
        metrics := getThemeMetrics()
        rects   := getThemeRects()

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorText:      Color = ?

        switch form
        {
        case Square:
            subRectBk = &rects.edit_SquareBk
            subRectBorder = &rects.edit_SquareBorder
        case Round:
            subRectBk = &rects.edit_RoundBk
            subRectBorder = &rects.edit_RoundBorder
        case Flat:
            subRectBk = &rects.edit_FlatBk
            subRectBorder = &rects.edit_FlatBorder
        }

        if bc.isDisabled
            colorBorder = colors.edit_DisabledBorder
        else if hasFocus()
            colorBorder = colors.edit_FocusBorder
        else if isHot
            colorBorder = colors.edit_HotBorder
        else
            colorBorder = colors.edit_Border

        colorBk = colors.edit_SquareBk

        clientRect := getClientRect()
        theme.drawSubRect(painter, clientRect, subRectBk, colorBk)
        theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder)

        clientRect = getClientRectPadding()

        // Icon
        if icon.isValid()
        {
            icon.paint(bc.painter, clientRect.x, clientRect.vertCenter() - icon.size * 0.5, colors.edit_Text)
            clientRect.x += icon.size + metrics.iconText_Margin
            clientRect.width -= icon.size + metrics.iconText_Margin
        }

        // Note
        if !note.isEmpty() and textContent.isEmpty()
        {
            var format: RichStringFormat
            colorText = colors.edit_Note
            format.palette[0] = colorText
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault
            Theme.drawIconText(bc.painter, clientRect, @{}, &note, format, metrics.iconText_Margin)
        }

        // Compute scroll position
        caretPos := getCaretPos()
        decalX := Math.min(Painter.measureString("X", theme.res.fontDefault.regular).width * 10, clientRect.width * 0.5)
        maxDecalX := Painter.measureString(textContent, theme.res.fontDefault.regular).width - decalX
        if caretPos.x - clientRect.x < scrollPosX
            scrollPosX = caretPos.x - clientRect.x - decalX
        else if caretPos.x - clientRect.x >= scrollPosX + clientRect.width
            scrollPosX = Math.min(caretPos.x - clientRect.x - decalX, maxDecalX)
        scrollPosX = Math.max(scrollPosX, 0)

        painter.pushClippingRect(clientRect)
        painter.translateTransform(-scrollPosX, 0)
        font := theme.res.fontDefault.regular

        // Selection
        if selBeg != selEnd and hasFocus()
        {
            pos := clientRect
            pos.x += posSelBeg
            pos.y = clientRect.vertCenter() - font.height * 0.5
            pos.height = font.height
            pos.width = posSelEnd - posSelBeg
            bc.painter.fillRect(pos, colors.edit_SelectedBk)
        }

        // Content
        colorText = colors.edit_Text
        y := clientRect.vertCenter() - font.height * 0.5
        bc.painter.drawString(clientRect.x, y, textContent, font, colorText)

        // Caret
        if visibleCaret and hasFocus()
        {
            painter.fillRect(caretPos, Argb.White)
        }

        painter.popClippingRect()
        painter.translateTransform(scrollPosX, 0)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MouseDoubleClick:
            if evt.button != .Left
                break
            x := moveCaretLeftStartWord(selBeg)
            y := moveCaretRightEndWord(selEnd)
            setSelection(x, y)

        case MousePressed:
            if evt.button != .Left
                break
            setFocus()
            marginLeft := getTextMarginLeft()
            x := getByteIndexAtPos(evt.pos.x - marginLeft + scrollPosX)
            setSelection(x, x)
            forceVisibleCaret()
            evt.accepted = true
            isDragging = true
            captureMouse()

        case MouseReleased:
            if evt.button != .Left
                break
            if isDragging
                releaseMouse()
            isDragging = false
            evt.accepted = true

        case MouseMove:
            if !isDragging
                break
            evt.accepted = true
            marginLeft := getTextMarginLeft()
            x := getByteIndexAtPos(evt.pos.x - marginLeft + scrollPosX)
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
        res := Wnd.create'EditBox(parent, null, position, id)
        if res.position.height == 0
            res.position.height = res.getThemeMetrics().editBox_Height
        if res.position.width == 0
            res.position.width  = res.getThemeMetrics().editBox_Width
        res.setForm(EditBoxForm.Square)
        res.textContent = content

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

    // Set the editbox form
    mtd setForm(form: EditBoxForm)
    {
        self.form = #scopefct.form
        switch self.form
        {
        case Square:
            padding = getTheme().metrics.editBox_SquarePadding
        case Round:
            padding = getTheme().metrics.editBox_RoundPadding
        case Flat:
            padding = getTheme().metrics.editBox_FlatPadding
        }

        invalidate()
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
            if ctrl
                selEnd = moveCaretLeftStartWord(selEnd)
            else
                selEnd = moveCaretLeft(selEnd)
            if shift
                setSelection(selBeg, selEnd)
            else
                setSelection(selEnd, selEnd)
            return true

        case Right:
            if ctrl
                selEnd = moveCaretRightEndWord(selEnd)
            else
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

    mtd getTextMarginLeft()->f32
    {
        rect := getClientRectPadding()
        if icon.isValid()
            rect.x += icon.size + getThemeMetrics().iconText_Margin
        return rect.x
    }

    mtd getFormPadding()->Math.Vector4
    {
        switch form
        {
        case Square:
            return getTheme().metrics.editBox_SquarePadding
        case Round:
            return getTheme().metrics.editBox_RoundPadding
        case Flat:
            return getTheme().metrics.editBox_FlatPadding
        }

        return @{}
    }

    mtd forceVisibleCaret()
    {
        if timerCaret
        {
            visibleCaret = true
            timerCaret.restart()
        }
    }

    mtd getCaretPos()->Math.Rectangle
    {
        var result: retval = ?

        clientRect := getClientRectPadding()
        if icon.isValid()
            clientRect.x += icon.size + 4

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
        lastWidth := 0'f32
        loop idx: 1..textContent.length-1
        {
            str := @mkstring(textContent.buffer, idx)
            met := Painter.measureString(str, font)
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
        met := Painter.measureString(str, font)
        posSelBeg, posSelEnd = met.width

        if selBeg != selEnd
        {
            str = @mkstring(textContent.buffer, selBeg)
            met = Painter.measureString(str, font)
            posSelBeg = met.width
        }
    }

    mtd moveCaretLeftStartWord(sel: uint)->uint
    {
        if !sel
            return sel

        s := sel
        s = moveCaretLeft(s)
        if s == sel
            return sel

        test := !Latin1.isSpace(textContent.buffer[s])
        loop
        {
            ns := moveCaretLeft(s)
            if ns == s
                break
            if Latin1.isSpace(textContent.buffer[ns]) == test
                return s
            s = ns
        }

        return 0
    }

    mtd moveCaretRightEndWord(sel: uint)->uint
    {
        if sel == textContent.length
            return sel

        test := !Latin1.isSpace(textContent.buffer[sel])
        s := sel
        while s != textContent.length
        {
            s = moveCaretRight(s)
            if Latin1.isSpace(textContent.buffer[s]) == test
                return s
        }

        return textContent.length
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