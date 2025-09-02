using Gui

newCmdId("CaptureDelay")
newCmdId("CaptureDelay0")
newCmdId("CaptureDelay1")
newCmdId("CaptureDelay2")
newCmdId("CaptureDelay5")
newCmdId("CaptureDelay9")

struct ActionDelay {}
impl IActionUI for ActionDelay
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.CaptureDelay, CmdId.CaptureDelay0, CmdId.CaptureDelay1, CmdId.CaptureDelay2, CmdId.CaptureDelay5, CmdId.CaptureDelay9:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd
        switch cxt.id
        {
        case CmdId.CaptureDelay:
            evt.setFlags |= .Name
            switch main.captureOptions.delay
            {
            case 0:
                evt.name = "Delay Before Capture"
            case 1:
                evt.name = "Delay Before Capture (1 second)"
            case 2:
                evt.name = "Delay Before Capture (2 seconds)"
            case 5:
                evt.name = "Delay Before Capture (5 seconds)"
            case 9:
                evt.name = "Delay Before Capture (9 seconds)"
            }

        case CmdId.CaptureDelay0:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "No Delay"
            evt.checked       = main.captureOptions.delay == 0
        case CmdId.CaptureDelay1:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "1 second"
            evt.checked       = main.captureOptions.delay == 1
        case CmdId.CaptureDelay2:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "2 seconds"
            evt.checked       = main.captureOptions.delay == 2
        case CmdId.CaptureDelay5:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "5 seconds"
            evt.checked       = main.captureOptions.delay == 5
        case CmdId.CaptureDelay9:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "9 seconds"
            evt.checked       = main.captureOptions.delay == 9
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd
        switch cxt.id
        {
        case CmdId.CaptureDelay0:
            main.captureOptions.delay = 0
        case CmdId.CaptureDelay1:
            main.captureOptions.delay = 1
        case CmdId.CaptureDelay2:
            main.captureOptions.delay = 2
        case CmdId.CaptureDelay5:
            main.captureOptions.delay = 5
        case CmdId.CaptureDelay9:
            main.captureOptions.delay = 9
        }

        return true
    }
}
