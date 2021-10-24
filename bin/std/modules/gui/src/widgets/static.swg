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
        Style.drawTexture(evt.bc.painter, tme.widgetsImg, self.position, @{1, 1, 30, 20}, 6, 6, Argb.White)
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