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
    ToggleOverwrite
    //CMD_CLIPBOARD_COPY
    //CMD_CLIPBOARD_PASTE
    //CMD_CLIPBOARD_CUT
    Undo
    Redo
    SelectRuneLeft
    SelectRuneRight
    SelectRuneUp
    SelectRuneDown
    SelectRectLeft
    SelectRectRight
    SelectRectUp
    SelectRectDown
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
        addMapping(Input.Key.Left,      Input.KeyModifiers.Alt,         RichEditCommand.SelectRectLeft)
        addMapping(Input.Key.Right,     Input.KeyModifiers.None,        RichEditCommand.CursorRight)
        addMapping(Input.Key.Right,     Input.KeyModifiers.Shift,       RichEditCommand.SelectRuneRight)
        addMapping(Input.Key.Right,     Input.KeyModifiers.Control,     RichEditCommand.CursorWordRight)
        addMapping(Input.Key.Right,     Input.KeyModifiers.CtrlShift,   RichEditCommand.SelectWordRight)
        addMapping(Input.Key.Right,     Input.KeyModifiers.Alt,         RichEditCommand.SelectRectRight)
        addMapping(Input.Key.Up,        Input.KeyModifiers.None,        RichEditCommand.CursorUp)
        addMapping(Input.Key.Up,        Input.KeyModifiers.Shift,       RichEditCommand.SelectRuneUp)
        addMapping(Input.Key.Up,        Input.KeyModifiers.Control,     RichEditCommand.ScrollLineUp)
        addMapping(Input.Key.Up,        Input.KeyModifiers.Alt,         RichEditCommand.SelectRectUp)
        addMapping(Input.Key.Down,      Input.KeyModifiers.None,        RichEditCommand.CursorDown)
        addMapping(Input.Key.Down,      Input.KeyModifiers.Shift,       RichEditCommand.SelectRuneDown)
        addMapping(Input.Key.Down,      Input.KeyModifiers.Shift,       RichEditCommand.SelectRuneDown)
        addMapping(Input.Key.Down,      Input.KeyModifiers.Control,     RichEditCommand.ScrollLineDown)
        addMapping(Input.Key.Down,      Input.KeyModifiers.Alt,         RichEditCommand.SelectRectDown)
        addMapping(Input.Key.PageUp,    Input.KeyModifiers.None,        RichEditCommand.CursorPageUp)
        addMapping(Input.Key.PageUp,    Input.KeyModifiers.Shift,       RichEditCommand.SelectPageUp)
        addMapping(Input.Key.PageDown,  Input.KeyModifiers.None,        RichEditCommand.CursorPageDown)
        addMapping(Input.Key.PageDown,  Input.KeyModifiers.Shift,       RichEditCommand.SelectPageDown)
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
        addMapping(Input.Key.Insert,    Input.KeyModifiers.None,        RichEditCommand.ToggleOverwrite)
        addMapping(Input.Key.A,         Input.KeyModifiers.Control,     RichEditCommand.SelectAllFile)
        addMapping(Input.Key.Y,         Input.KeyModifiers.Control,     RichEditCommand.Redo)
        addMapping(Input.Key.Z,         Input.KeyModifiers.Control,     RichEditCommand.Undo)
    }

    // Execute a command
    mtd executeCommand(cmd: RichEditCommand)
    {
        switch cmd
        {
        case CursorLeft:
            if isSelectionEmpty()
                moveCursorLeft(&cursorPos)
            else
                setCursorPos(getBegSelection())
            clearSelection()
            resetVolatileFind()

        case CursorRight:
            if isSelectionEmpty()
                moveCursorRight(&cursorPos)
            else
                setCursorPos(getEndSelection())
            clearSelection()
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
            clearSelection()
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
            clearSelection()
            resetVolatileFind()

        case CursorStartLine:
            moveCursorStartLine(&cursorPos)
            clearSelection()
            resetVolatileFind()

        case CursorEndLine:
            moveCursorEndLine(&cursorPos)
            clearSelection()
            resetVolatileFind()

        case CursorStartFile:
            moveCursorStartFile(&cursorPos)
            clearSelection()
            resetVolatileFind()

        case CursorEndFile:
            moveCursorEndFile(&cursorPos)
            clearSelection()
            resetVolatileFind()

        case CursorWordLeft:
            moveCursorWordLeft(&cursorPos)
            clearSelection()
            resetVolatileFind()

        case CursorWordRight:
            moveCursorWordRight(&cursorPos, true)
            clearSelection()
            resetVolatileFind()

        case CursorPageUp:
            moveCursorPageUp(&cursorPos)
            clearSelection()
            resetVolatileFind()

        case CursorPageDown:
            moveCursorPageDown(&cursorPos)
            clearSelection()
            resetVolatileFind()

        case ScrollLineUp:
            scrollLineUp()

        case ScrollLineDown:
            scrollLineDown()

        case SelectRuneLeft:
            moveCursorLeft(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectRuneRight:
            moveCursorRight(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectRuneUp:
            moveCursorUp(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectRuneDown:
            moveCursorDown(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectStartLine:
            moveCursorStartLine(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectEndLine:
            moveCursorEndLine(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectStartFile:
            moveCursorStartFile(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectEndFile:
            moveCursorEndFile(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectAllFile:
            moveCursorStartFile(&selBeg)
            moveCursorEndFile(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectWordLeft:
            moveCursorWordLeft(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectWordRight:
            moveCursorWordRight(&cursorPos, false)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectPageUp:
            moveCursorPageUp(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectPageDown:
            moveCursorPageDown(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(false)

        case SelectRectLeft:
            moveCursorLeft(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(true)

        case SelectRectRight:
            moveCursorRight(&cursorPos)
            resetVolatileFind()
            setRectangularSelection(true)

        case SelectRectUp:
            cp := cursorPos
            moveCursorUp(&cp)
            if cp.charPosInLine == cursorPos.charPosInLine
            {
                cursorPos = cp
                resetVolatileFind()
                setRectangularSelection(true)
            }

        case SelectRectDown:
            cp := cursorPos
            moveCursorDown(&cp)
            if cp.charPosInLine == cursorPos.charPosInLine
            {
                cursorPos = cp
                resetVolatileFind()
                setRectangularSelection(true)
            }

        case DeleteLeft:
            if isReadOnly()
                return
            deleteLeft()
            resetVolatileFind()

        case DeleteRight:
            if isReadOnly()
                return
            deleteRight()
            resetVolatileFind()

        case DeleteWordLeft:
            if isReadOnly()
                return
            deleteWordLeft()
            resetVolatileFind()

        case DeleteWordRight:
            if isReadOnly()
                return
            deleteWordRight()
            resetVolatileFind()

        case EditTabulation:
            if isReadOnly()
                return
            if getBegSelection().lineIndex == getEndSelection().lineIndex
            {
                pushUndo()
                deleteSelection()
                insertTextPrivate("\t", 0, false)
                popUndo()
            }
            else
                insertBegLine("\t", false)
            resetVolatileFind()

        case EditBackTabulation:
            if isReadOnly()
                return
            deleteBegLine("\t", false)
            resetVolatileFind()

        case Undo:
            if isReadOnly()
                return
            undo()
            resetVolatileFind()

        case Redo:
            if isReadOnly()
                return
            redo()
            resetVolatileFind()

        case ToggleOverwrite:
            overwriteMode = !overwriteMode
            invalidate()
        }
    }
}