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
    func create(parent: *Wnd)->*Static
    {
        res := Wnd.create'Static(parent)
        return res
    }
}