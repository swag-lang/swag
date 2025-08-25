#global public
using Core, Pixel

// Specifies the visual geometry of the EditBox.
enum EditBoxForm
{
    Square          // Square corners and standard border
    Round           // Rounded corners
    Flat            // Flat borderless style
    Transparent     // No background, transparent style
}

// Defines the type of input that the EditBox should accept and validate.
enum EditBoxInputMode
{
    String     // Free‑form UTF‑8 text
    S64        // Signed 64‑bit integer
    U64        // Unsigned 64‑bit integer
    F64        // 64‑bit floating‑point number
    X64        // Unsigned 64‑bit hexadecimal
}

// Possible outcomes when validating the EditBox content.
enum EditBoxCheckResult
{
    Incomplete         // Text is incomplete (e.g. "-" or "0x")
    Valid              // Content is valid
    InvalidDismiss     // Invalid but silently dismissed
    InvalidShow        // Invalid and must be visually highlighted
}

#[Swag.EnumFlags]
// Optional behaviour modifiers for the EditBox.
enum EditBoxFlags
{
    Zero          = 0     // No flags
    AutoLoseFocus         // Automatically lose focus after Enter or Escape
    ReadOnly              // Content cannot be edited
    RightAlign            // Text is right‑aligned
    ClearButton           // Show a clear ("x") button when content is not empty
}

struct EditBox
{
    using wnd:            Wnd                                      // Base window interface

    icon:                 Icon                                     // Optional icon shown on the left
    text:                 String                                   // Current UTF‑8 text content
    unit:                 String                                   // Suffix unit (e.g. "kg")
    form                  = EditBoxForm.Square                     // Visual form of the EditBox
    editBoxFlags          = EditBoxFlags.AutoLoseFocus             // Active behaviour flags
    inputMode             = EditBoxInputMode.String                // Current input mode
    maxLength:            s32                                      // Maximum number of runes (0 = unlimited)
    rightMargin:          f32                                      // Extra right margin inside the client rectangle

    sigChanged:           SigArray'func||(*EditBox)                // Emitted when the text changes
    sigLoseFocus:         SigArray'func||(*EditBox)                // Emitted when the EditBox loses focus
    sigEnterPressed:      SigArray'func||(*EditBox)                // Emitted when Enter is pressed
    sigEscapePressed:     SigArray'func||(*EditBox)                // Emitted when Escape is pressed
    sigCheckContent:      func||(*EditBox)->EditBoxCheckResult     // Optional custom validator

    private
    {
        originalText: String // Text captured when focus was gained

        // Numeric bounds used when inputMode is S64/U64/X64/F64.
        using minMax: union
        {
            minMaxS64:     { min: s64, max: s64 } = {Swag.S64.Min, Swag.S64.Max}      // Signed integer bounds
            minMaxU64:     { min: u64, max: u64 } = {0, Swag.U64.Max}                 // Unsigned integer bounds
            minMaxF64:     { min: f64, max: f64 } = {-Swag.F64.Max, Swag.F64.Max}     // Floating‑point bounds
        }

        aniBk:              BlendColor{speed: 4}     // Background colour animation
        aniBorder:          BlendColor{speed: 4}     // Border colour animation

        isInvalid:          bool                     // True if current content is invalid
        isHot:              bool                     // True while the mouse is hovering
        isDragging:         bool                     // True while the user is selecting text
        selBeg, selEnd:     u64                      // Selection start/end (byte indices)
        timerCaret:         *Timer                   // Blinking‑caret timer
        clearBtn:           *IconButton              // Optional clear button instance
        visibleCaret:       bool                     // True when the caret should be visible
        posSelBeg:          f32                      // X‑position of selection start in pixels
        posSelEnd:          f32                      // X‑position of selection end in pixels
        scrollPosX:         f32                      // Horizontal scroll offset
        countRunes:         s32                      // Number of runes currently in text
    }
}

