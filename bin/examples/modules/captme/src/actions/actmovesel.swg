using Gui

struct ActionAlign {}

impl ActionAlign
{
    newCmdId("Left")
    newCmdId("Top")
    newCmdId("Right")
    newCmdId("Bottom")
    newCmdId("Left1")
    newCmdId("Top1")
    newCmdId("Right1")
    newCmdId("Bottom1")
}

impl IActionUI for ActionAlign
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case ActionAlign.Left, ActionAlign.Top, ActionAlign.Right, ActionAlign.Bottom, ActionAlign.Left1, ActionAlign.Top1, ActionAlign.Right1, ActionAlign.Bottom1:
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
        case ActionAlign.Left, ActionAlign.Top, ActionAlign.Right, ActionAlign.Bottom:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel
        case ActionAlign.Left1, ActionAlign.Top1, ActionAlign.Right1, ActionAlign.Bottom1:
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
        case ActionAlign.Left1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = -1
        case ActionAlign.Top1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = -1
        case ActionAlign.Right1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = 1
        case ActionAlign.Bottom1:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = 1
        case ActionAlign.Left:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = -5
        case ActionAlign.Top:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = -5
        case ActionAlign.Right:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosX = 5
        case ActionAlign.Bottom:
            undo         = capture.newUndo(.OffsetSelection)
            undo.newPosY = 5
        }

        Debug.assert(undo != null)
        capture.pushUndo(undo)
        main.invalidate()

        return true
    }
}
