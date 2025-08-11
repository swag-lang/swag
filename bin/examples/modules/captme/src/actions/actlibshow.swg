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
        let libWnd = cast(*LibraryWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdLibShowUpToDateLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Up To Date Linked Image"
            evt.checked = libWnd.showUpToDateLinkedImage
        case CmdIdLibShowUnSavedLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "UnSaved Linked Image"
            evt.checked = libWnd.showUnSavedLinkedImage

        case CmdIdLibShowLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Has Linked Image"
            evt.checked = libWnd.showLinkedImage
        case CmdIdLibShowUnLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Doesn't Have Linked Image"
            evt.checked = libWnd.showUnLinkedImage
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let libWnd = cast(*LibraryWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdLibShowUpToDateLinkedImage:
            libWnd.showUpToDateLinkedImage = !libWnd.showUpToDateLinkedImage
            libWnd.view.filter()
        case CmdIdLibShowUnSavedLinkedImage:
            libWnd.showUnSavedLinkedImage = !libWnd.showUnSavedLinkedImage
            libWnd.view.filter()
        case CmdIdLibShowLinkedImage:
            libWnd.showLinkedImage = !libWnd.showLinkedImage
            libWnd.view.filter()
        case CmdIdLibShowUnLinkedImage:
            libWnd.showUnLinkedImage = !libWnd.showUnLinkedImage
            libWnd.view.filter()
        }

        return true
    }
}