impl IWnd for EditBox
{
    // Handles gaining and losing keyboard focus.
    mtd impl onFocusEvent(evt: *FocusEvent)
    {
        switch evt.kind
        {
        case SetFocus:
            me.originalText = me.text
            me.timerCaret   = me.surface.app.addTimer(500'ms, me)
            me.visibleCaret = true
            me.computeCaretPos()
            me.invalidate()

        case KillFocus:
            me.sigLoseFocus.call(me)
            me.surface.app.deleteTimer(me.timerCaret)
            me.timerCaret   = null
            me.visibleCaret = false
            me.invalidate()
        }
    }

    // Renders the EditBox, including background, border, text, selection and caret.
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let painter = evt.bc.painter

        let theme   = me.getTheme()
        let colors  = me.getThemeColors()
        let metrics = me.getThemeMetrics()
        let rects   = me.getThemeRects()

        var subRectBk:     *ThemeImageRect
        var subRectBorder: *ThemeImageRect
        var colorBk:       Color = undefined
        var colorBorder:   Color = undefined
        var colorText:     Color = undefined
        var colorUnit:     Color = undefined

        switch me.form
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
        case me.isInvalid:
            colorBorder = colors.edit_ErrorBorder
            colorBk     = colors.edit_ErrorBk
        case me.hasFocus():
            colorBorder = colors.edit_FocusBorder
            colorBk     = colors.edit_FocusBk
        case me.isHot:
            colorBorder = colors.edit_HotBorder
            colorBk     = colors.edit_HotBk
        default:
            colorBorder = colors.edit_Border
            colorBk     = colors.edit_Bk
        }

        me.aniBk.apply(me, colorBk)
        me.aniBorder.apply(me, colorBorder)

        var clientRect = me.getClientRect()
        if me.form != .Transparent:
            theme.drawSubRect(painter, clientRect, subRectBk, me.aniBk.cur())
        if me.form != .Transparent or me.isInvalid:
            theme.drawSubRect(painter, clientRect, subRectBorder, me.aniBorder.cur())

        clientRect = me.getEditRect()

        // Icon
        if me.icon.isValid()
        {
            me.icon.paint(bc.painter, clientRect.x, clientRect.vertCenter() - me.icon.sizeY * 0.5, colors.edit_Text)
            clientRect.x += me.icon.sizeX + metrics.iconText_Margin
            clientRect.width -= me.icon.sizeX + metrics.iconText_Margin
        }

        // Placeholder note
        if !me.name.isEmpty() and me.text.isEmpty()
        {
            var format: RichStringFormat
            format.palette[0]    = colors.edit_Note
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = me.getFont()
            Theme.drawIconText(bc.painter, clientRect, {}, &me.name, format, metrics.iconText_Margin)
        }

        // Compute scroll position
        let fontR     = me.getFont().regular
        var caretPos  = me.getCaretPos()
        let decalX    = Math.min(Painter.measureString("X", fontR).width * 10, clientRect.width * 0.5)
        let sizeX     = Painter.measureString(me.text, fontR).width
        let maxDecalX = sizeX - decalX

        if caretPos.x - clientRect.x < me.scrollPosX:
            me.scrollPosX = caretPos.x - clientRect.x - decalX
        elif caretPos.x - clientRect.x >= me.scrollPosX + clientRect.width:
            me.scrollPosX = Math.min(caretPos.x - clientRect.x - decalX, maxDecalX)

        if me.editBoxFlags.has(.RightAlign):
            me.scrollPosX = Math.min(me.scrollPosX, 0)
        else:
            me.scrollPosX = Math.max(me.scrollPosX, 0)

        painter.pushClippingRect(clientRect)
        painter.translateTransform(-me.scrollPosX, 0)

        // Selection
        if me.selBeg != me.selEnd and me.hasFocus()
        {
            var pos = clientRect
            if me.editBoxFlags.has(.RightAlign):
                pos.x = pos.right() - sizeX + me.posSelBeg
            else:
                pos.x += me.posSelBeg
            pos.x -= me.getTextMarginRight()
            pos.y      = clientRect.vertCenter() - fontR.height * 0.5
            pos.height = fontR.height
            pos.width  = me.posSelEnd - me.posSelBeg
            bc.painter.fillRect(pos, colors.edit_SelectedBk)
        }

