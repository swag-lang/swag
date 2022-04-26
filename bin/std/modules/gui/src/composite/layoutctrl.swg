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
    inter:          f32 = 5
}

impl IWnd for LayoutCtrl
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        visit c, i: childs
        {
            c.margin.clear()
            switch kind
            {
            case VerticalStack:
                c.dockStyle = .Top
                if i > 0
                    c.margin.y = inter
            }
        }

        frameWnd.IWnd.onResizeEvent(evt)
    }
}

impl LayoutCtrl
{
    // Create a layout control
    func create(parent: *Wnd, kind: LayoutKind, position: Math.Rectangle = @{})->*LayoutCtrl
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