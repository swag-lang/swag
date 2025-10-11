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
            .originalText = .text
            .timerCaret   = .surface.app.addTimer(500'ms, me)
            .visibleCaret = true
            .computeCaretPos()
            .invalidate()

        case KillFocus:
            .sigLoseFocus.call(me)
            .surface.app.deleteTimer(.timerCaret)
            .timerCaret   = null
            .visibleCaret = false
            .invalidate()
        }
    }

    // Renders the EditBox, including background, border, text, selection and caret.
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let painter = evt.bc.painter

        let theme   = .getTheme()
        let colors  = .getThemeColors()
        let metrics = .getThemeMetrics()
        let rects   = .getThemeRects()

        var subRectBk:     *ThemeImageRect
        var subRectBorder: *ThemeImageRect
        var colorBk:       Color = undefined
        var colorBorder:   Color = undefined
        var colorText:     Color = undefined
        var colorUnit:     Color = undefined

        switch .form
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
        case .isInvalid:
            colorBorder = colors.edit_ErrorBorder
            colorBk     = colors.edit_ErrorBk
        case .hasFocus():
            colorBorder = colors.edit_FocusBorder
            colorBk     = colors.edit_FocusBk
        case .isHot:
            colorBorder = colors.edit_HotBorder
            colorBk     = colors.edit_HotBk
        default:
            colorBorder = colors.edit_Border
            colorBk     = colors.edit_Bk
        }

        .aniBk.apply(me, colorBk)
        .aniBorder.apply(me, colorBorder)

        var clientRect = .getClientRect()
        if .form != .Transparent do
            theme.drawSubRect(painter, clientRect, subRectBk, .aniBk.cur())
        if .form != .Transparent or .isInvalid do
            theme.drawSubRect(painter, clientRect, subRectBorder, .aniBorder.cur())

        clientRect = .getEditRect()

        // Icon
        if .icon.isValid()
        {
            .icon.paint(bc.painter, clientRect.x, clientRect.vertCenter() - .icon.sizeY * 0.5, colors.edit_Text)
            clientRect.x += .icon.sizeX + metrics.iconText_Margin
            clientRect.width -= .icon.sizeX + metrics.iconText_Margin
        }

        // Placeholder note
        if !.name.isEmpty() and .text.isEmpty()
        {
            var format: RichStringFormat
            format.palette[0]    = colors.edit_Note
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = .getFont()
            Theme.drawIconText(bc.painter, clientRect, {}, &.name, format, metrics.iconText_Margin)
        }

        // Compute scroll position
        let fontR     = .getFont().regular
        let caretPos  = .getCaretPos()
        let decalX    = Math.min(Painter.measureString("X", fontR).width * 10, clientRect.width * 0.5)
        let sizeX     = Painter.measureString(.text, fontR).width
        let maxDecalX = sizeX - decalX

        if caretPos.x - clientRect.x < .scrollPosX do
            .scrollPosX = caretPos.x - clientRect.x - decalX
        elif caretPos.x - clientRect.x >= .scrollPosX + clientRect.width do
            .scrollPosX = Math.min(caretPos.x - clientRect.x - decalX, maxDecalX)

        if .editBoxFlags.has(.RightAlign) do
            .scrollPosX = Math.min(.scrollPosX, 0)
        else do
            .scrollPosX = Math.max(.scrollPosX, 0)

        painter.pushClippingRect(clientRect)
        painter.translateTransform(-.scrollPosX, 0)

        // Selection
        if .selBeg != .selEnd and .hasFocus()
        {
            var pos = clientRect
            if .editBoxFlags.has(.RightAlign) do
                pos.x = pos.right() - sizeX + .posSelBeg
            else do
                pos.x += .posSelBeg
            pos.x -= .getTextMarginRight()
            pos.y      = clientRect.vertCenter() - fontR.height * 0.5
            pos.height = fontR.height
            pos.width  = .posSelEnd - .posSelBeg
            bc.painter.fillRect(pos, colors.edit_SelectedBk)
        }

        // Content
        let y = clientRect.vertCenter() - fontR.height * 0.5
        if .editBoxFlags.has(.RightAlign)
        {
            clientRect.x = clientRect.right() - sizeX - .getTextMarginRight()
        }

        bc.painter.drawString(clientRect.x, y, .text, fontR, colorText)

        if !.unit.isEmpty()
        {
            bc.painter.drawString(clientRect.x + sizeX + metrics.editBox_UnitMargin, y, .unit, fontR, colorUnit)
        }

        // Caret
        if .visibleCaret and .hasFocus()
        {
            painter.fillRect(caretPos, colors.edit_Text)
        }

        painter.popClippingRect()
        painter.translateTransform(.scrollPosX, 0)
    }

    // Processes mouse interaction with the EditBox (clicks, drags, hover, wheel).
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        let pos = .surfaceToLocal(evt.surfacePos)
        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false
            return

        case MouseDoubleClick:
            if evt.button != .Left do
                break
            let x = .moveCaretLeftStartWord(.selBeg)
            let y = .moveCaretRightEndWord(.selEnd)
            .setSelection(x, y)

        case MousePressed:
            if evt.button != .Left do
                break
            let x = .getByteIndexAtPos(pos.x)
            .setSelection(x, x)
            .forceVisibleCaret()
            evt.accepted = true
            .isDragging  = true
            .captureMouse()

        case MouseReleased:
            if evt.button != .Left do
                break
            if .isDragging do
                .releaseMouse()
            .isDragging  = false
            evt.accepted = true

        case MouseMove:
            if !.isDragging do
                break
            evt.accepted = true
            let x = .getByteIndexAtPos(pos.x)
            .setSelection(.selBeg, x)

        case MouseEnter:
            .isHot = true
            .invalidate()

        case MouseLeave:
            .isHot = false
            .invalidate()
        }
    }

    // Blinking caret timer handler.
    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        .visibleCaret = !.visibleCaret
        var pos = .getCaretPos()
        pos.inflate(1)
        .invalidateRect(pos)
    }

    // Handles keyboard input (navigation, edition, clipboard shortcuts, etc.).
    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed
        {
            evt.accepted = .keyPressed(evt.key, evt.modifiers)
            if evt.accepted do
                .forceVisibleCaret()
        }
        elif evt.kind == .Rune and !.editBoxFlags.has(.ReadOnly)
        {
            if evt.char < 32 do
                evt.accepted = false
            else
            {
                .forceVisibleCaret()
                .deleteSelectionPrivate()
                .insertRune(evt.char)
            }
        }
    }
}

