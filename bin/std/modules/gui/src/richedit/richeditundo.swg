#global public
using Core

enum RichEditUndoType
{
    Delete
    Insert
    Selection
    Show
    Hide
}

struct RichEditUndo
{
    type:           RichEditUndoType = Selection
    concatChars:    bool
    selBeg:         RichEditCursorPos
    cursorPos:      RichEditCursorPos
    text:           Array'rune
    serial:         uint
    hasSelRect:     bool

    begSelectionBefore: RichEditCursorPos
    endSelectionBefore: RichEditCursorPos
    begSelectionAfter:  RichEditCursorPos
    endSelectionAfter:  RichEditCursorPos
}

impl RichEdit
{
    mtd resetUndo()
    {
        // Its impossible to restore an unmodified state in that case
        if modifiedUndoMarker > currentUndo
            modifiedUndoMarker = Swag.UInt.Max

        while undoMgr.count > currentUndo
        {
            back := undoMgr.popBack()
            Memory.delete(back)
        }
    }

    mtd pushUndo()
    {
        // Nested undo, do not increment serial
        if undoCounter == 0
            serialUndo += 1
        undoCounter += 1

        selBegUndo    = selBeg
        cursorPosUndo = cursorPos
    }

    mtd popUndo()
    {
        Debug.assert(undoCounter != 0)
        undoCounter -= 1
        cmdSetSelRect(false)

        loop i: toUndo.count
        {
            undo := toUndo[i]
            undo.serial             = serialUndo
            undo.begSelectionBefore = selBegUndo
            undo.endSelectionBefore = cursorPosUndo
            undo.begSelectionAfter  = selBeg
            undo.endSelectionAfter  = cursorPos
            undoMgr.add(undo)
            currentUndo += 1
        }

        toUndo.clear()
    }

    mtd undoInsertText(text: string, selRect: bool)
    {
        undo := Memory.new'RichEditUndo()
        undo.type       = .Insert
        undo.text       = Unicode.fromUtf8(text)
        undo.selBeg     = selBeg
        undo.cursorPos  = cursorPos
        undo.hasSelRect = selRect
        undo.serial     = 0
        resetUndo()
        toUndo.add(undo)
        insertTextPrivate(&cursorPos, text, selRect)
        cmdClearSelection()
    }

    mtd undoDeleteText(posStart, posEnd: RichEditCursorPos, selRect: bool)
    {
        undo := Memory.new'RichEditUndo()
        undo.type       = .Delete
        undo.text       = getTextPrivate(posStart, posEnd, selRect)
        undo.selBeg     = selBeg
        undo.cursorPos  = cursorPos
        undo.hasSelRect = selRect
        undo.serial     = 0
        resetUndo()
        toUndo.add(undo)
        deleteTextPrivate(posStart, posEnd, selRect);
    }

    mtd undoRestoreSelection()
    {
        undo := Memory.new'RichEditUndo()
        undo.type      = .Selection
        undo.selBeg    = selBeg
        undo.cursorPos = cursorPos
        undo.serial    = 0
        resetUndo()
        toUndo.add(undo)
    }

    mtd getLastModificationUndo()->uint
    {
        count := currentUndo
        while count
        {
            if (undoMgr[count - 1].type == .Delete) or
               (undoMgr[count - 1].type == .Insert)
                return count
            count -= 1
        }

        return count
    }
}