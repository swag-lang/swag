using Gui

struct ActionMoveSel {}

impl ActionMoveSel
{
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
}

impl IActionUI for ActionMoveSel
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case ActionMoveSel.AlignLeft, ActionMoveSel.AlignCenter, ActionMoveSel.AlignRight,
             ActionMoveSel.AlignTop, ActionMoveSel.AlignMiddle, ActionMoveSel.AlignBottom,
             ActionMoveSel.SendToFront, ActionMoveSel.SendToBack, ActionMoveSel.MoveForward, ActionMoveSel.MoveBackward:
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
        case ActionMoveSel.AlignLeft:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Left"
            evt.disabled = !hasSel or hasSingleSel

        case ActionMoveSel.AlignCenter:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Center"
            evt.disabled = !hasSel or hasSingleSel

        case ActionMoveSel.AlignRight:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Right"
            evt.disabled = !hasSel or hasSingleSel

        case ActionMoveSel.AlignTop:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Top"
            evt.disabled = !hasSel or hasSingleSel

        case ActionMoveSel.AlignMiddle:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Middle"
            evt.disabled = !hasSel or hasSingleSel

        case ActionMoveSel.AlignBottom:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Bottom"
            evt.disabled = !hasSel or hasSingleSel

        case ActionMoveSel.SendToFront:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Send To Front"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon     = main.getTheme().getIcon24(.BringToFront)

        case ActionMoveSel.SendToBack:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Send To Back"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon     = main.getTheme().getIcon24(.SendToBack)

        case ActionMoveSel.MoveForward:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Move Forward"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon     = main.getTheme().getIcon24(.BringForward)

        case ActionMoveSel.MoveBackward:
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
        case ActionMoveSel.AlignLeft:
            undo = capture.newUndo(.AlignLeft)
        case ActionMoveSel.AlignCenter:
            undo = capture.newUndo(.AlignCenter)
        case ActionMoveSel.AlignRight:
            undo = capture.newUndo(.AlignRight)
        case ActionMoveSel.AlignTop:
            undo = capture.newUndo(.AlignTop)
        case ActionMoveSel.AlignMiddle:
            undo = capture.newUndo(.AlignMiddle)
        case ActionMoveSel.AlignBottom:
            undo = capture.newUndo(.AlignBottom)
        case ActionMoveSel.SendToFront:
            undo = capture.newUndo(.MoveToFront)
        case ActionMoveSel.SendToBack:
            undo = capture.newUndo(.MoveToBack)
        case ActionMoveSel.MoveForward:
            undo = capture.newUndo(.MoveForward)
        case ActionMoveSel.MoveBackward:
            undo = capture.newUndo(.MoveBackward)
        }

        Debug.assert(undo != null)
        capture.pushUndo(undo)
        main.invalidate()

        return true
    }
}
