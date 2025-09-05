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
        case Left, Up, Right, Down,
             Home, End:
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
        case Left, Up, Right, Down,
             Home, End:
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
        case Left:
            libWnd.moveSelLeft()
        case Up:
            libWnd.moveSelTop()
        case Right:
            libWnd.moveSelRight()
        case Down:
            libWnd.moveSelBottom()
        case Home:
            libWnd.moveSelHome()
        case End:
            libWnd.moveSelEnd()
        }

        return true
    }
}
