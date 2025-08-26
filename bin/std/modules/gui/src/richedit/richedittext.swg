#global marked
using Core, Pixel

impl RichEditCtrl
{
    mtd getTextPrivate(start, end: RichEditCursor)->{ text: Array'rune, styles: Array'u8 }
    {
        var result: retval
        if !me.hasSelRect
        {
            // First line
            var line  = me.lines[start.lineIndex]
            var count = line.length()
            if count
            {
                let pchar  = line.chars.buffer
                let pstyle = line.styles.buffer
                if start.lineIndex == end.lineIndex:
                    count = end.charPosInLine
                for var i = start.charPosInLine; i < count; i += 1
                {
                    result.text.add(pchar[i])
                    result.styles.add(pstyle[i])
                }
            }

            if start.lineIndex != end.lineIndex
            {
                result.text.add('\n')
                result.styles.add(0)
            }

            // Inside lines
            if end.lineIndex > start.lineIndex + 1
            {
                // Pre allocate string to optimize append
                count = 0
                for var i = start.lineIndex + 1; i < end.lineIndex; i += 1:
                    count += me.lines[i].length()
                result.text.reserve(result.text.count + count)
                result.styles.reserve(result.styles.count + count)

                for var i = start.lineIndex + 1; i < end.lineIndex; i += 1
                {
                    result.text.add(me.lines[i].chars.toSlice())
                    result.styles.add(me.lines[i].styles.toSlice())
                    result.text.add('\n')
                    result.styles.add(0)
                }
            }

            // Last line
            if start.lineIndex != end.lineIndex
            {
                line  = me.lines[end.lineIndex]
                count = line.length()
                if count
                {
                    let pchar  = line.chars.buffer
                    let pstyle = line.styles.buffer
                    for var i = 0; i < end.charPosInLine; i += 1
                    {
                        result.text.add(pchar[i])
                        result.styles.add(pstyle[i])
                    }
                }
            }
        }
        else
        {
            var li = start.lineIndex
            while li <= end.lineIndex
            {
                var rankBeg, rankEnd: u64
                if me.getSelectionRectRange(li, &rankBeg, &rankEnd)
                {
                    for i in rankEnd - rankBeg
                    {
                        result.text.add(me.lines[li].chars[rankBeg + i])
                        result.styles.add(me.lines[li].styles[rankBeg + i])
                    }
                }

                result.text.add('\n')
                result.styles.add(0)
                li += 1
            }
        }

        return result
    }

    mtd unwrap(lineIndex: u64)
    {
        if !(me.richEdFlags & .WordWrap):
            return

        // Make one single line with current wrapped lines
        var i = lineIndex + 1
        while i < me.lines.count
        {
            if !me.lines[i].wordWrap:
                break

            if me.cursorPos.lineIndex == i
            {
                me.cursorPos.lineIndex = lineIndex
                me.cursorPos.charPosInLine += me.lines[lineIndex].chars.count
            }

            if me.selBeg.lineIndex == i
            {
                me.selBeg.lineIndex = lineIndex
                me.selBeg.charPosInLine += me.lines[lineIndex].chars.count
            }

            me.lines[lineIndex].chars.add(me.lines[i].chars.toSlice())
            me.lines[lineIndex].styles.add(me.lines[i].styles.toSlice())
            i += 1
        }

        // Remove current wrapped lines
        while i != lineIndex + 1
        {
            me.deleteLine(lineIndex + 1)
            i -= 1
        }
    }

