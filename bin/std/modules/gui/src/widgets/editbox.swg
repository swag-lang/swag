using Core, Pixel

public enum EditBoxForm
{
    Square
    Round
    Flat
}

public enum EditBoxInputMode
{
    String
    S64
    U64
    F64
}

public enum EditBoxCheckResult
{
    Incomplete
    Valid
    InvalidDismiss
    InvalidShow
}

public struct EditBox
{
    using wnd:          Wnd
    icon:               Icon
    text:               String
    form:               EditBoxForm = Square
    autoLoseFocus:      bool = true
    maxLength:          s32
    inputMode:          EditBoxInputMode = String

    sigChanged:         func(*EditBox)
    sigLoseFocus:       func(*EditBox)
    sigEnterPressed:    func(*EditBox)
    sigEscapePressed:   func(*EditBox)
    sigCheckContent:    func(*EditBox)->EditBoxCheckResult

    using minMax: union{
        minMaxS64: {min: s64, max: s64} = @{Swag.S64.Min, Swag.S64.Max}
        minMaxU64: {min: u64, max: u64} = @{0, Swag.U64.Max}
        minMaxF64: {min: f64, max: f64} = @{-Swag.F64.Max, Swag.F64.Max}
    }

    isInvalid:          bool
    isHot:              bool
    isDragging:         bool
    selBeg, selEnd:     uint
    timerCaret:         *Timer
    visibleCaret:       bool
    posSelBeg:          f32
    posSelEnd:          f32
    scrollPosX:         f32
    countRunes:         s32
}

impl IWnd for EditBox
{
    mtd onFocusEvent(evt: *FocusEvent)
    {
        if evt.kind == .SetFocus
        {
            timerCaret = surface.app.addTimer(500, self)
            visibleCaret = true
            computeCaretPos()
        }
        else
        {
            if sigLoseFocus
                sigLoseFocus(self)
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
        else if isInvalid
            colorBorder = colors.edit_ErrorBorder
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

        // name
        if !name.isEmpty() and text.isEmpty()
        {
            var format: RichStringFormat
            colorText = colors.edit_Note
            format.palette[0] = colorText
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault
            Theme.drawIconText(bc.painter, clientRect, @{}, &name, format, metrics.iconText_Margin)
        }

        // Compute scroll position
        caretPos := getCaretPos()
        decalX := Math.min(Painter.measureString("X", theme.res.fontDefault.regular).width * 10, clientRect.width * 0.5)
        maxDecalX := Painter.measureString(text, theme.res.fontDefault.regular).width - decalX
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
        bc.painter.drawString(clientRect.x, y, text, font, colorText)

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
                insertRune(evt.char)
            }
        }
    }
}

