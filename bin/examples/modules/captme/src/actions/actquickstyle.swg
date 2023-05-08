using Gui

const CmdIdResetQuickStyle = "CmdIdResetQuickStyle"

struct ActionQuickStyle {}
impl IActionUI for ActionQuickStyle
{
    mtd accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdResetQuickStyle:
             return true
        }

        return false
    }

    mtd update(cxt: ActionContext, evt: *CommandUIEvent)->bool
    {
        switch cxt.id
        {
        case CmdIdResetQuickStyle:
            evt.setFlags |= .Name
            evt.name = "Restore Default Styles"
        }

        return true
    }

    mtd execute(cxt: ActionContext)->bool
    {
        main := cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdResetQuickStyle:
            main.quickWnd.resetQuickStyles()
        }

        return true
    }
}
