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
        evt.painter.begin()
        evt.painter.fillRect(self.base.wnd.position, Argb.Blue)
        evt.painter.end()
        evt.renderer.draw(evt.painter)
    }
}

impl Static
{
    func new(parent: *Wnd)->*Static
    {
        res := Memory.new'Static()
        res.base.wnd.itf = cast(IWnd) dref res
        res.base.wnd.surface = parent.surface
        res.base.wnd.parent = parent
        parent.childs.add(&res.base.wnd)

        return res
    }
}