#global public
using Core, Pixel

struct Static
{
    using base: Widget
    vv: s32
}

impl IWnd for Static
{
    mtd onPaintEvent(using evt: *PaintEvent)
    {
        if vv % 2
            painter.fillRoundRect(self.position, 10, 10, Argb.Red)
        else
            painter.fillRoundRect(self.position, 10, 10, Argb.Blue)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if evt.button == Input.MouseButton.Left and evt.kind == EventKind.MousePressed
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