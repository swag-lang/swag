using Gui

struct ActionFlatten {}

impl ActionFlatten
{
    newCmdId("Flatten")
    newCmdId("FlattenAll")
}

impl IActionUI for ActionFlatten
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case Flatten, FlattenAll:
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
        case Flatten:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Flatten"
            evt.disabled = !capture or !hasSel

        case FlattenAll:
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
        case Flatten:
            undo = capture.newUndo(.Flatten)
        case FlattenAll:
            undo = capture.newUndo(.FlattenAll)
        }

        Debug.assert(undo != null)
        capture.pushUndo(undo)
        return true
    }
}
