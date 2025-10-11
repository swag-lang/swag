using Gui

struct ActionEdit {}

impl ActionEdit
{
    newCmdId("EditText")
    newCmdId("ClearImageInside")
}

impl IActionUI for ActionEdit
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case EditText, ClearImageInside:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()
        let hasSel  = capture and capture.selection.count

        var lastSelType: typeinfo
        if capture
        {
            if capture.selection.count
            {
                let f = capture.forms[capture.selection.back()]
                lastSelType = @kindof(f)
            }
        }

        switch cxt.id
        {
        case EditText:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !hasSel or !lastSelType or lastSelType != FormText
            evt.name     = "Edit"

        case ClearImageInside:
            evt.setFlags |= .Disabled | .Name | .Hidden
            evt.disabled, evt.hidden = true
            evt.name = "Clear Inside Image"

            if capture and hasSel
            {
                foreach it in capture.selection
                {
                    let itfForm = capture.forms[it]
                    if @kindof(itfForm) != FormImage do
                        continue
                    let b = cast(*FormImage) @dataof(itfForm)
                    if b.kind != .Image do
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

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = assume main.getCapture()

        switch cxt.id
        {
        case EditText:
            let lastSel = cast(*FormText) @dataof(capture.forms[capture.selection.back()])
            discard PropWnd.editText(main, lastSel.text)
            capture.selectionHasChanged()

        case ClearImageInside:
            capture.clearImageInside()
        }

        return true
    }
}
