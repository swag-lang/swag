#global public
using Core, Pixel

enum EditBoxForm
{
    Square
    Round
    Flat
    Transparent
}

enum EditBoxInputMode
{
    String
    S64
    U64
    F64
}

enum EditBoxCheckResult
{
    Incomplete
    Valid
    InvalidDismiss
    InvalidShow
}

#[Swag.EnumFlags]
enum EditBoxFlags
{
    Zero = 0
    AutoLoseFocus
    ReadOnly
    RightAlign
}

struct EditBox
{
    using wnd: Wnd

    icon:               Icon
    text:               String
    form                = EditBoxForm.Square
    editBoxFlags        = EditBoxFlags.AutoLoseFocus
    inputMode           = EditBoxInputMode.String
    maxLength:          s32
    rightMargin:        f32

    sigChanged:         SigArray'closure(*Self)
    sigLoseFocus:       SigArray'closure(*Self)
    sigEnterPressed:    SigArray'closure(*Self)
    sigEscapePressed:   SigArray'closure(*Self)
    sigCheckContent:    closure(*Self)->EditBoxCheckResult

    using minMax: union{
        minMaxS64: {min: s64, max: s64} = {Swag.S64.Min, Swag.S64.Max}
        minMaxU64: {min: u64, max: u64} = {0, Swag.U64.Max}
        minMaxF64: {min: f64, max: f64} = {-Swag.F64.Max, Swag.F64.Max}
    }

    aniBk:      BlendColor{speed: 4}
    aniBorder:  BlendColor{speed: 4}

    isInvalid:          bool
    isHot:              bool
    isDragging:         bool
    selBeg, selEnd:     u64
    timerCaret:         *Timer
    visibleCaret:       bool
    posSelBeg:          f32
    posSelEnd:          f32
    scrollPosX:         f32
    countRunes:         s32
}

