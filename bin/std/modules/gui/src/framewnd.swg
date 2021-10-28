using Core

public struct FrameWnd
{
    using wnd:  Wnd
    view:       *Wnd

    marginsTopLeft:     Math.Point
    marginsBottomRight: Math.Point
}

impl IWnd for FrameWnd
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        view.position.x = marginsTopLeft.x
        view.position.y = marginsTopLeft.y
        view.position.width = position.width - marginsTopLeft.x - marginsBottomRight.x
        view.position.height = position.height - marginsTopLeft.y - marginsBottomRight.y
    }
}

impl FrameWnd
{
    mtd(T) createView()
    {
        view = Wnd.create'T(self, "", position)
    }
}