internal impl EditBox
{
    // Shows or hides the clear button based on current text and flags.
    mtd showHideClearButon()
    {
        if !.editBoxFlags.has(.ClearButton) do
            return

        if .text != null and @countof(.text)
        {
            if !.clearBtn
            {
                .clearBtn           = IconButton.create(me, .getTheme().getIcon24(ThemeIcons24.WndClose, 18), {0, 0, 24, 24})
                .clearBtn.dockStyle = .Right
                .clearBtn.setForm(IconButtonForm.Round)
                .clearBtn.sigPressed += mtd||(btn) { .setText(null); }
                .applyLayout()
            }
        }
        elif .clearBtn
        {
            .clearBtn.destroy()
            .clearBtn = null
            .applyLayout()
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
                .setSelection(0, .text.length)
                return true
            }

        case Insert:
            if ctrl and !shift and .selBeg != .selEnd
            {
                Clipboard.addString(@mkstring(.text.buffer + .getSelBeg(), .getSelEnd() - .getSelBeg()))
                return true
            }
            elif shift and !ctrl
            {
                let str = Clipboard.getString()
                .insertString(str.toString())
                return true
            }

        case C:
            if ctrl and .selBeg != .selEnd
            {
                Clipboard.addString(@mkstring(.text.buffer + .getSelBeg(), .getSelEnd() - .getSelBeg()))
                return true
            }

        case X:
            if ctrl and .selBeg != .selEnd
            {
                Clipboard.addString(@mkstring(.text.buffer + .getSelBeg(), .getSelEnd() - .getSelBeg()))
                .deleteSelection()
                return true
            }

        case V:
            if ctrl
            {
                let str = Clipboard.getString()
                .insertString(str.toString())
                return true
            }

        case Return:
            .sigEnterPressed.call(me)
            if .editBoxFlags.has(.AutoLoseFocus) do
                .getTopView().setFocus()
            return true

        case Escape:
            .sigEscapePressed.call(me)
            if .editBoxFlags.has(.AutoLoseFocus) do
                .getTopView().setFocus()
            return true

        case Left:
            if ctrl do
                .selEnd = .moveCaretLeftStartWord(.selEnd)
            else do
                .selEnd = .moveCaretLeft(.selEnd)
            if shift do
                .setSelection(.selBeg, .selEnd)
            else do
                .setSelection(.selEnd, .selEnd)
            return true

        case Right:
            if ctrl do
                .selEnd = .moveCaretRightEndWord(.selEnd)
            else do
                .selEnd = .moveCaretRight(.selEnd)
            if shift do
                .setSelection(.selBeg, .selEnd)
            else do
                .setSelection(.selEnd, .selEnd)
            return true

        case Home:
            if shift do
                .setSelection(.selBeg, 0)
            else do
                .setSelection(0, 0)
            return true

        case End:
            if shift do
                .setSelection(.selBeg, .text.length)
            else do
                .setSelection(.text.length, .text.length)
            return true

        case Back:
            if .selBeg == .selEnd
            {
                if !.selBeg do
                    return true
                .selBeg = .moveCaretLeft(.selBeg)
            }

            .deleteSelection()
            return true

        case Delete:
            if .selBeg == .selEnd
            {
                .selEnd = .moveCaretRight(.selBeg)
            }

            .deleteSelection()
            return true
        }