        // Content
        let y = clientRect.vertCenter() - fontR.height * 0.5
        if me.editBoxFlags.has(.RightAlign)
        {
            clientRect.x = clientRect.right() - sizeX - me.getTextMarginRight()
        }

        bc.painter.drawString(clientRect.x, y, me.text, fontR, colorText)

        if !me.unit.isEmpty()
        {
            bc.painter.drawString(clientRect.x + sizeX + metrics.editBox_UnitMargin, y, me.unit, fontR, colorUnit)
        }

        // Caret
        if me.visibleCaret and me.hasFocus()
        {
            painter.fillRect(caretPos, Argb.White)
        }

        painter.popClippingRect()
        painter.translateTransform(me.scrollPosX, 0)
    }

    // Processes mouse interaction with the EditBox (clicks, drags, hover, wheel).
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        var pos = me.surfaceToLocal(evt.surfacePos)
        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false
            return

        case MouseDoubleClick:
            if evt.button != .Left:
                break
            let x = me.moveCaretLeftStartWord(me.selBeg)
            let y = me.moveCaretRightEndWord(me.selEnd)
            me.setSelection(x, y)

        case MousePressed:
            if evt.button != .Left:
                break
            let x = me.getByteIndexAtPos(pos.x)
            me.setSelection(x, x)
            me.forceVisibleCaret()
            evt.accepted  = true
            me.isDragging = true
            me.captureMouse()

        case MouseReleased:
            if evt.button != .Left:
                break
            if me.isDragging:
                me.releaseMouse()
            me.isDragging = false
            evt.accepted  = true

        case MouseMove:
            if !me.isDragging:
                break
            evt.accepted = true
            let x = me.getByteIndexAtPos(pos.x)
            me.setSelection(me.selBeg, x)

        case MouseEnter:
            me.isHot = true
            me.invalidate()

        case MouseLeave:
            me.isHot = false
            me.invalidate()
        }
    }

    // Blinking caret timer handler.
    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        me.visibleCaret = !me.visibleCaret
        var pos = me.getCaretPos()
        pos.inflate(1)
        me.invalidateRect(pos)
    }

    // Handles keyboard input (navigation, edition, clipboard shortcuts, etc.).
    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed
        {
            evt.accepted = me.keyPressed(evt.key, evt.modifiers)
            if evt.accepted:
                me.forceVisibleCaret()
        }
        elif evt.kind == .Rune and !me.editBoxFlags.has(.ReadOnly)
        {
            if evt.char < 32:
                evt.accepted = false
            else
            {
                me.forceVisibleCaret()
                me.deleteSelectionPrivate()
                me.insertRune(evt.char)
            }
        }
    }
}

internal impl EditBox
{
    // Shows or hides the clear button based on current text and flags.
    mtd showHideClearButon()
    {
        if !me.editBoxFlags.has(.ClearButton):
            return

        if me.text != null and @countof(me.text)
        {
            if !me.clearBtn
            {
                me.clearBtn           = IconButton.create(me, me.getTheme().getIcon24(ThemeIcons24.WndClose, 18), {0, 0, 24, 24})
                me.clearBtn.dockStyle = .Right
                me.clearBtn.setForm(IconButtonForm.Round)
                me.clearBtn.sigPressed += mtd||(btn) { me.setText(null); }
                me.applyLayout()
            }
        }
        elif me.clearBtn
        {
            me.clearBtn.destroy()
            me.clearBtn = null
            me.applyLayout()
        }
    }

