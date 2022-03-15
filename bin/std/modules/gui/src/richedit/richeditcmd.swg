using Core

public enum RichEditCommand
{
    None
    CursorLeft
    CursorRight
    CursorUp
    CursorDown
    CursorStartFile
    CursorEndFile
    CursorStartLine
    CursorEndLine
    SelectRuneRight
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
        addMapping(Input.Key.Left,  Input.KeyModifiers.None,    RichEditCommand.CursorLeft)
        addMapping(Input.Key.Right, Input.KeyModifiers.None,    RichEditCommand.CursorRight)
        addMapping(Input.Key.Right, Input.KeyModifiers.Shift,   RichEditCommand.SelectRuneRight)
        addMapping(Input.Key.Up,    Input.KeyModifiers.None,    RichEditCommand.CursorUp)
        addMapping(Input.Key.Down,  Input.KeyModifiers.None,    RichEditCommand.CursorDown)
        addMapping(Input.Key.Home,  Input.KeyModifiers.None,    RichEditCommand.CursorStartLine)
        addMapping(Input.Key.Home,  Input.KeyModifiers.Control, RichEditCommand.CursorStartFile)
        addMapping(Input.Key.End,   Input.KeyModifiers.None,    RichEditCommand.CursorEndLine)
        addMapping(Input.Key.End,   Input.KeyModifiers.Control, RichEditCommand.CursorEndFile)
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
                cmdSetCursorPos(getBegSelection())
            cmdClearSelection()
            resetVolatileFind()

        case CursorRight:
            if isSelectionEmpty()
                cmdCursorRight(&cursorPos)
            else
                cmdSetCursorPos(getEndSelection())
            cmdClearSelection()
            resetVolatileFind()

        case CursorUp:
            if !isSelectionEmpty()
            {
                pt := getCharacterPos(cursorPos)
                cmdSetCursorPos(getBegSelection())
                cmdCursorUp(&cursorPos, pt)
            }
            else
                cmdCursorUp(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorDown:
            if !isSelectionEmpty()
            {
                pt := getCharacterPos(cursorPos)
                cmdSetCursorPos(getEndSelection())
                cmdCursorDown(&cursorPos, pt)
            }
            else
                cmdCursorDown(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorStartLine:
            cmdCursorStartLine(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorEndLine:
            cmdCursorEndLine(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorStartFile:
            cmdCursorStartFile(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorEndFile:
            cmdCursorEndFile(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case SelectRuneRight:
            cmdCursorRight(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)
        }
    }

    mtd cmdSetCursorPos(cp: RichEditCharPos)
    {
        cursorPos = cp
    }

    mtd cmdSetSelRect(selRect: bool)
    {
        hasSelRect = selRect
    }

    mtd cmdClearSelection()
    {
        selBeg = cursorPos
        cmdSetSelRect(false)
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

    #[Swag.Discardable]
    mtd cmdCursorUp(cp: *RichEditCharPos)->bool
    {
        pt := getCharacterPos(cp)
        return cmdCursorUp(cp, pt)
    }

    #[Swag.Discardable]
    mtd cmdCursorUp(cp: *RichEditCharPos, posPrev: Math.Point)->bool
    {
        prev := getPreviousVisibleLine(cp.lineIndex)
        if prev == cp.lineIndex
            return false

        cp.lineIndex = prev
        line := lines[cp.lineIndex]
        cp.charPosInLine = Math.min(line.length(), cp.charPosInLine)
        posNew := getCharacterPos(cp)

        if posPrev.x == posNew.x
            return true
        if posNew.x < posPrev.x
        {
            while posNew.x < posPrev.x and cp.charPosInLine < line.length()
            {
                posNew.x += getSizeChar(line, cp.charPosInLine, posNew.x).x
                cp.charPosInLine += 1
            }
        }
        else
        {
            line = lines[cp.lineIndex]
            while posNew.x > posPrev.x and cp.charPosInLine
            {
                cp.charPosInLine -= 1
                posNew.x -= getSizeChar(line, cp.charPosInLine, posNew.x).x
            }
        }

        return true
    }

    #[Swag.Discardable]
    mtd cmdCursorDown(cp: *RichEditCharPos)->bool
    {
        pt := getCharacterPos(cp)
        return cmdCursorDown(cp, pt)
    }

    #[Swag.Discardable]
    mtd cmdCursorDown(cp: *RichEditCharPos, posPrev: Math.Point)->bool
    {
        next := getNextVisibleLine(cp.lineIndex)
        if next == cp.lineIndex
            return false

        cp.lineIndex = next
        line := lines[cp.lineIndex]
        cp.charPosInLine = Math.min(line.length(), cp.charPosInLine)
        posNew := getCharacterPos(cp)

        if posPrev.x == posNew.x
            return true
        if posNew.x < posPrev.x
        {
            while posNew.x < posPrev.x and cp.charPosInLine < line.length()
            {
                posNew.x += getSizeChar(line, cp.charPosInLine, posNew.x).x
                cp.charPosInLine += 1
            }
        }
        else
        {
            line = lines[cp.lineIndex]
            while posNew.x > posPrev.x and cp.charPosInLine
            {
                cp.charPosInLine -= 1
                posNew.x -= getSizeChar(line, cp.charPosInLine, posNew.x).x
            }
        }

        return true
    }

    mtd cmdCursorStartLine(cp: *RichEditCharPos)
    {
        line := lines[cp.lineIndex]
        i    := cast(s32) cp.charPosInLine

        var firstBlank: bool
        if !i
        {
            firstBlank = true
        }
        else
        {
            firstBlank = false
            i -= 1
            while i >= 0
            {
                if !Unicode.isSpace(line.chars[i])
                {
                    firstBlank = true
                    break
                }

                i -= 1
            }
        }

        cp.charPosInLine = 0
        if firstBlank
        {
            while cp.charPosInLine < line.length() and Unicode.isSpace(line.chars[cp.charPosInLine])
                cp.charPosInLine += 1
        }
    }

    mtd cmdCursorEndLine(cp: *RichEditCharPos)
    {
        cp.charPosInLine = lines[cp.lineIndex].length()
    }

    mtd cmdCursorStartFile(cp: *RichEditCharPos)
    {
        cp.lineIndex     = getFirstVisibleLine()
        cp.charPosInLine = 0
    }

    mtd cmdCursorEndFile(cp: *RichEditCharPos)
    {
        cp.lineIndex     = getLastVisibleLine();
        cp.charPosInLine = lines[cp.lineIndex].length()
    }
}