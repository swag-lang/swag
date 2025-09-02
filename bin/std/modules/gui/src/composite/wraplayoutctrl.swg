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
            me.resultHeight = y + me.wrapHeight
            var rect = Math.Rectangle{x, y, me.wrapWidth, me.wrapHeight}
            c.setPosition(rect)
            x += me.wrapWidth + me.spacing
            if x + me.wrapWidth > client.right()
            {
                x = me.padding.x
                y += me.wrapHeight + me.spacing
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