    mtd wrap(lineIndex: u64)
    {
        if !(me.richEdFlags & .WordWrap):
            return

        let fontR = me.getFont().getFont(me.styles[0].fontMode)
        if !fontR:
            return

        // Do the wrap
        var curLine    = lineIndex
        var i          = 0'u64
        var clientRect = me.edView.getClientRect()
        clientRect.width -= me.getMarginRight()

        var str: String

        var cutAti = i
        while i < me.lines[curLine].length()
        {
            let l  = me.lines[curLine]
            let ch = l.chars[i]
            str += ch

            if !Unicode.isLetterOrDigit(ch):
                cutAti = i + 1

            if cutAti > 1
            {
                let x = me.getMarginLeft() + Painter.measureString(str, fontR).width
                if x > clientRect.right()
                {
                    if cutAti and cutAti == i + 1:
                        cutAti -= 1

                    if me.cursorPos.lineIndex == curLine and me.cursorPos.charPosInLine >= cutAti
                    {
                        me.cursorPos.lineIndex += 1
                        me.cursorPos.charPosInLine -= cutAti
                    }

                    if me.selBeg.lineIndex == curLine and me.selBeg.charPosInLine >= cutAti
                    {
                        me.selBeg.lineIndex += 1
                        me.selBeg.charPosInLine -= cutAti
                    }

                    i = cutAti
                    let newL = me.newLine(curLine + 1)
                    newL.wordWrap = true

                    newL.chars.add(@mkslice(l.chars.buffer + i, l.chars.count - i))
                    newL.styles.add(@mkslice(l.styles.buffer + i, l.chars.count - i))
                    newL.dirtyStyle       = false
                    newL.lexerEndingState = l.lexerEndingState
                    newL.lexerCptCmt      = l.lexerCptCmt
                    l.chars.resize(i)
                    l.styles.resize(i)

                    curLine += 1
                    i, cutAti = 0
                    str.clear()
                    continue
                }
            }

            i += 1
        }
    }

    mtd insertRunePrivate(cp: *RichEditCursor, c: rune, txtStyle: RichEditStyleRef)
    {
        if c == '\r':
            return

        if c == '\n'
        {
            let linep = me.lines[cp.lineIndex]
            discard me.newLine(cp.lineIndex + 1)
            let line = me.lines[cp.lineIndex + 1]
            if cp.charPosInLine < linep.length():
                line.insertRunes(0, linep, cp.charPosInLine, linep.length() - cp.charPosInLine)
            linep.setLength(cp.charPosInLine)
            cp.lineIndex += 1
            cp.charPosInLine = 0
        }
        elif c == '\t' and (me.richEdFlags & .TabToSpaces)
        {
            let line = me.lines[cp.lineIndex]
            for me.tabSize
            {
                let posIns = Math.min(cp.charPosInLine, line.length())
                line.insertRune(posIns, ' ', txtStyle)
                cp.charPosInLine += 1
            }
        }
        else
        {
            let line   = me.lines[cp.lineIndex]
            let posIns = Math.min(cp.charPosInLine, line.length())
            line.insertRune(posIns, c, txtStyle)
            cp.charPosInLine += 1
        }

        me.recomputeScroll = true
    }

    #[Swag.Overload]
    mtd insertTextPrivate(cp: *RichEditCursor, runes: Array'rune, nstyles: Array'u8, selRect: bool)
    {
        if runes.isEmpty():
            return
        Debug.assert(runes.count == nstyles.count)

        if !selRect
        {
            for i in runes
            {
                let c = runes[i]
                let s = nstyles[i]
                me.insertRunePrivate(cp, c, s)
            }
        }
        else
        {
            var lineIndex = cp.lineIndex
            var posInLine = cp.charPosInLine

            var rankBeg, rankEnd: u64
            for
            {
                let line = me.lines[lineIndex]
                if rankEnd == runes.count or runes[rankEnd] == '\n'
                {
                    var posIns = posInLine
                    if posIns > line.length():
                        posIns = line.length()
                    line.insertRunes(posIns, @mkslice(runes.buffer + rankBeg, rankEnd - rankBeg), @mkslice(nstyles.buffer + rankBeg, rankEnd - rankBeg))
                    if rankEnd == runes.count:
                        break
                    if rankEnd >= runes.count - 1:
                        break

                    lineIndex += 1
                    if lineIndex == me.lines.count
                    {
                        me.newLine(lineIndex)
                        posInLine = 0
                    }

                    rankEnd += 1
                    rankBeg = rankEnd
                    continue
                }

                if rankEnd == runes.count:
                    break
                rankEnd += 1
            }

            cp.lineIndex     = lineIndex
            cp.charPosInLine = posInLine + (rankEnd - rankBeg)
        }

        me.recomputeScroll = true
    }

