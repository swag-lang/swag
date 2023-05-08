using Gui

const CmdIdUndo = "CmdIdUndo"
const CmdIdRedo = "CmdIdRedo"

struct ActionUndo {}
impl IActionUI for ActionUndo
{
    mtd accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdUndo,
             CmdIdRedo:
             return true
        }

        return false
    }

    mtd update(cxt: ActionContext, evt: *CommandUIEvent)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()

        switch cxt.id
        {
        case CmdIdUndo:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Undo"
            evt.disabled = !capture or !capture.canUndo()

        case CmdIdRedo:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Redo"
            evt.disabled = !capture or !capture.canRedo()
        }

        return true
    }

    mtd execute(cxt: ActionContext)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()

        switch cxt.id
        {
        case CmdIdUndo: capture.undo()
        case CmdIdRedo: capture.redo()
        }

        capture.selectionHasChanged()
        return true
    }
}