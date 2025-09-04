using Gui

struct ActionLibSel {}

impl ActionLibSel
{
    newCmdId("Left")
    newCmdId("Right")
    newCmdId("Up")
    newCmdId("Down")
    newCmdId("Home")
    newCmdId("End")
}

impl IActionUI for ActionLibSel
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case ActionLibSel.Left, ActionLibSel.Up, ActionLibSel.Right, ActionLibSel.Down,
             ActionLibSel.Home, ActionLibSel.End:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let libWnd = cast(*LibraryWnd) cxt.wnd
        let hasSel = libWnd.view.focusItem != Swag.U32.Max

        switch cxt.id
        {
        case ActionLibSel.Left, ActionLibSel.Up, ActionLibSel.Right, ActionLibSel.Down,
             ActionLibSel.Home, ActionLibSel.End:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let libWnd = cast(*LibraryWnd) cxt.wnd

        switch cxt.id
        {
        case ActionLibSel.Left:
            libWnd.moveSelLeft()
        case ActionLibSel.Up:
            libWnd.moveSelTop()
        case ActionLibSel.Right:
            libWnd.moveSelRight()
        case ActionLibSel.Down:
            libWnd.moveSelBottom()
        case ActionLibSel.Home:
            libWnd.moveSelHome()
        case ActionLibSel.End:
            libWnd.moveSelEnd()
        }

        return true
    }
}
