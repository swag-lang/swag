using Gui

const CmdIdSelectAll        = "CmdIdSelectAll"
const CmdIdDeSelectAll      = "CmdIdDeSelectAll"
const CmdIdInvertSelection  = "CmdIdInvertSelection"

struct ActionSelection {}
impl IActionUI for ActionSelection
{
    mtd accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdSelectAll,
             CmdIdDeSelectAll,
             CmdIdInvertSelection:
             return true
        }

        return false
    }

    mtd update(cxt: ActionContext, evt: *CommandUIEvent)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()
        hasSel  := capture and capture.selection.count

        switch cxt.id
        {
        case CmdIdSelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !capture.forms.count
            evt.name = "Select All Objects"

        case CmdIdDeSelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !hasSel
            evt.name = "Deselect All Objects"

        case CmdIdInvertSelection:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture
            evt.name = "Invert Selection"
        }

        return true
    }

    mtd execute(cxt: ActionContext)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()

        switch cxt.id
        {
        case CmdIdSelectAll:
            capture.selection.clear()
            loop capture.forms
                capture.selection.add(@index)
            capture.selectionHasChanged()

        case CmdIdDeSelectAll:
            capture.selection.clear()
            capture.selectionHasChanged()

        case CmdIdInvertSelection:
            var newSel: Array'u64
            loop i: capture.forms
            {
                if !capture.selection.contains(i)
                    newSel.add(i)
            }
            capture.selection = newSel
            capture.selectionHasChanged()
        }

        return true
    }
}