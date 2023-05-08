using Gui

const CmdEditText           = "CmdEditText"
const CmdIdClearImageInside = "CmdIdClearImageInside"

struct ActionEdit {}
impl IActionUI for ActionEdit
{
    mtd accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdEditText,
             CmdIdClearImageInside:
             return true
        }

        return false
    }

    mtd update(cxt: ActionContext, evt: *CommandUIEvent)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()
        hasSel  := capture and capture.selection.count

        var lastSelType: typeinfo
        if capture
        {
            if capture.selection.count
            {
                f := capture.forms[capture.selection.back()]
                lastSelType = @kindof(f)
            }
        }

        switch cxt.id
        {
        case CmdEditText:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !hasSel or !lastSelType or lastSelType != FormText
            evt.name = "Edit"

        case CmdIdClearImageInside:
            evt.setFlags |= .Disabled | .Name | .Hidden
            evt.disabled, evt.hidden = true
            evt.name = "Clear Inside Image"

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
                    if b.image.isValid()
                    {
                        evt.disabled = false
                        break
                    }
                }
            }
        }

        return true
    }

    mtd execute(cxt: ActionContext)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()

        switch cxt.id
        {
        case CmdEditText:
            lastSel := cast(*FormText) @dataof(capture.forms[capture.selection.back()])
            discard main.propWnd.editText(lastSel.text)
            capture.selectionHasChanged()

        case CmdIdClearImageInside:
            capture.clearImageInside()
        }

        return true
    }
}