#global public
using Core

enum LayoutKind
{
    VerticalStack
}

struct LayoutCtrl
{
    using frameWnd: FrameWnd
    kind:           LayoutKind = VerticalStack
}

impl IWnd for LayoutCtrl
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        visit c: childs
        {
            switch kind
            {
            case VerticalStack:
                c.dockStyle = .Top
                c.margin.y = 10
            }
        }

        frameWnd.IWnd.onResizeEvent(evt)
    }
}

impl LayoutCtrl
{
    // Create a layout control
    func create(parent: *Wnd, position: Math.Rectangle, kind: LayoutKind)->*LayoutCtrl
    {
        res := Wnd.create'LayoutCtrl(parent, position)
        return res
    }

    // Recompute layout of all childs
    mtd computeLayout()
    {
        sendResizeEvent()
    }
}