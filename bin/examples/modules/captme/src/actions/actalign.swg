using Gui

newCmdId("AlignLeft")
newCmdId("AlignCenter")
newCmdId("AlignRight")
newCmdId("AlignTop")
newCmdId("AlignMiddle")
newCmdId("AlignBottom")
newCmdId("SendToFront")
newCmdId("SendToBack")
newCmdId("MoveForward")
newCmdId("MoveBackward")

struct ActionMoveSel {}
impl IActionUI for ActionMoveSel
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.AlignLeft, CmdId.AlignCenter, CmdId.AlignRight,
             CmdId.AlignTop, CmdId.AlignMiddle, CmdId.AlignBottom,
             CmdId.SendToFront, CmdId.SendToBack, CmdId.MoveForward, CmdId.MoveBackward:
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
        case CmdId.AlignLeft:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Left"
            evt.disabled = !hasSel or hasSingleSel

        case CmdId.AlignCenter:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Center"
            evt.disabled = !hasSel or hasSingleSel

        case CmdId.AlignRight:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Right"
            evt.disabled = !hasSel or hasSingleSel

        case CmdId.AlignTop:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Top"
            evt.disabled = !hasSel or hasSingleSel

        case CmdId.AlignMiddle:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Middle"
            evt.disabled = !hasSel or hasSingleSel

        case CmdId.AlignBottom:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Bottom"
            evt.disabled = !hasSel or hasSingleSel

        case CmdId.SendToFront:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Send To Front"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon     = main.getTheme().getIcon24(.BringToFront)

        case CmdId.SendToBack:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Send To Back"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon     = main.getTheme().getIcon24(.SendToBack)

        case CmdId.MoveForward:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Move Forward"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon     = main.getTheme().getIcon24(.BringForward)

        case CmdId.MoveBackward:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Move Backward"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon     = main.getTheme().getIcon24(.SendBackward)
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = assume main.getCapture()

        var undo: *Undo
        switch cxt.id
        {
        case CmdId.AlignLeft:
            undo = capture.newUndo(.AlignLeft)
        case CmdId.AlignCenter:
            undo = capture.newUndo(.AlignCenter)
        case CmdId.AlignRight:
            undo = capture.newUndo(.AlignRight)
        case CmdId.AlignTop:
            undo = capture.newUndo(.AlignTop)
        case CmdId.AlignMiddle:
            undo = capture.newUndo(.AlignMiddle)
        case CmdId.AlignBottom:
            undo = capture.newUndo(.AlignBottom)
        case CmdId.SendToFront:
            undo = capture.newUndo(.MoveToFront)
        case CmdId.SendToBack:
            undo = capture.newUndo(.MoveToBack)
        case CmdId.MoveForward:
            undo = capture.newUndo(.MoveForward)
        case CmdId.MoveBackward:
            undo = capture.newUndo(.MoveBackward)
        }

        Debug.assert(undo != null)
        capture.pushUndo(undo)
        main.invalidate()

        return true
    }
}
