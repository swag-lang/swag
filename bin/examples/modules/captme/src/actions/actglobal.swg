using Gui

const CmdIdLibrary    = "CmdIdLibrary"
const CmdIdExit       = "CmdIdExit"
const CmdIdShowEditor = "CmdIdShowEditor"

struct ActionGlobal {}
impl IActionUI for ActionGlobal
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdLibrary,
             CmdIdExit,
             CmdIdShowEditor:
             return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdShowEditor:
            evt.setFlags |= .Name
            evt.name = "Show Editor"

        case CmdIdExit:
            evt.setFlags |= .Name
            evt.name = "Exit"

        case CmdIdLibrary:
            evt.setFlags |= .Name | .ToolTip | .Checked
            evt.name = "Library"
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
        case CmdIdShowEditor:
            main.surface.show()
            main.surface.bringToTop()

        case CmdIdExit:
            main.libWnd.requestClose()
            main.postQuitEvent()

        case CmdIdLibrary:
            main.swapLibraryMode()
        }

        return true
    }
}

impl MainWnd
{
    mtd swapLibraryMode()
    {
        libraryMode = !libraryMode

        editWnd.show(!libraryMode)
        quickStyleBar.show(!libraryMode)
        recentBar.show(!libraryMode)
        editZoomBar.show(!libraryMode)
        editToolBar.show(!libraryMode)

        libWnd.show(libraryMode)

        if libraryMode do
            libWnd.fill()

        applyLayout()
    }
}