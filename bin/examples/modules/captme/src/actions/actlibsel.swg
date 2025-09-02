using Gui

newCmdId("LibMoveSelLeft")
newCmdId("LibMoveSelRight")
newCmdId("LibMoveSelUp")
newCmdId("LibMoveSelDown")
newCmdId("LibMoveSelHome")
newCmdId("LibMoveSelEnd")

struct ActionLibSel {}
impl IActionUI for ActionLibSel
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.LibMoveSelLeft, CmdId.LibMoveSelUp, CmdId.LibMoveSelRight, CmdId.LibMoveSelDown,
             CmdId.LibMoveSelHome, CmdId.LibMoveSelEnd:
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
        case CmdId.LibMoveSelLeft, CmdId.LibMoveSelUp, CmdId.LibMoveSelRight, CmdId.LibMoveSelDown,
             CmdId.LibMoveSelHome, CmdId.LibMoveSelEnd:
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
        case CmdId.LibMoveSelLeft:
            libWnd.moveSelLeft()
        case CmdId.LibMoveSelUp:
            libWnd.moveSelTop()
        case CmdId.LibMoveSelRight:
            libWnd.moveSelRight()
        case CmdId.LibMoveSelDown:
            libWnd.moveSelBottom()
        case CmdId.LibMoveSelHome:
            libWnd.moveSelHome()
        case CmdId.LibMoveSelEnd:
            libWnd.moveSelEnd()
        }

        return true
    }
}
