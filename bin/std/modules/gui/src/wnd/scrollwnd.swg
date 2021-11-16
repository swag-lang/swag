using Core

public struct ScrollWnd
{
    using wnd: Wnd
}

impl ScrollWnd
{
    mtd getScrollPos()->Math.Point
    {
        return @{0,0}
    }
}