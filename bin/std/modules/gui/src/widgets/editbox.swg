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
    X64
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
    Zero          = 0
    AutoLoseFocus
    ReadOnly
    RightAlign
    ClearButton
}

struct EditBox
{
    using wnd:            Wnd

    icon:                 Icon
    text:                 String
    originalText:         String
    unit:                 String
    form                  = EditBoxForm.Square
    editBoxFlags          = EditBoxFlags.AutoLoseFocus
    inputMode             = EditBoxInputMode.String
    maxLength:            s32
    rightMargin:          f32

    sigChanged:           SigArray'func||(*Self)
    sigLoseFocus:         SigArray'func||(*Self)
    sigEnterPressed:      SigArray'func||(*Self)
    sigEscapePressed:     SigArray'func||(*Self)
    sigCheckContent:      func||(*Self)->EditBoxCheckResult
    using minMax: union
    {
        minMaxS64:     { min: s64, max: s64 } = {Swag.S64.Min, Swag.S64.Max}
        minMaxU64:     { min: u64, max: u64 } = {0, Swag.U64.Max}
        minMaxF64:     { min: f64, max: f64 } = {-Swag.F64.Max, Swag.F64.Max}
    }

    aniBk:              BlendColor{speed: 4}
    aniBorder:          BlendColor{speed: 4}

    isInvalid:          bool
    isHot:              bool
    isDragging:         bool
    selBeg, selEnd:     u64
    timerCaret:         *Timer
    clearBtn:           *IconButton
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
            originalText = text
            timerCaret   = surface.app.addTimer(500'ms, self)
            visibleCaret = true
            computeCaretPos()
            invalidate()

        case KillFocus:
            sigLoseFocus.call(self)
            surface.app.deleteTimer(timerCaret)
            timerCaret   = null
            visibleCaret = false
            invalidate()
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let painter = evt.bc.painter

        let theme   = getTheme()
        let colors  = getThemeColors()
        let metrics = getThemeMetrics()
        let rects   = getThemeRects()

        var subRectBk:     *ThemeImageRect
        var subRectBorder: *ThemeImageRect
        var colorBk:       Color = undefined
        var colorBorder:   Color = undefined
        var colorText:     Color = undefined
        var colorUnit:     Color = undefined

        switch form
        {
        case Square, Transparent:
            subRectBk     = &rects.edit_SquareBk
            subRectBorder = &rects.edit_SquareBorder
        case Round:
            subRectBk     = &rects.edit_RoundBk
            subRectBorder = &rects.edit_RoundBorder
        case Flat:
            subRectBk     = &rects.edit_FlatBk
            subRectBorder = &rects.edit_FlatBorder
        }

        colorText = colors.edit_Text
        colorUnit = colors.edit_TextUnit

        switch
        {
        case bc.isDisabled:
            colorBorder = colors.edit_DisabledBorder
            colorBk     = colors.edit_DisabledBk
            colorText   = colors.edit_DisabledText
        case isInvalid:
            colorBorder = colors.edit_ErrorBorder
            colorBk     = colors.edit_ErrorBk
        case hasFocus():
            colorBorder = colors.edit_FocusBorder
            colorBk     = colors.edit_FocusBk
        case isHot:
            colorBorder = colors.edit_HotBorder
            colorBk     = colors.edit_HotBk
        default:
            colorBorder = colors.edit_Border
            colorBk     = colors.edit_Bk
        }

        aniBk.apply(self, colorBk)
        aniBorder.apply(self, colorBorder)

        var clientRect = getClientRect()
        if form != .Transparent:
            theme.drawSubRect(painter, clientRect, subRectBk, aniBk.cur())
        if form != .Transparent or isInvalid:
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
            format.palette[0]    = colors.edit_Note
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = getFont()
            Theme.drawIconText(bc.painter, clientRect, {}, &name, format, metrics.iconText_Margin)
        }

        // Compute scroll position
        let fontR     = getFont().regular
        var caretPos  = getCaretPos()
        let decalX    = Math.min(Painter.measureString("X", fontR).width * 10, clientRect.width * 0.5)
        let sizeX     = Painter.measureString(text, fontR).width
        let maxDecalX = sizeX - decalX

