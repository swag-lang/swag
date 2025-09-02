using Gui

newCmdId("Copy")
newCmdId("Paste")
newCmdId("Cut")
newCmdId("PasteImageInside")
newCmdId("CopyAsImage")

struct ActionCopyPaste {}
impl IActionUI for ActionCopyPaste
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.Copy, CmdId.Cut, CmdId.Paste, CmdId.PasteImageInside, CmdId.CopyAsImage:
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
        case CmdId.Copy:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.disabled = !hasSel
            evt.name     = "Copy"
            evt.icon     = main.getTheme().getIcon24(.Copy)

        case CmdId.Cut:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.disabled = !hasSel
            evt.name     = "Cut"
            evt.icon     = main.getTheme().getIcon24(.Cut)

        case CmdId.Paste:
            evt.setFlags |= .Disabled | .Name | .Icon
            if !capture:
                evt.disabled = true
            elif !Clipboard.hasTypedValue(#typeof(Capture.forms)) and !Clipboard.hasImage():
                evt.disabled = true
            evt.name = "Paste"
            evt.icon = main.getTheme().getIcon24(.Paste)

        case CmdId.PasteImageInside:
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

        case CmdId.CopyAsImage:
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
        case CmdId.Copy:
            capture.copy()
        case CmdId.Cut:
            capture.cut()
        case CmdId.Paste:
            capture.paste()
        case CmdId.PasteImageInside:
            capture.pasteImageInside()
        case CmdId.CopyAsImage:
            capture.copyAll()
        }

        return true
    }
}