impl IWnd for EditBox
{
    mtd impl onFocusEvent(evt: *FocusEvent)
    {
        switch evt.kind
        {
        case SetFocus:
            timerCaret = surface.app.addTimer(500'ms, self)
            visibleCaret = true
            computeCaretPos()
            invalidate()

        case KillFocus:
            sigLoseFocus.call(self)
            surface.app.deleteTimer(timerCaret)
            timerCaret = null
            visibleCaret = false
            invalidate()
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc = evt.bc
        let painter = evt.bc.painter

        let theme = getTheme()
        let colors = getThemeColors()
        let metrics = getThemeMetrics()
        let rects = getThemeRects()

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var colorBk:        Color = undefined
        var colorBorder:    Color = undefined
        var colorText:      Color = undefined

        switch form
        {
        case Square, Transparent:
            subRectBk = &rects.edit_SquareBk
            subRectBorder = &rects.edit_SquareBorder
        case Round:
            subRectBk = &rects.edit_RoundBk
            subRectBorder = &rects.edit_RoundBorder
        case Flat:
            subRectBk = &rects.edit_FlatBk
            subRectBorder = &rects.edit_FlatBorder
        }

        colorText = colors.edit_Text

        switch
        {
        case bc.isDisabled:
            colorBorder = colors.edit_DisabledBorder
            colorBk = colors.edit_DisabledBk
            colorText = colors.edit_DisabledText
        case isInvalid:
            colorBorder = colors.edit_ErrorBorder
            colorBk = colors.edit_ErrorBk
        case hasFocus():
            colorBorder = colors.edit_FocusBorder
            colorBk = colors.edit_FocusBk
        case isHot:
            colorBorder = colors.edit_HotBorder
            colorBk = colors.edit_HotBk
        default:
            colorBorder = colors.edit_Border
            colorBk = colors.edit_Bk
        }

        aniBk.apply(self, colorBk)
        aniBorder.apply(self, colorBorder)

        var clientRect = getClientRect()
        if form != .Transparent do
            theme.drawSubRect(painter, clientRect, subRectBk, aniBk.cur())
        if form != .Transparent or isInvalid do
            theme.drawSubRect(painter, clientRect, subRectBorder, aniBorder.cur())

        clientRect = getEditRect()

        // Icon
        if icon.isValid()
        {
            icon.paint(bc.painter, clientRect.x, clientRect.vertCenter() - icon.sizeY * 0.5, colors.edit_Text)
            clientRect.x += icon.sizeX + metrics.iconText_Margin
            clientRect.width -= icon.sizeX + metrics.iconText_Margin
        }

        // name
        if !name.isEmpty() and text.isEmpty()
        {
            var format: RichStringFormat
            format.palette[0] = colors.edit_Note
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = getFont()
            Theme.drawIconText(bc.painter, clientRect, {}, &name, format, metrics.iconText_Margin)
        }

        // Compute scroll position
        let fontR = getFont().regular
        var caretPos = getCaretPos()
        let decalX = Math.min(Painter.measureString("X", fontR).width * 10, clientRect.width * 0.5)
        let sizeX = Painter.measureString(text, fontR).width
        let maxDecalX = sizeX - decalX

        if caretPos.x - clientRect.x < scrollPosX do
            scrollPosX = caretPos.x - clientRect.x - decalX
        elif caretPos.x - clientRect.x >= scrollPosX + clientRect.width do
            scrollPosX = Math.min(caretPos.x - clientRect.x - decalX, maxDecalX)

        if editBoxFlags.has(.RightAlign) do
            scrollPosX = Math.min(scrollPosX, 0)
        else do
            scrollPosX = Math.max(scrollPosX, 0)

        painter.pushClippingRect(clientRect)
        painter.translateTransform(-scrollPosX, 0)

        // Selection
        if selBeg != selEnd and hasFocus()
        {
            var pos = clientRect
            if editBoxFlags.has(.RightAlign) do
                pos.x = pos.right() - sizeX + posSelBeg
            else do
                pos.x += posSelBeg
            pos.y = clientRect.vertCenter() - fontR.height * 0.5
            pos.height = fontR.height
            pos.width = posSelEnd - posSelBeg
            bc.painter.fillRect(pos, colors.edit_SelectedBk)
        }

        // Content
        let y = clientRect.vertCenter() - fontR.height * 0.5
        if editBoxFlags.has(.RightAlign) do
            clientRect.x = clientRect.right() - sizeX
        bc.painter.drawString(clientRect.x, y, text, fontR, colorText)

        // Caret
        if visibleCaret and hasFocus()
        {
            painter.fillRect(caretPos, Argb.White)
        }

        painter.popClippingRect()
        painter.translateTransform(scrollPosX, 0)
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        var pos = surfaceToLocal(evt.surfacePos)
        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false
            return

        case MouseDoubleClick:
            if evt.button != .Left do
                break
            let x = moveCaretLeftStartWord(selBeg)
            let y = moveCaretRightEndWord(selEnd)
            setSelection(x, y)

        case MousePressed:
            if evt.button != .Left do
                break
            let x = getByteIndexAtPos(pos.x)
            setSelection(x, x)
            forceVisibleCaret()
            evt.accepted = true
            isDragging = true
            captureMouse()

        case MouseReleased:
            if evt.button != .Left do
                break
            if isDragging do
                releaseMouse()
            isDragging = false
            evt.accepted = true

        case MouseMove:
            if !isDragging do
                break
            evt.accepted = true
            let x = getByteIndexAtPos(pos.x)
            setSelection(selBeg, x)

        case MouseEnter:
            isHot = true
            invalidate()

        case MouseLeave:
            isHot = false
            invalidate()
        }
    }

    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        visibleCaret = !visibleCaret
        var pos = getCaretPos()
        pos.inflate(1)
        invalidateRect(pos)
    }

    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed
        {
            evt.accepted = keyPressed(evt.key, evt.modifiers)
            if evt.accepted do
                forceVisibleCaret()
        }
        elif evt.kind == .Rune and !editBoxFlags.has(.ReadOnly)
        {
            if evt.char < 32 do
                evt.accepted = false
            else
            {
                forceVisibleCaret()
                deleteSelectionPrivate()
                insertRune(evt.char)
            }
        }
    }
}

