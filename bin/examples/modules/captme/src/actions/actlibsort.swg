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
        case ActionLibSort.Recent, ActionLibSort.FileSize, ActionLibSort.Invert, ActionLibSort.Similar:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let libWnd = cast(*LibraryWnd) cxt.wnd

        switch cxt.id
        {
        case ActionLibSort.Recent:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Write Time"
            evt.checked       = libWnd.sortMethod == .Recent

        case ActionLibSort.FileSize:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "File Size"
            evt.checked       = libWnd.sortMethod == .FileSize

        case ActionLibSort.Similar:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Similarity"
            evt.checked       = libWnd.sortMethod == .Similar

        case ActionLibSort.Invert:
            evt.setFlags |= .Name | .Checked
            if libWnd.sortMethod == .Recent:
                evt.name = "Oldest First"
            elif libWnd.sortMethod == .FileSize:
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
        case ActionLibSort.Recent:
            libWnd.setSortMethod(.Recent)
        case ActionLibSort.FileSize:
            libWnd.setSortMethod(.FileSize)
        case ActionLibSort.Similar:
            libWnd.setSortMethod(.Similar)

        case ActionLibSort.Invert:
            libWnd.setSortOrder(!libWnd.invertSort)
        }

        return true
    }
}
