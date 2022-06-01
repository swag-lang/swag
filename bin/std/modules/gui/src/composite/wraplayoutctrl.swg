#global public
using Core

struct WrapLayoutCtrl
{
    using frameWnd: FrameWnd
    spacing:        f32 = 5
    wrapWidth:      f32 = 64
    wrapHeight:     f32 = 64
}

impl IWnd for WrapLayoutCtrl
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if !childs.count
        {
            frameWnd.IWnd.onResizeEvent(evt)
            return
        }

        client := getClientRectPadding()

        x := padding.x
        y := padding.y
        visit c: childs
        {
            rect := Math.Rectangle{x, y, wrapWidth, wrapHeight}
            c.setPosition(rect)
            x += wrapWidth + spacing
            if x > client.right()
            {
                x = padding.x
                y += wrapHeight + spacing
            }
        }

        frameWnd.IWnd.onResizeEvent(evt)
    }
}

impl WrapLayoutCtrl
{
    // Create a layout control
    func create(parent: *Wnd, position: Math.Rectangle = @{})->*WrapLayoutCtrl
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