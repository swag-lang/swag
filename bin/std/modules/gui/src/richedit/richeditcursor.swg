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
            size.x = .getTabOffset(cast(s32) x)
        }
        else
        {
            let styleIdx = line.styles[i]
            let fontR    = .getFont().getFont(.styles[styleIdx].fontMode)
            size = Painter.measureRune(line.chars[i], fontR)
            size.y += .lineSpacing
        }

        return size
    }

    mtd getCursorDisplayPos(cp: RichEditCursor)->Math.Point
    {
        var result: retval

        let lineIndex = Math.min(cp.lineIndex, .lines.count - 1)
        let line      = .lines[lineIndex]
        result.y = line.drawingRect.bottom()

        result.x = .getMarginLeft()
        for i in Math.min(cp.charPosInLine, line.length())
        {
            let size = .getRuneSize(line, i, result.x)
            result.x += size.x
        }

        return result
    }

    mtd getTabOffset(x: s32, numBlanks: *s32 = null)->s32
    {
        let of        = cast(s32) (x - .getMarginLeft())
        let fontR     = .getFont().getFont(.styles[0].fontMode)
        let sizeBlank = Painter.measureRune(' ', fontR)
        let sx        = cast(s32) (sizeBlank.x * .tabSize)

        var tab = (of / sx) * sx
        tab = of - tab
        if numBlanks do
            dref numBlanks = cast(s32) (tab / sizeBlank.x)

        tab = sx - tab
        return tab
    }

    mtd getXExtent()->Math.Point
    {
        let fontR = .getFont().getFont(.styles[0].fontMode)
        if !fontR do
            return {}
        return Painter.measureRune('X', fontR)
    }

    mtd getCaretPos(cp: RichEditCursor)->Math.Rectangle
    {
        let pt   = .getCursorDisplayPos(cp)
        let size = .getXExtent()

        var result: retval
        result.x      = pt.x
        result.y      = .overwriteMode ? pt.y - 2 : pt.y - size.y
        result.width  = .overwriteMode ? size.x : 2
        result.height = .overwriteMode ? 2 : size.y
        return result
    }

    internal mtd ensureCaretIsVisibleInternal(center: bool)
    {
        .checkSelection()
        .ensureCursorIsVisible(.cursorPos, center)
        .invalidate()
    }

    mtd ensureCaretIsVisible(center = false)
    {
        .forceCaretVisible       = true
        .forceCaretVisibleCenter = center
        .ensureCaretIsVisibleInternal(center)
    }

    mtd ensureCursorIsVisible(cp: RichEditCursor, center: bool)
    {
        let size   = .getXExtent()
        let decalx = size.x * 10

        let clientRect = .edView.getClientRect()

        let pos = .getCursorDisplayPos(cp)
        var sx  = .edView.scrollPos.x
        var sy  = .edView.scrollPos.y

        // Need to change horz scroll
        if pos.x < sx + .getMarginLeft() do
            sx = pos.x - decalx
        elif pos.x + size.x > sx + clientRect.width - .getMarginRight() do
            sx = pos.x + decalx - clientRect.width - .getMarginRight()

        // Need to change vertical scroll
        if pos.y - size.y < sy
        {
            sy = pos.y - size.y
            if center do
                sy -= clientRect.height / 2
        }
        elif pos.y > sy + clientRect.height
        {
            sy = pos.y - clientRect.height
            if center do
                sy += clientRect.height / 2
        }

        if sy != .edView.scrollPos.y do
            .dirtyFirstLineView = true

        .scrollWnd.setScrollPos(sx, sy)
    }

    mtd getCursorPosFromPoint(pt: Math.Point)->RichEditCursor
    {
        var cp: retval

        var found = false

        // Before the first visible line
        if pt.y < .lines[0].drawingRect.y
        {
            cp.lineIndex     = 0
            cp.charPosInLine = 0
            found            = true
        }
        else
        {
            var i  = 0'u64
            var pi = Swag.U64.Max
            while i != pi and i != .lines.count
            {
                let line  = .lines[i]
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
                        let size = .getRuneSize(line, j, rect.x)
                        rect.width = size.x
                        if rect.contains(pt)
                        {
                            cp.lineIndex     = i
                            cp.charPosInLine = j
                            if pt.x >= rect.horzCenter() do
                                cp.charPosInLine += 1
                            found = true
                            break
                        }

                        rect.x = rect.right()
                    }
                }

                if found do
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
            cp.lineIndex     = .lines.count - 1
            cp.charPosInLine = .lines[cp.lineIndex].length()
        }

        // Check visibility
        if !.lines[cp.lineIndex].isVisible() do
            .moveCursorDown(&cp)
        if !.lines[cp.lineIndex].isVisible() do
            .moveCursorUp(&cp)

        return cp
    }

    mtd setCursorPos(cp: RichEditCursor)
    {
        .cursorPos = cp
    }

    #[Swag.Discardable]
    mtd moveCursorLeft(cp: *RichEditCursor)->bool
    {
        if cp.charPosInLine == 0
        {
            let prev = .getPreviousVisibleLine(cp.lineIndex)
            if prev == cp.lineIndex do
                return false
            cp.lineIndex     = prev
            cp.charPosInLine = .lines[prev].length()
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
        if cp.charPosInLine == .lines[cp.lineIndex].length()
        {
            let next = .getNextVisibleLine(cp.lineIndex)
            if next == cp.lineIndex do
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
        let pt = .getCursorDisplayPos(cp)
        return .moveCursorUp(cp, pt)
    }

    #[Swag.Discardable, Swag.Overload]
    internal mtd moveCursorUp(cp: *RichEditCursor, posPrev: Math.Point)->bool
    {
        let prev = .getPreviousVisibleLine(cp.lineIndex)
        if prev == cp.lineIndex do
            return false

        cp.lineIndex = prev
        var line = .lines[cp.lineIndex]
        cp.charPosInLine = Math.min(line.length(), cp.charPosInLine)
        var posNew = .getCursorDisplayPos(cp)

        if posPrev.x == posNew.x do
            return true
        if posNew.x < posPrev.x
        {
            while posNew.x < posPrev.x and cp.charPosInLine < line.length()
            {
                posNew.x += .getRuneSize(line, cp.charPosInLine, posNew.x).x
                cp.charPosInLine += 1
            }
        }
        else
        {
            line = .lines[cp.lineIndex]
            while posNew.x > posPrev.x and cp.charPosInLine
            {
                cp.charPosInLine -= 1
                posNew.x -= .getRuneSize(line, cp.charPosInLine, posNew.x).x
            }
        }

        return true
    }

    #[Swag.Discardable, Swag.Overload]
    mtd moveCursorDown(cp: *RichEditCursor)->bool
    {
        let pt = .getCursorDisplayPos(cp)
        return .moveCursorDown(cp, pt)
    }

    #[Swag.Discardable, Swag.Overload]
    internal mtd moveCursorDown(cp: *RichEditCursor, posPrev: Math.Point)->bool
    {
        let next = .getNextVisibleLine(cp.lineIndex)
        if next == cp.lineIndex do
            return false

        cp.lineIndex = next
        var line = .lines[cp.lineIndex]
        cp.charPosInLine = Math.min(line.length(), cp.charPosInLine)
        var posNew = .getCursorDisplayPos(cp)

        if posPrev.x == posNew.x do
            return true
        if posNew.x < posPrev.x
        {
            while posNew.x < posPrev.x and cp.charPosInLine < line.length()
            {
                posNew.x += .getRuneSize(line, cp.charPosInLine, posNew.x).x
                cp.charPosInLine += 1
            }
        }
        else
        {
            line = .lines[cp.lineIndex]
            while posNew.x > posPrev.x and cp.charPosInLine
            {
                cp.charPosInLine -= 1
                posNew.x -= .getRuneSize(line, cp.charPosInLine, posNew.x).x
            }
        }

        return true
    }

    mtd moveCursorStartLine(cp: *RichEditCursor)
    {
        let line = .lines[cp.lineIndex]
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
            while cp.charPosInLine < line.length() and Unicode.isSpace(line.chars[cp.charPosInLine]) do
                cp.charPosInLine += 1
        }
    }

    mtd moveCursorEndLine(cp: *RichEditCursor)
    {
        cp.charPosInLine = .lines[cp.lineIndex].length()
    }

    mtd moveCursorStartFile(cp: *RichEditCursor)
    {
        cp.lineIndex     = .getFirstVisibleLine()
        cp.charPosInLine = 0
    }

    mtd moveCursorEndFile(cp: *RichEditCursor)
    {
        cp.lineIndex     = .getLastVisibleLine()
        cp.charPosInLine = .lines[cp.lineIndex].length()
    }

    mtd moveCursorPageUp(cp: *RichEditCursor)
    {
        let client = .edView.getClientRect()
        var count  = cast(s64) (client.height / .getXExtent().y)
        while count
        {
            if cp.lineIndex == 0 do
                break
            .moveCursorUp(cp)
            count -= 1
        }
    }

    mtd moveCursorPageDown(cp: *RichEditCursor)
    {
        let client = .edView.getClientRect()
        var count  = cast(s64) (client.height / .getXExtent().y)
        while count
        {
            if cp.lineIndex == .lines.count - 1 do
                break
            .moveCursorDown(cp)
            count -= 1
        }
    }

    mtd moveCursorWordLeft(cp: *RichEditCursor)
    {
        var line = .lines[cp.lineIndex]
        if cp.charPosInLine == 0
        {
            .moveCursorLeft(cp)
            return
        }

        if Unicode.isSpace(line.chars[cp.charPosInLine - 1])
        {
            if !.moveCursorLeft(cp) do
                return
            line = .lines[cp.lineIndex]
            while cp.charPosInLine != 0 and Unicode.isSpace(line.chars[cp.charPosInLine - 1])
            {
                if !.moveCursorLeft(cp) do
                    return
                line = .lines[cp.lineIndex]
            }

            if !cp.charPosInLine do
                return
        }

        if isWord(line.chars[cp.charPosInLine - 1])
        {
            while cp.charPosInLine != 0 and isWord(line.chars[cp.charPosInLine - 1])
            {
                if !.moveCursorLeft(cp) do
                    return
                line = .lines[cp.lineIndex]
            }
        }
        elif zapWordChar(line.chars[cp.charPosInLine - 1])
        {
            while cp.charPosInLine != 0 and zapWordChar(line.chars[cp.charPosInLine - 1])
            {
                if !.moveCursorLeft(cp) do
                    return
                line = .lines[cp.lineIndex]
            }
        }
        else
        {
            .moveCursorLeft(cp)
        }
    }

    mtd moveCursorWordRight(cp: *RichEditCursor, withSpace: bool)
    {
        var line = .lines[cp.lineIndex]
        if cp.charPosInLine == line.length() or Unicode.isSpace(line.chars[cp.charPosInLine])
        {
            if !.moveCursorRight(cp) do
                return
            line = .lines[cp.lineIndex]
            while cp.charPosInLine != line.length() and Unicode.isSpace(line.chars[cp.charPosInLine])
            {
                if !.moveCursorRight(cp) do
                    return
                line = .lines[cp.lineIndex]
            }
        }
        elif isWord(line.chars[cp.charPosInLine])
        {
            while cp.charPosInLine != line.length() and isWord(line.chars[cp.charPosInLine])
            {
                if !.moveCursorRight(cp) do
                    return
                line = .lines[cp.lineIndex]
            }

            if withSpace
            {
                while cp.charPosInLine != line.length() and Unicode.isSpace(line.chars[cp.charPosInLine])
                {
                    if !.moveCursorRight(cp) do
                        return
                    line = .lines[cp.lineIndex]
                }
            }
        }
        elif zapWordChar(line.chars[cp.charPosInLine])
        {
            while cp.charPosInLine != line.length() and zapWordChar(line.chars[cp.charPosInLine])
            {
                if !.moveCursorRight(cp) do
                    return
                line = .lines[cp.lineIndex]
            }

            while cp.charPosInLine != line.length() and Unicode.isSpace(line.chars[cp.charPosInLine])
            {
                if !.moveCursorRight(cp) do
                    return
                line = .lines[cp.lineIndex]
            }
        }
        else
        {
            .moveCursorRight(cp)
        }
    }
}
