using Core

public struct FrameWnd
{
    using wnd:  Wnd
    view:       *Wnd
}

impl IWnd for FrameWnd
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if !view
            return
        view.move(padding.x, padding.y)
        view.resize(position.width - padding.x - padding.z, position.height - padding.y - padding.w)
    }
}

public impl FrameWnd
{
    #[Swag.Discardable]
    mtd(T) createView()->*T
    {
        view = Wnd.create'T(self, "", position)
        view.backgroundStyle = .Window
        sendResizeEvent()
        return cast(*T) view
    }
}