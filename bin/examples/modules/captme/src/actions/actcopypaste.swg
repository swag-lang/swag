using Gui

struct ActionCopyPaste {}

impl ActionCopyPaste
{
    newCmdId("Copy")
    newCmdId("Paste")
    newCmdId("Cut")
    newCmdId("PasteImageInside")
    newCmdId("CopyAsImage")
}

impl IActionUI for ActionCopyPaste
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case ActionCopyPaste.Copy, ActionCopyPaste.Cut, ActionCopyPaste.Paste, ActionCopyPaste.PasteImageInside,
             ActionCopyPaste.CopyAsImage:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()
        let hasSel  = capture and capture.selection.count

        switch cxt.id
        {
        case ActionCopyPaste.Copy:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.disabled = !hasSel
            evt.name     = "Copy"
            evt.icon     = main.getTheme().getIcon24(.Copy)

        case ActionCopyPaste.Cut:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.disabled = !hasSel
            evt.name     = "Cut"
            evt.icon     = main.getTheme().getIcon24(.Cut)

        case ActionCopyPaste.Paste:
            evt.setFlags |= .Disabled | .Name | .Icon
            if !capture:
                evt.disabled = true
            elif !Clipboard.hasTypedValue(#typeof(Capture.forms)) and !Clipboard.hasImage():
                evt.disabled = true
            evt.name = "Paste"
            evt.icon = main.getTheme().getIcon24(.Paste)

        case ActionCopyPaste.PasteImageInside:
            evt.setFlags |= .Disabled | .Name | .Hidden
            evt.disabled, evt.hidden = true
            evt.name = "Paste Inside Image"

            let hasImg = Clipboard.hasImage()
            if capture and hasSel
            {
                foreach it in capture.selection
                {
                    let itfForm = capture.forms[it]
                    if @kindof(itfForm) != FormImage:
                        continue
                    let b = cast(*FormImage) @dataof(itfForm)
                    if b.kind != .Image:
                        continue
                    evt.hidden = false
                    if hasImg
                    {
                        evt.disabled = false
                        break
                    }
                }
            }

        case ActionCopyPaste.CopyAsImage:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = capture == null
            evt.name     = "Copy Image to Clipboard"
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = assume main.getCapture()

        switch cxt.id
        {
        case ActionCopyPaste.Copy:
            capture.copy()
        case ActionCopyPaste.Cut:
            capture.cut()
        case ActionCopyPaste.Paste:
            capture.paste()
        case ActionCopyPaste.PasteImageInside:
            capture.pasteImageInside()
        case ActionCopyPaste.CopyAsImage:
            capture.copyAll()
        }

        return true
    }
}
