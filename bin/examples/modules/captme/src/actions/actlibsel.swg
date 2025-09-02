using Gui

const CmdIdLibMoveSelLeft  = "CmdIdMoveSelLeft"
const CmdIdLibMoveSelRight = "CmdIdMoveSelRight"
const CmdIdLibMoveSelUp    = "CmdIdMoveSelTop"
const CmdIdLibMoveSelDown  = "CmdIdMoveSelBottom"
const CmdIdLibMoveSelHome  = "CmdIdLibMoveSelHome"
const CmdIdLibMoveSelEnd   = "CmdIdLibMoveSelEnd"

struct ActionLibSel {}
impl IActionUI for ActionLibSel
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdLibMoveSelLeft, CmdIdLibMoveSelUp, CmdIdLibMoveSelRight, CmdIdLibMoveSelDown,
             CmdIdLibMoveSelHome, CmdIdLibMoveSelEnd:
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
        case CmdIdLibMoveSelLeft, CmdIdLibMoveSelUp, CmdIdLibMoveSelRight, CmdIdLibMoveSelDown,
             CmdIdLibMoveSelHome, CmdIdLibMoveSelEnd:
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
        case CmdIdLibMoveSelUp:
            libWnd.moveSelTop()
        case CmdIdLibMoveSelRight:
            libWnd.moveSelRight()
        case CmdIdLibMoveSelDown:
            libWnd.moveSelBottom()
        case CmdIdLibMoveSelHome:
            libWnd.moveSelHome()
        case CmdIdLibMoveSelEnd:
            libWnd.moveSelEnd()
        }

        return true
    }
}
