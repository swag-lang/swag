using Gui

struct ActionGlobal {}

impl ActionGlobal
{
    newCmdId("Library")
    newCmdId("Exit")
    newCmdId("ShowEditor")
}

impl IActionUI for ActionGlobal
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case ActionGlobal.Library, ActionGlobal.Exit, ActionGlobal.ShowEditor:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case ActionGlobal.ShowEditor:
            evt.setFlags |= .Name
            evt.name = "Show Editor"

        case ActionGlobal.Exit:
            evt.setFlags |= .Name | .Icon
            evt.name = "Quit"
            evt.icon = main.getTheme().getIcon24(.Quit)

        case ActionGlobal.Library:
            evt.setFlags |= .Name | .ToolTip | .Checked
            evt.name    = "Library"
            evt.toolTip = "Display captures library"
            evt.checked = main.libraryMode
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case ActionGlobal.ShowEditor:
            main.surface.show()
            main.surface.bringToTop()

        case ActionGlobal.Exit:
            if main.libWnd:
                main.libWnd.requestClose()
            main.postQuitEvent()

        case ActionGlobal.Library:
            main.swapLibraryMode()
        }

        return true
    }
}

impl MainWnd
{
    mtd swapLibraryMode()
    {
        me.libraryMode = !me.libraryMode

        me.editWnd.show(!me.libraryMode)
        me.quickStyleBar.show(!me.libraryMode)
        me.recentBar.show(!me.libraryMode)
        me.editZoomBar.show(!me.libraryMode)
        me.editToolBar.show(!me.libraryMode)

        me.libWnd.show(me.libraryMode)

        if me.libraryMode:
            me.libWnd.fill()

        me.applyLayout()
    }
}
