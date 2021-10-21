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
        res.itf = cast(IWnd) dref res
        res.surface = parent.surface
        res.parent = parent
        parent.childs.add(&res.wnd)

        return res
    }
}