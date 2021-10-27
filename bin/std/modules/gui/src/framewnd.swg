public struct FrameWnd
{
    using wnd:  Wnd
    view:       *Wnd
}

impl IWnd for FrameWnd
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        view.position.width = position.width
        view.position.height = position.height
    }
}

impl FrameWnd
{
    mtd(T) createView()
    {
        view = Wnd.create'T(self, "", position)
    }
}