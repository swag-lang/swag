using Gui

struct ActionSelection {}

impl ActionSelection
{
    newCmdId("SelectAll")
    newCmdId("SelectSameAll")
    newCmdId("DeSelectAll")
    newCmdId("InvertSelection")
    newCmdId("DeleteSelection")
    newCmdId("Snap")
    newCmdId("Freeze")
}

impl IActionUI for ActionSelection
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case SelectAll, SelectSameAll, DeSelectAll, InvertSelection, DeleteSelection, Snap, Freeze:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()
        let hasSel  = capture and capture.selection.count
        let isLib   = main.libraryMode

        switch cxt.id
        {
        case SelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !capture.forms.count or isLib
            evt.name     = "Select All Objects"

        case SelectSameAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !capture.forms.count or !hasSel or isLib
            evt.name     = "Select Similar Objects"

        case DeSelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !hasSel or isLib
            evt.name     = "Deselect All Objects"

        case InvertSelection:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or isLib
            evt.name     = "Invert Selection"

        case DeleteSelection:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.disabled = !hasSel
            evt.name     = "Delete"
            evt.icon     = main.getTheme().getIcon24(.Remove)

        case Snap:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Snap To Forms"
            evt.checked = main.snap

        case Freeze:
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
        case SelectAll:
            capture.selection.clear()
            for i in capture.forms do
                capture.selection.add(i)
            capture.selectionHasChanged()

        case SelectSameAll:
            let firstType = capture.getSelectedFormType()
            capture.selection.clear()
            foreach form, i in capture.forms
            {
                if @kindof(form) == firstType do
                    capture.selection.add(i)
            }

            capture.selectionHasChanged()

        case DeSelectAll:
            capture.selection.clear()
            capture.selectionHasChanged()

        case InvertSelection:
            var newSel: Array'u64
            for i in capture.forms where !capture.selection.contains(i) do
                newSel.add(i)
            capture.selection = newSel
            capture.selectionHasChanged()

        case DeleteSelection:
            capture.pushUndo(capture.newUndo(.DeleteSelection))

        case Snap:
            main.snap = !main.snap

        case Freeze:
            main.freezeSelection(!main.freezeSel)
        }

        return true
    }
}