internal impl EditBox
{
    mtd keyPressed(key: Input.Key, mdf: Input.KeyModifiers)->bool
    {
        let ctrl = mdf & .Control
        let shift = mdf & .Shift

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
            elif shift and !ctrl
            {
                var str = Clipboard.getString()
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
                var str = Clipboard.getString()
                insertString(str.toString())
                return true
            }

        case Return:
            sigEnterPressed.call(self)
            if editBoxFlags.has(.AutoLoseFocus) do
                getTopView().setFocus()
            return true

        case Escape:
            sigEscapePressed.call(self)
            if editBoxFlags.has(.AutoLoseFocus) do
                getTopView().setFocus()
            return true

        case Left:
            if ctrl do
                selEnd = moveCaretLeftStartWord(selEnd)
            else do
                selEnd = moveCaretLeft(selEnd)
            if shift do
                setSelection(selBeg, selEnd)
            else do
                setSelection(selEnd, selEnd)
            return true

        case Right:
            if ctrl do
                selEnd = moveCaretRightEndWord(selEnd)
            else do
                selEnd = moveCaretRight(selEnd)
            if shift do
                setSelection(selBeg, selEnd)
            else do
                setSelection(selEnd, selEnd)
            return true

        case Home:
            if shift do
                setSelection(selBeg, 0)
            else do
                setSelection(0, 0)
            return true

        case End:
            if shift do
                setSelection(selBeg, text.length)
            else do
                setSelection(text.length, text.length)
            return true

        case Back:
            if selBeg == selEnd
            {
                if !selBeg do
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
        var res = EditBoxCheckResult.Valid
        if sigCheckContent
        {
            res = sigCheckContent(self)
        }
        else
        {
            var eat = 0'u32
            switch inputMode
            {
            case S64:
                var val = catch StrConv.parseS64(text)
                eat = val.eat
                if @err() == null and eat == text.length and (val.value < minMaxS64.min or val.value > minMaxS64.max) do
                    res = .InvalidShow

            case U64:
                var val = catch StrConv.parseU64(text)
                eat = val.eat
                if @err() == null and eat == text.length and (val.value < minMaxU64.min or val.value > minMaxU64.max) do
                    res = .InvalidShow

            case F64:
                var val = catch StrConv.parseF64(text)
                eat = val.eat
                if @err() == null and eat == text.length and (val.value < minMaxF64.min or val.value > minMaxF64.max) do
                    res = .InvalidShow
            }

            if res == .Valid
            {
                if @err() == Errors.ParseIncompleteError do
                    res = .Incomplete
                elif @err() == Errors.OverflowError do
                    res = .InvalidShow
                elif @err() != null or eat != @countof(text) do
                    res = .InvalidDismiss
            }
        }

        if res == .Valid do
            isInvalid = false
        elif res == .InvalidShow do
            isInvalid = true

        return res
    }

    mtd insertRune(r: rune)
    {
        if maxLength and countRunes == maxLength do
            return

        // Convert rune to utf8
        var utf8: [5] u8
        let cpt = Utf8.fromUnicode(utf8, [r])
        if !cpt do return
        utf8[cpt] = 0

        // Should we check the content ?
        let mustValidate = sigCheckContent != null or inputMode != .String
        if !mustValidate
        {
            insertUtf8(@mkstring(&utf8[0], cpt))
            notifyChanged()
            return
        }

        var saveText = text
        let saveSelBeg = selBeg
        let saveSelEnd = selEnd
        insertUtf8(@mkstring(&utf8[0], cpt))

        let res = checkContent()
        if res == .InvalidDismiss
        {
            text = saveText
            setSelection(saveSelBeg, saveSelEnd)
            countRunes -= 1
        }
        else do
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
        if selBeg == selEnd do
            return false

        checkSelection()

        let selB = getSelBeg()
        let selE = getSelEnd()
        if selB == text.length do
            return false

        countRunes -= cast(s32) Utf8.countRunes(text[selB..selE-1])
        text.remove(selB, selE - selB)
        discard checkContent()

        setSelection(selB, selB)
        return true
    }

    mtd getEditRect()->Math.Rectangle
    {
        var rect = getClientRectPadding()
        rect.moveRight(-rightMargin)
        return rect
    }

    mtd getTextMarginLeft()->f32
    {
        var rect = getEditRect()
        if icon.isValid() do
            rect.x += icon.sizeX + getThemeMetrics().iconText_Margin
        return rect.x
    }

    mtd getFormPadding()->Math.Vector4
    {
        #[Swag.Complete]
        switch form
        {
        case Square:
            return getThemeMetrics().editBox_SquarePadding
        case Round:
            return getThemeMetrics().editBox_RoundPadding
        case Flat:
            return getThemeMetrics().editBox_FlatPadding
        case Transparent:
            break
        }

        return {}
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
        var result: retval = undefined

        var clientRect = getEditRect()
        if icon.isValid() do
            clientRect.x += icon.sizeX + 4

        result = clientRect
        let fontR = getFont().regular
        if !text.isEmpty()
        {
            if editBoxFlags.has(.RightAlign)
            {
                let sizeX = Painter.measureString(text, fontR).width
                result.x = clientRect.right() - sizeX + posSelEnd - 2
            }
            else do
                result.x += posSelEnd
        }
        elif editBoxFlags.has(.RightAlign)
        {
            result.x = clientRect.right() - posSelEnd - 2
        }

        result.height = fontR.height
        result.y = clientRect.vertCenter() - result.height * 0.5

        result.width = 2
        return result
    }

    mtd getByteIndexAtPos(xmouse: f32)->u64
    {
        if !text.length do
            return 0

        var x = xmouse
        x += scrollPosX

        let fontR = getFont().regular

        // Could be better, of course...
        if editBoxFlags.has(.RightAlign)
        {
            var clientRect = getEditRect()
            let sizeX = Painter.measureString(text, fontR).width
            var lastWidth = 0'f32
            var diff = 0'f32
            loop idx: 1 until text.length
            {
                let str = @mkstring(text.buffer, idx)
                let met = Painter.measureString(str, fontR).width
                diff = (met - lastWidth) * 0.5
                if x <= clientRect.right() - sizeX + met
                {
                    if x < clientRect.right() - sizeX + lastWidth + diff do
                        return idx - 1
                    return idx
                }

                lastWidth = met
            }

            if x < clientRect.right() - sizeX + lastWidth + diff do
                return text.length-1

        }
        else
        {
            x -= getTextMarginLeft()
            var lastWidth = 0'f32
            var diff = 0'f32
            loop idx: 1 until text.length
            {
                let str = @mkstring(text.buffer, idx)
                let met = Painter.measureString(str, fontR).width
                diff = (met - lastWidth) * 0.5
                if x <= met
                {
                    if x < lastWidth + diff do
                        return idx - 1
                    return idx
                }

                lastWidth = met
            }

            if x < lastWidth + diff do
                return text.length-1
        }

        return text.length
    }

    mtd computeCaretPos()
    {
        if getFocus() != self do
            return

        let fontR = getFont().regular

        var str = @mkstring(text.buffer, selEnd)
        var met = Painter.measureString(str, fontR)
        posSelBeg, posSelEnd = met.width

        if selBeg != selEnd
        {
            str = @mkstring(text.buffer, selBeg)
            met = Painter.measureString(str, fontR)
            posSelBeg = met.width
        }
    }

    mtd moveCaretLeftStartWord(sel: u64)->u64
    {
        if !sel do
            return sel

        var s = sel
        s = moveCaretLeft(s)
        if s == sel do
            return sel

        let test = !Latin1.isSpace(text.buffer[s])
        loop
        {
            let ns = moveCaretLeft(s)
            if ns == s do
                break
            if Latin1.isSpace(text.buffer[ns]) == test do
                return s
            s = ns
        }

        return 0
    }

    mtd moveCaretRightEndWord(sel: u64)->u64
    {
        if sel == text.length do
            return sel

        let test = !Latin1.isSpace(text.buffer[sel])
        var s = sel
        while s != text.length
        {
            s = moveCaretRight(s)
            if Latin1.isSpace(text.buffer[s]) == test do
                return s
        }

        return text.length
    }

    mtd moveCaretLeft(sel: u64)->u64
    {
        if !sel do
            return sel
        let str = @mkstring(text.buffer, sel)
        let (?, eat) = Utf8.decodeLastRune(str)
        return sel - eat
    }

    mtd moveCaretRight(sel: u64)->u64
    {
        if sel == text.length do
            return sel
        let str = @mkstring(text.buffer + sel, text.length - sel)
        let (?, eat) = Utf8.decodeRune(str)
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
        sigChanged.call(self)
        computeCaretPos()
        invalidate()
    }
}

impl EditBox
{
    #[Swag.Discardable]
    func create(parent: *Wnd, content: string, position: Math.Rectangle = {}, id: WndId = null)->*EditBox
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0 do
            pos.height = parent.getThemeMetrics().editBox_Height
        if pos.width == 0 do
            pos.width = parent.getThemeMetrics().editBox_Width
        let res = Wnd.create'EditBox(parent, null, pos, id)

