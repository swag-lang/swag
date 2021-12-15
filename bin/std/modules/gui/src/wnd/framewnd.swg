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
        view.move(marginsTopLeft.x, marginsTopLeft.y)
        view.resize(position.width - marginsTopLeft.x - marginsBottomRight.x, position.height - marginsTopLeft.y - marginsBottomRight.y)
    }
}

impl FrameWnd
{
    mtd(T) createView()
    {
        view = Wnd.create'T(self, "", position)
        view.backgroundStyle = .Window
        sendResizeEvent()
    }
}