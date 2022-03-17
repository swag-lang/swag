#global public
using Core

public enum RichEditCommand
{
    None
    CursorLeft
    CursorRight
    CursorUp
    CursorDown
    CursorStartLine
    CursorEndLine
    CursorStartFile
    CursorEndFile
    CursorWordLeft
    CursorWordRight
    CursorPageUp
    CursorPageDown
    //CMD_CURSOR_MATCH_PAIR
    ScrollLineUp
    ScrollLineDown
    DeleteLeft
    DeleteRight
    //CMD_EDIT_CUT_LINE
    //CMD_EDIT_DELETE_LINE
    EditTabulation
    EditBackTabulation
    //CMD_EDIT_DELETE_WORD_LEFT,
    //CMD_EDIT_DELETE_WORD_RIGHT
    //CMD_EDIT_DELETE_LINE_LEFT
    //CMD_EDIT_DELETE_LINE_RIGHT
    //CMD_EDIT_OVERWRITE
    //CMD_CLIPBOARD_COPY
    //CMD_CLIPBOARD_PASTE
    //CMD_CLIPBOARD_CUT
    Undo
    Redo
    SelectRuneLeft
    SelectRuneRight
    SelectRuneUp
    SelectRuneDown
    //CMD_SEL_CHAR_LEFT_RECT
    //CMD_SEL_CHAR_RIGHT_RECT
    //CMD_SEL_CHAR_UP_RECT
    //CMD_SEL_CHAR_DOWN_RECT
    SelectStartLine
    SelectEndLine
    SelectStartFile
    SelectEndFile
    SelectAllFile
    SelectWordLeft
    SelectWordRight
    SelectPageUp
    SelectPageDown
    //CMD_SEL_HIDE_LINE
    //CMD_MARKER_BOOKMARK_ADD
    //CMD_MARKER_BOOKMARK_PREV
    //CMD_MARKER_BOOKMARK_NEXT
    //CMD_MARKER_BOOKMARK_FAST
    //CMD_MAKE_UPPER
    //CMD_MAKE_LOWER
    //CMD_COMMENT_CPP
    //CMD_UNCOMMENT_CPP
}

