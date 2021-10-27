public struct FrameWnd
{
    using wnd:  Wnd
    view:       *Wnd
}

impl IWnd for FrameWnd
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        view.position = position
        //view.position.inflate(-50)
    }
}

impl FrameWnd
{
    mtd(T) createView()
    {
        view = Wnd.create'T(self, "", position)
    }
}