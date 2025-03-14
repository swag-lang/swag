using Gui

const CmdIdMoveSelLeft    = "CmdIdMoveSelLeft"
const CmdIdMoveSelTop     = "CmdIdMoveSelTop"
const CmdIdMoveSelRight   = "CmdIdMoveSelRight"
const CmdIdMoveSelBottom  = "CmdIdMoveSelBottom"
const CmdIdMoveSelLeft1   = "CmdIdMoveSelLeft1"
const CmdIdMoveSelTop1    = "CmdIdMoveSelTop1"
const CmdIdMoveSelRight1  = "CmdIdMoveSelRight1"
const CmdIdMoveSelBottom1 = "CmdIdMoveSelBottom1"

struct ActionAlign {}
impl IActionUI for ActionAlign
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdMoveSelLeft, CmdIdMoveSelTop, CmdIdMoveSelRight, CmdIdMoveSelBottom, CmdIdMoveSelLeft1, CmdIdMoveSelTop1, CmdIdMoveSelRight1, CmdIdMoveSelBottom1:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        let capture = main.getCapture()
        let hasSel  = capture and capture.selection.count

        switch cxt.id
        {
        case CmdIdMoveSelLeft, CmdIdMoveSelTop, CmdIdMoveSelRight, CmdIdMoveSelBottom:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel
        case CmdIdMoveSelLeft1, CmdIdMoveSelTop1, CmdIdMoveSelRight1, CmdIdMoveSelBottom1:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = assume main.getCapture()

        var undo: *Undo
        switch cxt.id
        {
        case CmdIdMoveSelLeft1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = -1
        case CmdIdMoveSelTop1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = -1
        case CmdIdMoveSelRight1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = 1
        case CmdIdMoveSelBottom1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = 1
        case CmdIdMoveSelLeft:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = -5
        case CmdIdMoveSelTop:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = -5
        case CmdIdMoveSelRight:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = 5
        case CmdIdMoveSelBottom:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = 5
        }

        Debug.assert(undo != null)
        capture.pushUndo(undo)
        main.invalidate()

        return true
    }
}
