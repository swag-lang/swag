impl RichEdit
{
    mtd cmdSelCursorSet(cp: RichEditCharPos)
    {
        cursorPos = cp
    }

    mtd cmdSelSetRect(selRect: bool)
    {
        hasSelRect = selRect
    }

    mtd cmdSelClear()
    {
        selBeg = cursorPos
        cmdSelSetRect(false)
    }

    mtd cmdDeleteSelection()
    {
        if isSelectionEmpty()
            return
        pushUndo()
        deleteSelectionPrivate()
        popUndo()
    }

    mtd cmdCursorRight(cp: *RichEditCharPos)->bool
    {
        if cp.charPosInLine == lines[cp.lineIndex].length()
        {
            next := getNextVisibleLine(cp.lineIndex)
            if next == cp.lineIndex
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
}