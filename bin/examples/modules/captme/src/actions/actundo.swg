using Gui

newCmdId("Undo")
newCmdId("Redo")

struct ActionUndo {}
impl IActionUI for ActionUndo
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.Undo, CmdId.Redo:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()

        switch cxt.id
        {
        case CmdId.Undo:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.name     = "Undo"
            evt.disabled = !capture or !capture.canUndo()
            evt.icon     = main.getTheme().getIcon24(.Undo)

        case CmdId.Redo:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.name     = "Redo"
            evt.disabled = !capture or !capture.canRedo()
            evt.icon     = main.getTheme().getIcon24(.Redo)
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = assume main.getCapture()

        switch cxt.id
        {
        case CmdId.Undo:
            capture.undo()
        case CmdId.Redo:
            capture.redo()
        }

        capture.selectionHasChanged()
        return true
    }
}
