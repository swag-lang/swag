using Core

public enum RichEditCommand
{
    None
    CursorLeft
    CursorRight
}

impl RichEdit
{
    mtd addMapping(key: Input.Key, mdf: Input.KeyModifiers, cmd: RichEditCommand)
    {
        var m: RichEditMapping
        m.key = key
        m.modifiers = mdf
        m.command = cmd
        mapping.add(m)
    }

    mtd getMappingCommand(key: Input.Key, mdf: Input.KeyModifiers)->RichEditCommand
    {
        visit &p: mapping
        {
            if p.key == key and p.modifiers == mdf
                return p.command
        }

        return RichEditCommand.None
    }

    mtd setDefaultMapping()
    {
        addMapping(Input.Key.Left, Input.KeyModifiers.None, RichEditCommand.CursorLeft)
        addMapping(Input.Key.Right, Input.KeyModifiers.None, RichEditCommand.CursorRight)
    }

    // Execute a command
    public mtd cmdExecute(cmd: RichEditCommand)
    {
        switch cmd
        {
        case CursorLeft:
            if isSelectionEmpty()
                cmdCursorLeft(&cursorPos)
            else
                cmdSelCursorSet(getBegSelection())
            cmdSelClear()
            resetVolatileFind()

        case CursorRight:
            if isSelectionEmpty()
                cmdCursorRight(&cursorPos)
            else
                cmdSelCursorSet(getEndSelection())
            cmdSelClear()
            resetVolatileFind()
        }
    }

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

    #[Swag.Discardable]
    mtd cmdCursorLeft(cp: *RichEditCharPos)->bool
    {
        if cp.charPosInLine == 0
        {
            prev := getPreviousVisibleLine(cp.lineIndex)
            if prev == cp.lineIndex
                return false
            cp.lineIndex     = prev
            cp.charPosInLine = lines[prev].length()
        }
        else
        {
            cp.charPosInLine -= 1
        }

        return true
    }

    #[Swag.Discardable]
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