    #[Swag.Overload]
    mtd insertTextPrivate(cp: *RichEditCursor, text: string, txtStyle: RichEditStyleRef)
    {
        if me.lines.isEmpty():
            me.newLine(0)
        elif cp.lineIndex == me.lines.count:
            me.newLine(cp.lineIndex)

        var str = text
        for
        {
            let (c, eat) = Utf8.decodeRune(str)
            if !eat:
                break
            str = Tokenize.eatCount(str, cast(u64) eat)
            me.insertRunePrivate(cp, c, txtStyle)
        }

        me.recomputeScroll = true
    }

    mtd deleteTextPrivate(posStart, posEnd: RichEditCursor, selRect: bool)
    {
        // Nothing to delete
        if posStart.lineIndex == posEnd.lineIndex and posStart.charPosInLine == posEnd.charPosInLine:
            return

        if !selRect
        {
            var ps = posStart
            var pe = posEnd

            // Remove inside lines
            while pe.lineIndex > ps.lineIndex + 1
            {
                me.deleteLine(ps.lineIndex + 1)
                pe.lineIndex -= 1
            }

            let lineSrc = me.lines[ps.lineIndex]
            let lineEnd = me.lines[pe.lineIndex]

            if ps.lineIndex == pe.lineIndex
            {
                lineSrc.removeRunes(ps.charPosInLine, pe.charPosInLine - ps.charPosInLine)
            }
            else
            {
                lineSrc.removeRunes(ps.charPosInLine, lineSrc.length() - ps.charPosInLine)
                lineEnd.removeRunes(0, pe.charPosInLine)
                if lineEnd.length():
                    lineSrc.insertRunes(ps.charPosInLine, lineEnd, 0, lineEnd.length())
                me.deleteLine(pe.lineIndex)
            }
        }
        else
        {
            var ps = posStart
            var pe = posEnd
            if ps.lineIndex > pe.lineIndex:
                swap(&ps, &pe)

            var li = ps.lineIndex
            while li <= pe.lineIndex
            {
                var rankBeg, rankEnd: u64
                if me.getSelectionRectRange(li, &rankBeg, &rankEnd)
                {
                    me.lines[li].removeRunes(rankBeg, rankEnd - rankBeg)
                }

                li += 1
            }
        }

        me.recomputeScroll = true
    }

    func isWord(c: rune)->bool
    {
        return Unicode.isLetterOrDigit(c) or c == '_'
    }

    func zapWordChar(c: rune)->bool
    {
        if isWord(c):
            return false
        if Unicode.isSpace(c):
            return false
        if c != '(' and c != ')' and c != '[' and c != ']' and c != '{' and c != '}':
            return true
        return false
    }
}

public impl RichEditCtrl
{
    // Clear content
    mtd clear()
    {
        me.clearUndo()
        foreach l in me.lines:
            me.freeLines.add(l)
        me.lines.clear()
        me.countVisibleLines    = 0
        me.cursorPos            = {}
        me.selBeg               = {}
        me.dirtyFirstLineView   = true
        me.firstVisibleLineView = 0
        me.lastVisibleLineView  = 0
    }

    // Get the selected text
    mtd getSelectedText()->String
    {
        var res = me.getTextPrivate(me.getBegSelection(), me.getEndSelection())
        var txt = Utf8.fromUnicode(res.text.toSlice())
        return txt
    }

    // Get the full text
    mtd getText()->String
    {
        var sel0: RichEditCursor
        var sel1: RichEditCursor
        me.moveCursorStartFile(&sel0)
        me.moveCursorEndFile(&sel1)
        var res = me.getTextPrivate(sel0, sel1)
        var txt = Utf8.fromUnicode(res.text.toSlice())
        return txt
    }

    // Get the full text with the associated styles per character
    mtd getTextAndStyles()->{ text: Core.Array'rune, styles: Core.Array'u8 }
    {
        var sel0: RichEditCursor
        var sel1: RichEditCursor
        me.moveCursorStartFile(&sel0)
        me.moveCursorEndFile(&sel1)
        return me.getTextPrivate(sel0, sel1)
    }

