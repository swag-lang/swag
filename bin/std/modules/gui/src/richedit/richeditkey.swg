using Core

impl RichEdit
{
    mtd charPressed(char: rune, mdf: Input.KeyModifiers)->bool
    {
        if char < 32
            return false
        if isReadOnly()
            return true

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
                cmdDeleteSelection()
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
        cpt := Utf8.encodeRune(az, char)

        // Add a deletion
        if currentUndo > 1
        {
            pundo = undoMgr[currentUndo - 2]
            if pundo.type == .Delete and pundo.concatChars
            {
                if cursorPos.charPosInLine != lines[cursorPos.lineIndex].length()
                {
                    pundo.text.add(getRune(cursorPos))
                    discard cmdCursorRight(&pundo.selBeg)
                    deleteTextPrivate(cursorPos, pundo.selBeg, false)
                    cmdSelClear()
                }
            }
        }

        // Append the character to the last undo
        pundo = undoMgr[currentUndo - 1]
        pundo.text.add(char)

        // Do action
        insertTextPrivate(&cursorPos, @mkstring(&az[0], cpt), false)
        cmdSelClear()
        ensureCaretIsVisible()
        invalidate()

        pundo.begSelectionAfter = selBeg
        pundo.endSelectionAfter = cursorPos
        return true
    }

    mtd keyPressed(key: Input.Key, mdf: Input.KeyModifiers)->bool
    {
        return true
    }
}