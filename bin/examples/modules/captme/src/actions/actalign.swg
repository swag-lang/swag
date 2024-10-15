using Gui

const CmdIdAlignLeft    = "CmdIdAlignLeft"
const CmdIdAlignCenter  = "CmdIdAlignCenter"
const CmdIdAlignRight   = "CmdIdAlignRight"
const CmdIdAlignTop     = "CmdIdAlignTop"
const CmdIdAlignMiddle  = "CmdIdAlignMiddle"
const CmdIdAlignBottom  = "CmdIdAlignBottom"
const CmdIdBringToFront = "CmdIdBringToFront"
const CmdIdSendToBack   = "CmdIdSendToBack"
const CmdIdBringForward = "CmdIdBringForward"
const CmdIdSendBackward = "CmdIdSendBackward"

struct ActionMoveSel {}
impl IActionUI for ActionMoveSel
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdAlignLeft, CmdIdAlignCenter, CmdIdAlignRight,
             CmdIdAlignTop, CmdIdAlignMiddle, CmdIdAlignBottom,
             CmdIdBringToFront, CmdIdSendToBack, CmdIdBringForward, CmdIdSendBackward:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        let capture      = main.getCapture()
        let hasSel       = capture and capture.selection.count
        let hasSingleSel = capture and capture.selection.count == 1

        switch cxt.id
        {
        case CmdIdAlignLeft:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Left"
            evt.disabled = !hasSel or hasSingleSel

        case CmdIdAlignCenter:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Center"
            evt.disabled = !hasSel or hasSingleSel

        case CmdIdAlignRight:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Right"
            evt.disabled = !hasSel or hasSingleSel

        case CmdIdAlignTop:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Top"
            evt.disabled = !hasSel or hasSingleSel

        case CmdIdAlignMiddle:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Middle"
            evt.disabled = !hasSel or hasSingleSel

        case CmdIdAlignBottom:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Bottom"
            evt.disabled = !hasSel or hasSingleSel

        case CmdIdBringToFront:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Bring To Front"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon     = main.getTheme().getIcon24(.BringToFront)

        case CmdIdSendToBack:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Send To Back"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon     = main.getTheme().getIcon24(.SendToBack)

        case CmdIdBringForward:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Bring Forward"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon     = main.getTheme().getIcon24(.BringForward)

        case CmdIdSendBackward:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Send Backward"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon     = main.getTheme().getIcon24(.SendBackward)
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()

        var undo: *Undo
        switch cxt.id
        {
        case CmdIdAlignLeft:
            undo = capture.newUndo(.AlignLeft)
        case CmdIdAlignCenter:
            undo = capture.newUndo(.AlignCenter)
        case CmdIdAlignRight:
            undo = capture.newUndo(.AlignRight)
        case CmdIdAlignTop:
            undo = capture.newUndo(.AlignTop)
        case CmdIdAlignMiddle:
            undo = capture.newUndo(.AlignMiddle)
        case CmdIdAlignBottom:
            undo = capture.newUndo(.AlignBottom)
        case CmdIdBringToFront:
            undo = capture.newUndo(.MoveToFront)
        case CmdIdSendToBack:
            undo = capture.newUndo(.MoveToBack)
        case CmdIdBringForward:
            undo = capture.newUndo(.MoveForward)
        case CmdIdSendBackward:
            undo = capture.newUndo(.MoveBackward)
        }

        Debug.assert(undo != null)
        capture.pushUndo(undo)
        main.invalidate()

        return true
    }
}
