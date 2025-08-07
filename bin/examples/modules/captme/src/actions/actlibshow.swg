using Gui

const CmdIdLibShowUpToDateLinkedImage    = "CmdIdLibShowUpToDateLinkedImage"
const CmdIdLibShowLinkedImage            = "CmdIdLibShowLinkedImage"
const CmdIdLibShowUnSavedLinkedImage = "CmdIdLibShowUnSavedLinkedImage"
const CmdIdLibShowUnLinkedImage         = "CmdIdLibShowUnLinkedImage"

struct ActionLibFilter {}
impl IActionUI for ActionLibFilter
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdLibShowUpToDateLinkedImage, CmdIdLibShowLinkedImage, CmdIdLibShowUnSavedLinkedImage, CmdIdLibShowUnLinkedImage:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdLibShowUpToDateLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Up To Date Linked Image"
            evt.checked = main.libWnd.showUpToDateLinkedImage
        case CmdIdLibShowUnSavedLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "UnSaved Linked Image"
            evt.checked = main.libWnd.showUnSavedLinkedImage

        case CmdIdLibShowLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Has Linked Image"
            evt.checked = main.libWnd.showLinkedImage
        case CmdIdLibShowUnLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Doesn't Have Linked Image"
            evt.checked = main.libWnd.showUnLinkedImage
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdLibShowUpToDateLinkedImage:
            main.libWnd.showUpToDateLinkedImage = !main.libWnd.showUpToDateLinkedImage
            main.libWnd.invalidate()
        case CmdIdLibShowUnSavedLinkedImage:
            main.libWnd.showUnSavedLinkedImage = !main.libWnd.showUnSavedLinkedImage
            main.libWnd.invalidate()

        case CmdIdLibShowLinkedImage:
            main.libWnd.showLinkedImage = !main.libWnd.showLinkedImage
            main.libWnd.invalidate()
        case CmdIdLibShowUnLinkedImage:
            main.libWnd.showUnLinkedImage = !main.libWnd.showUnLinkedImage
            main.libWnd.invalidate()
        }

        return true
    }
}
