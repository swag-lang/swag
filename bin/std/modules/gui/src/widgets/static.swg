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
        if vv % 2
            evt.painter.fillRoundRect(self.position, 10, 10, Argb.Red)
        else
            evt.painter.fillRoundRect(self.position, 10, 10, Argb.Blue)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if evt.button == Input.MouseButton.Left and evt.kind == EventKind.MousePressed
        {
            vv += 1
            base.wnd.surface.invalidate()
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