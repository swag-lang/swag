using Core, Pixel

public struct SurfaceWnd
{
    using wnd: Wnd
}

impl IWnd for SurfaceWnd
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        evt.painter.begin()

        wnd.IWnd.onPaintEvent(evt)

        evt.painter.end()
        evt.renderer.draw(evt.painter)
    }
}