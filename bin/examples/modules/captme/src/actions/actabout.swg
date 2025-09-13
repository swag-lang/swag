using Gui

struct ActionAbout {}

impl ActionAbout
{
    newCmdId("About")
}

impl IActionUI for ActionAbout
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case ActionAbout.About:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd
        switch cxt.id
        {
        case About:
            evt.setFlags |= .Name | .Icon
            evt.name = "About..."
            evt.icon = main.getTheme().getIcon24(.About)
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd
        let view = main.editWnd.editView

        const Date = #run Time.DateTime.now()

        var params: AboutDlgParams
        params.dlgTitle     = "About CaptMe"
        params.messageTitle = Format.toString("CaptMe %.%.%", #swagversion, #swagrevision, #swagbuildnum).toString()

        var msg: String
        msg += Format.toString("Build on %{j}", Date)
        msg += "\n\n"
        msg += "<b>CaptMe</b> is open source and coded in <url>Swag</url>. You can find the code <url>here</url>."
        msg += "\n\n"
        msg += Format.toString("Copyright \u00A9 % Christophe Beaudet", Date.year)
        params.message = msg.toString()

        params.iconTitle = Icon.from(&main.icons48, 11)
        params.bigIcon   = Icon.from(&main.appIcon, 0)
        params.bigIcon.sizeX, params.bigIcon.sizeY = 96
        params.bigIcon.color = Argb.White

        let dlg = AboutDlg.create(view.surface, params, height: 330)
        dlg.labelMessage.sigUrlClicked += func(lbl, txt)
        {
            switch txt
            {
            case "Swag":
                Env.locateUrl("https://www.swag-lang.org/")
            case "here":
                Env.locateUrl("https://github.com/swag-lang/swag/tree/master/bin/examples/modules/captme/")
            }
        }

        dlg.doModal(view.surface)

        return true
    }
}
