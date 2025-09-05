using Gui

struct ActionQuickStyle {}

impl ActionQuickStyle
{
    newCmdId("Reset")
}

impl IActionUI for ActionQuickStyle
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case Reset:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        switch cxt.id
        {
        case Reset:
            evt.setFlags |= .Name
            evt.name = "Restore Default Styles"
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case Reset:
            main.quickWnd.resetQuickStyles()
        }

        return true
    }
}
