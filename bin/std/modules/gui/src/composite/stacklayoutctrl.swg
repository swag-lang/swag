#global public
using Core

enum StackLayoutKind
{
    Top
    TopAutoSize
    Bottom
    Left
    LeftAutoSize
    Right
    HorzCenter
}

struct StackLayoutCtrl
{
    using frameWnd: FrameWnd
    layoutKind:     StackLayoutKind = Top
    spacing:        f32 = 5
}

impl IWnd for StackLayoutCtrl
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if !childs.count
        {
            frameWnd.IWnd.onResizeEvent(evt)
            return
        }

        if layoutKind == .HorzCenter
        {
            totalW := 0'f32
            visit c, i: childs
                totalW += c.position.width
            totalW += cast(f32) ((childs.count - 1) * spacing)

            x := position.width * 0.5 - totalW * 0.5
            visit c, i: childs
            {
                pos := c.position
                pos.x = Math.round(x)
                c.setPosition(pos)
                x += c.position.width + spacing
            }
        }
        else
        {
            visit c, i: childs
            {
                c.margin.clear()
                switch layoutKind
                {
                case Top, TopAutoSize:
                    c.dockStyle = .Top
                    if i > 0
                        c.margin.y = spacing

                case Bottom:
                    c.dockStyle = .Bottom
                    if i > 0
                        c.margin.w = spacing

                case Left, LeftAutoSize:
                    c.dockStyle = .Left
                    if i > 0
                        c.margin.x = spacing

                case Right:
                    c.dockStyle = .Right
                    if i > 0
                        c.margin.z = spacing
                }
            }

            applyLayout(evt.oldSize)
        }

        frameWnd.IWnd.onResizeEvent(evt)

        switch layoutKind
        {
        case TopAutoSize:
            b := childs.back()
            y := b.position.bottom() + padding.w
            position.height = y

        case LeftAutoSize:
            b := childs.back()
            x := b.position.right() + padding.z
            position.width = x
        }
    }
}

impl StackLayoutCtrl
{
    // Create a layout control
    func create(parent: *Wnd, kind: StackLayoutKind = .Top, position: Math.Rectangle = @{})->*StackLayoutCtrl
    {
        res := Wnd.create'StackLayoutCtrl(parent, position)
        res.layoutKind = kind
        return res
    }

    // Recompute layout of all childs
    mtd computeLayout()
    {
        sendResizeEvent()
    }
}