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
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.name = "Undo"
            evt.disabled = !capture or !capture.canUndo()
            evt.icon = main.getTheme().getIcon24(.Undo)

        case CmdIdRedo:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.name = "Redo"
            evt.disabled = !capture or !capture.canRedo()
            evt.icon = main.getTheme().getIcon24(.Redo)
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