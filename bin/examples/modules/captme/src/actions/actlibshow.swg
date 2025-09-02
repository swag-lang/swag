using Gui

newCmdId("LibShowUpToDateLinkedImage")
newCmdId("LibShowLinkedImage")
newCmdId("LibShowUnSavedLinkedImage")
newCmdId("LibShowUnLinkedImage")

struct ActionLibFilter {}
impl IActionUI for ActionLibFilter
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.LibShowUpToDateLinkedImage, CmdId.LibShowLinkedImage, CmdId.LibShowUnSavedLinkedImage, CmdId.LibShowUnLinkedImage:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let libWnd = cast(*LibraryWnd) cxt.wnd

        switch cxt.id
        {
        case CmdId.LibShowUpToDateLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Up To Date Linked Image"
            evt.checked = libWnd.showUpToDateLinkedImage
        case CmdId.LibShowUnSavedLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "UnSaved Linked Image"
            evt.checked = libWnd.showUnSavedLinkedImage

        case CmdId.LibShowLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Has Linked Image"
            evt.checked = libWnd.showLinkedImage
        case CmdId.LibShowUnLinkedImage:
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
        case CmdId.LibShowUpToDateLinkedImage:
            libWnd.showUpToDateLinkedImage = !libWnd.showUpToDateLinkedImage
            libWnd.view.filter()
        case CmdId.LibShowUnSavedLinkedImage:
            libWnd.showUnSavedLinkedImage = !libWnd.showUnSavedLinkedImage
            libWnd.view.filter()
        case CmdId.LibShowLinkedImage:
            libWnd.showLinkedImage = !libWnd.showLinkedImage
            libWnd.view.filter()
        case CmdId.LibShowUnLinkedImage:
            libWnd.showUnLinkedImage = !libWnd.showUnLinkedImage
            libWnd.view.filter()
        }

        return true
    }
}
