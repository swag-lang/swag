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
        case ActionLibFilter.UpToDateLinkedImage, ActionLibFilter.LinkedImage, ActionLibFilter.UnSavedLinkedImage, ActionLibFilter.UnLinkedImage:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let libWnd = cast(*LibraryWnd) cxt.wnd

        switch cxt.id
        {
        case ActionLibFilter.UpToDateLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Up To Date Linked Image"
            evt.checked = libWnd.showUpToDateLinkedImage
        case ActionLibFilter.UnSavedLinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "UnSaved Linked Image"
            evt.checked = libWnd.showUnSavedLinkedImage

        case ActionLibFilter.LinkedImage:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Has Linked Image"
            evt.checked = libWnd.showLinkedImage
        case ActionLibFilter.UnLinkedImage:
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
        case ActionLibFilter.UpToDateLinkedImage:
            libWnd.showUpToDateLinkedImage = !libWnd.showUpToDateLinkedImage
            libWnd.view.filter()
        case ActionLibFilter.UnSavedLinkedImage:
            libWnd.showUnSavedLinkedImage = !libWnd.showUnSavedLinkedImage
            libWnd.view.filter()
        case ActionLibFilter.LinkedImage:
            libWnd.showLinkedImage = !libWnd.showLinkedImage
            libWnd.view.filter()
        case ActionLibFilter.UnLinkedImage:
            libWnd.showUnLinkedImage = !libWnd.showUnLinkedImage
            libWnd.view.filter()
        }

        return true
    }
}
