using Gui

struct ActionCaptureDelay {}

impl ActionCaptureDelay
{
    newCmdId("Delay")
    newCmdId("Delay0")
    newCmdId("Delay1")
    newCmdId("Delay2")
    newCmdId("Delay5")
    newCmdId("Delay9")
}

impl IActionUI for ActionCaptureDelay
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case Delay, Delay0, Delay1, Delay2, Delay5, Delay9:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd
        switch cxt.id
        {
        case Delay:
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

        case Delay0:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "No Delay"
            evt.checked       = main.captureOptions.delay == 0
        case Delay1:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "1 second"
            evt.checked       = main.captureOptions.delay == 1
        case Delay2:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "2 seconds"
            evt.checked       = main.captureOptions.delay == 2
        case Delay5:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "5 seconds"
            evt.checked       = main.captureOptions.delay == 5
        case Delay9:
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
        case Delay0:
            main.captureOptions.delay = 0
        case Delay1:
            main.captureOptions.delay = 1
        case Delay2:
            main.captureOptions.delay = 2
        case Delay5:
            main.captureOptions.delay = 5
        case Delay9:
            main.captureOptions.delay = 9
        }

        return true
    }
}
