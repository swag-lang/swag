using Gui

const CmdIdLibSortRecent = "CmdIdLibSortRecent"
const CmdIdLibSortFileSize = "CmdIdLibSortFileSize"
const CmdIdLibSortSimilar = "CmdIdLibSortSimilar"
const CmdIdLibSortInvert = "CmdIdLibSortInvert"

struct ActionLibSort {}
impl IActionUI for ActionLibSort
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdLibSortRecent, CmdIdLibSortFileSize, CmdIdLibSortInvert, CmdIdLibSortSimilar:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let libWnd = cast(*LibraryWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdLibSortRecent:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Write Time"
            evt.checked       = libWnd.sortMethod == .Recent

        case CmdIdLibSortFileSize:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "File Size"
            evt.checked       = libWnd.sortMethod == .FileSize

        case CmdIdLibSortSimilar:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Similarity"
            evt.checked       = libWnd.sortMethod == .Similar

        case CmdIdLibSortInvert:
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
        case CmdIdLibSortRecent:
            libWnd.setSortMethod(.Recent)
        case CmdIdLibSortFileSize:
            libWnd.setSortMethod(.FileSize)
        case CmdIdLibSortSimilar:
            libWnd.setSortMethod(.Similar)

        case CmdIdLibSortInvert:
            libWnd.setSortOrder(!libWnd.invertSort)
        }

        return true
    }
}