impl EditBox
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
                setSelection(0, text.length)
                return true
            }

        case Insert:
            if ctrl and !shift and selBeg != selEnd
            {
                Clipboard.addString(@mkstring(text.buffer + getSelBeg(), getSelEnd() - getSelBeg()))
                return true
            }
            else if shift and !ctrl
            {
                (str, ok) := Clipboard.getString()
                if ok
                    insertString(str.toString())
                return true
            }

        case C:
            if ctrl and selBeg != selEnd
            {
                Clipboard.addString(@mkstring(text.buffer + getSelBeg(), getSelEnd() - getSelBeg()))
                return true
            }

        case X:
            if ctrl and selBeg != selEnd
            {
                Clipboard.addString(@mkstring(text.buffer + getSelBeg(), getSelEnd() - getSelBeg()))
                deleteSelection()
                return true
            }

        case V:
            if ctrl
            {
                (str, ok) := Clipboard.getString()
                if ok
                    insertString(str.toString())
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
                setSelection(selBeg, text.length)
            else
                setSelection(text.length, text.length)
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

        return false
    }

    mtd insertString(str: string)
    {
        Utf8.visitRunes(|r| str)
        {
            insertRune(r)
        }
    }

    mtd insertUtf8(s: string)
    {
        deleteSelectionPrivate()
        text.insert(selEnd, s)
        setSelection(selEnd + @countof(s), selEnd + @countof(s))
        countRunes += 1
    }

    mtd checkContent()->EditBoxCheckResult
    {
        res := EditBoxCheckResult.Valid
        if sigCheckContent
        {
            res = sigCheckContent(self)
        }
        else
        {
            eat := 0'u32
            switch inputMode
            {
            case S64:
                val := catch Parse.getS64(text)
                eat = val.eat
                if !@err and eat == text.length and (val.value < minMaxS64.min or val.value > minMaxS64.max)
                    res = .InvalidShow

            case U64:
                val := catch Parse.getU64(text)
                eat = val.eat
                if !@err and eat == text.length and (val.value < minMaxU64.min or val.value > minMaxU64.max)
                    res = .InvalidShow

            case F64:
                val := catch Parse.getF64(text)
                eat = val.eat
                if !@err and eat == text.length and (val.value < minMaxF64.min or val.value > minMaxF64.max)
                    res = .InvalidShow
            }

            if res == .Valid
            {
                if @err == Parse.ErrorIncomplete
                    res = .Incomplete
                else if @err == Parse.ErrorOutOfRange or @err == Parse.ErrorOverflow
                    res = .InvalidShow
                else if @err or eat != @countof(text)
                    res = .InvalidDismiss
            }
        }

        if res == .Valid
            isInvalid = false
        else if res == .InvalidShow
            isInvalid = true

        return res
    }

    mtd insertRune(r: rune)
    {
        if maxLength and countRunes == maxLength
            return

        // Convert rune to utf8
        var utf8: [5] u8
        cpt := Utf8.fromUnicode(utf8, @[r])
        if !cpt return
        utf8[cpt] = 0

        // Should we check the content ?
        mustValidate := sigCheckContent != null or inputMode != .String
        if !mustValidate
        {
            insertUtf8(@mkstring(&utf8[0], cpt))
            notifyChanged()
            return
        }

        saveText   := text
        saveSelBeg := selBeg
        saveSelEnd := selEnd
        insertUtf8(@mkstring(&utf8[0], cpt))

        res := checkContent()
        if res == .InvalidDismiss
        {
            text = saveText
            setSelection(saveSelBeg, saveSelEnd)
            countRunes -= 1
        }
        else
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
        if selB == text.length
            return false

        countRunes -= cast(s32) Utf8.countRunes(text[selB..selE-1])
        text.remove(selB, selE - selB)
        discard checkContent()

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
        if !text.isEmpty()
            result.x += posSelEnd

        font := getTheme().res.fontDefault.regular
        result.height = font.height
        result.y = clientRect.vertCenter() - result.height * 0.5

        result.width = 2
        return result
    }

    mtd getByteIndexAtPos(x: f32)->uint
    {
        if !text.length
            return 0

        font := getTheme().res.fontDefault.regular

        // Can be better, of course...
        lastWidth := 0'f32
        loop idx: 1..text.length-1
        {
            str := @mkstring(text.buffer, idx)
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

        return text.length
    }

    mtd computeCaretPos()
    {
        if getFocus() != self
            return
        font := getTheme().res.fontDefault.regular

        str := @mkstring(text.buffer, selEnd)
        met := Painter.measureString(str, font)
        posSelBeg, posSelEnd = met.width

        if selBeg != selEnd
        {
            str = @mkstring(text.buffer, selBeg)
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

        test := !Latin1.isSpace(text.buffer[s])
        loop
        {
            ns := moveCaretLeft(s)
            if ns == s
                break
            if Latin1.isSpace(text.buffer[ns]) == test
                return s
            s = ns
        }

        return 0
    }

    mtd moveCaretRightEndWord(sel: uint)->uint
    {
        if sel == text.length
            return sel

        test := !Latin1.isSpace(text.buffer[sel])
        s := sel
        while s != text.length
        {
            s = moveCaretRight(s)
            if Latin1.isSpace(text.buffer[s]) == test
                return s
        }

        return text.length
    }

    mtd moveCaretLeft(sel: uint)->uint
    {
        if !sel
            return sel
        str := @mkstring(text.buffer, sel)
        (?, eat) := Utf8.decodeLastRune(str)
        return sel - eat
    }

    mtd moveCaretRight(sel: uint)->uint
    {
        if sel == text.length
            return sel
        str := @mkstring(text.buffer + sel, text.length - sel)
        (?, eat) := Utf8.decodeRune(str)
        return sel + eat
    }

    mtd checkSelection()
    {
        selBeg = Math.min(selBeg, text.length)
        selEnd = Math.min(selEnd, text.length)
        invalidate()
    }

    mtd notifyChanged()
    {
        if sigChanged
            sigChanged(self)
        computeCaretPos()
        invalidate()
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
        res.text = content
        res.focusStrategy = .MousePressed

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
        if selBeg == start and selEnd == end
            return
        selBeg = start
        selEnd = end
        checkSelection()
        computeCaretPos()
    }

    // Select all text
    mtd selectAll()
    {
        setSelection(0, text.length)
    }

    // Set the editbox note
    mtd setNote(note: string)
    {
        name = note
        invalidate()
    }

    // Set the editbox content
    mtd setText(str: string)
    {
        if text == str
            return
        text.clear()
        selBeg, selEnd = 0
        insertString(str)
        notifyChanged()
    }

    // Editbox will edit signed integers
    mtd setInputS64(min = Swag.S64.Min, max = Swag.S64.Max)
    {
        inputMode = .S64
        minMaxS64.min = min
        minMaxS64.max = max
    }

    // Editbox will edit unsigned integers
    mtd setInputU64(min = 0'u64, max = Swag.U64.Max)
    {
        inputMode = .U64
        minMaxU64.min = min
        minMaxU64.max = max
    }

    // Editbox will edit floating points
    mtd setInputF64(min = -Swag.F64.Max, max = Swag.F64.Max)
    {
        inputMode = .F64
        minMaxF64.min = min
        minMaxF64.max = max
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