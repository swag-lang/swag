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
        bc := evt.bc

        if showRightBar
        {
            @print(margin.z)
            clientRect := getClientRect()
            clientRect.x = clientRect.right() - 20
            clientRect.width = 20
            bc.painter.fillRect(clientRect, Argb.Red)
        }
    }
}

impl ScrollWnd
{
    mtd computeLayout()
    {
        showRightBar, showBottomBar = false
        margin.z, margin.w = 0

        if scrollSize.y > position.height
        {
            showRightBar = true
            margin.z = 20
        }

        if scrollSize.x > position.width
        {
            showBottomBar = true
            margin.w = 20
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