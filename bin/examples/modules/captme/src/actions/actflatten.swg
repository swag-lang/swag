using Gui

newCmdId("Flatten")
newCmdId("FlattenAll")

struct ActionFlatten {}
impl IActionUI for ActionFlatten
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.Flatten, CmdId.FlattenAll:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()
        let hasSel  = capture and capture.selection.count

        switch cxt.id
        {
        case CmdId.Flatten:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Flatten"
            evt.disabled = !capture or !hasSel

        case CmdId.FlattenAll:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Flatten All"
            evt.disabled = !capture or !capture.forms.count
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
        case CmdId.Flatten:
            undo = capture.newUndo(.Flatten)
        case CmdId.FlattenAll:
            undo = capture.newUndo(.FlattenAll)
        }

        Debug.assert(undo != null)
        capture.pushUndo(undo)
        return true
    }
}
