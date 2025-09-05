using Gui

struct ActionGlobal {}

impl ActionGlobal
{
    newCmdId("ShowLibrary")
    newCmdId("Exit")
    newCmdId("ShowEditor")
}

impl IActionUI for ActionGlobal
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case ShowEditor, ShowLibrary, Exit:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case ShowEditor:
            evt.setFlags |= .Name
            evt.name = "Show Editor"

        case ShowLibrary:
            evt.setFlags |= .Name | .ToolTip | .Checked
            evt.name    = "Library"
            evt.toolTip = "Display captures library"
            evt.checked = main.libraryMode

        case Exit:
            evt.setFlags |= .Name | .Icon
            evt.name = "Quit"
            evt.icon = main.getTheme().getIcon24(.Quit)
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case ShowEditor:
            main.surface.show()
            main.surface.bringToTop()

        case ShowLibrary:
            main.swapLibraryMode()

        case Exit:
            if main.libWnd:
                main.libWnd.requestClose()
            main.postQuitEvent()
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
