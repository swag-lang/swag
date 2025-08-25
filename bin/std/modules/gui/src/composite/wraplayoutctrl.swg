#global public
using Core

struct WrapLayoutCtrl
{
    using wnd:        Wnd

    spacing:          f32 = 5
    wrapWidth:        f32 = 64
    wrapHeight:       f32 = 64
    resultHeight:     f32
}

impl IWnd for WrapLayoutCtrl
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if !me.childs.count
        {
            me.wnd.IWnd.onResizeEvent(evt)
            return
        }

        var client = me.getClientRectPadding()

        var x = me.padding.x
        var y = me.padding.y
        foreach c in me.childs
        {
            if c.wndFlags & .Hidden:
                continue
            resultHeight = y + wrapHeight
            var rect = Math.Rectangle{x, y, wrapWidth, wrapHeight}
            c.setPosition(rect)
            x += wrapWidth + spacing
            if x + wrapWidth > client.right()
            {
                x = padding.x
                y += wrapHeight + spacing
            }
        }

        me.wnd.IWnd.onResizeEvent(evt)
    }
}

impl WrapLayoutCtrl
{
    // Create a layout control
    func create(parent: *Wnd, position: Math.Rectangle = {})->*WrapLayoutCtrl
    {
        let res = Wnd.create'WrapLayoutCtrl(parent, position)
        return res
    }

    // Recompute layout of all childs
    mtd computeLayout()
    {
        me.sendResizeEvent()
    }
}
