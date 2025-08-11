using Gui

const CmdIdLibMoveSelLeft   = "CmdIdMoveSelLeft"
const CmdIdLibMoveSelTop    = "CmdIdMoveSelTop"
const CmdIdLibMoveSelRight  = "CmdIdMoveSelRight"
const CmdIdLibMoveSelBottom = "CmdIdMoveSelBottom"

struct ActionLibSel {}
impl IActionUI for ActionLibSel
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdLibMoveSelLeft, CmdIdLibMoveSelTop, CmdIdLibMoveSelRight, CmdIdLibMoveSelBottom:
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
        case CmdIdLibMoveSelLeft, CmdIdLibMoveSelTop, CmdIdLibMoveSelRight, CmdIdLibMoveSelBottom:
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
        case CmdIdLibMoveSelLeft:
            libWnd.moveSelLeft()
        case CmdIdLibMoveSelTop:
            libWnd.moveSelTop()
        case CmdIdLibMoveSelRight:
            libWnd.moveSelRight()
        case CmdIdLibMoveSelBottom:
            libWnd.moveSelBottom()
        }

        return true
    }
}
