using Core, Pixel

public struct EditBox
{
    using scrollwnd:    ScrollWnd
    note:               RichString
    icon:               Icon
    textContent:        String
    selBeg, selEnd:     uint

    timerCaret:         *Timer
    visibleCaret:       bool
    caretPosX:          f32
}

impl IWnd for EditBox
{
    mtd onFocusEvent(evt: *FocusEvent)
    {
        if evt.kind == .SetFocus
            timerCaret = surface.app.addTimer(500, self)
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
        colors := &bc.theme.colors
        rects  := &bc.theme.rects

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorText:      Color = ?

        subRectBk = &rects.edit_SquareBk
        subRectBorder = &rects.edit_SquareBorder

        if bc.isDisabled
            colorBorder = colors.edit_DisabledBorder
        if hasFocus()
            colorBorder = colors.edit_FocusBorder
        else
            colorBorder = colors.edit_Border

        colorBk = colors.edit_SquareBk

        bc.theme.drawSubRect(painter, clientRect, subRectBk, colorBk)
        bc.theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder)

        clientRect.inflate(-bc.theme.metrics.editBox_SquarePadding)

        // Note
        if !note.isEmpty() and textContent.isEmpty()
        {
            var format: RichStringFormat
            colorText = colors.edit_Note
            format.palette[0] = colorText
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = bc.theme.res.fontDefault
            Theme.drawIconText(bc.painter, clientRect, icon, &note, format, bc.theme.metrics.iconText_Margin)
        }

        bc.painter.drawString(clientRect.x, clientRect.y, textContent, bc.theme.res.fontDefault.regular, Argb.White)

        // Caret
        if visibleCaret and hasFocus()
        {
            pos := getCaretPos()
            pos.x += bc.theme.metrics.editBox_SquarePadding
            painter.fillRect(pos, Argb.White)
        }
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MousePressed:
            if evt.button == .Left
                setFocus()
            evt.accepted = true
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
        if evt.kind != .KeyPressed
            return
        keyPressed(evt.key)
    }
}

public impl EditBox
{
    func create(parent: *Wnd, content: string, position: Math.Rectangle, id: string = null)->*EditBox
    {
        pos := position
        if pos.height == 0 pos.height = parent.getThemeMetrics().btnPush_Height
        if pos.width == 0  pos.width  = parent.getThemeMetrics().btnPush_Width
        res := Wnd.create'EditBox(parent, content, pos, id)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }

    mtd setSelection(start, end: uint)
    {
        selBeg = start
        selEnd = end
        //checkSelection()
    }

    protected mtd getCaretPos()->Math.Rectangle
    {
        var result: retval = ?

        clientRect := getClientRect()
        clientRect.applyPadding(padding)

        result = clientRect
        if !textContent.isEmpty()
            result.x += caretPosX

        font := getTheme().res.fontDefault.regular
        result.height = font.height
        result.y = clientRect.vertCenter() - result.height * 0.5

        result.width = 2
        return result
    }

    protected mtd computeCaretPos()
    {
        str := @mkstring(textContent.buffer, selEnd)
        var painter: Painter{fake: true}
        met := painter.measureString(str, getTheme().res.fontDefault.regular)
        caretPosX = met.width
    }

    protected mtd keyPressed(key: Input.Key)
    {
        switch key
        {
        case Back:
            if selBeg
            {
                selBeg = moveCaretLeft(selBeg)
                deleteSelection()
            }

            return
        }

        r := surface.app.keyb.keyToRune(key)
        if !r
            return
        insertChar(r)
    }

    protected mtd insertChar(r: rune)
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

    protected mtd moveCaretLeft(sel: uint)->uint
    {
        if !sel
            return sel
        return sel - 1
    }

    // Delete selected text
    mtd deleteSelection()
    {
        if deleteSelectionPrivate()
            notifyChanged()
    }

    #[Swag.Discardable]
    protected mtd deleteSelectionPrivate()->bool
    {
        if selBeg == selEnd
            return false
        checkSelection()
        if selBeg == textContent.length
            return false
        textContent.remove(selBeg, selEnd - selBeg)
        setSelection(selBeg, selBeg)
        return true
    }

    protected mtd checkSelection()
    {
    }

    protected mtd notifyChanged()
    {
        computeCaretPos()
        invalidate()
    }
}