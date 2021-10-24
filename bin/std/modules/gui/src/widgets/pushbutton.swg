#global public
using Core, Pixel

struct PushButton
{
    using base: Widget
    pressed:    bool
    pressing:   bool
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
        switch evt.kind
        {
        case MouseMove:
            if pressing
            {
                if !position.contains(evt.pos)
                {
                    pressed = false
                    invalidate()
                }
                else if !pressed
                {
                    pressed = true
                    invalidate()
                }
            }

        case MousePressed:
            if evt.button == .Left
            {
                pressed, pressing = true
                captureMouse()
                invalidate()
            }

        case MouseReleased:
            if evt.button == .Left
            {
                releaseMouse()
                pressed, pressing = false
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