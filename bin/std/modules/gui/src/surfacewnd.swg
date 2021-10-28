public struct SurfaceWnd
{
    using base: FrameWnd
}

impl IWnd for SurfaceWnd
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
    }
}