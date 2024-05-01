using Gui

const CmdIdAbout = "CmdIdAbout"

struct ActionAbout {}
impl IActionUI for ActionAbout
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdAbout:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        switch cxt.id
        {
        case CmdIdAbout:
            evt.setFlags |= .Name
            evt.name     = "About"
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd
        let view = main.editWnd.editView

        var msg: String
        msg = "<b>CaptMe<b> was written in <url>Swag<url>."
        MessageDlg.ok(view.surface, msg, Icon.from(&main.icons48, 11))
        return true
    }
}
