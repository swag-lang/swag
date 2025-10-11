using Gui

struct ActionGlobal {}

impl ActionGlobal
{
    newCmdId("ShowLibrary")
    newCmdId("Exit")
    newCmdId("Minimize")
    newCmdId("ShowEditor")
}

impl IActionUI for ActionGlobal
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case ShowEditor, ShowLibrary, Exit, Minimize:
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

        case Minimize:
            evt.setFlags |= .Name | .Icon
            evt.name = "Minimize"
            evt.icon = main.getTheme().getIcon24(.Minimize)
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
            if main.libWnd do
                main.libWnd.requestClose()
            main.postQuitEvent()

        case Minimize:
            main.surface.hide()
        }

        return true
    }
}

impl MainWnd
{
    mtd swapLibraryMode()
    {
        .libraryMode = !.libraryMode

        .editWnd.show(!.libraryMode)
        .quickStyleBar.show(!.libraryMode)
        .recentBar.show(!.libraryMode)
        .editZoomBar.show(!.libraryMode)
        .editToolBar.show(!.libraryMode)

        .libWnd.show(.libraryMode)

        if .libraryMode do
            .libWnd.fill()

        .applyLayout()
    }
}
