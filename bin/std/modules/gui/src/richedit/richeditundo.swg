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
    selBeg:         RichEditCharPos
    cursorPos:      RichEditCharPos
    text:           Array'rune
    serial:         uint
    hasSelRect:     bool

    begSelectionBefore: RichEditCharPos
    endSelectionBefore: RichEditCharPos
    begSelectionAfter:  RichEditCharPos
    endSelectionAfter:  RichEditCharPos
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

    mtd undoDeleteText(posStart, posEnd: RichEditCharPos, selRect: bool)
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

        // Do action
        //deleteTextPrivate(_posstart, _posend, _selrect);
    }
}