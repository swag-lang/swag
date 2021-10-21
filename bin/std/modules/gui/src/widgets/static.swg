#global public
using Core, Pixel

struct Static
{
    using base: Widget
}

impl IWnd for Static
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        evt.painter.fillRoundRect(self.position, 10, 10, Argb.Blue)
    }
}

impl Static
{
    func new(parent: *Wnd)->*Static
    {
        res := Memory.new'Static()
        res.itf = cast(IWnd) dref res
        res.surface = parent.surface
        res.parent = parent
        parent.childs.add(&res.wnd)

        return res
    }
}