        return false
    }

    // Inserts an arbitrary UTF‑8 string (after deleting the current selection).
    mtd insertString(str: #null string)
    {
        Utf8.visitRunes(|r| str)
        {
            .insertRune(r)
        }
    }

    // Inserts a raw UTF‑8 slice without validation.
    mtd insertUtf8(s: string)
    {
        .deleteSelectionPrivate()
        .text.insert(.selEnd, s)
        .setSelection(.selEnd + @countof(s), .selEnd + @countof(s))
        .countRunes += 1
    }

    // Validates the current content according to inputMode or custom callback.
    mtd checkContent()->EditBoxCheckResult
    {
        var res = EditBoxCheckResult.Valid
        if .sigCheckContent
        {
            res = .sigCheckContent(me)
        }
        else
        {
            var eat = 0'u32
            switch .inputMode
            {
            case S64:
                let val = catch StrConv.parseS64(.text)
                eat = val.eat
                if @err == null and eat == .text.length and (val.value < .minMaxS64.min or val.value > .minMaxS64.max) do
                    res = .InvalidShow

            case U64:
                let val = catch StrConv.parseU64(.text)
                eat = val.eat
                if @err == null and eat == .text.length and (val.value < .minMaxU64.min or val.value > .minMaxU64.max) do
                    res = .InvalidShow

            case X64:
                let val = catch StrConv.parseX64(.text)
                eat = val.eat
                if @err == null and eat == .text.length and (val.value < .minMaxU64.min or val.value > .minMaxU64.max) do
                    res = .InvalidShow

            case F64:
                let val = catch StrConv.parseF64(.text)
                eat = val.eat
                if @err == null and eat == .text.length and (val.value < .minMaxF64.min or val.value > .minMaxF64.max) do
                    res = .InvalidShow
            }

            if res == .Valid
            {
                if @err == Errors.ParseIncompleteError do
                    res = .Incomplete
                elif @err == Errors.OverflowError do
                    res = .InvalidShow
                elif @err != null or eat != @countof(.text) do
                    res = .InvalidDismiss
            }
        }

        if res == .Valid do
            .isInvalid = false
        elif res == .InvalidShow do
            .isInvalid = true

        return res
    }

    // Inserts a single Unicode rune, performing maxLength and validation checks.
    mtd insertRune(r: rune)
    {
        if .maxLength and .countRunes == .maxLength do
            return

        // Convert rune to UTF‑8.
        var utf8: [5] u8
        let cpt   = Utf8.fromUnicode(utf8, [r])
        if !cpt do
            return
        utf8[cpt] = 0

        // Determine whether we need to validate the content.
        let mustValidate = .sigCheckContent != null or .inputMode != .String
        if !mustValidate
        {
            .insertUtf8(@mkstring(&utf8[0], cpt))
            .notifyChanged()
            return
        }

        let saveText   = .text
        let saveSelBeg = .selBeg
        let saveSelEnd = .selEnd
        .insertUtf8(@mkstring(&utf8[0], cpt))

        let res = .checkContent()
        if res == .InvalidDismiss
        {
            .text = saveText
            .setSelection(saveSelBeg, saveSelEnd)
            .countRunes -= 1
        }
        else do
            .notifyChanged()
    }

    #[Swag.Inline]
    {
        // Returns the first byte of the current selection.
        mtd getSelBeg() => .selBeg <= .selEnd ? .selBeg : .selEnd
        // Returns the last byte of the current selection.
        mtd getSelEnd() => .selBeg <= .selEnd ? .selEnd : .selBeg
    }

    #[Swag.Discardable]
    // Deletes the current selection without emitting change notifications.
    mtd deleteSelectionPrivate()->bool
    {
        if .selBeg == .selEnd do
            return false

        .checkSelection()

        let selB = .getSelBeg()
        let selE = .getSelEnd()
        if selB == .text.length do
            return false

        .countRunes -= cast(s32) Utf8.countRunes(.text[selB to selE - 1])
        .text.remove(selB, selE - selB)
        discard .checkContent()

        .setSelection(selB, selB)
        return true
    }

    // Returns the client rectangle adjusted by padding and right margin.
    mtd getEditRect()->Math.Rectangle
    {
        var rect = .getClientRectPadding()
        rect.moveRight(-.rightMargin)
        return rect
    }

    // Returns the left padding for text rendering (accounts for icon).
    mtd getTextMarginLeft()->f32
    {
        var rect = .getEditRect()
        if .icon.isValid() do
            rect.x += .icon.sizeX + .getThemeMetrics().iconText_Margin
        return rect.x
    }

    // Returns the right padding for text rendering (accounts for 'unit').
    mtd getTextMarginRight()->f32
    {
        if !.editBoxFlags.has(.RightAlign) do
            return 0
        if .unit.isEmpty() do
            return 0
        return Painter.measureString(.unit, .getFont().regular).width + .getThemeMetrics().editBox_UnitMargin
    }

    // Returns the padding vector for the current visual form.
    mtd getFormPadding()->Math.Vector4
    {
        #[Swag.Complete]
        switch .form
        {
        case Square:
            return .getThemeMetrics().editBox_SquarePadding
        case Round:
            return .getThemeMetrics().editBox_RoundPadding
        case Flat:
            return .getThemeMetrics().editBox_FlatPadding
        case Transparent:
            break
        }

        return {}
    }

    // Forces the caret to become visible and restarts its blink timer.
    mtd forceVisibleCaret()
    {
        if .timerCaret
        {
            .visibleCaret = true
            .timerCaret.restart()
        }
    }

    // Computes the caret rectangle in surface coordinates.
    mtd getCaretPos()->Math.Rectangle
    {
        var result: retval = undefined

        var clientRect = .getEditRect()
        if .icon.isValid() do
            clientRect.x += .icon.sizeX + 4

        result = clientRect
        let fontR = .getFont().regular
        if !.text.isEmpty()
        {
            if .editBoxFlags.has(.RightAlign)
            {
                let sizeX = Painter.measureString(.text, fontR).width
                result.x = clientRect.right() - sizeX + .posSelEnd - 2
            }
            else do
                result.x += .posSelEnd
        }
        elif .editBoxFlags.has(.RightAlign)
        {
            result.x = clientRect.right() - .posSelEnd - 2
        }

        result.x -= .getTextMarginRight()
        result.height = fontR.height
        result.y      = clientRect.vertCenter() - result.height * 0.5

        result.width = 2
        return result
    }

    // Converts an X mouse position into a byte index inside "text".
    mtd getByteIndexAtPos(xmouse: f32)->u64
    {
        if !.text.length do
            return 0

        var x = xmouse
        x += .scrollPosX

        let fontR = .getFont().regular

        // Naïve incremental search. Could be improved for performance.
        if .editBoxFlags.has(.RightAlign)
        {
            var clientRect = .getEditRect()
            clientRect.x -= .getTextMarginRight()
            let sizeX     = Painter.measureString(.text, fontR).width
            var lastWidth = 0'f32
            var diff      = 0'f32
            for idx in 1 until .text.length
            {
                let str = @mkstring(.text.buffer, idx)
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
                return .text.length - 1
        }
        else
        {
            x -= .getTextMarginLeft()
            var lastWidth = 0'f32
            var diff      = 0'f32
            for idx in 1 until .text.length
            {
                let str = @mkstring(.text.buffer, idx)
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
                return .text.length - 1
        }

        return .text.length
    }

    // Updates cached caret and selection pixel positions.
    mtd computeCaretPos()
    {
        if .getFocus() != me do
            return

        let fontR = .getFont().regular

        var str = @mkstring(.text.buffer, .selEnd)
        var met = Painter.measureString(str, fontR)
        .posSelBeg, .posSelEnd = met.width

        if .selBeg != .selEnd
        {
            str        = @mkstring(.text.buffer, .selBeg)
            met        = Painter.measureString(str, fontR)
            .posSelBeg = met.width
        }
    }

    // Moves caret left to the start of the previous word.
    mtd moveCaretLeftStartWord(sel: u64)->u64
    {
        if !sel do
            return sel

        var s = sel
        s = .moveCaretLeft(s)
        if s == sel do
            return sel

        let test = !Latin1.isSpace(.text.buffer[s])
        for
        {
            let ns = .moveCaretLeft(s)
            if ns == s do
                break
            if Latin1.isSpace(.text.buffer[ns]) == test do
                return s
            s = ns
        }

        return 0
    }

    // Moves caret right to the end of the current/next word.
    mtd moveCaretRightEndWord(sel: u64)->u64
    {
        if sel == .text.length do
            return sel

        let test = !Latin1.isSpace(.text.buffer[sel])
        var s    = sel
        while s != .text.length
        {
            s = .moveCaretRight(s)
            if Latin1.isSpace(.text.buffer[s]) == test do
                return s
        }

        return .text.length
    }

    // Moves caret left by one rune.
    mtd moveCaretLeft(sel: u64)->u64
    {
        if !sel do
            return sel
        let str = @mkstring(.text.buffer, sel)
        let (?, eat) = Utf8.decodeLastRune(str)
        return sel - eat
    }

    // Moves caret right by one rune.
    mtd moveCaretRight(sel: u64)->u64
    {
        if sel == .text.length do
            return sel
        let str = @mkstring(.text.buffer + sel, .text.length - sel)
        let (?, eat) = Utf8.decodeRune(str)
        return sel + eat
    }

    // Clamps selection indices and triggers repaint.
    mtd checkSelection()
    {
        .selBeg = Math.min(.selBeg, .text.length)
        .selEnd = Math.min(.selEnd, .text.length)
        .invalidate()
    }

    // Emits sigChanged, recomputes caret, updates clear button and repaints.
    mtd notifyChanged()
    {
        .sigChanged.call(me)
        .computeCaretPos()
        .showHideClearButon()
        .invalidate()
    }
}

impl EditBox
{
    // Factory method that creates a new EditBox instance.
    #[Swag.Discardable]
    func create(parent: *Wnd, content: #null string, position: Math.Rectangle = {}, id: WndId = null)->*EditBox
    {
        var pos = position
        if pos.height == 0 do
            pos.height = parent.getThemeMetrics().editBox_Height
        if pos.width == 0 do
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
        if .editBoxFlags.has(.ReadOnly) do
            return
        if .deleteSelectionPrivate() do
            .notifyChanged()
    }

    // Sets the current selection.
    mtd setSelection(start, end: u64)
    {
        .selBeg = start
        .selEnd = end
        .checkSelection()
        .computeCaretPos()
    }

    // Selects the whole content.
    mtd selectAll()
    {
        .setSelection(0, .text.length)
    }

    // Defines the placeholder (note) text.
    mtd setNote(note: string)
    {
        .name = note
        .invalidate()
    }

    // Assigns the EditBox content without emitting change notifications.
    mtd setTextSilent(str: string)
    {
        .sigChanged.disable()
        .setText(str)
        .sigChanged.enable()
    }

    // Assigns the EditBox content and emits change notifications.
    mtd setText(str: #null string)
    {
        if .text == str do
            return
        .text.clear()
        .selBeg, .selEnd = 0
        .countRunes = 0
        .insertString(str)
        .showHideClearButon()
        .notifyChanged()
    }

    // Returns the current text as a String.
    mtd getText()->string
    {
        return .text.toString()
    }

    // Restores the text captured on focus gain.
    mtd restoreText()
    {
        .setText(.originalText)
    }

    // Configures the EditBox to accept signed 64‑bit integers.
    mtd setInputS64(min = Swag.S64.Min, max = Swag.S64.Max)
    {
        .inputMode     = .S64
        .minMaxS64.min = min
        .minMaxS64.max = max
    }

    // Configures the EditBox to accept unsigned 64‑bit integers.
    mtd setInputU64(min = 0'u64, max = Swag.U64.Max)
    {
        .inputMode     = .U64
        .minMaxU64.min = min
        .minMaxU64.max = max
    }

    // Configures the EditBox to accept hexadecimal unsigned 64‑bit integers.
    mtd setInputX64(min = 0'u64, max = Swag.U64.Max)
    {
        .inputMode     = .X64
        .minMaxU64.min = min
        .minMaxU64.max = max
    }

    // Configures the EditBox to accept 64‑bit floating‑point numbers.
    mtd setInputF64(min = -Swag.F64.Max, max = Swag.F64.Max)
    {
        .inputMode     = .F64
        .minMaxF64.min = min
        .minMaxF64.max = max
    }

    // Sets the visual form of the EditBox.
    mtd setForm(form: EditBoxForm)
    {
        .form = form

        #[Swag.Complete]
        switch .form
        {
        case Square:
            .setAllPaddings(.getThemeMetrics().editBox_SquarePadding)
        case Round:
            .setAllPaddings(.getThemeMetrics().editBox_RoundPadding)
        case Flat:
            .setAllPaddings(.getThemeMetrics().editBox_FlatPadding)
        case Transparent:
            .setAllPaddings(0)
        }

        .invalidate()
    }

    // Returns true when the current content is valid.
    mtd isValid()->bool => !.isInvalid
}
