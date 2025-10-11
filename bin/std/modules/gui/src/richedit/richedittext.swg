using Core, Pixel

impl RichEditCtrl
{
    mtd getTextPrivate(start, end: RichEditCursor)->{ text: Array'rune, styles: Array'u8 }
    {
        var result: retval
        if !.hasSelRect
        {
            // First line
            var line  = .lines[start.lineIndex]
            var count = line.length()
            if count
            {
                let pchar  = line.chars.buffer
                let pstyle = line.styles.buffer
                if start.lineIndex == end.lineIndex do
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
                for var i = start.lineIndex + 1; i < end.lineIndex; i += 1 do
                    count += .lines[i].length()
                result.text.reserve(result.text.count + count)
                result.styles.reserve(result.styles.count + count)

                for var i = start.lineIndex + 1; i < end.lineIndex; i += 1
                {
                    result.text.add(.lines[i].chars.toSlice())
                    result.styles.add(.lines[i].styles.toSlice())
                    result.text.add('\n')
                    result.styles.add(0)
                }
            }

            // Last line
            if start.lineIndex != end.lineIndex
            {
                line  = .lines[end.lineIndex]
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
                if .getSelectionRectRange(li, &rankBeg, &rankEnd)
                {
                    for i in rankEnd - rankBeg
                    {
                        result.text.add(.lines[li].chars[rankBeg + i])
                        result.styles.add(.lines[li].styles[rankBeg + i])
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
        if !(.richEdFlags & .WordWrap) do
            return

        // Make one single line with current wrapped lines
        var i = lineIndex + 1
        while i < .lines.count
        {
            if !.lines[i].wordWrap do
                break

            if .cursorPos.lineIndex == i
            {
                .cursorPos.lineIndex = lineIndex
                .cursorPos.charPosInLine += .lines[lineIndex].chars.count
            }

            if .selBeg.lineIndex == i
            {
                .selBeg.lineIndex = lineIndex
                .selBeg.charPosInLine += .lines[lineIndex].chars.count
            }

            .lines[lineIndex].chars.add(.lines[i].chars.toSlice())
            .lines[lineIndex].styles.add(.lines[i].styles.toSlice())
            i += 1
        }

        // Remove current wrapped lines
        while i != lineIndex + 1
        {
            .deleteLine(lineIndex + 1)
            i -= 1
        }
    }

    mtd wrap(lineIndex: u64)
    {
        if !(.richEdFlags & .WordWrap) do
            return

        let fontR = .getFont().getFont(.styles[0].fontMode)
        if !fontR do
            return

        // Do the wrap
        var curLine    = lineIndex
        var i          = 0'u64
        var clientRect = .edView.getClientRect()
        clientRect.width -= .getMarginRight()

        var str: String

        var cutAti = i
        while i < .lines[curLine].length()
        {
            let l  = .lines[curLine]
            let ch = l.chars[i]
            str += ch

            if !Unicode.isLetterOrDigit(ch) do
                cutAti = i + 1

            if cutAti > 1
            {
                let x = .getMarginLeft() + Painter.measureString(str, fontR).width
                if x > clientRect.right()
                {
                    if cutAti and cutAti == i + 1 do
                        cutAti -= 1

                    if .cursorPos.lineIndex == curLine and .cursorPos.charPosInLine >= cutAti
                    {
                        .cursorPos.lineIndex += 1
                        .cursorPos.charPosInLine -= cutAti
                    }

                    if .selBeg.lineIndex == curLine and .selBeg.charPosInLine >= cutAti
                    {
                        .selBeg.lineIndex += 1
                        .selBeg.charPosInLine -= cutAti
                    }

                    i = cutAti
                    let newL = .newLine(curLine + 1)
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
        if c == '\r' do
            return

        if c == '\n'
        {
            let linep = .lines[cp.lineIndex]
            discard .newLine(cp.lineIndex + 1)
            let line = .lines[cp.lineIndex + 1]
            if cp.charPosInLine < linep.length() do
                line.insertRunes(0, linep, cp.charPosInLine, linep.length() - cp.charPosInLine)
            linep.setLength(cp.charPosInLine)
            cp.lineIndex += 1
            cp.charPosInLine = 0
        }
        elif c == '\t' and (.richEdFlags & .TabToSpaces)
        {
            let line = .lines[cp.lineIndex]
            for .tabSize
            {
                let posIns = Math.min(cp.charPosInLine, line.length())
                line.insertRune(posIns, ' ', txtStyle)
                cp.charPosInLine += 1
            }
        }
        else
        {
            let line   = .lines[cp.lineIndex]
            let posIns = Math.min(cp.charPosInLine, line.length())
            line.insertRune(posIns, c, txtStyle)
            cp.charPosInLine += 1
        }

        .recomputeScroll = true
    }

    #[Swag.Overload]
    mtd insertTextPrivate(cp: *RichEditCursor, runes: Array'rune, nstyles: Array'u8, selRect: bool)
    {
        if runes.isEmpty() do
            return
        Debug.assert(runes.count == nstyles.count)

        if !selRect
        {
            for i in runes
            {
                let c = runes[i]
                let s = nstyles[i]
                .insertRunePrivate(cp, c, s)
            }
        }
        else
        {
            var lineIndex = cp.lineIndex
            var posInLine = cp.charPosInLine

            var rankBeg, rankEnd: u64
            for
            {
                let line = .lines[lineIndex]
                if rankEnd == runes.count or runes[rankEnd] == '\n'
                {
                    var posIns = posInLine
                    if posIns > line.length() do
                        posIns = line.length()
                    line.insertRunes(posIns, @mkslice(runes.buffer + rankBeg, rankEnd - rankBeg), @mkslice(nstyles.buffer + rankBeg, rankEnd - rankBeg))
                    if rankEnd == runes.count do
                        break
                    if rankEnd >= runes.count - 1 do
                        break

                    lineIndex += 1
                    if lineIndex == .lines.count
                    {
                        .newLine(lineIndex)
                        posInLine = 0
                    }

                    rankEnd += 1
                    rankBeg = rankEnd
                    continue
                }

                if rankEnd == runes.count do
                    break
                rankEnd += 1
            }

            cp.lineIndex     = lineIndex
            cp.charPosInLine = posInLine + (rankEnd - rankBeg)
        }

        .recomputeScroll = true
    }

    #[Swag.Overload]
    mtd insertTextPrivate(cp: *RichEditCursor, text: string, txtStyle: RichEditStyleRef)
    {
        if .lines.isEmpty() do
            .newLine(0)
        elif cp.lineIndex == .lines.count do
            .newLine(cp.lineIndex)

        var str = text
        for
        {
            let (c, eat) = Utf8.decodeRune(str)
            if !eat do
                break
            str = Tokenize.eatCount(str, cast(u64) eat)
            .insertRunePrivate(cp, c, txtStyle)
        }

        .recomputeScroll = true
    }

    mtd deleteTextPrivate(posStart, posEnd: RichEditCursor, selRect: bool)
    {
        // Nothing to delete
        if posStart.lineIndex == posEnd.lineIndex and posStart.charPosInLine == posEnd.charPosInLine do
            return

        if !selRect
        {
            let ps = posStart
            var pe = posEnd

            // Remove inside lines
            while pe.lineIndex > ps.lineIndex + 1
            {
                .deleteLine(ps.lineIndex + 1)
                pe.lineIndex -= 1
            }

            let lineSrc = .lines[ps.lineIndex]
            let lineEnd = .lines[pe.lineIndex]

            if ps.lineIndex == pe.lineIndex
            {
                lineSrc.removeRunes(ps.charPosInLine, pe.charPosInLine - ps.charPosInLine)
            }
            else
            {
                lineSrc.removeRunes(ps.charPosInLine, lineSrc.length() - ps.charPosInLine)
                lineEnd.removeRunes(0, pe.charPosInLine)
                if lineEnd.length() do
                    lineSrc.insertRunes(ps.charPosInLine, lineEnd, 0, lineEnd.length())
                .deleteLine(pe.lineIndex)
            }
        }
        else
        {
            var ps = posStart
            var pe = posEnd
            if ps.lineIndex > pe.lineIndex do
                swap(&ps, &pe)

            var li = ps.lineIndex
            while li <= pe.lineIndex
            {
                var rankBeg, rankEnd: u64
                if .getSelectionRectRange(li, &rankBeg, &rankEnd)
                {
                    .lines[li].removeRunes(rankBeg, rankEnd - rankBeg)
                }

                li += 1
            }
        }

        .recomputeScroll = true
    }

    func isWord(c: rune)->bool
    {
        return Unicode.isLetterOrDigit(c) or c == '_'
    }

    func zapWordChar(c: rune)->bool
    {
        if isWord(c) do
            return false
        if Unicode.isSpace(c) do
            return false
        if c != '(' and c != ')' and c != '[' and c != ']' and c != '{' and c != '}' do
            return true
        return false
    }
}

public impl RichEditCtrl
{
    // Clear content
    mtd clear()
    {
        .clearUndo()
        foreach l in .lines do
            .freeLines.add(l)
        .lines.clear()
        .countVisibleLines    = 0
        .cursorPos            = {}
        .selBeg               = {}
        .dirtyFirstLineView   = true
        .firstVisibleLineView = 0
        .lastVisibleLineView  = 0
    }

    // Get the selected text
    mtd getSelectedText()->String
    {
        let res = .getTextPrivate(.getBegSelection(), .getEndSelection())
        let txt = Utf8.fromUnicode(res.text.toSlice())
        return txt
    }

    // Get the full text
    mtd getText()->String
    {
        var sel0: RichEditCursor
        var sel1: RichEditCursor
        .moveCursorStartFile(&sel0)
        .moveCursorEndFile(&sel1)
        let res = .getTextPrivate(sel0, sel1)
        let txt = Utf8.fromUnicode(res.text.toSlice())
        return txt
    }

    // Get the full text with the associated styles per character
    mtd getTextAndStyles()->{ text: Core.Array'rune, styles: Core.Array'u8 }
    {
        var sel0: RichEditCursor
        var sel1: RichEditCursor
        .moveCursorStartFile(&sel0)
        .moveCursorEndFile(&sel1)
        return .getTextPrivate(sel0, sel1)
    }

    // Set the text content
    mtd setText(text: string)
    {
        .clear()

        var cp: RichEditCursor
        .insertTextPrivate(&cp, text, 0)

        .resetModified()
        .invalidate()
    }

    // Append a text with a given style
    mtd addText(text: string, txtStyle: RichEditStyleRef = 0)
    {
        var cp: RichEditCursor
        if .lines.count
        {
            cp.lineIndex     = .lines.count - 1
            cp.charPosInLine = .lines[cp.lineIndex].length()
        }

        .insertTextPrivate(&cp, text, txtStyle)
        .setModified(true)
    }

    // Insert a text with a given style
    #[Swag.Overload]
    mtd insertText(text: string)
    {
        let undo = Memory.new'RichEditUndo()
        undo.type  = .Insert
        undo.runes = Unicode.fromUtf8(text)
        undo.styles.resize(undo.runes.count)
        undo.selBeg     = .selBeg
        undo.cursorPos  = .cursorPos
        undo.hasSelRect = .hasSelRect
        undo.serial     = 0
        .resetUndo()
        .toUndo.add(undo)
        .insertTextPrivate(&.cursorPos, text, 0)
        .clearSelection()
        .setModified(true)
    }

    // Insert a text with a style per character
    #[Swag.Overload]
    mtd insertText(text: string, txtStyles: Array'u8)
    {
        let undo = Memory.new'RichEditUndo()
        undo.type       = .Insert
        undo.runes      = Unicode.fromUtf8(text)
        undo.styles     = txtStyles
        undo.selBeg     = .selBeg
        undo.cursorPos  = .cursorPos
        undo.hasSelRect = .hasSelRect
        undo.serial     = 0
        .resetUndo()
        .toUndo.add(undo)
        .insertTextPrivate(&.cursorPos, undo.runes, undo.styles, .hasSelRect)
        .clearSelection()
        .setModified(true)
    }

    // Get the rune at the given cursor pos
    mtd getRune(cp: RichEditCursor)->{ char: rune, style: RichEditStyleRef }
    {
        var result: retval
        if cp.lineIndex >= .lines.count do
            return result
        let len = .lines[cp.lineIndex].chars.count
        if cp.charPosInLine > len do
            return result
        if cp.charPosInLine == len do
            return {'\n', 0}
        result.char  = .lines[cp.lineIndex].chars[cp.charPosInLine]
        result.style = .lines[cp.lineIndex].styles[cp.charPosInLine]
        return result
    }

    // Insert a rune
    mtd insertRune(ch: rune)
    {
        let c = ch

        // A new undo action needs to be regenerate ?
        var forceUndo = false
        var pundo     = .currentUndo ? .undoMgr[.currentUndo - 1] : null
        if !.hasSelRect and !.isSelectionEmpty() do
            forceUndo = true
        elif .hasSelRect and .cursorPos.charPosInLine != .selBeg.charPosInLine do
            forceUndo = true
        elif !.currentUndo do
            forceUndo = true
        elif pundo and !pundo.concatChars do
            forceUndo = true
        elif pundo and pundo.cursorPos.lineIndex != .cursorPos.lineIndex do
            forceUndo = true
        elif pundo and .cursorPos.charPosInLine != pundo.cursorPos.charPosInLine + pundo.runes.count do
            forceUndo = true

        if forceUndo
        {
            .resetUndo()

            // Delete actual selection
            if !.hasSelRect and !.isSelectionEmpty() do
                .deleteSelection()
            elif .hasSelRect and .cursorPos.charPosInLine != .selBeg.charPosInLine do
                .deleteSelection()
            else do
                .serialUndo += 1

            // Register action of deletion for overmode
            if .overwriteMode and .cursorPos.charPosInLine != .lines[.cursorPos.lineIndex].length()
            {
                pundo                    = Memory.new'RichEditUndo()
                pundo.type               = .Delete
                pundo.concatChars        = true
                pundo.selBeg             = .selBeg
                pundo.cursorPos          = .cursorPos
                pundo.serial             = .serialUndo
                pundo.begSelectionBefore = .selBeg
                pundo.endSelectionBefore = .cursorPos
                .undoMgr.add(pundo)
                .currentUndo += 1
            }

            // Register action of insertion
            pundo                    = Memory.new'RichEditUndo()
            pundo.type               = .Insert
            pundo.concatChars        = true
            pundo.selBeg             = .selBeg
            pundo.cursorPos          = .cursorPos
            pundo.serial             = .serialUndo
            pundo.begSelectionBefore = .selBeg
            pundo.endSelectionBefore = .cursorPos
            pundo.hasSelRect         = .hasSelRect
            .undoMgr.add(pundo)
            .currentUndo += 1
        }

        var az: [4] u8
        let cpt = Utf8.encodeRune(az, c)

        // Add a deletion
        if .currentUndo > 1
        {
            pundo = .undoMgr[.currentUndo - 2]
            if pundo.type == .Delete and pundo.concatChars
            {
                if .cursorPos.charPosInLine != .lines[.cursorPos.lineIndex].length()
                {
                    let r = .getRune(.cursorPos)
                    pundo.runes.add(r.char)
                    pundo.styles.add(r.style)
                    discard .moveCursorRight(&pundo.selBeg)
                    .deleteTextPrivate(.cursorPos, pundo.selBeg, false)
                    .clearSelection()
                }
            }
        }

        // Append the character to the last undo
        pundo = .undoMgr[.currentUndo - 1]
        pundo.runes.add(c)

        // Get the style from the previous character
        let line = .lines[.cursorPos.lineIndex]
        var st:  u8
        if !.cursorPos.charPosInLine and line.length() do
            st = line.styles[0]
        elif .cursorPos.charPosInLine and .cursorPos.charPosInLine - 1 < line.length() do
            st = line.styles[.cursorPos.charPosInLine - 1]
        pundo.styles.add(st)

        // Do action
        if .hasSelRect
        {
            let sb = .getBegSelection()
            let se = .getEndSelection()
            for caretI in sb.lineIndex to se.lineIndex
            {
                var cp = se
                cp.lineIndex = caretI
                if cp.charPosInLine > .lines[cp.lineIndex].length() do
                    continue
                .insertTextPrivate(&cp, @mkstring(&az[0], cpt), st)
            }

            .moveCursorRight(&.selBeg)
            .moveCursorRight(&.cursorPos)
        }
        else
        {
            .insertTextPrivate(&.cursorPos, @mkstring(&az[0], cpt), st)
            .clearSelection()
        }

        pundo.begSelectionAfter = .selBeg
        pundo.endSelectionAfter = .cursorPos
        .setModified(true)
    }

    // Insert text at the start of each selected line
    mtd insertBegLine(txt: string)
    {
        var first = true

        var beg = .getBegSelection()
        let end = .getEndSelection()

        var savepos0 = .selBeg
        var savepos1 = .cursorPos
        beg.charPosInLine = 0

        let multiline = end.lineIndex > beg.lineIndex

        while beg.lineIndex <= end.lineIndex
        {
            if beg.lineIndex == end.lineIndex and end.charPosInLine == 0 and multiline do
                break

            .pushUndo()
            if !first do
                .serialUndo -= 1

            .setCursorPos(beg)
            .selBeg = beg
            .insertText(txt)

            .popUndo()

            if beg.lineIndex == savepos0.lineIndex and savepos0.charPosInLine do
                savepos0.charPosInLine += 1
            if beg.lineIndex == savepos1.lineIndex and savepos1.charPosInLine do
                savepos1.charPosInLine += 1

            beg.lineIndex += 1
            first = false
        }

        .selBeg = savepos0
        .setCursorPos(savepos1)

        // A dummy undo just to restore the selection
        if !first
        {
            .pushUndo()
            .serialUndo -= 1
            .undoRestoreSelection()
            .popUndo()
        }

        .setModified(true)
    }

    // Delete a given text at the start of each selected line
    mtd deleteBegLine(txt: string, lastLine: bool)
    {
        var arrR   = Unicode.fromUtf8(txt)
        var first  = true
        let lenTxt = arrR.count
        var beg    = .getBegSelection()
        let end    = .getEndSelection()

        if !lastLine
        {
            if beg.charPosInLine == 0 and beg.lineIndex == end.lineIndex do
                return
        }
        else
        {
            beg.charPosInLine = 0
        }

        let multiline = beg.lineIndex == end.lineIndex ? false : true
        let size      = .getXExtent()
        var savepos0  = .selBeg
        var savepos1  = .cursorPos

        while beg.lineIndex <= end.lineIndex
        {
            // Go to the first non white character
            let line = .lines[beg.lineIndex]
            if multiline
            {
                while beg.charPosInLine < line.length() and Unicode.isSpace(line.chars[beg.charPosInLine])
                {
                    if Slice.equals(@mkslice(line.chars.buffer + beg.charPosInLine, lenTxt), arrR.toSlice()) do
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
                    var charPt = .getCursorDisplayPos(beg)
                    var x      = cast(f32) .getTabOffset(cast(s32) charPt.x)
                    x = (.tabSize * size.x) - x
                    if x == 0 do
                        x = .tabSize * size.x
                    x = charPt.x - x

                    while charPt.x > x and beg.charPosInLine and Unicode.isSpace(line.chars[beg.charPosInLine - 1])
                    {
                        beg.charPosInLine -= 1
                        charPt = .getCursorDisplayPos(beg)
                    }
                }
            }

            // Delete
            if beg1.charPosInLine != beg.charPosInLine
            {
                .pushUndo()
                if !first do
                    .serialUndo -= 1
                .selBeg = beg
                .setCursorPos(beg1)
                .deleteSelectionPrivate(beg, beg1, false)
                .popUndo()

                if beg.lineIndex == savepos0.lineIndex and savepos0.charPosInLine do
                    savepos0.charPosInLine -= beg1.charPosInLine - beg.charPosInLine
                if beg.lineIndex == savepos1.lineIndex and savepos1.charPosInLine do
                    savepos1.charPosInLine -= beg1.charPosInLine - beg.charPosInLine
            }

            first = false

            // Next line
            if beg.lineIndex == end.lineIndex do
                break
            beg.lineIndex += 1
            beg.charPosInLine = 0
            if beg.lineIndex == end.lineIndex and end.charPosInLine == 0 do
                break
        }

        .selBeg = savepos0
        .setCursorPos(savepos1)

        // A dummy undo just to restore the selection
        if !first
        {
            .pushUndo()
            .serialUndo -= 1
            .undoRestoreSelection()
            .popUndo()
        }

        .setModified(true)
    }

    // Evaluate the rendering size
    mtd evaluateSize()->{ w: f32, h: f32 }
    {
        var result: retval

        let x = .getXExtent()

        result.h = cast(f32) (.lines.count * x.y)
        result.h += cast(f32) ((.lines.count - 1) * .lineSpacing)

        let fontR = .getFont().getFont(.styles[0].fontMode)
        if fontR
        {
            foreach l in .lines
            {
                let arr     = Painter.getGlyphs(l.chars, fontR)
                let measure = Painter.measureString(arr)
                result.w = Math.max(result.h, measure.width)
            }
        }

        result.w += .getMarginLeft() + .getMarginRight()
        result.h += .getThemeMetrics().richEdit_Padding * 2
        result.w += .getThemeMetrics().richEdit_Padding * 2
        return result
    }
}
