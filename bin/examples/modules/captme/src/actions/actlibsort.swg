using Gui

const CmdIdLibSortRecent   = "CmdIdLibSortRecent"
const CmdIdLibSortFileSize = "CmdIdLibSortFileSize"
const CmdIdLibSortSimilar  = "CmdIdLibSortSimilar"
const CmdIdLibSortInvert   = "CmdIdLibSortInvert"

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
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdLibSortRecent:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name = "Write Time"
            evt.checked = main.libWnd.sortMethod == .Recent

        case CmdIdLibSortFileSize:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name = "File Size"
            evt.checked = main.libWnd.sortMethod == .FileSize

        case CmdIdLibSortSimilar:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name = "Similarity"
            evt.checked = main.libWnd.sortMethod == .Similar

        case CmdIdLibSortInvert:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Invert Order"
            evt.checked = main.libWnd.invertSort
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdLibSortRecent:
            main.libWnd.setSortMethod(.Recent)
        case CmdIdLibSortFileSize:
            main.libWnd.setSortMethod(.FileSize)
        case CmdIdLibSortSimilar:
            main.libWnd.setSortMethod(.Similar)

        case CmdIdLibSortInvert:
            main.libWnd.setSortOrder(!main.libWnd.invertSort)
        }

        return true
    }
}