#global public
using Core, Pixel

struct RichEditCharPos
{
    lineIndex:      uint
    charPosInLine:  uint
}

impl RichEdit
{
    mtd getSizeChar(line: *RichEditLine, i: uint, x: f32)->Math.Point
    {
        var size: retval

        if line.chars[i] == "\t"'rune
        {
            Debug.assert(false) // todo
        }
        else
        {
            styleIdx := line.styles[i]
            font := getTheme().res.fontDefault.getFont(styles[styleIdx].fontMode)
            size = Painter.measureRune(line.chars[i], font)
            size.y += lineSpacing
        }

        return size
    }

    mtd getCharacterPos(cp: RichEditCharPos)->Math.Point
    {
        var result: retval

        lineIndex := Math.min(cp.lineIndex, lines.count - 1)
        pline     := lines[lineIndex]
        result.y  = pline.drawingRect.bottom()

        result.x = getMarginLeft()
        loop i: cp.charPosInLine
        {
            size := getSizeChar(pline, i, result.x)
            result.x += size.x
        }

        return result
    }

    mtd getXExtent()->Math.Point
    {
        font := getTheme().res.fontDefault.getFont(styles[0].fontMode)
        return Painter.measureRune("X"'rune, font)
    }

    mtd getCaretPos()->Math.Rectangle
    {
        pt := getCharacterPos(cursorPos)
        size := getXExtent()

        var result: retval
        result.x = pt.x
        result.y = overwriteMode ? pt.y - 2 : pt.y - size.y
        result.width = overwriteMode ? size.x : 2
        result.height = overwriteMode ? 2 : size.y
        return result
    }

    mtd ensureCaretIsVisibleInternal(center: bool)
    {
        checkSelection()
        ensureCharacterIsVisible(cursorPos, center)
        invalidate()
    }

    mtd ensureCaretIsVisible(center = false)
    {
        forceCaretVisible       = true;
        forceCaretVisibleCenter = center
        ensureCaretIsVisibleInternal(center)
    }

    mtd ensureCharacterIsVisible(cp: RichEditCharPos, center: bool)
    {
        size   := getXExtent()
        decalx := size.x * 10

        clientRect := edView.getClientRect()

        pos := getCharacterPos(cp)
        sx  := edView.scrollPos.x
        sy  := edView.scrollPos.y

        // Need to change horz scroll
        if pos.x < sx + getMarginLeft()
            sx = pos.x - decalx
        elif pos.x + size.x > sx + clientRect.width - getMarginRight()
            sx = pos.x + decalx - clientRect.width - getMarginRight()

        // Need to change vertical scroll
        if pos.y - size.y < sy
        {
            sy = pos.y - size.y
            if center
                sy -= clientRect.height / 2
        }
        elif pos.y > sy + clientRect.height
        {
            sy = pos.y - clientRect.height
            if center
                sy += clientRect.height / 2
        }

        if sy != edView.scrollPos.y
            dirtyFirstLineView = true

        scrollWnd.setScrollPos(sx, sy)
    }
}