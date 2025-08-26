#global marked
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
    DeleteLine
    //CMD_EDIT_DELETE_LINE
    EditTabulation
    EditBackTabulation
    DeleteWordLeft
    DeleteWordRight
    //CMD_EDIT_DELETE_LINE_LEFT
    //CMD_EDIT_DELETE_LINE_RIGHT
    ToggleOverwrite
    ClipboardCopy
    ClipboardPaste
    ClipboardCut
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
}

impl RichEditCtrl
{
    mtd clearMapping()
    {
        me.mapping.clear()
    }

    mtd addMapping(key: Input.Key, mdf: Input.KeyModifiers, cmd: RichEditCommand)
    {
        var m: RichEditMapping
        m.key       = key
        m.modifiers = mdf
        m.command   = cmd
        me.mapping.add(m)
    }

    mtd getMappingCommand(key: Input.Key, mdf: Input.KeyModifiers)->RichEditCommand
    {
        foreach &p in me.mapping
        {
            if p.key == key and p.modifiers == mdf:
                return p.command
        }

        return RichEditCommand.None
    }

    mtd setDefaultMapping()
    {
        me.addMapping(.Left, .Zero, .CursorLeft)
        me.addMapping(.Left, .Shift, .SelectRuneLeft)
        me.addMapping(.Left, .Control, .CursorWordLeft)
        me.addMapping(.Left, .CtrlShift, .SelectWordLeft)
        me.addMapping(.Left, .Alt, .SelectRectLeft)
        me.addMapping(.Right, .Zero, .CursorRight)
        me.addMapping(.Right, .Shift, .SelectRuneRight)
        me.addMapping(.Right, .Control, .CursorWordRight)
        me.addMapping(.Right, .CtrlShift, .SelectWordRight)
        me.addMapping(.Right, .Alt, .SelectRectRight)
        me.addMapping(.Up, .Zero, .CursorUp)
        me.addMapping(.Up, .Shift, .SelectRuneUp)
        me.addMapping(.Up, .Control, .ScrollLineUp)
        me.addMapping(.Up, .Alt, .SelectRectUp)
        me.addMapping(.Down, .Zero, .CursorDown)
        me.addMapping(.Down, .Shift, .SelectRuneDown)
        me.addMapping(.Down, .Shift, .SelectRuneDown)
        me.addMapping(.Down, .Control, .ScrollLineDown)
        me.addMapping(.Down, .Alt, .SelectRectDown)
        me.addMapping(.PageUp, .Zero, .CursorPageUp)
        me.addMapping(.PageUp, .Shift, .SelectPageUp)
        me.addMapping(.PageDown, .Zero, .CursorPageDown)
        me.addMapping(.PageDown, .Shift, .SelectPageDown)
        me.addMapping(.Home, .Zero, .CursorStartLine)
        me.addMapping(.Home, .Control, .CursorStartFile)
        me.addMapping(.Home, .Shift, .SelectStartLine)
        me.addMapping(.Home, .CtrlShift, .SelectStartFile)
        me.addMapping(.End, .Zero, .CursorEndLine)
        me.addMapping(.End, .Control, .CursorEndFile)
        me.addMapping(.End, .Shift, .SelectEndLine)
        me.addMapping(.End, .CtrlShift, .SelectEndFile)
        me.addMapping(.Back, .Zero, .DeleteLeft)
        me.addMapping(.Back, .Control, .DeleteWordLeft)
        me.addMapping(.Delete, .Zero, .DeleteRight)
        me.addMapping(.Delete, .Control, .DeleteWordRight)
        me.addMapping(.Tab, .Zero, .EditTabulation)
        me.addMapping(.Tab, .Shift, .EditBackTabulation)
        me.addMapping(.Insert, .Zero, .ToggleOverwrite)
        me.addMapping(.Insert, .Control, .ClipboardCopy)
        me.addMapping(.Insert, .Shift, .ClipboardPaste)
        me.addMapping(.A, .Control, .SelectAllFile)
        me.addMapping(.C, .Control, .ClipboardCopy)
        me.addMapping(.L, .Control, .DeleteLine)
        me.addMapping(.V, .Control, .ClipboardPaste)
        me.addMapping(.X, .Control, .ClipboardCut)
        me.addMapping(.Y, .Control, .Redo)
        me.addMapping(.Z, .Control, .Undo)
    }

