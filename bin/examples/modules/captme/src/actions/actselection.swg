using Gui

const CmdIdSelectAll       = "CmdIdSelectAll"
const CmdIdDeSelectAll     = "CmdIdDeSelectAll"
const CmdIdInvertSelection = "CmdIdInvertSelection"
const CmdIdDeleteSelection = "CmdIdDeleteSelection"
const CmdIdSnap            = "CmdIdSnap"
const CmdIdFreeze          = "CmdIdFreeze"

struct ActionSelection {}
impl IActionUI for ActionSelection
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdSelectAll, CmdIdDeSelectAll, CmdIdInvertSelection, CmdIdDeleteSelection, CmdIdSnap, CmdIdFreeze:
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

        case CmdIdFreeze:
            evt.setFlags |= .Name | .Checked | .Name
            evt.name    = "Freeze Selection"
            evt.checked = main.freezeSelection            
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
            for capture.forms:
                capture.selection.add(#index)
            capture.selectionHasChanged()

        case CmdIdDeSelectAll:
            capture.selection.clear()
            capture.selectionHasChanged()

        case CmdIdInvertSelection:
            var newSel: Array'u64
            for i in capture.forms where !capture.selection.contains(i):
                newSel.add(i)
            capture.selection = newSel
            capture.selectionHasChanged()

        case CmdIdDeleteSelection:
            capture.pushUndo(capture.newUndo(.DeleteSelection))

        case CmdIdSnap:
            main.snap = !main.snap

        case CmdIdFreeze:
            if main.freezeSelection
            {
                main.embInfoFreeze.destroy()
                main.embInfoFreeze = null
                main.freezeSelection = false
            }
            else
            {  
                main.embInfoFreeze = EmbInfoCtrl.create(main.editWnd.view, "Selection is freezed", .Message, flags: .Bottom)
                main.embInfoFreeze.sigOnClose += func|main|(ctrl)
                { 
                    main.freezeSelection = false                   
                }

                capture.selection.clear()
                capture.selectionHasChanged()
                main.freezeSelection = true
                foreach it in capture.forms
                {
                    let p = cast(*BaseForm) @dataof(it)
                    p.freeze = true
                }                              
            }
        }

        return true
    }
}
