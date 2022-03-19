impl RichEdit
{
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

    #[Swag.Discardable]
    mtd deleteSelectionPrivate()->bool
    {
        if isSelectionEmpty()
            return false

        cp := getBegSelection()
        undoDeleteText(cp, getEndSelection(), hasSelRect)
        computeScrollY()
        setCursorPos(cp)
        selBeg = cp
        return true
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

        // In case of a rectangular selection, recompute selection
        //ComputeRectangularSelection(GetBegSelection(), GetEndSelection());
    }
}