        if caretPos.x - clientRect.x < scrollPosX:
            scrollPosX = caretPos.x - clientRect.x - decalX
        elif caretPos.x - clientRect.x >= scrollPosX + clientRect.width:
            scrollPosX = Math.min(caretPos.x - clientRect.x - decalX, maxDecalX)

        if editBoxFlags.has(.RightAlign):
            scrollPosX = Math.min(scrollPosX, 0)
        else:
            scrollPosX = Math.max(scrollPosX, 0)

        painter.pushClippingRect(clientRect)
        painter.translateTransform(-scrollPosX, 0)

        // Selection
        if selBeg != selEnd and hasFocus()
        {
            var pos = clientRect
            if editBoxFlags.has(.RightAlign):
                pos.x = pos.right() - sizeX + posSelBeg
            else:
                pos.x += posSelBeg
            pos.y      = clientRect.vertCenter() - fontR.height * 0.5
            pos.height = fontR.height
            pos.width  = posSelEnd - posSelBeg
            bc.painter.fillRect(pos, colors.edit_SelectedBk)
        }

        // Content
        let y = clientRect.vertCenter() - fontR.height * 0.5
        if editBoxFlags.has(.RightAlign):
            clientRect.x = clientRect.right() - sizeX
        bc.painter.drawString(clientRect.x, y, text, fontR, colorText)

        if !unit.isEmpty()
        {
            bc.painter.drawString(clientRect.x + sizeX + metrics.editBox_UnitMargin, y, unit, fontR, colorUnit)
        }

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
            if evt.button != .Left:
                break
            let x = moveCaretLeftStartWord(selBeg)
            let y = moveCaretRightEndWord(selEnd)
            setSelection(x, y)

        case MousePressed:
            if evt.button != .Left:
                break
            let x = getByteIndexAtPos(pos.x)
            setSelection(x, x)
            forceVisibleCaret()
            evt.accepted = true
            isDragging   = true
            captureMouse()

        case MouseReleased:
            if evt.button != .Left:
                break
            if isDragging:
                releaseMouse()
            isDragging   = false
            evt.accepted = true

