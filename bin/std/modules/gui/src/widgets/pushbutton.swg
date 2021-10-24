#global public
using Core, Pixel

struct PushButton
{
    using base: Widget
    pressed:    bool
}

impl IWnd for PushButton
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        tme := getTheme()

        var srcRect: Math.Rectangle
        if pressed
            srcRect = @{1, 22, 30, 20}
        else
            srcRect = @{1, 1, 30, 20}

        Style.drawTexture(evt.bc.painter, tme.widgetsImg, self.position, srcRect, 6, 6, Argb.White)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if evt.button == .Left
        {
            switch evt.kind
            {
            case MouseReleased:
                pressed = false
                invalidate()
            case MousePressed:
                pressed = true
                invalidate()
            }
        }
    }
}

impl PushButton
{
    func create(parent: *Wnd)->*PushButton
    {
        res := Wnd.create'PushButton(parent)
        return res
    }
}