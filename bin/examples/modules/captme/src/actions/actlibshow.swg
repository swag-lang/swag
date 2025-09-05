using Gui

struct ActionLibFilter {}

impl ActionLibFilter
{
    newCmdId("UpToDateLinkedImage")
    newCmdId("LinkedImage")
    newCmdId("UnSavedLinkedImage")
    newCmdId("UnLinkedImage")
}

impl IActionUI for ActionLibFilter
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case UpToDateLinkedImage, LinkedImage, UnSavedLinkedImage, UnLinkedImage:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let libWnd = cast(*LibraryWnd) cxt.wnd

        switch cxt.id
        {
        case UpToDateLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Up To Date Linked Image"
            evt.checked = libWnd.showUpToDateLinkedImage
        case UnSavedLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "UnSaved Linked Image"
            evt.checked = libWnd.showUnSavedLinkedImage

        case LinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Has Linked Image"
            evt.checked = libWnd.showLinkedImage
        case UnLinkedImage:
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
        case UpToDateLinkedImage:
            libWnd.showUpToDateLinkedImage = !libWnd.showUpToDateLinkedImage
            libWnd.view.filter()
        case UnSavedLinkedImage:
            libWnd.showUnSavedLinkedImage = !libWnd.showUnSavedLinkedImage
            libWnd.view.filter()
        case LinkedImage:
            libWnd.showLinkedImage = !libWnd.showLinkedImage
            libWnd.view.filter()
        case UnLinkedImage:
            libWnd.showUnLinkedImage = !libWnd.showUnLinkedImage
            libWnd.view.filter()
        }

        return true
    }
}
