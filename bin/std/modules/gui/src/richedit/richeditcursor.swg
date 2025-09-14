#global public
using Core, Pixel

struct RichEditCursor
{
    lineIndex:         u64
    charPosInLine:     u64
}

impl RichEditCtrl
{
    mtd getRuneSize(line: *RichEditLine, i: u64, x: f32)->Math.Point
    {
        var size: retval

        if line.chars[i] == '\t'
        {
            size.x = me.getTabOffset(cast(s32) x)
        }
        else
        {
            let styleIdx = line.styles[i]
            let fontR    = me.getFont().getFont(me.styles[styleIdx].fontMode)
            size = Painter.measureRune(line.chars[i], fontR)
            size.y += me.lineSpacing
        }

        return size
    }

    mtd getCursorDisplayPos(cp: RichEditCursor)->Math.Point
    {
        var result: retval

        let lineIndex = Math.min(cp.lineIndex, me.lines.count - 1)
        let line      = me.lines[lineIndex]
        result.y = line.drawingRect.bottom()

        result.x = me.getMarginLeft()
        for i in Math.min(cp.charPosInLine, line.length())
        {
            let size = me.getRuneSize(line, i, result.x)
            result.x += size.x
        }

        return result
    }

    mtd getTabOffset(x: s32, numBlanks: *s32 = null)->s32
    {
        let of        = cast(s32) (x - me.getMarginLeft())
        let fontR     = me.getFont().getFont(me.styles[0].fontMode)
        let sizeBlank = Painter.measureRune(' ', fontR)
        let sx        = cast(s32) (sizeBlank.x * me.tabSize)

        var tab = (of / sx) * sx
        tab = of - tab
        if numBlanks:
            dref numBlanks = cast(s32) (tab / sizeBlank.x)

        tab = sx - tab
        return tab
    }

    mtd getXExtent()->Math.Point
    {
        let fontR = me.getFont().getFont(me.styles[0].fontMode)
        if !fontR:
            return {}
        return Painter.measureRune('X', fontR)
    }

    mtd getCaretPos(cp: RichEditCursor)->Math.Rectangle
    {
        let pt   = me.getCursorDisplayPos(cp)
        let size = me.getXExtent()

        var result: retval
        result.x      = pt.x
        result.y      = me.overwriteMode ? pt.y - 2 : pt.y - size.y
        result.width  = me.overwriteMode ? size.x : 2
        result.height = me.overwriteMode ? 2 : size.y
        return result
    }

    internal mtd ensureCaretIsVisibleInternal(center: bool)
    {
        me.checkSelection()
        me.ensureCursorIsVisible(me.cursorPos, center)
        me.invalidate()
    }

    mtd ensureCaretIsVisible(center = false)
    {
        me.forceCaretVisible       = true
        me.forceCaretVisibleCenter = center
        me.ensureCaretIsVisibleInternal(center)
    }

    mtd ensureCursorIsVisible(cp: RichEditCursor, center: bool)
    {
        let size   = me.getXExtent()
        let decalx = size.x * 10

        let clientRect = me.edView.getClientRect()

        let pos = me.getCursorDisplayPos(cp)
        var sx  = me.edView.scrollPos.x
        var sy  = me.edView.scrollPos.y

        // Need to change horz scroll
        if pos.x < sx + me.getMarginLeft():
            sx = pos.x - decalx
        elif pos.x + size.x > sx + clientRect.width - me.getMarginRight():
            sx = pos.x + decalx - clientRect.width - me.getMarginRight()

        // Need to change vertical scroll
        if pos.y - size.y < sy
        {
            sy = pos.y - size.y
            if center:
                sy -= clientRect.height / 2
        }
        elif pos.y > sy + clientRect.height
        {
            sy = pos.y - clientRect.height
            if center:
                sy += clientRect.height / 2
        }

        if sy != me.edView.scrollPos.y:
            me.dirtyFirstLineView = true

        me.scrollWnd.setScrollPos(sx, sy)
    }

