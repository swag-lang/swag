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
    HorzCenterVertCenter
    VertCenter
}

struct StackLayoutCtrl
{
    using wnd: Wnd

    layoutKind: StackLayoutKind = Top
    spacing:    f32 = 5
}

impl IWnd for StackLayoutCtrl
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if !childs.count
        {
            wnd.IWnd.onResizeEvent(evt)
            return
        }

        if layoutKind == .HorzCenter or layoutKind == .HorzCenterVertCenter
        {
            totalW := 0'f32
            visit c, i: childs
                totalW += c.position.width + c.margin.x + c.margin.z
            totalW += cast(f32) ((childs.count - 1) * spacing)

            x := position.width * 0.5 - totalW * 0.5
            x = Math.max(x, padding.x)
            y := getClientRectPadding().vertCenter()
            visit c: childs
            {
                pos := c.position
                if layoutKind == .HorzCenterVertCenter
                    pos.y = y - (pos.height * 0.5)
                pos.x = Math.round(x + c.margin.x)
                c.setPosition(pos)
                x += c.position.width + spacing + c.margin.z
            }
        }
        elif layoutKind == .VertCenter
        {
            totalH := 0'f32
            visit c, i: childs
                totalH += c.position.height + c.margin.y + c.margin.w
            totalH += cast(f32) ((childs.count - 1) * spacing)

            y := position.height * 0.5 - totalH * 0.5
            y = Math.max(y, padding.y)
            visit c: childs
            {
                pos := c.position
                pos.y = Math.round(y + c.margin.y)
                c.setPosition(pos)
                y += c.position.height + spacing + c.margin.w
            }
        }
        else
        {
            visit c, i: childs
            {
                switch layoutKind
                {
                case Top, TopAutoSize:
                    c.dockStyle = .Top
                    if i > 0
                        c.margin.y = Math.max(c.margin.y, spacing)

                case Bottom:
                    c.dockStyle = .Bottom
                    if i > 0
                        c.margin.w = Math.max(c.margin.w, spacing)

                case Left, LeftAutoSize:
                    c.dockStyle = .Left
                    if i > 0
                        c.margin.x = Math.max(c.margin.x, spacing)

                case Right:
                    c.dockStyle = .Right
                    if i > 0
                        c.margin.z = Math.max(c.margin.z, spacing)
                }
            }

            applyLayout(evt.oldSize)
        }

        wnd.IWnd.onResizeEvent(evt)

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
    func create(parent: *Wnd, kind: StackLayoutKind = .Top, position: Math.Rectangle = {})->*StackLayoutCtrl
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