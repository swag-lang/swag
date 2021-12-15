using Core, Pixel

public struct ScrollWnd
{
    using frameWnd: FrameWnd
    scrollSize:     Math.Point
    showBottomBar:  bool
    showRightBar:   bool
}

impl IWnd for ScrollWnd
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        computeLayout()
        FrameWnd.IWnd.onResizeEvent(self, evt)
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        metrics := getThemeMetrics()

        if showRightBar
        {
            clientRect := getClientRect()
            clientRect.x = clientRect.right() - padding.z
            clientRect.width = padding.z
            bc.painter.fillRect(clientRect, Argb.Red)
        }
    }
}

impl ScrollWnd
{
    mtd computeLayout()
    {
        metrics := getThemeMetrics()
        showRightBar, showBottomBar = false
        padding.z, padding.w = 0

        if scrollSize.y > position.height
        {
            showRightBar = true
            padding.z = metrics.scrollBar_size
        }

        if scrollSize.x > position.width
        {
            showBottomBar = true
            padding.w = metrics.scrollBar_size
        }

        invalidate()
    }
}

public impl ScrollWnd
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*ScrollWnd
    {
        res := Wnd.create'ScrollWnd(parent, "", position, id)
        res.createView'Wnd()
        res.sendCreateEvent()
        return res
    }

    // Get the current scroll position
    mtd getScrollPos()->Math.Point
    {
        return view.scrollPos
    }

    // Set the current scroll position
    mtd setScrollPos(x, y: f32)
    {
        view.scrollPos.x = x
        view.scrollPos.y = y
    }

    // Set the scroll size (window virtual size)
    mtd setScrollSize(x, y: f32)
    {
        scrollSize.x = x
        scrollSize.y = y
        computeLayout()
        sendResizeEvent()
    }
}