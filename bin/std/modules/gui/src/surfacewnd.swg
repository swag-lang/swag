using Pixel

public struct SurfaceWnd
{
    using base: FrameWnd
}

impl IWnd for SurfaceWnd
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        evt.bc.painter.fillRect(position, Argb.Red)
    }
}