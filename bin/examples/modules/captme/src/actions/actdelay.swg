using Gui

const CmdIdCaptureDelay  = "CmdIdCaptureDelay"
const CmdIdCaptureDelay0 = "CmdIdCaptureDelay0"
const CmdIdCaptureDelay1 = "CmdIdCaptureDelay1"
const CmdIdCaptureDelay2 = "CmdIdCaptureDelay2"
const CmdIdCaptureDelay5 = "CmdIdCaptureDelay5"
const CmdIdCaptureDelay9 = "CmdIdCaptureDelay9"

struct ActionDelay {}
impl IActionUI for ActionDelay
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdCaptureDelay, CmdIdCaptureDelay0, CmdIdCaptureDelay1, CmdIdCaptureDelay2, CmdIdCaptureDelay5, CmdIdCaptureDelay9:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: nn *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdCaptureDelay:
            evt.setFlags |= .Name
            switch main.captureDelay
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

        case CmdIdCaptureDelay0:
            evt.setFlags |= .Name | .Checked
            evt.name    = "No Delay"
            evt.checked = main.captureDelay == 0
        case CmdIdCaptureDelay1:
            evt.setFlags |= .Name | .Checked
            evt.name    = "1 second"
            evt.checked = main.captureDelay == 1
        case CmdIdCaptureDelay2:
            evt.setFlags |= .Name | .Checked
            evt.name    = "2 seconds"
            evt.checked = main.captureDelay == 2
        case CmdIdCaptureDelay5:
            evt.setFlags |= .Name | .Checked
            evt.name    = "5 seconds"
            evt.checked = main.captureDelay == 5
        case CmdIdCaptureDelay9:
            evt.setFlags |= .Name | .Checked
            evt.name    = "9 seconds"
            evt.checked = main.captureDelay == 9
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdCaptureDelay0:
            main.captureDelay = 0
        case CmdIdCaptureDelay1:
            main.captureDelay = 1
        case CmdIdCaptureDelay2:
            main.captureDelay = 2
        case CmdIdCaptureDelay5:
            main.captureDelay = 5
        case CmdIdCaptureDelay9:
            main.captureDelay = 9
        }

        return true
    }
}
