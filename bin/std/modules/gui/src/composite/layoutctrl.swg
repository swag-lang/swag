#global public
using Core

enum LayoutKind
{
    TopStack
    TopStackAutoSize
    BottomStack
    LeftStack
    LeftStackAutoSize
    RightStack
}

struct LayoutCtrl
{
    using frameWnd: FrameWnd
    layoutKind:     LayoutKind = TopStack
    inter:          f32 = 5
}

impl IWnd for LayoutCtrl
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if !childs.count
        {
            frameWnd.IWnd.onResizeEvent(evt)
            return
        }

        visit c, i: childs
        {
            c.margin.clear()
            switch layoutKind
            {
            case TopStack, TopStackAutoSize:
                c.dockStyle = .Top
                if i > 0
                    c.margin.y = inter

            case BottomStack:
                c.dockStyle = .Bottom
                if i > 0
                    c.margin.w = inter

            case LeftStack, LeftStackAutoSize:
                c.dockStyle = .Left
                if i > 0
                    c.margin.x = inter

            case RightStack:
                c.dockStyle = .Right
                if i > 0
                    c.margin.z = inter
            }
        }

        applyLayout(evt.oldSize)
        frameWnd.IWnd.onResizeEvent(evt)

        switch layoutKind
        {
        case TopStackAutoSize:
            b := childs.back()
            y := b.position.bottom() + padding.w
            position.height = y

        case LeftStackAutoSize:
            b := childs.back()
            x := b.position.right() + padding.z
            position.width = x
        }
    }
}

impl LayoutCtrl
{
    // Create a layout control
    func create(parent: *Wnd, kind: LayoutKind, position: Math.Rectangle = @{})->*LayoutCtrl
    {
        res := Wnd.create'LayoutCtrl(parent, position)
        res.layoutKind = kind
        return res
    }

    // Recompute layout of all childs
    mtd computeLayout()
    {
        sendResizeEvent()
    }
}