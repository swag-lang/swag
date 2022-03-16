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

    mtd getBegSelection()->RichEditCharPos
    {
        if selBeg.lineIndex < cursorPos.lineIndex
            return selBeg
        if selBeg.lineIndex > cursorPos.lineIndex
            return cursorPos
        if selBeg.charPosInLine <= cursorPos.charPosInLine
            return selBeg
        return cursorPos
    }

    mtd getEndSelection()->RichEditCharPos
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
        cmdSetCursorPos(cp)
        selBeg = cp
        return true
    }
}