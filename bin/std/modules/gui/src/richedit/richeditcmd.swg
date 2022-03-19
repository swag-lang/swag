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
    DeleteWordLeft
    DeleteWordRight
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
        addMapping(Input.Key.Back,      Input.KeyModifiers.Control,     RichEditCommand.DeleteWordLeft)
        addMapping(Input.Key.Delete,    Input.KeyModifiers.None,        RichEditCommand.DeleteRight)
        addMapping(Input.Key.Delete,    Input.KeyModifiers.Control,     RichEditCommand.DeleteWordRight)
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
                moveCursorLeft(&cursorPos)
            else
                setCursorPos(getBegSelection())
            cmdClearSelection()
            resetVolatileFind()

        case CursorRight:
            if isSelectionEmpty()
                moveCursorRight(&cursorPos)
            else
                setCursorPos(getEndSelection())
            cmdClearSelection()
            resetVolatileFind()

        case CursorUp:
            if !isSelectionEmpty()
            {
                pt := getCursorDisplayPos(cursorPos)
                setCursorPos(getBegSelection())
                moveCursorUp(&cursorPos, pt)
            }
            else
                moveCursorUp(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorDown:
            if !isSelectionEmpty()
            {
                pt := getCursorDisplayPos(cursorPos)
                setCursorPos(getEndSelection())
                moveCursorDown(&cursorPos, pt)
            }
            else
                moveCursorDown(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorStartLine:
            moveCursorStartLine(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorEndLine:
            moveCursorEndLine(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorStartFile:
            moveCursorStartFile(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorEndFile:
            moveCursorEndFile(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorWordLeft:
            moveCursorWordLeft(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorWordRight:
            moveCursorWordRight(&cursorPos, true)
            cmdClearSelection()
            resetVolatileFind()

        case CursorPageUp:
            moveCursorPageUp(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case CursorPageDown:
            moveCursorPageDown(&cursorPos)
            cmdClearSelection()
            resetVolatileFind()

        case ScrollLineUp:
            cmdScrollLineUp()

        case ScrollLineDown:
            cmdScrollLineDown()

        case SelectRuneLeft:
            moveCursorLeft(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectRuneRight:
            moveCursorRight(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectRuneUp:
            moveCursorUp(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectRuneDown:
            moveCursorDown(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectStartLine:
            moveCursorStartLine(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectEndLine:
            moveCursorEndLine(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectStartFile:
            moveCursorStartFile(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectEndFile:
            moveCursorEndFile(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectAllFile:
            moveCursorStartFile(&selBeg)
            moveCursorEndFile(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectWordLeft:
            moveCursorWordLeft(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectWordRight:
            moveCursorWordRight(&cursorPos, false)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectPageUp:
            moveCursorPageUp(&cursorPos)
            resetVolatileFind()
            cmdSetSelRect(false)

        case SelectPageDown:
            moveCursorPageDown(&cursorPos)
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

        case DeleteWordLeft:
            if isReadOnly()
                return
            cmdEditDeleteWordLeft()
            resetVolatileFind()

        case DeleteWordRight:
            if isReadOnly()
                return
            cmdEditDeleteWordRight()
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

    mtd cmdEditDeleteLeft()
    {
        pushUndo()
        if isSelectionEmpty()
            moveCursorLeft(&cursorPos)
        deleteSelectionPrivate()
        popUndo()
    }

    mtd cmdEditDeleteRight()
    {
        pushUndo()
        if isSelectionEmpty()
            moveCursorRight(&selBeg)

        //str := GetSelectedText()
        deleteSelectionPrivate()

        // Trim line if we delete an EOL followed by an empty line
/*        if (str == "\n")
        {
            CharPos end = GetBegSelection();
            moveCursorWordRight(end, true);
            moveCursorRight(end);
            str = GetTextPrivate(GetBegSelection(), end, false);
            str.Delete("\t");
            if (str == "\n")
            {
                moveCursorWordRight(m_SelBeg, true);
                DeleteSelectionPrivate();
            }
        }*/

        popUndo()
    }

    mtd cmdEditDeleteWordLeft()
    {
        pushUndo()
        deleteSelectionPrivate()
        moveCursorWordLeft(&cursorPos)
        deleteSelectionPrivate()
        cmdClearSelection()
        popUndo()
    }

    mtd cmdEditDeleteWordRight()
    {
        pushUndo()
        deleteSelectionPrivate()
        moveCursorWordRight(&cursorPos, true)
        deleteSelectionPrivate()
        cmdClearSelection()
        popUndo()
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
            if !moveCursorUp(&cursorPos)
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
            if !moveCursorDown(&cursorPos)
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