    // Execute a command
    mtd executeCommand(cmd: RichEditCommand)
    {
        switch cmd
        {
        case CursorLeft:
            if me.isSelectionEmpty():
                me.moveCursorLeft(&me.cursorPos)
            else:
                me.setCursorPos(me.getBegSelection())
            me.clearSelection()

        case CursorRight:
            if me.isSelectionEmpty():
                me.moveCursorRight(&me.cursorPos)
            else:
                me.setCursorPos(me.getEndSelection())
            me.clearSelection()

        case CursorUp:
            if !me.isSelectionEmpty()
            {
                var pt = me.getCursorDisplayPos(me.cursorPos)
                me.setCursorPos(me.getBegSelection())
                me.moveCursorUp(&me.cursorPos, pt)
            }
            else:
                me.moveCursorUp(&me.cursorPos)
            me.clearSelection()

        case CursorDown:
            if !me.isSelectionEmpty()
            {
                var pt = me.getCursorDisplayPos(me.cursorPos)
                me.setCursorPos(me.getEndSelection())
                me.moveCursorDown(&me.cursorPos, pt)
            }
            else:
                me.moveCursorDown(&me.cursorPos)
            me.clearSelection()

        case CursorStartLine:
            me.moveCursorStartLine(&me.cursorPos)
            me.clearSelection()

        case CursorEndLine:
            me.moveCursorEndLine(&me.cursorPos)
            me.clearSelection()

        case CursorStartFile:
            me.moveCursorStartFile(&me.cursorPos)
            me.clearSelection()

        case CursorEndFile:
            me.moveCursorEndFile(&me.cursorPos)
            me.clearSelection()

        case CursorWordLeft:
            me.moveCursorWordLeft(&me.cursorPos)
            me.clearSelection()

        case CursorWordRight:
            me.moveCursorWordRight(&me.cursorPos, true)
            me.clearSelection()

        case CursorPageUp:
            me.moveCursorPageUp(&me.cursorPos)
            me.clearSelection()

        case CursorPageDown:
            me.moveCursorPageDown(&me.cursorPos)
            me.clearSelection()

        case ScrollLineUp:
            me.scrollLineUp()

        case ScrollLineDown:
            me.scrollLineDown()

        case SelectRuneLeft:
            me.moveCursorLeft(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectRuneRight:
            me.moveCursorRight(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectRuneUp:
            me.moveCursorUp(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectRuneDown:
            me.moveCursorDown(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectStartLine:
            me.moveCursorStartLine(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectEndLine:
            me.moveCursorEndLine(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectStartFile:
            me.moveCursorStartFile(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectEndFile:
            me.moveCursorEndFile(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectAllFile:
            me.moveCursorStartFile(&me.selBeg)
            me.moveCursorEndFile(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectWordLeft:
            me.moveCursorWordLeft(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectWordRight:
            me.moveCursorWordRight(&me.cursorPos, false)
            me.setRectangularSelection(false)

        case SelectPageUp:
            me.moveCursorPageUp(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectPageDown:
            me.moveCursorPageDown(&me.cursorPos)
            me.setRectangularSelection(false)

        case SelectRectLeft:
            var cp = me.cursorPos
            me.moveCursorLeft(&cp)
            if cp.lineIndex == me.cursorPos.lineIndex
            {
                me.cursorPos = cp
                me.setRectangularSelection(true)
            }

        case SelectRectRight:
            var cp = me.cursorPos
            me.moveCursorRight(&cp)
            if cp.lineIndex == me.cursorPos.lineIndex
            {
                me.cursorPos = cp
                me.setRectangularSelection(true)
            }

        case SelectRectUp:
            var cp = me.cursorPos
            me.moveCursorUp(&cp)
            if cp.charPosInLine == me.cursorPos.charPosInLine
            {
                me.cursorPos = cp
                me.setRectangularSelection(true)
            }

        case SelectRectDown:
            var cp = me.cursorPos
            me.moveCursorDown(&cp)
            if cp.charPosInLine == me.cursorPos.charPosInLine
            {
                me.cursorPos = cp
                me.setRectangularSelection(true)
            }

        case DeleteLeft:
            if me.isReadOnly():
                return
            me.deleteLeft()

        case DeleteRight:
            if me.isReadOnly():
                return
            me.deleteRight()

        case DeleteWordLeft:
            if me.isReadOnly():
                return
            me.deleteWordLeft()

        case DeleteWordRight:
            if me.isReadOnly():
                return
            me.deleteWordRight()

        case DeleteLine:
            if me.isReadOnly():
                return
            me.deleteLine()

        case EditTabulation:
            if me.isReadOnly():
                return
            if me.getBegSelection().lineIndex == me.getEndSelection().lineIndex
            {
                me.pushUndo()
                me.deleteSelection()
                me.insertText("\t")
                me.popUndo()
            }
            else:
                me.insertBegLine("\t")

        case EditBackTabulation:
            if me.isReadOnly():
                return
            me.deleteBegLine("\t", false)

        case Undo:
            if me.isReadOnly():
                return
            me.undo()

        case Redo:
            if me.isReadOnly():
                return
            me.redo()

        case ToggleOverwrite:
            me.overwriteMode = !me.overwriteMode
            me.invalidate()

        case ClipboardCopy:
            me.copyToClipboard()

        case ClipboardPaste:
            if me.isReadOnly():
                return
            me.pasteFromClipboard()

        case ClipboardCut:
            if me.isReadOnly():
                return
            me.copyToClipboard()
            me.deleteSelection()
        }
    }
}
