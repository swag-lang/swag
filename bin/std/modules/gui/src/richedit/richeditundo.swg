#global marked
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
    type:                   RichEditUndoType = Selection
    concatChars:            bool
    selBeg:                 RichEditCursor
    cursorPos:              RichEditCursor
    runes:                  Array'rune
    styles:                 Array'u8
    serial:                 u64
    hasSelRect:             bool

    begSelectionBefore:     RichEditCursor
    endSelectionBefore:     RichEditCursor
    begSelectionAfter:      RichEditCursor
    endSelectionAfter:      RichEditCursor
}

internal impl RichEditCtrl
{
    mtd getLastModificationUndo()->u64
    {
        var count = me.currentUndo
        while count
        {
            if (me.undoMgr[count - 1].type == .Delete) or
               (me.undoMgr[count - 1].type == .Insert):
                return count
            count -= 1
        }

        return count
    }

    mtd resetUndo()
    {
        // Its impossible to restore an unmodified state in that case
        if me.modifiedUndoMarker > me.currentUndo:
            me.modifiedUndoMarker = Swag.U64.Max

        while me.undoMgr.count > me.currentUndo
        {
            let back = me.undoMgr.popBack()
            Memory.delete(back)
        }
    }
}

impl RichEditCtrl
{
    mtd clearUndo()
    {
        me.undoMgr.clear()
        me.toUndo.clear()
        me.currentUndo        = 0
        me.serialUndo         = 0
        me.modifiedUndoMarker = Swag.U64.Max
    }

    mtd pushUndo()
    {
        // Nested undo,: not increment serial
        if me.undoCounter == 0
        {
            me.serialUndo += 1
            me.selBegUndo    = me.selBeg
            me.cursorPosUndo = me.cursorPos
        }

        me.undoCounter += 1
    }

    mtd popUndo()
    {
        Debug.assert(me.undoCounter != 0)
        me.undoCounter -= 1

        for i in me.toUndo.count
        {
            let undo = me.toUndo[i]
            undo.serial             = me.serialUndo
            undo.begSelectionBefore = me.selBegUndo
            undo.endSelectionBefore = me.cursorPosUndo
            undo.begSelectionAfter  = me.selBeg
            undo.endSelectionAfter  = me.cursorPos
            me.undoMgr.add(undo)
            me.currentUndo += 1
        }

        me.toUndo.count = 0
    }

    mtd canUndo()->bool
    {
        if me.undoMgr.count == 0:
            return false
        if !me.currentUndo:
            return false
        return true
    }

    mtd undo()
    {
        if !me.canUndo():
            return

        me.currentUndo -= 1
        var undo   = me.undoMgr[me.currentUndo]
        let serial = undo.serial

        for
        {
            if undo.type == .Delete
            {
                me.selBeg    = undo.selBeg
                me.cursorPos = undo.cursorPos
                var cp = me.getBegSelection()
                me.insertTextPrivate(&cp, undo.runes, undo.styles, undo.hasSelRect)
            }
            elif undo.type == .Insert
            {
                me.selBeg    = undo.selBeg
                me.cursorPos = undo.cursorPos
                me.setRectangularSelection(undo.hasSelRect)
                me.computeRectangularSelection(me.cursorPos, undo.begSelectionAfter)
                me.deleteTextPrivate(me.cursorPos, undo.begSelectionAfter, undo.hasSelRect)
            }

            me.selBeg    = undo.begSelectionBefore
            me.cursorPos = undo.endSelectionBefore
            me.setRectangularSelection(undo.hasSelRect)
            if !me.currentUndo:
                break
            undo = me.undoMgr[me.currentUndo - 1]
            if undo.serial != serial:
                break
            me.currentUndo -= 1
        }

        if me.getLastModificationUndo() == me.modifiedUndoMarker:
            me.setModified(false)
        else:
            me.setModified(true)
        me.computeScrollY()
    }

    mtd canRedo()->bool
    {
        if me.undoMgr.count == 0:
            return false
        if me.currentUndo == me.undoMgr.count:
            return false
        return true
    }

    mtd redo()
    {
        if !me.canRedo():
            return

        var undo   = me.undoMgr[me.currentUndo]
        let serial = undo.serial
        me.currentUndo += 1

        for
        {
            if undo.type == .Delete
            {
                me.selBeg = undo.selBeg
                me.setCursorPos(undo.cursorPos)
                me.setRectangularSelection(undo.hasSelRect)
                me.computeRectangularSelection(me.getBegSelection(), me.getEndSelection())
                me.deleteTextPrivate(me.getBegSelection(), me.getEndSelection(), undo.hasSelRect)
            }
            elif undo.type == .Insert
            {
                me.selBeg = undo.selBeg
                me.setCursorPos(undo.cursorPos)
                var cp = me.getBegSelection()
                me.insertTextPrivate(&cp, undo.runes, undo.styles, undo.hasSelRect)
            }

            me.selBeg = undo.begSelectionAfter
            me.setCursorPos(undo.endSelectionAfter)
            me.setRectangularSelection(undo.hasSelRect)

            if me.currentUndo == me.undoMgr.count:
                break
            undo = me.undoMgr[me.currentUndo]
            if undo.serial != serial:
                break
            me.currentUndo += 1
        }

        if me.getLastModificationUndo() == me.modifiedUndoMarker:
            me.setModified(false)
        else:
            me.setModified(true)
        me.computeScrollY()
    }
}