        case MouseMove:
            if !isDragging:
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
            if evt.accepted:
                forceVisibleCaret()
        }
        elif evt.kind == .Rune and !editBoxFlags.has(.ReadOnly)
        {
            if evt.char < 32:
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
    mtd showHideClearButon()
    {
        if !editBoxFlags.has(.ClearButton):
            return

        if text != null and @countof(text)
        {
            if !clearBtn
            {
                clearBtn           = IconButton.create(self, getTheme().getIcon24(ThemeIcons24.WndClose, 18), {0, 0, 24, 24})
                clearBtn.dockStyle = .Right
                clearBtn.setForm(IconButtonForm.Round)
                clearBtn.sigPressed += func|self|(btn) { self.setText(null); }
                applyLayout()
            }
        }
        elif clearBtn
        {
            clearBtn.destroy()
            clearBtn = null
            applyLayout()
        }
    }

    mtd keyPressed(key: Input.Key, mdf: Input.KeyModifiers)->bool
    {
        let ctrl  = mdf & .Control
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
            if editBoxFlags.has(.AutoLoseFocus):
                getTopView().setFocus()
            return true

        case Escape:
            sigEscapePressed.call(self)
            if editBoxFlags.has(.AutoLoseFocus):
                getTopView().setFocus()
            return true

        case Left:
            if ctrl:
                selEnd = moveCaretLeftStartWord(selEnd)
            else:
                selEnd = moveCaretLeft(selEnd)
            if shift:
                setSelection(selBeg, selEnd)
            else:
                setSelection(selEnd, selEnd)
            return true

        case Right:
            if ctrl:
                selEnd = moveCaretRightEndWord(selEnd)
            else:
                selEnd = moveCaretRight(selEnd)
            if shift:
                setSelection(selBeg, selEnd)
            else:
                setSelection(selEnd, selEnd)
            return true

        case Home:
            if shift:
                setSelection(selBeg, 0)
            else:
                setSelection(0, 0)
            return true

        case End:
            if shift:
                setSelection(selBeg, text.length)
            else:
                setSelection(text.length, text.length)
            return true

        case Back:
            if selBeg == selEnd
            {
                if !selBeg:
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

    mtd insertString(str: #null string)
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
                if @err == null and eat == text.length and (val.value < minMaxS64.min or val.value > minMaxS64.max):
                    res = .InvalidShow

            case U64:
                var val = catch StrConv.parseU64(text)
                eat = val.eat
                if @err == null and eat == text.length and (val.value < minMaxU64.min or val.value > minMaxU64.max):
                    res = .InvalidShow

            case X64:
                var val = catch StrConv.parseX64(text)
                eat = val.eat
                if @err == null and eat == text.length and (val.value < minMaxU64.min or val.value > minMaxU64.max):
                    res = .InvalidShow

            case F64:
                var val = catch StrConv.parseF64(text)
                eat = val.eat
                if @err == null and eat == text.length and (val.value < minMaxF64.min or val.value > minMaxF64.max):
                    res = .InvalidShow
            }

            if res == .Valid
            {
                if @err == Errors.ParseIncompleteError:
                    res = .Incomplete
                elif @err == Errors.OverflowError:
                    res = .InvalidShow
                elif @err != null or eat != @countof(text):
                    res = .InvalidDismiss
            }
        }

        if res == .Valid:
            isInvalid = false
        elif res == .InvalidShow:
            isInvalid = true

        return res
    }

    mtd insertRune(r: rune)
    {
        if maxLength and countRunes == maxLength:
            return

        // Convert rune to utf8
        var utf8: [5] u8
        let cpt   = Utf8.fromUnicode(utf8, [r])
        if !cpt:
            return
        utf8[cpt] = 0

        // Should we check the content ?
        let mustValidate = sigCheckContent != null or inputMode != .String
        if !mustValidate
        {
            insertUtf8(@mkstring(&utf8[0], cpt))
            notifyChanged()
            return
        }

        var saveText   = text
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
        else:
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
        if selBeg == selEnd:
            return false

        checkSelection()

        let selB = getSelBeg()
        let selE = getSelEnd()
        if selB == text.length:
            return false

        countRunes -= cast(s32) Utf8.countRunes(text[selB..selE - 1])
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
        if icon.isValid():
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
        if icon.isValid():
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
            else:
                result.x += posSelEnd
        }
        elif editBoxFlags.has(.RightAlign)
        {
            result.x = clientRect.right() - posSelEnd - 2
        }

        result.height = fontR.height
        result.y      = clientRect.vertCenter() - result.height * 0.5

        result.width = 2
        return result
    }

    mtd getByteIndexAtPos(xmouse: f32)->u64
    {
        if !text.length:
            return 0

        var x = xmouse
        x += scrollPosX

        let fontR = getFont().regular

        // Could be better, of course...
        if editBoxFlags.has(.RightAlign)
        {
            var clientRect = getEditRect()
            let sizeX      = Painter.measureString(text, fontR).width
            var lastWidth  = 0'f32
            var diff       = 0'f32
            for idx in 1 until text.length
            {
                let str = @mkstring(text.buffer, idx)
                let met = Painter.measureString(str, fontR).width
                diff = (met - lastWidth) * 0.5
                if x <= clientRect.right() - sizeX + met
                {
                    if x < clientRect.right() - sizeX + lastWidth + diff:
                        return idx - 1
                    return idx
                }

                lastWidth = met
            }

            if x < clientRect.right() - sizeX + lastWidth + diff:
                return text.length - 1
        }
        else
        {
            x -= getTextMarginLeft()
            var lastWidth = 0'f32
            var diff      = 0'f32
            for idx in 1 until text.length
            {
                let str = @mkstring(text.buffer, idx)
                let met = Painter.measureString(str, fontR).width
                diff = (met - lastWidth) * 0.5
                if x <= met
                {
                    if x < lastWidth + diff:
                        return idx - 1
                    return idx
                }

                lastWidth = met
            }

            if x < lastWidth + diff:
                return text.length - 1
        }

        return text.length
    }

    mtd computeCaretPos()
    {
        if getFocus() != self:
            return

        let fontR = getFont().regular

        var str = @mkstring(text.buffer, selEnd)
        var met = Painter.measureString(str, fontR)
        posSelBeg, posSelEnd = met.width

        if selBeg != selEnd
        {
            str       = @mkstring(text.buffer, selBeg)
            met       = Painter.measureString(str, fontR)
            posSelBeg = met.width
        }
    }

    mtd moveCaretLeftStartWord(sel: u64)->u64
    {
        if !sel:
            return sel

        var s = sel
        s = moveCaretLeft(s)
        if s == sel:
            return sel

        let test = !Latin1.isSpace(text.buffer[s])
        for
        {
            let ns = moveCaretLeft(s)
            if ns == s:
                break
            if Latin1.isSpace(text.buffer[ns]) == test:
                return s
            s = ns
        }

        return 0
    }

    mtd moveCaretRightEndWord(sel: u64)->u64
    {
        if sel == text.length:
            return sel

        let test = !Latin1.isSpace(text.buffer[sel])
        var s    = sel
        while s != text.length
        {
            s = moveCaretRight(s)
            if Latin1.isSpace(text.buffer[s]) == test:
                return s
        }

        return text.length
    }

    mtd moveCaretLeft(sel: u64)->u64
    {
        if !sel:
            return sel
        let str = @mkstring(text.buffer, sel)
        let (?, eat) = Utf8.decodeLastRune(str)
        return sel - eat
    }

    mtd moveCaretRight(sel: u64)->u64
    {
        if sel == text.length:
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
        showHideClearButon()
        invalidate()
    }
}

