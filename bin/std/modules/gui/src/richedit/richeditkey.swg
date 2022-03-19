using Core

impl RichEdit
{
    mtd charPressed(key: Input.Key, ch: rune, mdf: Input.KeyModifiers)->bool
    {
        if isReadOnly()
            return true

        // If this is a command, forget
        cmd := getMappingCommand(key, mdf)
        if cmd != .None
            return true

        c := ch

        // Some special characters will be treated as commands
        if c < " "'rune
        {
            if c == "\t"'rune or c == "\b"'rune
                return true
            if c == "\r"'rune
                c = "\n"'rune
        }

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
                    cmdClearSelection()
                }
            }
        }

        // Append the character to the last undo
        pundo = undoMgr[currentUndo - 1]
        pundo.text.add(c)

        // Do action
        insertTextPrivate(&cursorPos, @mkstring(&az[0], cpt), false)
        cmdClearSelection()
        ensureCaretIsVisible()
        invalidate()

        pundo.begSelectionAfter = selBeg
        pundo.endSelectionAfter = cursorPos
        return true
    }

    mtd keyPressed(key: Input.Key, mdf: Input.KeyModifiers)->bool
    {
        cmd := getMappingCommand(key, mdf)
        if cmd == .None
            return true
        cmdExecute(cmd)
        ensureCaretIsVisible()
        return true
    }
}