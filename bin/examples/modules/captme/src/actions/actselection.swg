using Gui

const CmdIdSelectAll       = "CmdIdSelectAll"
const CmdIdDeSelectAll     = "CmdIdDeSelectAll"
const CmdIdInvertSelection = "CmdIdInvertSelection"
const CmdIdDeleteSelection = "CmdIdDeleteSelection"
const CmdIdSnap            = "CmdIdSnap"

struct ActionSelection {}
impl IActionUI for ActionSelection
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdSelectAll, CmdIdDeSelectAll, CmdIdInvertSelection, CmdIdDeleteSelection, CmdIdSnap:
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
        case CmdIdSelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !capture.forms.count
            evt.name     = "Select All Objects"

        case CmdIdDeSelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !hasSel
            evt.name     = "Deselect All Objects"

        case CmdIdInvertSelection:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture
            evt.name     = "Invert Selection"

        case CmdIdDeleteSelection:
            evt.setFlags |= .Disabled | .Name | .Icon
            evt.disabled = !hasSel
            evt.name     = "Delete"
            evt.icon     = main.getTheme().getIcon24(.Remove)

        case CmdIdSnap:
            evt.setFlags |= .Name | .Checked
            evt.name    = "Snap To Forms"
            evt.checked = main.snap
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()

        switch cxt.id
        {
        case CmdIdSelectAll:
            capture.selection.clear()
            loop capture.forms do
                capture.selection.add(#index)
            capture.selectionHasChanged()

        case CmdIdDeSelectAll:
            capture.selection.clear()
            capture.selectionHasChanged()

        case CmdIdInvertSelection:
            var newSel: Array'u64
            loop i: capture.forms
            {
                if !capture.selection.contains(i) do
                    newSel.add(i)
            }
            capture.selection = newSel
            capture.selectionHasChanged()

        case CmdIdDeleteSelection:
            let undo = capture.newUndo(.DeleteSelection)
            capture.pushUndo(undo)

        case CmdIdSnap:
            main.snap = !main.snap
        }

        return true
    }
}