impl RichEdit
{
    mtd clearMapping()
    {
        mapping.clear()
    }

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
        addMapping(Input.Key.Left,      Input.KeyModifiers.None,        RichEditCommand.CursorLeft)
        addMapping(Input.Key.Left,      Input.KeyModifiers.Shift,       RichEditCommand.SelectRuneLeft)
        addMapping(Input.Key.Left,      Input.KeyModifiers.Control,     RichEditCommand.CursorWordLeft)
        addMapping(Input.Key.Left,      Input.KeyModifiers.CtrlShift,   RichEditCommand.SelectWordLeft)
        addMapping(Input.Key.Right,     Input.KeyModifiers.None,        RichEditCommand.CursorRight)
        addMapping(Input.Key.Right,     Input.KeyModifiers.Shift,       RichEditCommand.SelectRuneRight)
        addMapping(Input.Key.Right,     Input.KeyModifiers.Control,     RichEditCommand.CursorWordRight)
        addMapping(Input.Key.Right,     Input.KeyModifiers.CtrlShift,   RichEditCommand.SelectWordRight)
        addMapping(Input.Key.Up,        Input.KeyModifiers.None,        RichEditCommand.CursorUp)
        addMapping(Input.Key.Up,        Input.KeyModifiers.Shift,       RichEditCommand.SelectRuneUp)
        addMapping(Input.Key.Up,        Input.KeyModifiers.Control,     RichEditCommand.ScrollLineUp)
        addMapping(Input.Key.PageUp,    Input.KeyModifiers.None,        RichEditCommand.CursorPageUp)
        addMapping(Input.Key.PageUp,    Input.KeyModifiers.Shift,       RichEditCommand.SelectPageUp)
        addMapping(Input.Key.PageDown,  Input.KeyModifiers.None,        RichEditCommand.CursorPageDown)
        addMapping(Input.Key.PageDown,  Input.KeyModifiers.Shift,       RichEditCommand.SelectPageDown)
        addMapping(Input.Key.Down,      Input.KeyModifiers.None,        RichEditCommand.CursorDown)
        addMapping(Input.Key.Down,      Input.KeyModifiers.Shift,       RichEditCommand.SelectRuneDown)
        addMapping(Input.Key.Down,      Input.KeyModifiers.Shift,       RichEditCommand.SelectRuneDown)
        addMapping(Input.Key.Down,      Input.KeyModifiers.Control,     RichEditCommand.ScrollLineDown)
        addMapping(Input.Key.Home,      Input.KeyModifiers.None,        RichEditCommand.CursorStartLine)
        addMapping(Input.Key.Home,      Input.KeyModifiers.Control,     RichEditCommand.CursorStartFile)
        addMapping(Input.Key.Home,      Input.KeyModifiers.Shift,       RichEditCommand.SelectStartLine)
        addMapping(Input.Key.Home,      Input.KeyModifiers.CtrlShift,   RichEditCommand.SelectStartFile)
        addMapping(Input.Key.End,       Input.KeyModifiers.None,        RichEditCommand.CursorEndLine)
        addMapping(Input.Key.End,       Input.KeyModifiers.Control,     RichEditCommand.CursorEndFile)
        addMapping(Input.Key.End,       Input.KeyModifiers.Shift,       RichEditCommand.SelectEndLine)
        addMapping(Input.Key.End,       Input.KeyModifiers.CtrlShift,   RichEditCommand.SelectEndFile)
        addMapping(Input.Key.Back,      Input.KeyModifiers.None,        RichEditCommand.DeleteLeft)
        addMapping(Input.Key.Delete,    Input.KeyModifiers.None,        RichEditCommand.DeleteRight)
        addMapping(Input.Key.Tab,       Input.KeyModifiers.None,        RichEditCommand.EditTabulation)
        addMapping(Input.Key.Tab,       Input.KeyModifiers.Shift,       RichEditCommand.EditBackTabulation)
        addMapping(Input.Key.A,         Input.KeyModifiers.Control,     RichEditCommand.SelectAllFile)
        addMapping(Input.Key.Y,         Input.KeyModifiers.Control,     RichEditCommand.Redo)
        addMapping(Input.Key.Z,         Input.KeyModifiers.Control,     RichEditCommand.Undo)
    }

    // Execute a command
    mtd cmdExecute(cmd: RichEditCommand)
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

        case CursorWordLeft:
            cmdCursorWordLeft(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorWordRight:
            cmdCursorWordRight(&cursorPos, true)
            cmdClearSelection()
            resetVolatileFind()

        case CursorPageUp:
            cmdCursorPageUp(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorPageDown:
            cmdCursorPageDown(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case ScrollLineUp:
            cmdScrollLineUp()

        case ScrollLineDown:
            cmdScrollLineDown()

        case SelectRuneLeft:
            cmdCursorLeft(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectRuneRight:
            cmdCursorRight(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectRuneUp:
            cmdCursorUp(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectRuneDown:
            cmdCursorDown(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectStartLine:
            cmdCursorStartLine(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectEndLine:
            cmdCursorEndLine(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectStartFile:
            cmdCursorStartFile(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectEndFile:
            cmdCursorEndFile(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectAllFile:
            cmdCursorStartFile(&selBeg)
            cmdCursorEndFile(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectWordLeft:
            cmdCursorWordLeft(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectWordRight:
            cmdCursorWordRight(&cursorPos, false)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectPageUp:
            cmdCursorPageUp(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectPageDown:
            cmdCursorPageDown(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case DeleteLeft:
            if isReadOnly()
                return
            cmdEditDeleteLeft()
            resetVolatileFind()

        case DeleteRight:
            if isReadOnly()
                return
            cmdEditDeleteRight()
            resetVolatileFind()

        case EditTabulation:
            if isReadOnly()
                return
            cmdEditTabulation()
            resetVolatileFind()

        case EditBackTabulation:
            if isReadOnly()
                return
            cmdEditBackTabulation()
            resetVolatileFind()

        case Undo:
            if isReadOnly()
                return
            cmdUndo()
            resetVolatileFind()

        case Redo:
            if isReadOnly()
                return
            cmdRedo()
            resetVolatileFind()
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
        cp.lineIndex     = getLastVisibleLine()
        cp.charPosInLine = lines[cp.lineIndex].length()
    }

    mtd cmdCursorPageUp(cp: *RichEditCharPos)
    {
        client := edView.getClientRect()
        count := cast(int) (client.height / getXExtent().y)
        while count
        {
            if cp.lineIndex == 0
                break
            cmdCursorUp(cp)
            count -= 1
        }
    }

    mtd cmdCursorPageDown(cp: *RichEditCharPos)
    {
        client := edView.getClientRect()
        count := cast(int) (client.height / getXExtent().y)
        while count
        {
            if cp.lineIndex == lines.count - 1
                break
            cmdCursorDown(cp)
            count -= 1
        }
    }

    mtd cmdCursorWordLeft(cp: *RichEditCharPos)
    {
        line := lines[cp.lineIndex]
        if cp.charPosInLine == 0
        {
            cmdCursorLeft(cp)
            return
        }

        if Unicode.isSpace(line.chars[cp.charPosInLine - 1])
        {
            if !cmdCursorLeft(cp)
                return
            line = lines[cp.lineIndex]
            while cp.charPosInLine != 0 and Unicode.isSpace(line.chars[cp.charPosInLine - 1])
            {
                if !cmdCursorLeft(cp)
                    return
                line = lines[cp.lineIndex]
            }

            if !cp.charPosInLine
                return
        }

        if Unicode.isWord(line.chars[cp.charPosInLine - 1])
        {
            while cp.charPosInLine != 0 and Unicode.isWord(line.chars[cp.charPosInLine - 1])
            {
                if !cmdCursorLeft(cp)
                    return
                line = lines[cp.lineIndex]
            }
        }
        else if (zapWordChar(line.chars[cp.charPosInLine - 1]))
        {
            while cp.charPosInLine != 0 and zapWordChar(line.chars[cp.charPosInLine - 1])
            {
                if !cmdCursorLeft(cp)
                    return
                line = lines[cp.lineIndex]
            }
        }
        else
        {
            cmdCursorLeft(cp)
        }
    }

    mtd cmdCursorWordRight(cp: *RichEditCharPos, withSpace: bool)
    {
        line := lines[cp.lineIndex]
        if cp.charPosInLine == line.length() or Unicode.isSpace(line.chars[cp.charPosInLine])
        {
            if !cmdCursorRight(cp)
                return
            line = lines[cp.lineIndex]
            while cp.charPosInLine != line.length() and Unicode.isSpace(line.chars[cp.charPosInLine])
            {
                if !cmdCursorRight(cp)
                    return
                line = lines[cp.lineIndex]
            }
        }
        else if (Unicode.isWord(line.chars[cp.charPosInLine]))
        {
            while cp.charPosInLine != line.length() and Unicode.isWord(line.chars[cp.charPosInLine])
            {
                if !cmdCursorRight(cp)
                    return
                line = lines[cp.lineIndex]
            }

            if withSpace
            {
                while cp.charPosInLine != line.length() and Unicode.isSpace(line.chars[cp.charPosInLine])
                {
                    if !cmdCursorRight(cp)
                        return
                    line = lines[cp.lineIndex]
                }
            }
        }
        else if zapWordChar(line.chars[cp.charPosInLine])
        {
            while cp.charPosInLine != line.length() and zapWordChar(line.chars[cp.charPosInLine])
            {
                if !cmdCursorRight(cp)
                    return
                line = lines[cp.lineIndex]
            }

            while cp.charPosInLine != line.length() and Unicode.isSpace(line.chars[cp.charPosInLine])
            {
                if !cmdCursorRight(cp)
                    return
                line = lines[cp.lineIndex]
            }
        }
        else
        {
            cmdCursorRight(cp)
        }
    }

    mtd cmdEditDeleteLeft()
    {
        pushUndo()
        if isSelectionEmpty()
            cmdCursorLeft(&cursorPos)
        deleteSelectionPrivate()
        popUndo()
    }

    mtd cmdEditDeleteRight()
    {
        pushUndo()
        if isSelectionEmpty()
            cmdCursorRight(&selBeg)

        //str := GetSelectedText()
        deleteSelectionPrivate()

        // Trim line if we delete an EOL followed by an empty line
/*        if (str == "\n")
        {
            CharPos end = GetBegSelection();
            CmdCursorWordRight(end, true);
            CmdCursorRight(end);
            str = GetTextPrivate(GetBegSelection(), end, false);
            str.Delete("\t");
            if (str == "\n")
            {
                CmdCursorWordRight(m_SelBeg, true);
                DeleteSelectionPrivate();
            }
        }*/

        popUndo()
    }

    mtd cmdUndo()
    {
        if undoMgr.count == 0
            return
        if !currentUndo
            return

        currentUndo -= 1
        undo   := undoMgr[currentUndo]
        serial := undo.serial

        loop
        {
            if undo.type == .Delete
            {
                selBeg = undo.selBeg
                cmdSetCursorPos(undo.cursorPos)
                cp := getBegSelection()
                insertTextPrivate(&cp, undo.text, undo.hasSelRect)
            }
            elif undo.type == .Insert
            {
                selBeg = undo.selBeg
                cmdSetCursorPos(undo.cursorPos)
                cmdSetSelRect(undo.hasSelRect)
                //ComputeRectangularSelection(m_CursorPos, pundo->m_BegSelectionAfter)
                deleteTextPrivate(cursorPos, undo.begSelectionAfter, undo.hasSelRect)
            }

            selBeg = undo.begSelectionBefore
            cmdSetCursorPos(undo.endSelectionBefore)
            cmdSetSelRect(undo.hasSelRect)
            if !currentUndo
                break
            undo = undoMgr[currentUndo - 1]
            if undo.serial != serial
                break
            currentUndo -= 1
        }

        if getLastModificationUndo() == modifiedUndoMarker
            setModified(false)
        else
            setModified(true)
        computeScrollY()
    }

    mtd cmdRedo()
    {
        if undoMgr.count == 0
            return
        if currentUndo == undoMgr.count
            return

        undo   := undoMgr[currentUndo]
        serial := undo.serial
        currentUndo += 1

        loop
        {
            if undo.type == .Delete
            {
                selBeg = undo.selBeg
                cmdSetCursorPos(undo.cursorPos)
                cmdSetSelRect(undo.hasSelRect)
                //ComputeRectangularSelection(GetBegSelection(), GetEndSelection())
                deleteTextPrivate(getBegSelection(), getEndSelection(), undo.hasSelRect)
            }
            elif undo.type == .Insert
            {
                selBeg = undo.selBeg
                cmdSetCursorPos(undo.cursorPos)
                cp := getBegSelection()
                insertTextPrivate(&cp, undo.text, undo.hasSelRect)
            }

            selBeg = undo.begSelectionAfter
            cmdSetCursorPos(undo.endSelectionAfter)
            cmdSetSelRect(undo.hasSelRect)

            if currentUndo == undoMgr.count
                break
            undo = undoMgr[currentUndo]
            if undo.serial != serial
                break
            currentUndo += 1
        }

        if getLastModificationUndo() == modifiedUndoMarker
            setModified(false)
        else
            setModified(true)
        computeScrollY()
    }

    mtd cmdScrollLineUp()
    {
        pt := scrollWnd.getScrollPos()
        size := getXExtent()

        sy := (pt.y / size.y) * size.y
        sy -= size.y

        scrollWnd.setScrollPos(pt.x, sy)
        sy = scrollWnd.getScrollPos().y

        clientRect := edView.getClientRect()
        pos := getCaretPos()
        while pos.bottom() > sy + clientRect.height
        {
            if !cmdCursorUp(&cursorPos)
                break
            cmdClearSelection()
            pos = getCaretPos()
        }
    }

    mtd cmdScrollLineDown()
    {
        pt := scrollWnd.getScrollPos()
        size := getXExtent()

        sy := (pt.y / size.y) * size.y
        sy += size.y

        scrollWnd.setScrollPos(pt.x, sy)
        sy = scrollWnd.getScrollPos().y

        pos := getCaretPos()
        while pos.y < sy
        {
            if !cmdCursorDown(&cursorPos)
                break
            cmdClearSelection()
            pos = getCaretPos()
        }
    }

    mtd cmdEditTabulation()
    {
        if getBegSelection().lineIndex == getEndSelection().lineIndex
        {
            pushUndo()
            deleteSelectionPrivate()
            undoInsertText("\t", false)
            popUndo()
        }
        else
            insertBegLine("\t", false)
    }

    mtd cmdEditBackTabulation()
    {
        deleteBegLine("\t", false)
    }
}

func zapWordChar(c: rune)->bool
{
    if Unicode.isLetterOrDigit(c)
        return false
    if Unicode.isSpace(c)
        return false
    if Unicode.isWord(c)
        return false
    if c != "("'rune and c != ")"'rune and c != "["'rune and c != "]"'rune and c != "{"'rune and c != "}"'rune
        return true
    return false
}
