using Core

public struct ScrollWnd
{
    using frameWnd: FrameWnd
    scrollSize:     Math.Point
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
    mtd setScrollPos(pos: Math.Point)
    {
        view.scrollPos = pos
    }

    // Set the scroll size (window virtual size)
    mtd setScrollSize(size: Math.Point)
    {
        scrollSize = size
    }
}