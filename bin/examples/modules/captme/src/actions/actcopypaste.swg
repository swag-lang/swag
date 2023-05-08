using Gui

const CmdIdCopy                 = "CmdIdCopy"
const CmdIdPaste                = "CmdIdPaste"
const CmdIdPasteImageInside     = "CmdIdPasteImageInside"
const CmdIdCopyAsImage          = "CmdIdCopyAsImage"

struct ActionCopyPaste {}
impl IActionUI for ActionCopyPaste
{
    mtd accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdCopy,
             CmdIdPaste,
             CmdIdPasteImageInside,
             CmdIdCopyAsImage:
             return true
        }

        return false
    }

    mtd update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()
        hasSel  := capture and capture.selection.count

        switch cxt.id
        {
        case CmdIdCopy:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.disabled = !hasSel
            evt.name = "Copy"
            evt.icon = main.getTheme().getIcon24(.Copy)

        case CmdIdPaste:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.disabled = !capture or (!Clipboard.hasFormat(main.clipFmt) and !Clipboard.hasImage())
            evt.name = "Paste"
            evt.icon = main.getTheme().getIcon24(.Paste)

        case CmdIdPasteImageInside:
            evt.setFlags |= .Disabled | .Name | .Hidden
            evt.disabled, evt.hidden = true
            evt.name = "Paste Inside Image"

            hasImg := Clipboard.hasImage()
            if capture and hasSel
            {
                visit it: capture.selection
                {
                    itfForm := capture.forms[it]
                    if @kindof(itfForm) != FormImage
                        continue
                    b := cast(*FormImage) @dataof(itfForm)
                    if b.kind != .Image
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
            evt.name = "Copy As Image"
        }

        return true
    }

    mtd execute(cxt: ActionContext)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()

        switch cxt.id
        {
        case CmdIdCopy:
            capture.copy()
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
