using Gui

newCmdId("SelectAll")
newCmdId("SelectSameAll")
newCmdId("DeSelectAll")
newCmdId("InvertSelection")
newCmdId("DeleteSelection")
newCmdId("Snap")
newCmdId("Freeze")

struct ActionSelection {}
impl IActionUI for ActionSelection
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.SelectAll, CmdId.SelectSameAll, CmdId.DeSelectAll, CmdId.InvertSelection, CmdId.DeleteSelection, CmdId.Snap, CmdId.Freeze:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        var capture = main.getCapture()
        let hasSel  = capture and capture.selection.count
        if main.libraryMode:
            capture = null

        switch cxt.id
        {
        case CmdId.SelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !capture.forms.count
            evt.name     = "Select All Objects"

        case CmdId.SelectSameAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !capture.forms.count or !hasSel
            evt.name     = "Select Similar Objects"

        case CmdId.DeSelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !hasSel
            evt.name     = "Deselect All Objects"

        case CmdId.InvertSelection:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture
            evt.name     = "Invert Selection"

        case CmdId.DeleteSelection:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.disabled = !hasSel
            evt.name     = "Delete"
            evt.icon     = main.getTheme().getIcon24(.Remove)

        case CmdId.Snap:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Snap To Forms"
            evt.checked = main.snap

        case CmdId.Freeze:
            evt.setFlags |= .Name | .Checked | .Name
            evt.name    = "Freeze Selection"
            evt.checked = main.freezeSel
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = assume main.getCapture()

        switch cxt.id
        {
        case CmdId.SelectAll:
            capture.selection.clear()
            for capture.forms:
                capture.selection.add(@index)
            capture.selectionHasChanged()

        case CmdId.SelectSameAll:
            let firstType = capture.getSelectedFormType()
            capture.selection.clear()
            foreach form in capture.forms
            {
                if @kindof(form) == firstType:
                    capture.selection.add(@index)
            }

            capture.selectionHasChanged()

        case CmdId.DeSelectAll:
            capture.selection.clear()
            capture.selectionHasChanged()

        case CmdId.InvertSelection:
            var newSel: Array'u64
            for i in capture.forms where !capture.selection.contains(i):
                newSel.add(i)
            capture.selection = newSel
            capture.selectionHasChanged()

        case CmdId.DeleteSelection:
            capture.pushUndo(capture.newUndo(.DeleteSelection))

        case CmdId.Snap:
            main.snap = !main.snap

        case CmdId.Freeze:
            if main.freezeSel:
                main.freezeSelection(false)
            else:
                main.freezeSelection(true)
        }

        return true
    }
}