    // Set the text content
    mtd setText(text: string)
    {
        me.clear()

        var cp: RichEditCursor
        me.insertTextPrivate(&cp, text, 0)

        me.resetModified()
        me.invalidate()
    }

    // Append a text with a given style
    mtd addText(text: string, txtStyle: RichEditStyleRef = 0)
    {
        var cp: RichEditCursor
        if me.lines.count
        {
            cp.lineIndex     = me.lines.count - 1
            cp.charPosInLine = me.lines[cp.lineIndex].length()
        }

        me.insertTextPrivate(&cp, text, txtStyle)
        me.setModified(true)
    }

    // Insert a text with a given style
    #[Swag.Overload]
    mtd insertText(text: string)
    {
        let undo = Memory.new'RichEditUndo()
        undo.type  = .Insert
        undo.runes = Unicode.fromUtf8(text)
        undo.styles.resize(undo.runes.count)
        undo.selBeg     = me.selBeg
        undo.cursorPos  = me.cursorPos
        undo.hasSelRect = me.hasSelRect
        undo.serial     = 0
        me.resetUndo()
        me.toUndo.add(undo)
        me.insertTextPrivate(&me.cursorPos, text, 0)
        me.clearSelection()
        me.setModified(true)
    }

    // Insert a text with a style per character
    #[Swag.Overload]
    mtd insertText(text: string, txtStyles: Array'u8)
    {
        let undo = Memory.new'RichEditUndo()
        undo.type       = .Insert
        undo.runes      = Unicode.fromUtf8(text)
        undo.styles     = txtStyles
        undo.selBeg     = me.selBeg
        undo.cursorPos  = me.cursorPos
        undo.hasSelRect = me.hasSelRect
        undo.serial     = 0
        me.resetUndo()
        me.toUndo.add(undo)
        me.insertTextPrivate(&me.cursorPos, undo.runes, undo.styles, me.hasSelRect)
        me.clearSelection()
        me.setModified(true)
    }

    // Get the rune at the given cursor pos
    mtd getRune(cp: RichEditCursor)->{ char: rune, style: RichEditStyleRef }
    {
        var result: retval
        if cp.lineIndex >= me.lines.count:
            return result
        let len = me.lines[cp.lineIndex].chars.count
        if cp.charPosInLine > len:
            return result
        if cp.charPosInLine == len:
            return {'\n', 0}
        result.char  = me.lines[cp.lineIndex].chars[cp.charPosInLine]
        result.style = me.lines[cp.lineIndex].styles[cp.charPosInLine]
        return result
    }

    // Insert a rune
    mtd insertRune(ch: rune)
    {
        let c = ch

        // A new undo action needs to be regenerate ?
        var forceUndo = false
        var pundo     = me.currentUndo ? me.undoMgr[me.currentUndo - 1] : null
        if !me.hasSelRect and !me.isSelectionEmpty():
            forceUndo = true
        elif me.hasSelRect and me.cursorPos.charPosInLine != me.selBeg.charPosInLine:
            forceUndo = true
        elif !me.currentUndo:
            forceUndo = true
        elif pundo and !pundo.concatChars:
            forceUndo = true
        elif pundo and pundo.cursorPos.lineIndex != me.cursorPos.lineIndex:
            forceUndo = true
        elif pundo and me.cursorPos.charPosInLine != pundo.cursorPos.charPosInLine + pundo.runes.count:
            forceUndo = true

        if forceUndo
        {
            me.resetUndo()

            // Delete actual selection
            if !me.hasSelRect and !me.isSelectionEmpty():
                me.deleteSelection()
            elif me.hasSelRect and me.cursorPos.charPosInLine != me.selBeg.charPosInLine:
                me.deleteSelection()
            else:
                me.serialUndo += 1

            // Register action of deletion for overmode
            if me.overwriteMode and me.cursorPos.charPosInLine != me.lines[me.cursorPos.lineIndex].length()
            {
                pundo                    = Memory.new'RichEditUndo()
                pundo.type               = .Delete
                pundo.concatChars        = true
                pundo.selBeg             = me.selBeg
                pundo.cursorPos          = me.cursorPos
                pundo.serial             = me.serialUndo
                pundo.begSelectionBefore = me.selBeg
                pundo.endSelectionBefore = me.cursorPos
                me.undoMgr.add(pundo)
                me.currentUndo += 1
            }

            // Register action of insertion
            pundo                    = Memory.new'RichEditUndo()
            pundo.type               = .Insert
            pundo.concatChars        = true
            pundo.selBeg             = me.selBeg
            pundo.cursorPos          = me.cursorPos
            pundo.serial             = me.serialUndo
            pundo.begSelectionBefore = me.selBeg
            pundo.endSelectionBefore = me.cursorPos
            pundo.hasSelRect         = me.hasSelRect
            me.undoMgr.add(pundo)
            me.currentUndo += 1
        }

        var az: [4] u8
        let cpt = Utf8.encodeRune(az, c)

        // Add a deletion
        if me.currentUndo > 1
        {
            pundo = me.undoMgr[me.currentUndo - 2]
            if pundo.type == .Delete and pundo.concatChars
            {
                if me.cursorPos.charPosInLine != me.lines[me.cursorPos.lineIndex].length()
                {
                    var r = me.getRune(me.cursorPos)
                    pundo.runes.add(r.char)
                    pundo.styles.add(r.style)
                    discard me.moveCursorRight(&pundo.selBeg)
                    me.deleteTextPrivate(me.cursorPos, pundo.selBeg, false)
                    me.clearSelection()
                }
            }
        }

        // Append the character to the last undo
        pundo = me.undoMgr[me.currentUndo - 1]
        pundo.runes.add(c)

        // Get the style from the previous character
        let line = me.lines[me.cursorPos.lineIndex]
        var st:  u8
        if !me.cursorPos.charPosInLine and line.length():
            st = line.styles[0]
        elif me.cursorPos.charPosInLine and me.cursorPos.charPosInLine - 1 < line.length():
            st = line.styles[me.cursorPos.charPosInLine - 1]
        pundo.styles.add(st)

        // Do action
        if me.hasSelRect
        {
            var sb = me.getBegSelection()
            var se = me.getEndSelection()
            for caretI in sb.lineIndex to se.lineIndex
            {
                var cp = se
                cp.lineIndex = caretI
                if cp.charPosInLine > me.lines[cp.lineIndex].length():
                    continue
                me.insertTextPrivate(&cp, @mkstring(&az[0], cpt), st)
            }

            me.moveCursorRight(&me.selBeg)
            me.moveCursorRight(&me.cursorPos)
        }
        else
        {
            me.insertTextPrivate(&me.cursorPos, @mkstring(&az[0], cpt), st)
            me.clearSelection()
        }

        pundo.begSelectionAfter = me.selBeg
        pundo.endSelectionAfter = me.cursorPos
        me.setModified(true)
    }

    // Insert text at the start of each selected line
    mtd insertBegLine(txt: string)
    {
        var first = true

        var beg = me.getBegSelection()
        var end = me.getEndSelection()

        var savepos0 = me.selBeg
        var savepos1 = me.cursorPos
        beg.charPosInLine = 0

        let multiline = end.lineIndex > beg.lineIndex

        while beg.lineIndex <= end.lineIndex
        {
            if beg.lineIndex == end.lineIndex and end.charPosInLine == 0 and multiline:
                break

            me.pushUndo()
            if !first:
                me.serialUndo -= 1

            me.setCursorPos(beg)
            me.selBeg = beg
            me.insertText(txt)

            me.popUndo()

            if beg.lineIndex == savepos0.lineIndex and savepos0.charPosInLine:
                savepos0.charPosInLine += 1
            if beg.lineIndex == savepos1.lineIndex and savepos1.charPosInLine:
                savepos1.charPosInLine += 1

            beg.lineIndex += 1
            first = false
        }

        me.selBeg = savepos0
        me.setCursorPos(savepos1)

        // A dummy undo just to restore the selection
        if !first
        {
            me.pushUndo()
            me.serialUndo -= 1
            me.undoRestoreSelection()
            me.popUndo()
        }

        me.setModified(true)
    }

    // Delete a given text at the start of each selected line
    mtd deleteBegLine(txt: string, lastLine: bool)
    {
        var arrR   = Unicode.fromUtf8(txt)
        var first  = true
        let lenTxt = arrR.count
        var beg    = me.getBegSelection()
        var end    = me.getEndSelection()

        if !lastLine
        {
            if beg.charPosInLine == 0 and beg.lineIndex == end.lineIndex:
                return
        }
        else
        {
            beg.charPosInLine = 0
        }

        let multiline = beg.lineIndex == end.lineIndex ? false : true
        var size      = me.getXExtent()
        var savepos0  = me.selBeg
        var savepos1  = me.cursorPos

        while beg.lineIndex <= end.lineIndex
        {
            // Go to the first non white character
            let line = me.lines[beg.lineIndex]
            if multiline
            {
                while beg.charPosInLine < line.length() and Unicode.isSpace(line.chars[beg.charPosInLine])
                {
                    if Slice.equals(@mkslice(line.chars.buffer + beg.charPosInLine, lenTxt), arrR.toSlice()):
                        break
                    beg.charPosInLine += 1
                }
            }

            var beg1 = beg
            if beg.charPosInLine < line.length() and Slice.equals(@mkslice(line.chars.buffer + beg.charPosInLine, lenTxt), arrR.toSlice())
            {
                beg1.charPosInLine += lenTxt
            }
            elif line.length() and beg.charPosInLine
            {
                if Unicode.isSpace(line.chars[beg.charPosInLine - 1])
                {
                    // X is the request position
                    var charPt = me.getCursorDisplayPos(beg)
                    var x      = cast(f32) me.getTabOffset(cast(s32) charPt.x)
                    x = (me.tabSize * size.x) - x
                    if x == 0:
                        x = me.tabSize * size.x
                    x = charPt.x - x

                    while charPt.x > x and beg.charPosInLine and Unicode.isSpace(line.chars[beg.charPosInLine - 1])
                    {
                        beg.charPosInLine -= 1
                        charPt = me.getCursorDisplayPos(beg)
                    }
                }
            }

            // Delete
            if beg1.charPosInLine != beg.charPosInLine
            {
                me.pushUndo()
                if !first:
                    me.serialUndo -= 1
                me.selBeg = beg
                me.setCursorPos(beg1)
                me.deleteSelectionPrivate(beg, beg1, false)
                me.popUndo()

                if beg.lineIndex == savepos0.lineIndex and savepos0.charPosInLine:
                    savepos0.charPosInLine -= beg1.charPosInLine - beg.charPosInLine
                if beg.lineIndex == savepos1.lineIndex and savepos1.charPosInLine:
                    savepos1.charPosInLine -= beg1.charPosInLine - beg.charPosInLine
            }

            first = false

            // Next line
            if beg.lineIndex == end.lineIndex:
                break
            beg.lineIndex += 1
            beg.charPosInLine = 0
            if beg.lineIndex == end.lineIndex and end.charPosInLine == 0:
                break
        }

        me.selBeg = savepos0
        me.setCursorPos(savepos1)

        // A dummy undo just to restore the selection
        if !first
        {
            me.pushUndo()
            me.serialUndo -= 1
            me.undoRestoreSelection()
            me.popUndo()
        }

        me.setModified(true)
    }

    // Evaluate the rendering size
    mtd evaluateSize()->{ w: f32, h: f32 }
    {
        var result: retval

        var x = me.getXExtent()

        result.h = cast(f32) (me.lines.count * x.y)
        result.h += cast(f32) ((me.lines.count - 1) * me.lineSpacing)

        let fontR = me.getFont().getFont(me.styles[0].fontMode)
        if fontR
        {
            foreach l in me.lines
            {
                let arr     = Painter.getGlyphs(l.chars, fontR)
                let measure = Painter.measureString(arr)
                result.w = Math.max(result.h, measure.width)
            }
        }

        result.w += me.getMarginLeft() + me.getMarginRight()
        result.h += me.getThemeMetrics().richEdit_Padding * 2
        result.w += me.getThemeMetrics().richEdit_Padding * 2
        return result
    }
}
