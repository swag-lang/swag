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
    runes:          Array'rune
    styles:         Array'u8
    serial:         u64
    hasSelRect:     bool

    begSelectionBefore: RichEditCursor
    endSelectionBefore: RichEditCursor
    begSelectionAfter:  RichEditCursor
    endSelectionAfter:  RichEditCursor
}

internal impl RichEditCtrl
{
    mtd getLastModificationUndo()->u64
    {
        var count = currentUndo
        while count
        {
            if (undoMgr[count - 1].type == .Delete) or
               (undoMgr[count - 1].type == .Insert)
                return count
            count -= 1
        }

        return count
    }

    mtd resetUndo()
    {
        // Its impossible to restore an unmodified state in that case
        if modifiedUndoMarker > currentUndo
            modifiedUndoMarker = Swag.U64.Max

        while undoMgr.count > currentUndo
        {
            let back = undoMgr.popBack()
            Memory.delete(back)
        }
    }
}

impl RichEditCtrl
{
    mtd clearUndo()
    {
        undoMgr.clear()
        toUndo.clear()
        currentUndo = 0
        serialUndo  = 0
        modifiedUndoMarker = Swag.U64.Max
    }

    mtd pushUndo()
    {
        // Nested undo, do not increment serial
        if undoCounter == 0
        {
            serialUndo += 1
            selBegUndo    = selBeg
            cursorPosUndo = cursorPos
        }

        undoCounter += 1
    }

    mtd popUndo()
    {
        Debug.assert(undoCounter != 0)
        undoCounter -= 1

        loop i: toUndo.count
        {
            let undo = toUndo[i]
            undo.serial             = serialUndo
            undo.begSelectionBefore = selBegUndo
            undo.endSelectionBefore = cursorPosUndo
            undo.begSelectionAfter  = selBeg
            undo.endSelectionAfter  = cursorPos
            undoMgr.add(undo)
            currentUndo += 1
        }

        toUndo.count = 0
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
        var undo = undoMgr[currentUndo]
        let serial = undo.serial

        loop
        {
            if undo.type == .Delete
            {
                selBeg    = undo.selBeg
                cursorPos = undo.cursorPos
                var cp = getBegSelection()
                insertTextPrivate(&cp, undo.runes, undo.styles, undo.hasSelRect)
            }
            elif undo.type == .Insert
            {
                selBeg    = undo.selBeg
                cursorPos = undo.cursorPos
                setRectangularSelection(undo.hasSelRect)
                computeRectangularSelection(cursorPos, undo.begSelectionAfter)
                deleteTextPrivate(cursorPos, undo.begSelectionAfter, undo.hasSelRect)
            }

            selBeg    = undo.begSelectionBefore
            cursorPos = undo.endSelectionBefore
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

        var undo = undoMgr[currentUndo]
        let serial = undo.serial
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
                var cp = getBegSelection()
                insertTextPrivate(&cp, undo.runes, undo.styles, undo.hasSelRect)
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