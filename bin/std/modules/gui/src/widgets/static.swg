#global public
using Core, Pixel

struct Static
{
    using base: Widget
    vv: s32
}

impl IWnd for Static
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        tme := surface.app.theme
        Theme.drawTexture(evt.painter, tme.widgetsImg, self.position, @{0, 0, 32, 22}, 0, 0, Argb.White)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if evt.button == .Left and evt.kind == .MousePressed
        {
            vv += 1
            surface.invalidate()
        }
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