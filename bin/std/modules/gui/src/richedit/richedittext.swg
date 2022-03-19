using Core, Pixel

impl RichEdit
{
    mtd getTextPrivate(start, end: RichEditCursor, selRect: bool)->Array'rune
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
                if start.lineIndex == end.lineIndex
                    count = end.charPosInLine
                for i := start.charPosInLine; i < count; i += 1
                    result.add(pchar[i])
            }

            if start.lineIndex != end.lineIndex
                result.add("\n"'rune)

            // Inside lines
            if end.lineIndex > start.lineIndex + 1
            {
                // Pre allocate string to optimize append
                count = 0
                for i := start.lineIndex + 1; i < end.lineIndex; i += 1
                    count += lines[i].length()
                result.reserve(result.count + count)

                for i := start.lineIndex + 1; i < end.lineIndex; i += 1
                {
                    result.add(lines[i].chars.toSlice())
                    result.add("\n"'rune)
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
                    for i := 0; i < end.charPosInLine; i += 1
                        result.add(pchar[i])
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
                        result.add(lines[li].chars[rankBeg + i])
                }

                result.add("\n"'rune)
                li += 1
            }
        }

        return result
    }

    mtd insertRunePrivate(cp: *RichEditCursor, c: rune, selRect: bool)
    {
        if !selRect
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
                line.insertRune(posIns, c)
                cp.charPosInLine += 1
            }
        }
        else
        {
            Debug.assert(false) // todo
        }
    }

    mtd insertTextPrivate(cp: *RichEditCursor, text: Array'rune, selRect: bool)
    {
        if text.isEmpty()
            return

        if !selRect
        {
            visit c: text
            {
                insertRunePrivate(cp, c, selRect)
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
                if rankEnd == text.count or text[rankEnd] == "\n"'rune
                {
                    posIns := posInLine
                    if posIns > line.length()
                        posIns = line.length()
                    line.insertRunes(posIns, @mkslice(text.buffer + rankBeg, rankEnd - rankBeg))
                    if rankEnd == text.count
                        break
                    if rankEnd >= text.count - 1
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

                if rankEnd == text.count
                    break
                rankEnd += 1
            }

            cp.lineIndex = lineIndex
            cp.charPosInLine = posInLine + (rankEnd - rankBeg)
        }
    }

    mtd insertTextPrivate(cp: *RichEditCursor, text: string, selRect: bool)
    {
        if lines.isEmpty()
            newLine(0)

        if !selRect
        {
            str  := text
            line := lines[cp.lineIndex]
            loop
            {
                (c, eat) := Utf8.decodeRune(str)
                if !eat
                    break
                str = Tokenize.eatCount(str, cast(uint) eat)
                insertRunePrivate(cp, c, selRect)
            }
        }
        else
        {
            Debug.assert(false) // todo
        }

        setModified(true)
    }

    mtd insertTextPrivate(text: string, selRect: bool)
    {
        undo := Memory.new'RichEditUndo()
        undo.type       = .Insert
        undo.text       = Unicode.fromUtf8(text)
        undo.selBeg     = selBeg
        undo.cursorPos  = cursorPos
        undo.hasSelRect = selRect
        undo.serial     = 0
        resetUndo()
        toUndo.add(undo)
        insertTextPrivate(&cursorPos, text, selRect)
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
                //MergeMarkers(ps.lineIndex, pe.lineIndex);
                deleteLine(pe.lineIndex)
            }
        }
        else
        {
            li := posStart.lineIndex
            while li <= posEnd.lineIndex
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
    // Set the edit text
    mtd setText(text: string)
    {
        if isReadOnly()
            return

        clearPrivate()

        var cp: RichEditCursor
        insertTextPrivate(&cp, text, false)

        resetModified()
        invalidate()
    }

    mtd getRune(cp: RichEditCursor)->rune
    {
        if cp.lineIndex >= lines.count
            return 0
        len := lines[cp.lineIndex].chars.count
        if cp.charPosInLine > len
            return 0
        if cp.charPosInLine == len
            return "\n"'rune;
        return lines[cp.lineIndex].chars[cp.charPosInLine]
    }

    mtd insertRune(ch: rune)
    {
        if isReadOnly()
            return

        c := ch

        // A new undo action needs to be regenerate ?
        forceUndo := false
        pundo := currentUndo ? undoMgr[currentUndo - 1] : null
        if !isSelectionEmpty()
            forceUndo = true
        if !currentUndo
            forceUndo = true
        if pundo and !pundo.concatChars
            forceUndo = true
        if pundo and pundo.cursorPos.lineIndex != cursorPos.lineIndex
            forceUndo = true
        if pundo and cursorPos.charPosInLine != pundo.cursorPos.charPosInLine + pundo.text.count
            forceUndo = true

        if forceUndo
        {
            resetUndo()

            // Delete actual selection
            if !isSelectionEmpty()
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
                    pundo.text.add(getRune(cursorPos))
                    discard moveCursorRight(&pundo.selBeg)
                    deleteTextPrivate(cursorPos, pundo.selBeg, false)
                    clearSelection()
                }
            }
        }

        // Append the character to the last undo
        pundo = undoMgr[currentUndo - 1]
        pundo.text.add(c)

        // Do action
        insertTextPrivate(&cursorPos, @mkstring(&az[0], cpt), false)
        clearSelection()
        invalidate()

        pundo.begSelectionAfter = selBeg
        pundo.endSelectionAfter = cursorPos
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
            insertTextPrivate(txt, false)

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