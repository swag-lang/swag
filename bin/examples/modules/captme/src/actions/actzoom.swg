using Gui

newCmdId("Zoom1")
newCmdId("ZoomFit")
newCmdId("ZoomIn")
newCmdId("ZoomOut")

struct ActionZoom {}
impl IActionUI for ActionZoom
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.Zoom1, CmdId.ZoomFit, CmdId.ZoomIn, CmdId.ZoomOut:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()

        switch cxt.id
        {
        case CmdId.Zoom1:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Actual Size"
            evt.disabled = !capture

        case CmdId.ZoomFit:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Fit To View"
            evt.disabled = !capture

        case CmdId.ZoomIn:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Zoom In"
            evt.disabled = !capture or !main.editWnd.editView.canZoomIn()
            evt.icon     = main.getTheme().getIcon24(.ZoomIn)

        case CmdId.ZoomOut:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Zoom Out"
            evt.disabled = !capture or !main.editWnd.editView.canZoomOut()
            evt.icon     = main.getTheme().getIcon24(.ZoomOut)
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd
        let view = main.editWnd.editView

        switch cxt.id
        {
        case CmdId.Zoom1:
            view.setZoom(1)

        case CmdId.ZoomFit:
            view.zoomToFit()

        case CmdId.ZoomIn:
            view.zoomIn()

        case CmdId.ZoomOut:
            view.zoomOut()
        }

        return true
    }
}
