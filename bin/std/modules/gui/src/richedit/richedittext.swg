using Core, Pixel

impl RichEdit
{
    mtd getTextPrivate(start, end: RichEditCursor, selRect: bool)->{text: Array'rune, styles: Array'u8}
    {
        var result: retval
        if !hasSelRect
        {
            // First line
            line := lines[start.lineIndex]
            count := line.length()
            if count
            {
                pchar := line.chars.buffer
                pstyle := line.styles.buffer
                if start.lineIndex == end.lineIndex
                    count = end.charPosInLine
                for i := start.charPosInLine; i < count; i += 1
                {
                    result.text.add(pchar[i])
                    result.styles.add(pstyle[i])
                }
            }

            if start.lineIndex != end.lineIndex
            {
                result.text.add("\n"'rune)
                result.styles.add(0)
            }

            // Inside lines
            if end.lineIndex > start.lineIndex + 1
            {
                // Pre allocate string to optimize append
                count = 0
                for i := start.lineIndex + 1; i < end.lineIndex; i += 1
                    count += lines[i].length()
                result.text.reserve(result.text.count + count)
                result.styles.reserve(result.styles.count + count)

                for i := start.lineIndex + 1; i < end.lineIndex; i += 1
                {
                    result.text.add(lines[i].chars.toSlice())
                    result.styles.add(lines[i].styles.toSlice())
                    result.text.add("\n"'rune)
                    result.styles.add(0)
                }
            }

            // Last line
            if start.lineIndex != end.lineIndex
            {
                line = lines[end.lineIndex]
                count = line.length()
                if count
                {
                    pchar := line.chars.buffer
                    pstyle := line.styles.buffer
                    for i := 0; i < end.charPosInLine; i += 1
                    {
                        result.text.add(pchar[i])
                        result.styles.add(pstyle[i])
                    }
                }
            }
        }
        else
        {
            li := start.lineIndex
            while li <= end.lineIndex
            {
                var rankBeg, rankEnd: uint
                if getSelelectionRectRange(li, &rankBeg, &rankEnd)
                {
                    loop i: rankEnd - rankBeg
                    {
                        result.text.add(lines[li].chars[rankBeg + i])
                        result.styles.add(lines[li].styles[rankBeg + i])
                    }
                }

                result.text.add("\n"'rune)
                result.styles.add(0)
                li += 1
            }
        }

        return result
    }

    mtd insertRunePrivate(cp: *RichEditCursor, c: rune, txtStyle: u8)
    {
        if c == "\r"'rune
            return
        if c == "\n"'rune
        {
            discard newLine(cp.lineIndex + 1)
            linep := lines[cp.lineIndex]
            line  := lines[cp.lineIndex + 1]
            if cp.charPosInLine < linep.length()
                line.insertRunes(0, linep, cp.charPosInLine, linep.length() - cp.charPosInLine)
            linep.setLength(cp.charPosInLine)
            cp.lineIndex += 1
            cp.charPosInLine = 0
        }
        else
        {
            line := lines[cp.lineIndex]
            posIns := Math.min(cp.charPosInLine, line.length())
            line.insertRune(posIns, c, txtStyle)
            cp.charPosInLine += 1
        }
    }

    mtd insertTextPrivate(cp: *RichEditCursor, runes: Array'rune, styles: Array'u8, selRect: bool)
    {
        if runes.isEmpty()
            return
        Debug.assert(runes.count == #self.styles.count)

        if !selRect
        {
            loop i: #self.runes
            {
                c := #self.runes[i]
                s := #self.styles[i]
                insertRunePrivate(cp, c, s)
            }
        }
        else
        {
            lineIndex := cp.lineIndex
            posInLine := cp.charPosInLine

            var rankBeg, rankEnd: uint
            loop
            {
                line := lines[lineIndex];
                if rankEnd == runes.count or runes[rankEnd] == "\n"'rune
                {
                    posIns := posInLine
                    if posIns > line.length()
                        posIns = line.length()
                    line.insertRunes(posIns, @mkslice(#self.runes.buffer + rankBeg, rankEnd - rankBeg), @mkslice(#self.styles.buffer + rankBeg, rankEnd - rankBeg))
                    if rankEnd == runes.count
                        break
                    if rankEnd >= runes.count - 1
                        break

                    lineIndex += 1
                    if lineIndex == lines.count
                    {
                        newLine(lineIndex)
                        posInLine = 0
                    }

                    rankEnd += 1
                    rankBeg = rankEnd
                    continue
                }

                if rankEnd == runes.count
                    break
                rankEnd += 1
            }

            cp.lineIndex = lineIndex
            cp.charPosInLine = posInLine + (rankEnd - rankBeg)
        }
    }

    mtd insertTextPrivate(cp: *RichEditCursor, text: string, txtStyle: u8, selRect: bool)
    {
        if lines.isEmpty()
            newLine(0)
        elif cp.lineIndex == lines.count
            newLine(cp.lineIndex)

        str  := text
        line := lines[cp.lineIndex]
        loop
        {
            (c, eat) := Utf8.decodeRune(str)
            if !eat
                break
            str = Tokenize.eatCount(str, cast(uint) eat)
            insertRunePrivate(cp, c, txtStyle)
        }

        setModified(true)
    }

    mtd insertTextPrivate(text: string, style: u8, selRect: bool)
    {
        undo := Memory.new'RichEditUndo()
        undo.type       = .Insert
        undo.runes      = Unicode.fromUtf8(text)
        undo.styles.resize(undo.runes.count)
        undo.selBeg     = selBeg
        undo.cursorPos  = cursorPos
        undo.hasSelRect = selRect
        undo.serial     = 0
        resetUndo()
        toUndo.add(undo)
        insertTextPrivate(&cursorPos, text, 0, selRect)
        clearSelection()
    }

    mtd deleteTextPrivate(posStart, posEnd: RichEditCursor, selRect: bool)
    {
        // Nothing to delete
        if posStart.lineIndex == posEnd.lineIndex and posStart.charPosInLine == posEnd.charPosInLine
            return

        if !selRect
        {
            ps := posStart
            pe := posEnd

            // Remove inside lines
            while pe.lineIndex > ps.lineIndex + 1
            {
                deleteLine(ps.lineIndex + 1)
                pe.lineIndex -= 1
            }

            lineSrc := lines[ps.lineIndex]
            lineEnd := lines[pe.lineIndex]

            if ps.lineIndex == pe.lineIndex
            {
                lineSrc.removeRunes(ps.charPosInLine, pe.charPosInLine - ps.charPosInLine)
            }
            else
            {
                lineSrc.removeRunes(ps.charPosInLine, lineSrc.length() - ps.charPosInLine)
                lineEnd.removeRunes(0, pe.charPosInLine)
                if lineEnd.length()
                    lineSrc.insertRunes(ps.charPosInLine, lineEnd, 0, lineEnd.length())
                deleteLine(pe.lineIndex)
            }
        }
        else
        {
            ps := posStart
            pe := posEnd
            if ps.lineIndex > pe.lineIndex
                swap(&ps, &pe)

            li := ps.lineIndex
            while li <= pe.lineIndex
            {
                var rankBeg, rankEnd: uint
                if getSelelectionRectRange(li, &rankBeg, &rankEnd)
                {
                    lines[li].removeRunes(rankBeg, rankEnd - rankBeg)
                }

                li += 1
            }
        }

        setModified(true)
    }

    func isWord(c: rune)->bool
    {
        return Unicode.isLetterOrDigit(c) or c == "_"'rune
    }

    func zapWordChar(c: rune)->bool
    {
        if isWord(c)
            return false
        if Unicode.isSpace(c)
            return false
        if c != "("'rune and c != ")"'rune and c != "["'rune and c != "]"'rune and c != "{"'rune and c != "}"'rune
            return true
        return false
    }
}

public impl RichEdit
{
    mtd clear()
    {
        clearUndo()
        visit l: lines
            freeLines.add(l)
        lines.clear()
        countVisibleLines = 0
        cursorPos = @{}
        selBeg = @{}
        dirtyFirstLineView = true
        firstVisibleLineView = 0
        lastVisibleLineView = 0
    }

    mtd setText(text: string)
    {
        clear()

        var cp: RichEditCursor
        insertTextPrivate(&cp, text, 0, false)

        resetModified()
        invalidate()
    }

    mtd addText(text: string, txtStyle: u8)
    {
        var cp: RichEditCursor
        if lines.count
        {
            cp.lineIndex = lines.count - 1
            cp.charPosInLine = lines[cp.lineIndex].length()
        }

        insertTextPrivate(&cp, text, txtStyle, false)
        setModified(true)
        invalidate()
    }

    mtd getRune(cp: RichEditCursor)->{char: rune, style: u8}
    {
        var result: retval
        if cp.lineIndex >= lines.count
            return result
        len := lines[cp.lineIndex].chars.count
        if cp.charPosInLine > len
            return result
        if cp.charPosInLine == len
            return @{"\n"'rune, 0};
        result.char = lines[cp.lineIndex].chars[cp.charPosInLine]
        result.style = lines[cp.lineIndex].styles[cp.charPosInLine]
        return result
    }

    mtd insertRune(ch: rune)
    {
        if isReadOnly()
            return

        c := ch

        // A new undo action needs to be regenerate ?
        forceUndo := false
        pundo := currentUndo ? undoMgr[currentUndo - 1] : null
        if !hasSelRect and !isSelectionEmpty()
            forceUndo = true
        elif hasSelRect and cursorPos.charPosInLine != selBeg.charPosInLine
            forceUndo = true
        elif !currentUndo
            forceUndo = true
        elif pundo and !pundo.concatChars
            forceUndo = true
        elif pundo and pundo.cursorPos.lineIndex != cursorPos.lineIndex
            forceUndo = true
        elif pundo and cursorPos.charPosInLine != pundo.cursorPos.charPosInLine + pundo.runes.count
            forceUndo = true

        if forceUndo
        {
            resetUndo()

            // Delete actual selection
            if !hasSelRect and !isSelectionEmpty()
                deleteSelection()
            elif hasSelRect and cursorPos.charPosInLine != selBeg.charPosInLine
                deleteSelection()
            else
                serialUndo += 1

            // Register action of deletion for overmode
            if overwriteMode and cursorPos.charPosInLine != lines[cursorPos.lineIndex].length()
            {
                pundo = Memory.new'RichEditUndo()
                pundo.type               = .Delete
                pundo.concatChars        = true
                pundo.selBeg             = selBeg
                pundo.cursorPos          = cursorPos
                pundo.serial             = serialUndo
                pundo.begSelectionBefore = selBeg
                pundo.endSelectionBefore = cursorPos
                undoMgr.add(pundo)
                currentUndo += 1
            }

            // Register action of insertion
            pundo = Memory.new'RichEditUndo()
            pundo.type               = .Insert
            pundo.concatChars        = true
            pundo.selBeg             = selBeg
            pundo.cursorPos          = cursorPos
            pundo.serial             = serialUndo
            pundo.begSelectionBefore = selBeg
            pundo.endSelectionBefore = cursorPos
            pundo.hasSelRect         = hasSelRect
            undoMgr.add(pundo)
            currentUndo += 1
        }

        var az: [4] u8
        cpt := Utf8.encodeRune(az, c)

        // Add a deletion
        if currentUndo > 1
        {
            pundo = undoMgr[currentUndo - 2]
            if pundo.type == .Delete and pundo.concatChars
            {
                if cursorPos.charPosInLine != lines[cursorPos.lineIndex].length()
                {
                    r := getRune(cursorPos)
                    pundo.runes.add(r.char)
                    pundo.styles.add(r.style)
                    discard moveCursorRight(&pundo.selBeg)
                    deleteTextPrivate(cursorPos, pundo.selBeg, false)
                    clearSelection()
                }
            }
        }

        // Append the character to the last undo
        pundo = undoMgr[currentUndo - 1]
        pundo.runes.add(c)

        // Get the style from the previous character
        line := lines[cursorPos.lineIndex]
        var st: u8
        if !cursorPos.charPosInLine and line.length()
            st = line.styles[0]
        elif cursorPos.charPosInLine and cursorPos.charPosInLine - 1 < line.length()
            st = line.styles[cursorPos.charPosInLine - 1]
        pundo.styles.add(st)

        // Do action
        if hasSelRect
        {
            sb := getBegSelection()
            se := getEndSelection()
            loop caretI: sb.lineIndex..se.lineIndex
            {
                cp := se
                cp.lineIndex = caretI
                if cp.charPosInLine > lines[cp.lineIndex].length()
                    continue
                insertTextPrivate(&cp, @mkstring(&az[0], cpt), st, false)
            }

            moveCursorRight(&selBeg)
            moveCursorRight(&cursorPos)
        }
        else
        {
            insertTextPrivate(&cursorPos, @mkstring(&az[0], cpt), st, false)
            clearSelection()
        }

        pundo.begSelectionAfter = selBeg
        pundo.endSelectionAfter = cursorPos
        invalidate()
    }

    mtd insertBegLine(txt: string, lastLine: bool)
    {
        if isReadOnly()
            return

        first := true

        beg := getBegSelection()
        end := getEndSelection()

        savepos0 := selBeg
        savepos1 := cursorPos
        beg.charPosInLine = 0

        multiline := end.lineIndex > beg.lineIndex

        while beg.lineIndex <= end.lineIndex
        {
            if beg.lineIndex == end.lineIndex and end.charPosInLine == 0 and multiline
                break

            pushUndo()
            if !first
                serialUndo -= 1

            setCursorPos(beg)
            selBeg = beg
            insertTextPrivate(txt, 0, false)

            popUndo()

            if beg.lineIndex == savepos0.lineIndex and savepos0.charPosInLine
                savepos0.charPosInLine += 1
            if beg.lineIndex == savepos1.lineIndex and savepos1.charPosInLine
                savepos1.charPosInLine += 1

            beg.lineIndex += 1
            first = false
        }

        selBeg = savepos0
        setCursorPos(savepos1)

        // A dummy undo just to restore the selection
        if !first
        {
            pushUndo()
            serialUndo -= 1
            undoRestoreSelection()
            popUndo()
        }
    }

    mtd deleteBegLine(txt: string, lastLine: bool)
    {
        if isReadOnly()
            return

        arrR   := Unicode.fromUtf8(txt)
        first  := true
        lenTxt := arrR.count
        beg    := getBegSelection()
        end    := getEndSelection()

        if !lastLine
        {
            if beg.charPosInLine == 0 and beg.lineIndex == end.lineIndex
                return
        }
        else
        {
            beg.charPosInLine = 0
        }

        multiline := beg.lineIndex == end.lineIndex ? false : true
        size      := getXExtent()
        savepos0  := selBeg
        savepos1  := cursorPos

        while beg.lineIndex <= end.lineIndex
        {
            // Go to the first non white character
            line := lines[beg.lineIndex]
            if multiline
            {
                while beg.charPosInLine < line.length() and Unicode.isSpace(line.chars[beg.charPosInLine])
                {
                    if Slice.equals(@mkslice(line.chars.buffer + beg.charPosInLine, lenTxt), arrR.toSlice())
                        break;
                    beg.charPosInLine += 1
                }
            }

            beg1 := beg
            if beg.charPosInLine < line.length() and Slice.equals(@mkslice(line.chars.buffer + beg.charPosInLine, lenTxt), arrR.toSlice())
            {
                beg1.charPosInLine += lenTxt
            }
            else if line.length() and beg.charPosInLine
            {
                if Unicode.isSpace(line.chars[beg.charPosInLine - 1])
                {
                    // X is the request position
                    charPt := getCursorDisplayPos(beg)
                    x := cast(f32) getTabOffset(cast(s32) charPt.x)
                    x = (tabSize * size.x) - x
                    if x == 0
                        x = tabSize * size.x
                    x = charPt.x - x

                    while charPt.x > x and beg.charPosInLine and Unicode.isSpace(line.chars[beg.charPosInLine - 1])
                    {
                        beg.charPosInLine -= 1
                        charPt = getCursorDisplayPos(beg)
                    }
                }
            }

            // Delete
            if beg1.charPosInLine != beg.charPosInLine
            {
                pushUndo()
                if !first
                    serialUndo -= 1
                selBeg = beg
                setCursorPos(beg1)
                deleteSelectionPrivate(beg, beg1, false)
                popUndo()

                if beg.lineIndex == savepos0.lineIndex and savepos0.charPosInLine
                    savepos0.charPosInLine -= beg1.charPosInLine - beg.charPosInLine
                if beg.lineIndex == savepos1.lineIndex and savepos1.charPosInLine
                    savepos1.charPosInLine -= beg1.charPosInLine - beg.charPosInLine
            }

            first = false

            // Next line
            if beg.lineIndex == end.lineIndex
                break
            beg.lineIndex += 1
            beg.charPosInLine = 0
            if beg.lineIndex == end.lineIndex and end.charPosInLine == 0
                break
        }

        selBeg = savepos0
        setCursorPos(savepos1)

        // A dummy undo just to restore the selection
        if !first
        {
            pushUndo()
            serialUndo -= 1
            undoRestoreSelection()
            popUndo()
        }
    }
}