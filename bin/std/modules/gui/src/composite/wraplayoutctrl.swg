#global public
using Core

struct WrapLayoutCtrl
{
    using wnd: Wnd

    spacing:        f32 = 5
    wrapWidth:      f32 = 64
    wrapHeight:     f32 = 64
    resultHeight:   f32
}

impl IWnd for WrapLayoutCtrl
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if !childs.count
        {
            wnd.IWnd.onResizeEvent(evt)
            return
        }

        client := getClientRectPadding()

        x := padding.x
        y := padding.y
        visit c: childs
        {
            if c.wndFlags & .Hidden
                continue
            resultHeight = y + wrapHeight
            rect := Math.Rectangle{x, y, wrapWidth, wrapHeight}
            c.setPosition(rect)
            x += wrapWidth + spacing
            if x + wrapWidth > client.right()
            {
                x = padding.x
                y += wrapHeight + spacing
            }
        }

        wnd.IWnd.onResizeEvent(evt)
    }
}

impl WrapLayoutCtrl
{
    // Create a layout control
    func create(parent: *Wnd, position: Math.Rectangle = {})->*WrapLayoutCtrl
    {
        res := Wnd.create'WrapLayoutCtrl(parent, position)
        return res
    }

    // Recompute layout of all childs
    mtd computeLayout()
    {
        sendResizeEvent()
    }
}