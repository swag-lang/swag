#global public
using Core

private impl RichEdit
{
    mtd getSelelectionRectRange(lineIndex: uint, rankBeg, rankEnd: *uint)->bool
    {
        line := lines[lineIndex]
        len  := line.length()

        x := getMarginLeft()
        rb := 0'uint
        while x < selRectangle.x and rb < len
        {
            x += getRuneSize(line, rb, x).x
            rb += 1
        }

        // Line is not concerned
        if x < selRectangle.x
            return false

        re := rb
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
        undo := Memory.new'RichEditUndo()
        undo.type       = .Delete
        (undo.runes, undo.styles) = getTextPrivate(posStart, posEnd, selRect)
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
        undo := Memory.new'RichEditUndo()
        undo.type      = .Selection
        undo.selBeg    = selBeg
        undo.cursorPos = cursorPos
        undo.serial    = 0
        resetUndo()
        toUndo.add(undo)
    }
}

impl RichEdit
{
    mtd setRectangularSelection(selRect: bool)
    {
        hasSelRect = selRect
    }

    mtd clearSelection()
    {
        selBeg = cursorPos
        setRectangularSelection(false)
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

        back := lines.back()
        if selBeg.lineIndex == lines.count - 1 and selBeg.charPosInLine > back.length()
            selBeg.charPosInLine = back.length()

        if cursorPos.lineIndex == lines.count - 1 and cursorPos.charPosInLine > back.length()
            cursorPos.charPosInLine = back.length()

        computeRectangularSelection(getBegSelection(), getEndSelection())
    }

    mtd computeRectangularSelection(beg, end: RichEditCursor)
    {
        if !hasSelRect
            return

        size := getXExtent()

        ptBeg := getCursorDisplayPos(beg)
        ptEnd := getCursorDisplayPos(end)

        if ptBeg.x > ptEnd.x
            swap(&ptBeg, &ptEnd)

        selRectangle.x = ptBeg.x
        selRectangle.y = Math.min(ptBeg.y, ptEnd.y) - size.y
        selRectangle.setRight(ptEnd.x)
        selRectangle.setBottom(Math.max(ptBeg.y, ptEnd.y))
    }

    mtd deleteSelection()
    {
        if isReadOnly()
            return
        if isSelectionEmpty()
            return

        pushUndo()
        cp := getBegSelection()
        deleteSelectionPrivate(cp, getEndSelection(), hasSelRect)
        computeScrollY()

        if hasSelRect
        {
            cursorPos.charPosInLine = selBeg.charPosInLine
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
            moveCursorLeft(&cursorPos)
        deleteSelection()
        popUndo()
    }

    mtd deleteRight()
    {
        pushUndo()
        if isSelectionEmpty()
            moveCursorRight(&selBeg)
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

    mtd scrollLineUp()
    {
        pt := scrollWnd.getScrollPos()
        size := getXExtent()

        sy := (pt.y / size.y) * size.y
        sy -= size.y

        scrollWnd.setScrollPos(pt.x, sy)
        sy = scrollWnd.getScrollPos().y

        clientRect := edView.getClientRect()
        pos := getCaretPos(cursorPos)
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
        pt := scrollWnd.getScrollPos()
        size := getXExtent()

        sy := (pt.y / size.y) * size.y
        sy += size.y

        scrollWnd.setScrollPos(pt.x, sy)
        sy = scrollWnd.getScrollPos().y

        pos := getCaretPos(cursorPos)
        while pos.y < sy
        {
            if !moveCursorDown(&cursorPos)
                break
            clearSelection()
            pos = getCaretPos(cursorPos)
        }
    }
}