    // Handles individual key presses as well as shortcut combinations.
    mtd keyPressed(key: Input.Key, mdf: Input.KeyModifiers)->bool
    {
        let ctrl  = mdf & .Control
        let shift = mdf & .Shift

        switch key
        {
        case A:
            if ctrl and !shift
            {
                me.setSelection(0, me.text.length)
                return true
            }

        case Insert:
            if ctrl and !shift and me.selBeg != me.selEnd
            {
                Clipboard.addString(@mkstring(me.text.buffer + me.getSelBeg(), me.getSelEnd() - me.getSelBeg()))
                return true
            }
            elif shift and !ctrl
            {
                var str = Clipboard.getString()
                me.insertString(str.toString())
                return true
            }

        case C:
            if ctrl and me.selBeg != me.selEnd
            {
                Clipboard.addString(@mkstring(me.text.buffer + me.getSelBeg(), me.getSelEnd() - me.getSelBeg()))
                return true
            }

        case X:
            if ctrl and me.selBeg != me.selEnd
            {
                Clipboard.addString(@mkstring(me.text.buffer + me.getSelBeg(), me.getSelEnd() - me.getSelBeg()))
                me.deleteSelection()
                return true
            }

        case V:
            if ctrl
            {
                var str = Clipboard.getString()
                me.insertString(str.toString())
                return true
            }

        case Return:
            me.sigEnterPressed.call(me)
            if me.editBoxFlags.has(.AutoLoseFocus):
                me.getTopView().setFocus()
            return true

        case Escape:
            me.sigEscapePressed.call(me)
            if me.editBoxFlags.has(.AutoLoseFocus):
                me.getTopView().setFocus()
            return true

        case Left:
            if ctrl:
                me.selEnd = me.moveCaretLeftStartWord(me.selEnd)
            else:
                me.selEnd = me.moveCaretLeft(me.selEnd)
            if shift:
                me.setSelection(me.selBeg, me.selEnd)
            else:
                me.setSelection(me.selEnd, me.selEnd)
            return true

        case Right:
            if ctrl:
                me.selEnd = me.moveCaretRightEndWord(me.selEnd)
            else:
                me.selEnd = me.moveCaretRight(me.selEnd)
            if shift:
                me.setSelection(me.selBeg, me.selEnd)
            else:
                me.setSelection(me.selEnd, me.selEnd)
            return true

        case Home:
            if shift:
                me.setSelection(me.selBeg, 0)
            else:
                me.setSelection(0, 0)
            return true

        case End:
            if shift:
                me.setSelection(me.selBeg, me.text.length)
            else:
                me.setSelection(me.text.length, me.text.length)
            return true

        case Back:
            if me.selBeg == me.selEnd
            {
                if !me.selBeg:
                    return true
                me.selBeg = me.moveCaretLeft(me.selBeg)
            }

            me.deleteSelection()
            return true

        case Delete:
            if me.selBeg == me.selEnd
            {
                me.selEnd = me.moveCaretRight(me.selBeg)
            }

            me.deleteSelection()
            return true
        }

        return false
    }