    mtd getCursorPosFromPoint(pt: Math.Point)->RichEditCursor
    {
        var cp: retval

        var found = false

        // Before the first visible line
        if pt.y < me.lines[0].drawingRect.y
        {
            cp.lineIndex     = 0
            cp.charPosInLine = 0
            found            = true
        }
        else
        {
            var i  = 0'u64
            var pi = Swag.U64.Max
            while i != pi and i != me.lines.count
            {
                let line  = me.lines[i]
                let count = line.length()
                var rect  = line.drawingRect

                // Before the first character
                if pt.y >= rect.y and pt.y < rect.bottom() and (pt.x < rect.x or !count)
                {
                    found            = true
                    cp.lineIndex     = i
                    cp.charPosInLine = 0
                    break
                }

                if count
                {
                    for j in count
                    {
                        let size = me.getRuneSize(line, j, rect.x)
                        rect.width = size.x
                        if rect.contains(pt)
                        {
                            cp.lineIndex     = i
                            cp.charPosInLine = j
                            if pt.x >= rect.horzCenter():
                                cp.charPosInLine += 1
                            found = true
                            break
                        }

                        rect.x = rect.right()
                    }
                }

                if found:
                    break

                // At the end of line
                if (pt.y >= rect.y and pt.y < rect.bottom() and pt.x >= rect.x)
                {
                    cp.lineIndex     = i
                    cp.charPosInLine = count
                    found            = true
                    break
                }

                pi = i
                i += 1
            }
        }

        // At the end of file
        if !found
        {
            cp.lineIndex     = me.lines.count - 1
            cp.charPosInLine = me.lines[cp.lineIndex].length()
        }

        // Check visibility
        if !me.lines[cp.lineIndex].isVisible():
            me.moveCursorDown(&cp)
        if !me.lines[cp.lineIndex].isVisible():
            me.moveCursorUp(&cp)

        return cp
    }

    mtd setCursorPos(cp: RichEditCursor)
    {
        me.cursorPos = cp
    }

    #[Swag.Discardable]
    mtd moveCursorLeft(cp: *RichEditCursor)->bool
    {
        if cp.charPosInLine == 0
        {
            let prev = me.getPreviousVisibleLine(cp.lineIndex)
            if prev == cp.lineIndex:
                return false
            cp.lineIndex     = prev
            cp.charPosInLine = me.lines[prev].length()
        }
        else
        {
            cp.charPosInLine -= 1
        }

        return true
    }

    #[Swag.Discardable]
    mtd moveCursorRight(cp: *RichEditCursor)->bool
    {
        if cp.charPosInLine == me.lines[cp.lineIndex].length()
        {
            let next = me.getNextVisibleLine(cp.lineIndex)
            if next == cp.lineIndex:
                return false
            cp.lineIndex     = next
            cp.charPosInLine = 0
        }
        else
        {
            cp.charPosInLine += 1
        }

        return true
    }

    #[Swag.Discardable, Swag.Overload]
    mtd moveCursorUp(cp: *RichEditCursor)->bool
    {
        let pt = me.getCursorDisplayPos(cp)
        return me.moveCursorUp(cp, pt)
    }

    #[Swag.Discardable, Swag.Overload]
    internal mtd moveCursorUp(cp: *RichEditCursor, posPrev: Math.Point)->bool
    {
        let prev = me.getPreviousVisibleLine(cp.lineIndex)
        if prev == cp.lineIndex:
            return false

        cp.lineIndex = prev
        var line = me.lines[cp.lineIndex]
        cp.charPosInLine = Math.min(line.length(), cp.charPosInLine)
        var posNew = me.getCursorDisplayPos(cp)

        if posPrev.x == posNew.x:
            return true
        if posNew.x < posPrev.x
        {
            while posNew.x < posPrev.x and cp.charPosInLine < line.length()
            {
                posNew.x += me.getRuneSize(line, cp.charPosInLine, posNew.x).x
                cp.charPosInLine += 1
            }
        }
        else
        {
            line = me.lines[cp.lineIndex]
            while posNew.x > posPrev.x and cp.charPosInLine
            {
                cp.charPosInLine -= 1
                posNew.x -= me.getRuneSize(line, cp.charPosInLine, posNew.x).x
            }
        }

        return true
    }

    #[Swag.Discardable, Swag.Overload]
    mtd moveCursorDown(cp: *RichEditCursor)->bool
    {
        let pt = me.getCursorDisplayPos(cp)
        return me.moveCursorDown(cp, pt)
    }

    #[Swag.Discardable, Swag.Overload]
    internal mtd moveCursorDown(cp: *RichEditCursor, posPrev: Math.Point)->bool
    {
        let next = me.getNextVisibleLine(cp.lineIndex)
        if next == cp.lineIndex:
            return false

        cp.lineIndex = next
        var line = me.lines[cp.lineIndex]
        cp.charPosInLine = Math.min(line.length(), cp.charPosInLine)
        var posNew = me.getCursorDisplayPos(cp)

        if posPrev.x == posNew.x:
            return true
        if posNew.x < posPrev.x
        {
            while posNew.x < posPrev.x and cp.charPosInLine < line.length()
            {
                posNew.x += me.getRuneSize(line, cp.charPosInLine, posNew.x).x
                cp.charPosInLine += 1
            }
        }
        else
        {
            line = me.lines[cp.lineIndex]
            while posNew.x > posPrev.x and cp.charPosInLine
            {
                cp.charPosInLine -= 1
                posNew.x -= me.getRuneSize(line, cp.charPosInLine, posNew.x).x
            }
        }

        return true
    }

