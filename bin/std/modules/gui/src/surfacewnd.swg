public struct SurfaceWnd
{
    using base: Wnd
}

impl IWnd for SurfaceWnd
{
    mtd paint(context: PaintContext) {}
}