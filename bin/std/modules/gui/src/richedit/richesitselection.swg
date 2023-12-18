#global public
using Core

internal impl RichEditCtrl
{
    mtd getSelectionRectRange(lineIndex: u64, rankBeg, rankEnd: *u64)->bool
    {
        let line = lines[lineIndex]
        let len = line.length()

        var x = getMarginLeft()
        var rb = 0'u64
        while x < selRectangle.x and rb < len
        {
            x += getRuneSize(line, rb, x).x
            rb += 1
        }

        // Line is not concerned
        if x < selRectangle.x
            return false

        var re = rb
        while x < selRectangle.right() and re < len
        {
            x += getRuneSize(line, re, x).x
            re += 1
        }

        dref rankBeg = rb
        dref rankEnd = re
        return true
    }

    mtd deleteSelectionPrivate(posStart, posEnd: RichEditCursor, selRect: bool)
    {
        let undo = Memory.new'RichEditUndo()
        undo.type       = .Delete
        (undo.runes, undo.styles) = getTextPrivate(posStart, posEnd)
        undo.selBeg     = selBeg
        undo.cursorPos  = cursorPos
        undo.hasSelRect = selRect
        undo.serial     = 0
        resetUndo()
        toUndo.add(undo)
        deleteTextPrivate(posStart, posEnd, selRect);
    }

    mtd undoRestoreSelection()
    {
        let undo = Memory.new'RichEditUndo()
        undo.type      = .Selection
        undo.selBeg    = selBeg
        undo.cursorPos = cursorPos
        undo.serial    = 0
        resetUndo()
        toUndo.add(undo)
    }

    mtd computeRectangularSelection(beg, end: RichEditCursor)
    {
        if !hasSelRect
            return

        var size = getXExtent()

        var ptBeg = getCursorDisplayPos(beg)
        var ptEnd = getCursorDisplayPos(end)

        if ptBeg.x > ptEnd.x
            swap(&ptBeg, &ptEnd)

        selRectangle.x = ptBeg.x
        selRectangle.y = Math.min(ptBeg.y, ptEnd.y) - size.y
        selRectangle.setRight(ptEnd.x)
        selRectangle.setBottom(Math.max(ptBeg.y, ptEnd.y))
    }
}

impl RichEditCtrl
{
    mtd selectAll()
    {
        moveCursorStartFile(&selBeg)
        moveCursorEndFile(&cursorPos)
        setRectangularSelection(false)
    }

    mtd setRectangularSelection(selRect: bool)
    {
        hasSelRect = selRect
    }

    mtd clearSelection()
    {
        selBeg = cursorPos
        setRectangularSelection(false)
    }

    mtd setSelection(start, end: RichEditCursor)
    {
        selBeg = start
        cursorPos = end
        checkSelection()
    }

    mtd isSelectionEmpty()->bool
    {
        if selBeg.lineIndex != cursorPos.lineIndex
            return false
        if selBeg.charPosInLine != cursorPos.charPosInLine
            return false
        return true
    }

    mtd getBegSelection()->RichEditCursor
    {
        if hasSelRect
        {
            var result: retval
            result.lineIndex = Math.min(selBeg.lineIndex, cursorPos.lineIndex)
            result.charPosInLine =  Math.min(selBeg.charPosInLine, cursorPos.charPosInLine)
            return result
        }

        if selBeg.lineIndex < cursorPos.lineIndex
            return selBeg
        if selBeg.lineIndex > cursorPos.lineIndex
            return cursorPos
        if selBeg.charPosInLine <= cursorPos.charPosInLine
            return selBeg
        return cursorPos
    }

