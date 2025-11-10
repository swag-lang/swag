struct Rect { x, y, w, h: s32 }

struct Wnd {}

impl Wnd
{
    #[Swag.Inline]
    mtd getClientRect()->Rect
    {
        return Rect{0, 0, 10, 20}
    }
}

impl Rect
{
    #[Swag.Inline]
    mtd contains(x, y: s32)->bool
    {
        return true
    }
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
#test
{
    var wnd: Wnd
    var g    = 0
    if wnd.getClientRect().contains(0, 0) do
        g = 1
    @assert(g == 1)
}
