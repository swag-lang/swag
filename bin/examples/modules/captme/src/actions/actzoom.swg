using Gui

const CmdIdZoom1    = "CmdIdZoom1"
const CmdIdZoomFit  = "CmdIdZoomFit"
const CmdIdZoomIn   = "CmdIdZoomIn"
const CmdIdZoomOut  = "CmdIdZoomOut"

struct ActionZoom {}
impl IActionUI for ActionZoom
{
    mtd accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdZoom1,
             CmdIdZoomFit,
             CmdIdZoomIn,
             CmdIdZoomOut:
             return true
        }

        return false
    }

    mtd update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()

        switch cxt.id
        {
        case CmdIdZoom1:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Actual Size"
            evt.disabled = !capture

        case CmdIdZoomFit:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Fit To View"
            evt.disabled = !capture

        case CmdIdZoomIn:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Zoom In"
            evt.disabled = !capture or !main.editWnd.editView.canZoomIn()

        case CmdIdZoomOut:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Zoom Out"
            evt.disabled = !capture or !main.editWnd.editView.canZoomOut()
        }

        return true
    }

    mtd execute(cxt: ActionContext)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        view := main.editWnd.editView

        switch cxt.id
        {
        case CmdIdZoom1:
            view.setZoom(1)

        case CmdIdZoomFit:
            view.zoomToFit()

        case CmdIdZoomIn:
            view.zoomIn()

        case CmdIdZoomOut:
            view.zoomOut()
        }

        return true
    }
}