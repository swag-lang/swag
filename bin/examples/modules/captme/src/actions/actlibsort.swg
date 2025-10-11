using Gui

struct ActionLibSort {}

impl ActionLibSort
{
    newCmdId("Recent")
    newCmdId("FileSize")
    newCmdId("Similar")
    newCmdId("Invert")
}

impl IActionUI for ActionLibSort
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case Recent, FileSize, Invert, Similar:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let libWnd = cast(*LibraryWnd) cxt.wnd

        switch cxt.id
        {
        case Recent:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Write Time"
            evt.checked       = libWnd.sortMethod == .Recent

        case FileSize:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "File Size"
            evt.checked       = libWnd.sortMethod == .FileSize

        case Similar:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Similarity"
            evt.checked       = libWnd.sortMethod == .Similar

        case Invert:
            evt.setFlags |= .Name | .Checked
            if libWnd.sortMethod == .Recent do
                evt.name = "Oldest First"
            elif libWnd.sortMethod == .FileSize do
                evt.name = "Smaller First"
            else
            {
                evt.setFlags |= .Disabled
                evt.disabled = true
                evt.name     = "Invert Order"
            }

            evt.checked = libWnd.invertSort
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let libWnd = cast(*LibraryWnd) cxt.wnd

        switch cxt.id
        {
        case Recent:
            libWnd.setSortMethod(.Recent)
        case FileSize:
            libWnd.setSortMethod(.FileSize)
        case Similar:
            libWnd.setSortMethod(.Similar)

        case Invert:
            libWnd.setSortOrder(!libWnd.invertSort)
        }

        return true
    }
}
