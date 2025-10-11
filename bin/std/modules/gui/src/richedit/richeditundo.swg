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
        var count = .currentUndo
        while count
        {
            if (.undoMgr[count - 1].type == .Delete) or
               (.undoMgr[count - 1].type == .Insert) do
                return count
            count -= 1
        }

        return count
    }

    mtd resetUndo()
    {
        // Its impossible to restore an unmodified state in that case
        if .modifiedUndoMarker > .currentUndo do
            .modifiedUndoMarker = Swag.U64.Max

        while .undoMgr.count > .currentUndo
        {
            let back = .undoMgr.popBack()
            Memory.delete(back)
        }
    }
}

impl RichEditCtrl
{
    mtd clearUndo()
    {
        .undoMgr.clear()
        .toUndo.clear()
        .currentUndo        = 0
        .serialUndo         = 0
        .modifiedUndoMarker = Swag.U64.Max
    }

    mtd pushUndo()
    {
        // Nested undo,: not increment serial
        if .undoCounter == 0
        {
            .serialUndo += 1
            .selBegUndo    = .selBeg
            .cursorPosUndo = .cursorPos
        }

        .undoCounter += 1
    }

    mtd popUndo()
    {
        Debug.assert(.undoCounter != 0)
        .undoCounter -= 1

        for i in .toUndo.count
        {
            let undo = .toUndo[i]
            undo.serial             = .serialUndo
            undo.begSelectionBefore = .selBegUndo
            undo.endSelectionBefore = .cursorPosUndo
            undo.begSelectionAfter  = .selBeg
            undo.endSelectionAfter  = .cursorPos
            .undoMgr.add(undo)
            .currentUndo += 1
        }

        .toUndo.count = 0
    }

    mtd canUndo()->bool
    {
        if .undoMgr.count == 0 do
            return false
        if !.currentUndo do
            return false
        return true
    }

    mtd undo()
    {
        if !.canUndo() do
            return

        .currentUndo -= 1
        var undo   = .undoMgr[.currentUndo]
        let serial = undo.serial

        for
        {
            if undo.type == .Delete
            {
                .selBeg    = undo.selBeg
                .cursorPos = undo.cursorPos
                var cp = .getBegSelection()
                .insertTextPrivate(&cp, undo.runes, undo.styles, undo.hasSelRect)
            }
            elif undo.type == .Insert
            {
                .selBeg    = undo.selBeg
                .cursorPos = undo.cursorPos
                .setRectangularSelection(undo.hasSelRect)
                .computeRectangularSelection(.cursorPos, undo.begSelectionAfter)
                .deleteTextPrivate(.cursorPos, undo.begSelectionAfter, undo.hasSelRect)
            }

            .selBeg    = undo.begSelectionBefore
            .cursorPos = undo.endSelectionBefore
            .setRectangularSelection(undo.hasSelRect)
            if !.currentUndo do
                break
            undo = .undoMgr[.currentUndo - 1]
            if undo.serial != serial do
                break
            .currentUndo -= 1
        }

        if .getLastModificationUndo() == .modifiedUndoMarker do
            .setModified(false)
        else do
            .setModified(true)
        .computeScrollY()
    }

    mtd canRedo()->bool
    {
        if .undoMgr.count == 0 do
            return false
        if .currentUndo == .undoMgr.count do
            return false
        return true
    }

    mtd redo()
    {
        if !.canRedo() do
            return

        var undo   = .undoMgr[.currentUndo]
        let serial = undo.serial
        .currentUndo += 1

        for
        {
            if undo.type == .Delete
            {
                .selBeg = undo.selBeg
                .setCursorPos(undo.cursorPos)
                .setRectangularSelection(undo.hasSelRect)
                .computeRectangularSelection(.getBegSelection(), .getEndSelection())
                .deleteTextPrivate(.getBegSelection(), .getEndSelection(), undo.hasSelRect)
            }
            elif undo.type == .Insert
            {
                .selBeg = undo.selBeg
                .setCursorPos(undo.cursorPos)
                var cp = .getBegSelection()
                .insertTextPrivate(&cp, undo.runes, undo.styles, undo.hasSelRect)
            }

            .selBeg = undo.begSelectionAfter
            .setCursorPos(undo.endSelectionAfter)
            .setRectangularSelection(undo.hasSelRect)

            if .currentUndo == .undoMgr.count do
                break
            undo = .undoMgr[.currentUndo]
            if undo.serial != serial do
                break
            .currentUndo += 1
        }

        if .getLastModificationUndo() == .modifiedUndoMarker do
            .setModified(false)
        else do
            .setModified(true)
        .computeScrollY()
    }
}
