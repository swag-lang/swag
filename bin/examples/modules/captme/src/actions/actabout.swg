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
            evt.name = "About"
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd
        let view = main.editWnd.editView

        var params: AboutDlgParams
        params.dlgTitle = "About CaptMe"
        params.messageTitle = "CaptMe"
        params.message  = "<b>CaptMe<b> was written in <url>Swag<url>."

        params.smallIcon = Icon.from(&main.icons48, 11)
        params.bigIcon = Icon.from(&main.appIcon, 0)
        params.bigIcon.sizeX, params.bigIcon.sizeY = 96

        let dlg = AboutDlg.create(view.surface, params)
        dlg.labelMessage.sigUrlClicked += func(lbl, txt) { File.locateWebPage("https://www.swag-lang.org/"); }
        dlg.doModal(view.surface)

        return true
    }
}
