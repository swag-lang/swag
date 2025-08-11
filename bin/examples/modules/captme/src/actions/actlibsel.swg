using Gui

const CmdIdLibMoveSelLeft    = "CmdIdMoveSelLeft"
const CmdIdLibMoveSelTop     = "CmdIdMoveSelTop"
const CmdIdLibMoveSelRight   = "CmdIdMoveSelRight"
const CmdIdLibMoveSelBottom  = "CmdIdMoveSelBottom"

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
        let main = cast(*MainWnd) cxt.wnd

        let capture = main.getCapture()
        let hasSel  = main.libraryMode and main.libWnd.view.focusItem != Swag.U32.Max

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
        let main    = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdMoveSelLeft:
            main.libWnd.moveSelLeft()
        case CmdIdMoveSelTop:
            main.libWnd.moveSelTop()
        case CmdIdMoveSelRight:
            main.libWnd.moveSelRight()
        case CmdIdMoveSelBottom:
            main.libWnd.moveSelBottom()
        }

        return true
    }
}
