using Gui

const CmdIdCopy             = "CmdIdCopy"
const CmdIdPaste            = "CmdIdPaste"
const CmdIdCut              = "CmdIdCut"
const CmdIdPasteImageInside = "CmdIdPasteImageInside"
const CmdIdCopyAsImage      = "CmdIdCopyAsImage"

struct ActionCopyPaste {}
impl IActionUI for ActionCopyPaste
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdCopy, CmdIdCut, CmdIdPaste, CmdIdPasteImageInside, CmdIdCopyAsImage:
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
        case CmdIdCopy:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.disabled = !hasSel
            evt.name     = "Copy"
            evt.icon     = main.getTheme().getIcon24(.Copy)

        case CmdIdCut:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.disabled = !hasSel
            evt.name     = "Cut"
            evt.icon     = main.getTheme().getIcon24(.Cut)            

        case CmdIdPaste:
            evt.setFlags |= .Disabled | .Name | .Icon
            if !capture:
                evt.disabled = true
            elif !Clipboard.hasTypedValue(#typeof(Capture.forms)) and !Clipboard.hasImage():
                evt.disabled = true
            evt.name = "Paste"
            evt.icon = main.getTheme().getIcon24(.Paste)

        case CmdIdPasteImageInside:
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

        case CmdIdCopyAsImage:
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
        case CmdIdCopy:
            capture.copy()
        case CmdIdCut:
            capture.cut()
        case CmdIdPaste:
            capture.paste()
        case CmdIdPasteImageInside:
            capture.pasteImageInside()
        case CmdIdCopyAsImage:
            capture.copyAll()
        }

        return true
    }
}
