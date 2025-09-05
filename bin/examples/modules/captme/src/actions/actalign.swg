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
        case AlignLeft, AlignCenter, AlignRight,
             AlignTop, AlignMiddle, AlignBottom,
             SendToFront, SendToBack, MoveForward, MoveBackward:
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
        case AlignLeft:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Left"
            evt.disabled = !hasSel or hasSingleSel

        case AlignCenter:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Center"
            evt.disabled = !hasSel or hasSingleSel

        case AlignRight:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Right"
            evt.disabled = !hasSel or hasSingleSel

        case AlignTop:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Top"
            evt.disabled = !hasSel or hasSingleSel

        case AlignMiddle:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Middle"
            evt.disabled = !hasSel or hasSingleSel

        case AlignBottom:
            evt.setFlags |= .Disabled | .Name
            evt.name     = "Bottom"
            evt.disabled = !hasSel or hasSingleSel

        case SendToFront:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Send To Front"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon     = main.getTheme().getIcon24(.BringToFront)

        case SendToBack:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Send To Back"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon     = main.getTheme().getIcon24(.SendToBack)

        case MoveForward:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name     = "Move Forward"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon     = main.getTheme().getIcon24(.BringForward)

        case MoveBackward:
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
        case AlignLeft:
            undo = capture.newUndo(.AlignLeft)
        case AlignCenter:
            undo = capture.newUndo(.AlignCenter)
        case AlignRight:
            undo = capture.newUndo(.AlignRight)
        case AlignTop:
            undo = capture.newUndo(.AlignTop)
        case AlignMiddle:
            undo = capture.newUndo(.AlignMiddle)
        case AlignBottom:
            undo = capture.newUndo(.AlignBottom)
        case SendToFront:
            undo = capture.newUndo(.MoveToFront)
        case SendToBack:
            undo = capture.newUndo(.MoveToBack)
        case MoveForward:
            undo = capture.newUndo(.MoveForward)
        case MoveBackward:
            undo = capture.newUndo(.MoveBackward)
        }

        Debug.assert(undo != null)
        capture.pushUndo(undo)
        main.invalidate()

        return true
    }
}
