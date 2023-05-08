using Gui

const CmdIdCapture              = "CmdIdCapture"
const CmdIdCaptureArea          = "CmdIdCaptureArea"
const CmdIdCaptureLastArea      = "CmdIdCaptureLastArea"
const CmdIdCaptureFullScreen    = "CmdIdCaptureFullScreen"
const CmdIdCaptureScreen0       = "CmdIdCaptureScreen0"
const CmdIdCaptureScreen1       = "CmdIdCaptureScreen1"
const CmdIdCaptureScreen2       = "CmdIdCaptureScreen2"
const CmdIdCaptureScreen3       = "CmdIdCaptureScreen3"
const CmdIdCaptureActiveWnd     = "CmdIdCaptureActiveWnd"

struct ActionCapture {}
impl IActionUI for ActionCapture
{
    mtd accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdCapture,
             CmdIdCaptureArea,
             CmdIdCaptureLastArea,
             CmdIdCaptureFullScreen,
             CmdIdCaptureScreen0,
             CmdIdCaptureScreen1,
             CmdIdCaptureScreen2,
             CmdIdCaptureScreen3,
             CmdIdCaptureActiveWnd:
             return true
        }

        return false
    }

    mtd update(cxt: ActionContext, evt: *CommandUIEvent)->bool
    {
        main := cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdCapture:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture"
            evt.icon = Icon.from(&main.icons48, 6)
        case CmdIdCaptureArea:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Area"
            evt.icon = Icon.from(&main.icons24, 7)
        case CmdIdCaptureLastArea:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Last Area"
            evt.icon = Icon.from(&main.icons24, 15)
        case CmdIdCaptureFullScreen:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Full Screen"
            evt.icon = Icon.from(&main.icons24, 6)
        case CmdIdCaptureActiveWnd:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Active Window"
            evt.icon = Icon.from(&main.icons24, 14)
        case CmdIdCaptureScreen0:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 0 (%x%)", main.monitors[0].rect.width, main.monitors[0].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case CmdIdCaptureScreen1:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 1 (%x%)", main.monitors[1].rect.width, main.monitors[1].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case CmdIdCaptureScreen2:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 2 (%x%)", main.monitors[2].rect.width, main.monitors[2].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case CmdIdCaptureScreen3:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 3 (%x%)", main.monitors[3].rect.width, main.monitors[3].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)

        }

        return true
    }

    mtd execute(cxt: ActionContext)->bool
    {
        main := cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdCapture:
            main.captureScreen(.Area)
        case CmdIdCaptureArea:
            main.captureScreen(.Area)
        case CmdIdCaptureLastArea:
            main.captureScreen(.LastArea)
        case CmdIdCaptureFullScreen:
            main.captureScreen(.FullScreen)
        case CmdIdCaptureActiveWnd:
            main.captureScreen(.ActiveWindow)
        case CmdIdCaptureScreen0:
            main.captureScreen(.Screen0)
        case CmdIdCaptureScreen1:
            main.captureScreen(.Screen1)
        case CmdIdCaptureScreen2:
            main.captureScreen(.Screen2)
        case CmdIdCaptureScreen3:
            main.captureScreen(.Screen3)
        }

        return true
    }
}