    mtd getEndSelection()->RichEditCursor
    {
        if hasSelRect
        {
            var result: retval
            result.lineIndex = Math.max(selBeg.lineIndex, cursorPos.lineIndex)
            result.charPosInLine =  Math.max(selBeg.charPosInLine, cursorPos.charPosInLine)
            return result
        }

        if selBeg.lineIndex < cursorPos.lineIndex
            return cursorPos
        if selBeg.lineIndex > cursorPos.lineIndex
            return selBeg
        if selBeg.charPosInLine <= cursorPos.charPosInLine
            return cursorPos
        return selBeg
    }

    mtd checkSelection()
    {
        if selBeg.lineIndex >= lines.count
            selBeg.lineIndex = lines.count - 1
        if cursorPos.lineIndex >= lines.count
            cursorPos.lineIndex = lines.count - 1

        if firstVisibleLineView >= lines.count or lastVisibleLineView >= lines.count
        {
            firstVisibleLineView = 0
            lastVisibleLineView = 0
            dirtyFirstLineView = true
            invalidate()
        }

        let back = lines.back()
        if selBeg.lineIndex == lines.count - 1 and selBeg.charPosInLine > back.length()
            selBeg.charPosInLine = back.length()

        if cursorPos.lineIndex == lines.count - 1 and cursorPos.charPosInLine > back.length()
            cursorPos.charPosInLine = back.length()

        computeRectangularSelection(getBegSelection(), getEndSelection())
    }

    mtd deleteSelection()
    {
        if isReadOnly()
            return
        if isSelectionEmpty()
            return

        pushUndo()
        var cp = getBegSelection()
        deleteSelectionPrivate(cp, getEndSelection(), hasSelRect)
        computeScrollY()

        if hasSelRect
        {
            cursorPos.charPosInLine = Math.min(cursorPos.charPosInLine, selBeg.charPosInLine)
            selBeg.charPosInLine = cursorPos.charPosInLine
        }
        else
        {
            setCursorPos(cp)
            selBeg = cp
        }

        popUndo()
    }

    mtd deleteLeft()
    {
        pushUndo()

        if isSelectionEmpty()
        {
            moveCursorLeft(&cursorPos)
        }
        elif hasSelRect and selBeg.charPosInLine == cursorPos.charPosInLine
        {
            var cp = cursorPos
            moveCursorLeft(&cp)
            if cp.lineIndex == cursorPos.lineIndex
            {
                cursorPos = cp
                computeRectangularSelection(getBegSelection(), getEndSelection())
            }
        }

        deleteSelection()
        popUndo()
    }

    mtd deleteRight()
    {
        pushUndo()
        if isSelectionEmpty() or (hasSelRect and selBeg.charPosInLine == cursorPos.charPosInLine)
        {
            moveCursorRight(&selBeg)
            computeRectangularSelection(getBegSelection(), getEndSelection())
        }

        deleteSelection()
        popUndo()
    }

    mtd deleteWordLeft()
    {
        pushUndo()
        deleteSelection()
        moveCursorWordLeft(&cursorPos)
        deleteSelection()
        clearSelection()
        popUndo()
    }

    mtd deleteWordRight()
    {
        pushUndo()
        deleteSelection()
        moveCursorWordRight(&cursorPos, true)
        deleteSelection()
        clearSelection()
        popUndo()
    }

    internal mtd registerClipboardFormat()
    {
        clipFormat = catch Clipboard.registerFormat("std.richedit.style")
    }

    mtd copyToClipboard()
    {
        if isSelectionEmpty()
            return
        var res = getTextPrivate(getBegSelection(), getEndSelection())

        registerClipboardFormat()
        if clipFormat Clipboard.addData(clipFormat, res.styles.toSlice())

        var txt = Utf8.fromUnicode(res.text.toSlice())
        Clipboard.addString(txt)
    }

    mtd pasteFromClipboard()
    {
        var txt = Clipboard.getString()
        registerClipboardFormat()

        pushUndo()
        deleteSelection()

        if clipFormat and Clipboard.hasFormat(clipFormat)
        {
            var st = Clipboard.getData(clipFormat)
            insertText(txt, st)
        }
        else
            insertText(txt)

        popUndo()
        forceCaretVisible = true
    }

