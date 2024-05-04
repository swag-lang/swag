using Gui, PropertyList

const CmdIdOptions = "CmdIdOptions"

struct EditorOptions
{
    #[Description("Display labels in the toolbar")]
    showToolBarLabels: bool = true
}

var g_EditorOptions: EditorOptions

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
        switch cxt.id
        {
        case CmdIdOptions:
            evt.setFlags |= .Name
            evt.name = "Options..."
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd
        let view = main.editWnd.editView

        let dlg = Dialog.createDialog(view.surface, 800, 600, title: "Options", flags: .Border | .CloseBtn | .Sizeable)
        dlg.addButton("Close", Dialog.BtnYes)

        let opt = PropertyListCtrl.create(dlg.getView())
        opt.setAllMargins(10)
        opt.dockStyle = .Center
        opt.addStruct(g_EditorOptions, "Editor")
        opt.addStruct(main.getApp().options, "Application")

        opt.prop.sigChanged += closure|main|(opt, item)
        {
            main.setTopBarHeight()
            main.invalidate()
        }

        discard dlg.doModal()
        return true
    }
}