impl EditBox
{
    #[Swag.Discardable]
    func create(parent: *Wnd, content: #null string, position: Math.Rectangle = {}, id: WndId = null)->*EditBox
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0:
            pos.height = parent.getThemeMetrics().editBox_Height
        if pos.width == 0:
            pos.width = parent.getThemeMetrics().editBox_Width
        let res = Wnd.create'EditBox(parent, null, pos, id)

        res.setForm(.Square)
        res.text          = content
        res.focusStrategy = .MousePressed
        return res
    }

    // Delete selected text
    mtd deleteSelection()
    {
        if editBoxFlags.has(.ReadOnly):
            return
        if deleteSelectionPrivate():
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

    // Set the editbox content, but: not notify change
    mtd setTextSilent(str: string)
    {
        sigChanged.disable()
        setText(str)
        sigChanged.enable()
    }

    // Set the editbox content
    mtd setText(str: #null string)
    {
        if text == str:
            return
        text.clear()
        selBeg, selEnd = 0
        countRunes = 0
        insertString(str)
        showHideClearButon()
        notifyChanged()
    }

    // Get the associated text
    mtd getText()->string
    {
        return text.toString()
    }

    // Restore the original text.
    // The original text is the one stored when the editbox has gained the focus.
    mtd restoreText()
    {
        setText(originalText)
    }

    // Editbox will edit signed integers
    mtd setInputS64(min = Swag.S64.Min, max = Swag.S64.Max)
    {
        inputMode     = .S64
        minMaxS64.min = min
        minMaxS64.max = max
    }

    // Editbox will edit unsigned integers
    mtd setInputU64(min = 0'u64, max = Swag.U64.Max)
    {
        inputMode     = .U64
        minMaxU64.min = min
        minMaxU64.max = max
    }

    // Editbox will edit unsigned integers in hexadecimal form
    mtd setInputX64(min = 0'u64, max = Swag.U64.Max)
    {
        inputMode     = .X64
        minMaxU64.min = min
        minMaxU64.max = max
    }

    // Editbox will edit floating points
    mtd setInputF64(min = -Swag.F64.Max, max = Swag.F64.Max)
    {
        inputMode     = .F64
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

    // Returns true if the value is valid.
    // Depends if the editbox has some max and min registered values.
    mtd isValid()->bool => !isInvalid
}
