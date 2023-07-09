using Gui

const CmdIdResetQuickStyle = "CmdIdResetQuickStyle"

struct ActionQuickStyle {}
impl IActionUI for ActionQuickStyle
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdResetQuickStyle:
             return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        switch cxt.id
        {
        case CmdIdResetQuickStyle:
            evt.setFlags |= .Name
            evt.name = "Restore Default Styles"
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        var main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdResetQuickStyle:
            main.quickWnd.resetQuickStyles()
        }

        return true
    }
}