    // Inserts an arbitrary UTF‑8 string (after deleting the current selection).
    mtd insertString(str: #null string)
    {
        Utf8.visitRunes(|r| str)
        {
            insertRune(r)
        }
    }

    // Inserts a raw UTF‑8 slice without validation.
    mtd insertUtf8(s: string)
    {
        me.deleteSelectionPrivate()
        me.text.insert(me.selEnd, s)
        me.setSelection(me.selEnd + @countof(s), me.selEnd + @countof(s))
        me.countRunes += 1
    }

    // Validates the current content according to inputMode or custom callback.
    mtd checkContent()->EditBoxCheckResult
    {
        var res = EditBoxCheckResult.Valid
        if me.sigCheckContent
        {
            res = me.sigCheckContent(me)
        }
        else
        {
            var eat = 0'u32
            switch me.inputMode
            {
            case S64:
                var val = catch StrConv.parseS64(me.text)
                eat = val.eat
                if @err == null and eat == me.text.length and (val.value < me.minMaxS64.min or val.value > me.minMaxS64.max):
                    res = .InvalidShow

            case U64:
                var val = catch StrConv.parseU64(me.text)
                eat = val.eat
                if @err == null and eat == me.text.length and (val.value < me.minMaxU64.min or val.value > me.minMaxU64.max):
                    res = .InvalidShow

            case X64:
                var val = catch StrConv.parseX64(me.text)
                eat = val.eat
                if @err == null and eat == me.text.length and (val.value < me.minMaxU64.min or val.value > me.minMaxU64.max):
                    res = .InvalidShow

            case F64:
                var val = catch StrConv.parseF64(me.text)
                eat = val.eat
                if @err == null and eat == me.text.length and (val.value < me.minMaxF64.min or val.value > me.minMaxF64.max):
                    res = .InvalidShow
            }

            if res == .Valid
            {
                if @err == Errors.ParseIncompleteError:
                    res = .Incomplete
                elif @err == Errors.OverflowError:
                    res = .InvalidShow
                elif @err != null or eat != @countof(me.text):
                    res = .InvalidDismiss
            }
        }

        if res == .Valid:
            me.isInvalid = false
        elif res == .InvalidShow:
            me.isInvalid = true

        return res
    }

    // Inserts a single Unicode rune, performing maxLength and validation checks.
    mtd insertRune(r: rune)
    {
        if me.maxLength and me.countRunes == me.maxLength:
            return

        // Convert rune to UTF‑8.
        var utf8: [5] u8
        let cpt   = Utf8.fromUnicode(utf8, [r])
        if !cpt:
            return
        utf8[cpt] = 0

        // Determine whether we need to validate the content.
        let mustValidate = me.sigCheckContent != null or me.inputMode != .String
        if !mustValidate
        {
            me.insertUtf8(@mkstring(&utf8[0], cpt))
            me.notifyChanged()
            return
        }

        var saveText   = me.text
        let saveSelBeg = me.selBeg
        let saveSelEnd = me.selEnd
        me.insertUtf8(@mkstring(&utf8[0], cpt))

        let res = me.checkContent()
        if res == .InvalidDismiss
        {
            me.text = saveText
            me.setSelection(saveSelBeg, saveSelEnd)
            me.countRunes -= 1
        }
        else:
            me.notifyChanged()
    }

    #[Swag.Inline]
    {
        // Returns the first byte of the current selection.
        mtd getSelBeg() => me.selBeg <= me.selEnd ? me.selBeg : me.selEnd
        // Returns the last byte of the current selection.
        mtd getSelEnd() => me.selBeg <= me.selEnd ? me.selEnd : me.selBeg
    }

    #[Swag.Discardable]
    // Deletes the current selection without emitting change notifications.
    mtd deleteSelectionPrivate()->bool
    {
        if me.selBeg == me.selEnd:
            return false

        me.checkSelection()

        let selB = me.getSelBeg()
        let selE = me.getSelEnd()
        if selB == me.text.length:
            return false

        me.countRunes -= cast(s32) Utf8.countRunes(me.text[selB..selE - 1])
        me.text.remove(selB, selE - selB)
        discard me.checkContent()

        me.setSelection(selB, selB)
        return true
    }

    // Returns the client rectangle adjusted by padding and right margin.
    mtd getEditRect()->Math.Rectangle
    {
        var rect = me.getClientRectPadding()
        rect.moveRight(-me.rightMargin)
        return rect
    }

    // Returns the left padding for text rendering (accounts for icon).
    mtd getTextMarginLeft()->f32
    {
        var rect = me.getEditRect()
        if me.icon.isValid():
            rect.x += me.icon.sizeX + me.getThemeMetrics().iconText_Margin
        return rect.x
    }

    // Returns the right padding for text rendering (accounts for 'unit').
    mtd getTextMarginRight()->f32
    {
        if !me.editBoxFlags.has(.RightAlign):
            return 0
        if me.unit.isEmpty():
            return 0
        return Painter.measureString(me.unit, me.getFont().regular).width + me.getThemeMetrics().editBox_UnitMargin
    }

    // Returns the padding vector for the current visual form.
    mtd getFormPadding()->Math.Vector4
    {
        #[Swag.Complete]
        switch me.form
        {
        case Square:
            return me.getThemeMetrics().editBox_SquarePadding
        case Round:
            return me.getThemeMetrics().editBox_RoundPadding
        case Flat:
            return me.getThemeMetrics().editBox_FlatPadding
        case Transparent:
            break
        }

        return {}
    }

    // Forces the caret to become visible and restarts its blink timer.
    mtd forceVisibleCaret()
    {
        if me.timerCaret
        {
            me.visibleCaret = true
            me.timerCaret.restart()
        }
    }

    // Computes the caret rectangle in surface coordinates.
    mtd getCaretPos()->Math.Rectangle
    {
        var result: retval = undefined

        var clientRect = me.getEditRect()
        if me.icon.isValid():
            clientRect.x += me.icon.sizeX + 4

        result = clientRect
        let fontR = me.getFont().regular
        if !me.text.isEmpty()
        {
            if me.editBoxFlags.has(.RightAlign)
            {
                let sizeX = Painter.measureString(me.text, fontR).width
                result.x = clientRect.right() - sizeX + me.posSelEnd - 2
            }
            else:
                result.x += me.posSelEnd
        }
        elif me.editBoxFlags.has(.RightAlign)
        {
            result.x = clientRect.right() - me.posSelEnd - 2
        }

        result.x -= me.getTextMarginRight()
        result.height = fontR.height
        result.y      = clientRect.vertCenter() - result.height * 0.5

        result.width = 2
        return result
    }

    // Converts an X mouse position into a byte index inside "text".
    mtd getByteIndexAtPos(xmouse: f32)->u64
    {
        if !me.text.length:
            return 0

        var x = xmouse
        x += me.scrollPosX

        let fontR = me.getFont().regular

        // Naïve incremental search. Could be improved for performance.
        if me.editBoxFlags.has(.RightAlign)
        {
            var clientRect = me.getEditRect()
            clientRect.x -= me.getTextMarginRight()
            let sizeX     = Painter.measureString(me.text, fontR).width
            var lastWidth = 0'f32
            var diff      = 0'f32
            for idx in 1 until me.text.length
            {
                let str = @mkstring(me.text.buffer, idx)
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
                return me.text.length - 1
        }
        else
        {
            x -= me.getTextMarginLeft()
            var lastWidth = 0'f32
            var diff      = 0'f32
            for idx in 1 until me.text.length
            {
                let str = @mkstring(me.text.buffer, idx)
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
                return me.text.length - 1
        }

        return me.text.length
    }

    // Updates cached caret and selection pixel positions.
    mtd computeCaretPos()
    {
        if me.getFocus() != me:
            return

        let fontR = me.getFont().regular

        var str = @mkstring(me.text.buffer, me.selEnd)
        var met = Painter.measureString(str, fontR)
        me.posSelBeg, me.posSelEnd = met.width

        if me.selBeg != me.selEnd
        {
            str          = @mkstring(me.text.buffer, me.selBeg)
            met          = Painter.measureString(str, fontR)
            me.posSelBeg = met.width
        }
    }

    // Moves caret left to the start of the previous word.
    mtd moveCaretLeftStartWord(sel: u64)->u64
    {
        if !sel:
            return sel

        var s = sel
        s = me.moveCaretLeft(s)
        if s == sel:
            return sel

        let test = !Latin1.isSpace(me.text.buffer[s])
        for
        {
            let ns = me.moveCaretLeft(s)
            if ns == s:
                break
            if Latin1.isSpace(me.text.buffer[ns]) == test:
                return s
            s = ns
        }

        return 0
    }

    // Moves caret right to the end of the current/next word.
    mtd moveCaretRightEndWord(sel: u64)->u64
    {
        if sel == me.text.length:
            return sel

        let test = !Latin1.isSpace(me.text.buffer[sel])
        var s    = sel
        while s != me.text.length
        {
            s = me.moveCaretRight(s)
            if Latin1.isSpace(me.text.buffer[s]) == test:
                return s
        }

        return me.text.length
    }

    // Moves caret left by one rune.
    mtd moveCaretLeft(sel: u64)->u64
    {
        if !sel:
            return sel
        let str = @mkstring(me.text.buffer, sel)
        let (?, eat) = Utf8.decodeLastRune(str)
        return sel - eat
    }

    // Moves caret right by one rune.
    mtd moveCaretRight(sel: u64)->u64
    {
        if sel == me.text.length:
            return sel
        let str = @mkstring(me.text.buffer + sel, me.text.length - sel)
        let (?, eat) = Utf8.decodeRune(str)
        return sel + eat
    }

    // Clamps selection indices and triggers repaint.
    mtd checkSelection()
    {
        me.selBeg = Math.min(me.selBeg, me.text.length)
        me.selEnd = Math.min(me.selEnd, me.text.length)
        me.invalidate()
    }

    // Emits sigChanged, recomputes caret, updates clear button and repaints.
    mtd notifyChanged()
    {
        me.sigChanged.call(me)
        me.computeCaretPos()
        me.showHideClearButon()
        me.invalidate()
    }
}

impl EditBox
{
    // Factory method that creates a new EditBox instance.
    #[Swag.Discardable]
    func create(parent: *Wnd, content: #null string, position: Math.Rectangle = {}, id: WndId = null)->*EditBox
    {
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

    // Deletes the selected text.
    mtd deleteSelection()
    {
        if me.editBoxFlags.has(.ReadOnly):
            return
        if me.deleteSelectionPrivate():
            me.notifyChanged()
    }

    // Sets the current selection.
    mtd setSelection(start, end: u64)
    {
        me.selBeg = start
        me.selEnd = end
        me.checkSelection()
        me.computeCaretPos()
    }

    // Selects the whole content.
    mtd selectAll()
    {
        me.setSelection(0, me.text.length)
    }

    // Defines the placeholder (note) text.
    mtd setNote(note: string)
    {
        me.name = note
        me.invalidate()
    }

    // Assigns the EditBox content without emitting change notifications.
    mtd setTextSilent(str: string)
    {
        me.sigChanged.disable()
        me.setText(str)
        me.sigChanged.enable()
    }

    // Assigns the EditBox content and emits change notifications.
    mtd setText(str: #null string)
    {
        if me.text == str:
            return
        me.text.clear()
        me.selBeg, me.selEnd = 0
        me.countRunes = 0
        me.insertString(str)
        me.showHideClearButon()
        me.notifyChanged()
    }

    // Returns the current text as a String.
    mtd getText()->string
    {
        return me.text.toString()
    }

    // Restores the text captured on focus gain.
    mtd restoreText()
    {
        me.setText(me.originalText)
    }

    // Configures the EditBox to accept signed 64‑bit integers.
    mtd setInputS64(min = Swag.S64.Min, max = Swag.S64.Max)
    {
        me.inputMode     = .S64
        me.minMaxS64.min = min
        me.minMaxS64.max = max
    }

    // Configures the EditBox to accept unsigned 64‑bit integers.
    mtd setInputU64(min = 0'u64, max = Swag.U64.Max)
    {
        me.inputMode     = .U64
        me.minMaxU64.min = min
        me.minMaxU64.max = max
    }

    // Configures the EditBox to accept hexadecimal unsigned 64‑bit integers.
    mtd setInputX64(min = 0'u64, max = Swag.U64.Max)
    {
        me.inputMode     = .X64
        me.minMaxU64.min = min
        me.minMaxU64.max = max
    }

    // Configures the EditBox to accept 64‑bit floating‑point numbers.
    mtd setInputF64(min = -Swag.F64.Max, max = Swag.F64.Max)
    {
        me.inputMode     = .F64
        me.minMaxF64.min = min
        me.minMaxF64.max = max
    }

    // Sets the visual form of the EditBox.
    mtd setForm(form: EditBoxForm)
    {
        me.form = #me.form

        #[Swag.Complete]
        switch me.form
        {
        case Square:
            me.setAllPaddings(me.getThemeMetrics().editBox_SquarePadding)
        case Round:
            me.setAllPaddings(me.getThemeMetrics().editBox_RoundPadding)
        case Flat:
            me.setAllPaddings(me.getThemeMetrics().editBox_FlatPadding)
        case Transparent:
            me.setAllPaddings(0)
        }

        me.invalidate()
    }

    // Returns true when the current content is valid.
    mtd isValid()->bool => !me.isInvalid
}
