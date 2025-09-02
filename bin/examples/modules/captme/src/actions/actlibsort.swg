using Gui

newCmdId("LibSortRecent")
newCmdId("LibSortFileSize")
newCmdId("LibSortSimilar")
newCmdId("LibSortInvert")

struct ActionLibSort {}
impl IActionUI for ActionLibSort
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.LibSortRecent, CmdId.LibSortFileSize, CmdId.LibSortInvert, CmdId.LibSortSimilar:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let libWnd = cast(*LibraryWnd) cxt.wnd

        switch cxt.id
        {
        case CmdId.LibSortRecent:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Write Time"
            evt.checked       = libWnd.sortMethod == .Recent

        case CmdId.LibSortFileSize:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "File Size"
            evt.checked       = libWnd.sortMethod == .FileSize

        case CmdId.LibSortSimilar:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Similarity"
            evt.checked       = libWnd.sortMethod == .Similar

        case CmdId.LibSortInvert:
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
        case CmdId.LibSortRecent:
            libWnd.setSortMethod(.Recent)
        case CmdId.LibSortFileSize:
            libWnd.setSortMethod(.FileSize)
        case CmdId.LibSortSimilar:
            libWnd.setSortMethod(.Similar)

        case CmdId.LibSortInvert:
            libWnd.setSortOrder(!libWnd.invertSort)
        }

        return true
    }
}