    #[Swag.Overload]
    mtd deleteLine()
    {
        pushUndo()
        var beg = getBegSelection()
        var end = getEndSelection()
        if end.charPosInLine == 0 and beg.lineIndex != end.lineIndex
            moveCursorLeft(&end)
        beg.charPosInLine = 0
        end.charPosInLine = lines[end.lineIndex].length()
        selBeg = beg
        setCursorPos(end)
        moveCursorRight(&cursorPos)
        copyToClipboard()
        deleteSelection()
        popUndo()
    }

    mtd selectWord()
    {
        let line = lines[cursorPos.lineIndex]
        let len = line.length()
        if !len
            return

        if cursorPos.charPosInLine == len
            cursorPos.charPosInLine -= 1
        let cp = cursorPos.charPosInLine ? line.chars[selBeg.charPosInLine - 1] : 0'rune
        let c = selBeg.charPosInLine == len ? 0'rune : line.chars[selBeg.charPosInLine]
        let cn = cursorPos.charPosInLine != len - 1 ? line.chars[selBeg.charPosInLine + 1] : 0'rune

        if Unicode.isSpace(c) and !Unicode.isSpace(cp) and !Unicode.isSpace(cn) and cursorPos.charPosInLine
            cursorPos.charPosInLine -= 1

        selBeg = cursorPos

        // Blanks
        if (Unicode.isSpace(line.chars[selBeg.charPosInLine]))
        {
            while selBeg.charPosInLine and Unicode.isSpace(line.chars[selBeg.charPosInLine - 1])
                selBeg.charPosInLine -= 1
            while cursorPos.charPosInLine < len and Unicode.isSpace(line.chars[cursorPos.charPosInLine])
                cursorPos.charPosInLine += 1
            return
        }

        // A word
        if isWord(line.chars[cursorPos.charPosInLine])
        {
            while selBeg.charPosInLine and isWord(line.chars[selBeg.charPosInLine - 1])
                selBeg.charPosInLine -= 1
            while (cursorPos.charPosInLine < len and isWord(line.chars[cursorPos.charPosInLine]))
                cursorPos.charPosInLine += 1
            return
        }

        // Nothing
        if !zapWordChar(line.chars[selBeg.charPosInLine])
        {
            if selBeg.charPosInLine == len
                selBeg.charPosInLine -= 1
            else
                cursorPos.charPosInLine += 1
            return
        }

        while selBeg.charPosInLine and zapWordChar(line.chars[selBeg.charPosInLine - 1])
            selBeg.charPosInLine -= 1
        while cursorPos.charPosInLine < len and zapWordChar(line.chars[cursorPos.charPosInLine])
            cursorPos.charPosInLine += 1
    }

    mtd scrollLineUp()
    {
        var pt = scrollWnd.getScrollPos()
        var size = getXExtent()

        var sy = (pt.y / size.y) * size.y
        sy -= size.y

        scrollWnd.setScrollPos(pt.x, sy)
        sy = scrollWnd.getScrollPos().y

        var clientRect = edView.getClientRect()
        var pos = getCaretPos(cursorPos)
        while pos.bottom() > sy + clientRect.height
        {
            if !moveCursorUp(&cursorPos)
                break
            clearSelection()
            pos = getCaretPos(cursorPos)
        }
    }

    mtd scrollLineDown()
    {
        var pt = scrollWnd.getScrollPos()
        var size = getXExtent()

        var sy = (pt.y / size.y) * size.y
        sy += size.y

        scrollWnd.setScrollPos(pt.x, sy)
        sy = scrollWnd.getScrollPos().y

        var pos = getCaretPos(cursorPos)
        while pos.y < sy
        {
            if !moveCursorDown(&cursorPos)
                break
            clearSelection()
            pos = getCaretPos(cursorPos)
        }
    }
}