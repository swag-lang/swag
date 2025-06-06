using Gui

const CmdIdOptions = "CmdIdOptions"

struct ActionOptions {}
impl IActionUI for ActionOptions
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdOptions:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdOptions:
            evt.setFlags |= .Name | .Icon
            evt.name = "Options..."
            evt.icon = main.getTheme().getIcon24(.Options)
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd
        let view = main.editWnd.editView

        let dlg = Dialog.createDialog(view.surface, 800, 600, title: "Options", flags: .Border | .CloseBtn | .Sizeable)
        dlg.addButton("Close", Dialog.BtnYes)

        let opt = PropertiesCtrl.create(dlg.getView(), flags: .SectionFilter | .FindBar)
        opt.setAllMargins(10)
        opt.dockStyle = .Center
        opt.addStruct(g_CaptureOptions, "Capture")
        opt.addStruct(g_EditorOptions, "Editor")
        opt.addStruct(main.getApp().options, "Application")

        opt.prop.sigChanged += func|main|(opt, item)
        {
            main.setTopBarHeight()
            main.invalidate()
        }

        discard dlg.doModal()
        return true
    }
}