    mtd moveCursorStartLine(cp: *RichEditCursor)
    {
        let line = me.lines[cp.lineIndex]
        var i    = cast(s32) cp.charPosInLine

        var firstBlank: bool
        if !i
        {
            firstBlank = true
        }
        else
        {
            firstBlank = false
            i -= 1
            while i >= 0
            {
                if !Unicode.isSpace(line.chars[i])
                {
                    firstBlank = true
                    break
                }

                i -= 1
            }
        }

        cp.charPosInLine = 0
        if firstBlank
        {
            while cp.charPosInLine < line.length() and Unicode.isSpace(line.chars[cp.charPosInLine]):
                cp.charPosInLine += 1
        }
    }

    mtd moveCursorEndLine(cp: *RichEditCursor)
    {
        cp.charPosInLine = me.lines[cp.lineIndex].length()
    }

    mtd moveCursorStartFile(cp: *RichEditCursor)
    {
        cp.lineIndex     = me.getFirstVisibleLine()
        cp.charPosInLine = 0
    }

    mtd moveCursorEndFile(cp: *RichEditCursor)
    {
        cp.lineIndex     = me.getLastVisibleLine()
        cp.charPosInLine = me.lines[cp.lineIndex].length()
    }

    mtd moveCursorPageUp(cp: *RichEditCursor)
    {
        let client = me.edView.getClientRect()
        var count  = cast(s64) (client.height / me.getXExtent().y)
        while count
        {
            if cp.lineIndex == 0:
                break
            me.moveCursorUp(cp)
            count -= 1
        }
    }

    mtd moveCursorPageDown(cp: *RichEditCursor)
    {
        let client = me.edView.getClientRect()
        var count  = cast(s64) (client.height / me.getXExtent().y)
        while count
        {
            if cp.lineIndex == me.lines.count - 1:
                break
            me.moveCursorDown(cp)
            count -= 1
        }
    }

    mtd moveCursorWordLeft(cp: *RichEditCursor)
    {
        var line = me.lines[cp.lineIndex]
        if cp.charPosInLine == 0
        {
            me.moveCursorLeft(cp)
            return
        }

        if Unicode.isSpace(line.chars[cp.charPosInLine - 1])
        {
            if !me.moveCursorLeft(cp):
                return
            line = me.lines[cp.lineIndex]
            while cp.charPosInLine != 0 and Unicode.isSpace(line.chars[cp.charPosInLine - 1])
            {
                if !me.moveCursorLeft(cp):
                    return
                line = me.lines[cp.lineIndex]
            }

            if !cp.charPosInLine:
                return
        }

        if isWord(line.chars[cp.charPosInLine - 1])
        {
            while cp.charPosInLine != 0 and isWord(line.chars[cp.charPosInLine - 1])
            {
                if !me.moveCursorLeft(cp):
                    return
                line = me.lines[cp.lineIndex]
            }
        }
        elif zapWordChar(line.chars[cp.charPosInLine - 1])
        {
            while cp.charPosInLine != 0 and zapWordChar(line.chars[cp.charPosInLine - 1])
            {
                if !me.moveCursorLeft(cp):
                    return
                line = me.lines[cp.lineIndex]
            }
        }
        else
        {
            me.moveCursorLeft(cp)
        }
    }

    mtd moveCursorWordRight(cp: *RichEditCursor, withSpace: bool)
    {
        var line = me.lines[cp.lineIndex]
        if cp.charPosInLine == line.length() or Unicode.isSpace(line.chars[cp.charPosInLine])
        {
            if !me.moveCursorRight(cp):
                return
            line = me.lines[cp.lineIndex]
            while cp.charPosInLine != line.length() and Unicode.isSpace(line.chars[cp.charPosInLine])
            {
                if !me.moveCursorRight(cp):
                    return
                line = me.lines[cp.lineIndex]
            }
        }
        elif isWord(line.chars[cp.charPosInLine])
        {
            while cp.charPosInLine != line.length() and isWord(line.chars[cp.charPosInLine])
            {
                if !me.moveCursorRight(cp):
                    return
                line = me.lines[cp.lineIndex]
            }

            if withSpace
            {
                while cp.charPosInLine != line.length() and Unicode.isSpace(line.chars[cp.charPosInLine])
                {
                    if !me.moveCursorRight(cp):
                        return
                    line = me.lines[cp.lineIndex]
                }
            }
        }
        elif zapWordChar(line.chars[cp.charPosInLine])
        {
            while cp.charPosInLine != line.length() and zapWordChar(line.chars[cp.charPosInLine])
            {
                if !me.moveCursorRight(cp):
                    return
                line = me.lines[cp.lineIndex]
            }

            while cp.charPosInLine != line.length() and Unicode.isSpace(line.chars[cp.charPosInLine])
            {
                if !me.moveCursorRight(cp):
                    return
                line = me.lines[cp.lineIndex]
            }
        }
        else
        {
            me.moveCursorRight(cp)
        }
    }
}
