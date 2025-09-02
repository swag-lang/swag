using Gui

newCmdId("MoveSelLeft")
newCmdId("MoveSelTop")
newCmdId("MoveSelRight")
newCmdId("MoveSelBottom")
newCmdId("MoveSelLeft1")
newCmdId("MoveSelTop1")
newCmdId("MoveSelRight1")
newCmdId("MoveSelBottom1")

struct ActionAlign {}
impl IActionUI for ActionAlign
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.MoveSelLeft, CmdId.MoveSelTop, CmdId.MoveSelRight, CmdId.MoveSelBottom, CmdId.MoveSelLeft1, CmdId.MoveSelTop1, CmdId.MoveSelRight1, CmdId.MoveSelBottom1:
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
        case CmdId.MoveSelLeft, CmdId.MoveSelTop, CmdId.MoveSelRight, CmdId.MoveSelBottom:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel
        case CmdId.MoveSelLeft1, CmdId.MoveSelTop1, CmdId.MoveSelRight1, CmdId.MoveSelBottom1:
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
        case CmdId.MoveSelLeft1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = -1
        case CmdId.MoveSelTop1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = -1
        case CmdId.MoveSelRight1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = 1
        case CmdId.MoveSelBottom1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = 1
        case CmdId.MoveSelLeft:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = -5
        case CmdId.MoveSelTop:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = -5
        case CmdId.MoveSelRight:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = 5
        case CmdId.MoveSelBottom:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = 5
        }

        Debug.assert(undo != null)
        capture.pushUndo(undo)
        main.invalidate()

        return true
    }
}
