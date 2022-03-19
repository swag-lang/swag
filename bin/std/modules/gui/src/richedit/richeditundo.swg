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
    selBeg:         RichEditCursor
    cursorPos:      RichEditCursor
    text:           Array'rune
    serial:         uint
    hasSelRect:     bool

    begSelectionBefore: RichEditCursor
    endSelectionBefore: RichEditCursor
    begSelectionAfter:  RichEditCursor
    endSelectionAfter:  RichEditCursor
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
        setRectangularSelection(false)

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
        clearSelection()
    }

    mtd undoDeleteText(posStart, posEnd: RichEditCursor, selRect: bool)
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

    mtd canUndo()->bool
    {
        if undoMgr.count == 0
            return false
        if !currentUndo
            return false
        return true
    }

    mtd undo()
    {
        if !canUndo()
            return

        currentUndo -= 1
        undo   := undoMgr[currentUndo]
        serial := undo.serial

        loop
        {
            if undo.type == .Delete
            {
                selBeg = undo.selBeg
                setCursorPos(undo.cursorPos)
                cp := getBegSelection()
                insertTextPrivate(&cp, undo.text, undo.hasSelRect)
            }
            elif undo.type == .Insert
            {
                selBeg = undo.selBeg
                setCursorPos(undo.cursorPos)
                setRectangularSelection(undo.hasSelRect)
                computeRectangularSelection(cursorPos, undo.begSelectionAfter)
                deleteTextPrivate(cursorPos, undo.begSelectionAfter, undo.hasSelRect)
            }

            selBeg = undo.begSelectionBefore
            setCursorPos(undo.endSelectionBefore)
            setRectangularSelection(undo.hasSelRect)
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

    mtd canRedo()->bool
    {
        if undoMgr.count == 0
            return false
        if currentUndo == undoMgr.count
            return false
        return true
    }

    mtd redo()
    {
        if !canRedo()
            return

        undo   := undoMgr[currentUndo]
        serial := undo.serial
        currentUndo += 1

        loop
        {
            if undo.type == .Delete
            {
                selBeg = undo.selBeg
                setCursorPos(undo.cursorPos)
                setRectangularSelection(undo.hasSelRect)
                computeRectangularSelection(getBegSelection(), getEndSelection())
                deleteTextPrivate(getBegSelection(), getEndSelection(), undo.hasSelRect)
            }
            elif undo.type == .Insert
            {
                selBeg = undo.selBeg
                setCursorPos(undo.cursorPos)
                cp := getBegSelection()
                insertTextPrivate(&cp, undo.text, undo.hasSelRect)
            }

            selBeg = undo.begSelectionAfter
            setCursorPos(undo.endSelectionAfter)
            setRectangularSelection(undo.hasSelRect)

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
}