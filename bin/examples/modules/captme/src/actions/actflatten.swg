using Gui

const CmdIdFlatten = "CmdIdFlatten"
const CmdIdFlattenAll = "CmdIdFlattenAll"

struct ActionFlatten {}
impl IActionUI for ActionFlatten
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdFlatten, CmdIdFlattenAll:
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
        case CmdIdFlatten:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Flatten"
            evt.disabled = !capture or !hasSel

        case CmdIdFlattenAll:
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
        case CmdIdFlatten:
            undo = capture.newUndo(.Flatten)
        case CmdIdFlattenAll:
            undo = capture.newUndo(.FlattenAll)
        }

        Debug.assert(undo != null)
        capture.pushUndo(undo)
        return true
    }
}
