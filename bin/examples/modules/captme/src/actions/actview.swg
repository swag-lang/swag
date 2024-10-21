using Gui

const CmdIdViewClip = "CmdIdViewClip"

struct ActionView {}
impl IActionUI for ActionView
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdViewClip:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        switch cxt.id
        {
        case CmdIdViewClip:
            evt.setFlags |= .Name | .Disabled | .Checked
            evt.name     = "Hide Outside Box"
            evt.checked  = g_EditorOptions.clipToSelection
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdViewClip:
            g_EditorOptions.clipToSelection = !g_EditorOptions.clipToSelection
        }

        return true
    }
}