        res.setForm(EditBoxForm.Square)
        res.text = content
        res.focusStrategy = .MousePressed
        return res
    }

    // Delete selected text
    mtd deleteSelection()
    {
        if editBoxFlags.has(.ReadOnly) do
            return
        if deleteSelectionPrivate() do
            notifyChanged()
    }

    // Set current selection
    mtd setSelection(start, end: u64)
    {
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

    // Set the editbox content, but do not notify change
    mtd setTextSilent(str: string)
    {
        sigChanged.disable()
        setText(str)
        sigChanged.enable()
    }

    // Set the editbox content
    mtd setText(str: string)
    {
        if text == str do
            return
        text.clear()
        selBeg, selEnd = 0
        countRunes = 0
        insertString(str)
        notifyChanged()
    }

    // Get the associated text
    mtd getText()->string
    {
        return text.toString()
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
        self.form = #self.form

        #[Swag.Complete]
        switch self.form
        {
        case Square:
            setAllPaddings(getThemeMetrics().editBox_SquarePadding)
        case Round:
            setAllPaddings(getThemeMetrics().editBox_RoundPadding)
        case Flat:
            setAllPaddings(getThemeMetrics().editBox_FlatPadding)
        case Transparent:
            setAllPaddings(0)
        }

        invalidate()
    }
}