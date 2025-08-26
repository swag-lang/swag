#global marked
#global public
using Core

enum StackLayoutKind
{
    Top                      // From top to bottom
    Bottom                   // From bottom to top
    Left                     // From left to right
    Right                    // From right to left
    HorzCenter               // Center horizontally
    HorzCenterVertCenter
    VertCenter               // Center vertically
    HorzFit                  // From left to right; fit the available room
    VertFit                  // From top to bottom; fit the available room
}

struct StackLayoutCtrl
{
    using wnd:        Wnd

    layoutKind:       StackLayoutKind = Top
    spacing:          f32 = 5
    fitToContent:     bool
}

impl IWnd for StackLayoutCtrl
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if !me.childs.count
        {
            me.wnd.IWnd.onResizeEvent(evt)
            return
        }

        if me.layoutKind == .HorzFit
        {
            var w = me.position.width
            w -= me.spacing * cast(s32) me.childs.count - 1
            w /= cast(s32) me.childs.count
            foreach c in me.childs
            {
                c.position.width = w
            }
        }
        elif me.layoutKind == .VertFit
        {
            var h = me.position.height
            h -= me.spacing * cast(s32) me.childs.count - 1
            h /= cast(s32) me.childs.count
            foreach c in me.childs
            {
                c.position.height = h
            }
        }

        if me.layoutKind == .HorzCenter or me.layoutKind == .HorzCenterVertCenter
        {
            var totalW = 0'f32
            foreach c, i in me.childs:
                totalW += c.position.width + c.margin.x + c.margin.z
            totalW += cast(f32) ((me.childs.count - 1) * me.spacing)

            var x = me.position.width * 0.5 - totalW * 0.5
            x = Math.max(x, me.padding.x)
            let y = me.getClientRectPadding().vertCenter()
            foreach c in me.childs
            {
                var pos = c.position
                if me.layoutKind == .HorzCenterVertCenter:
                    pos.y = y - (pos.height * 0.5)
                pos.x = Math.round(x + c.margin.x)
                c.setPosition(pos)
                x += c.position.width + me.spacing + c.margin.z
            }
        }
        elif me.layoutKind == .VertCenter
        {
            var totalH = 0'f32
            foreach c, i in me.childs:
                totalH += c.position.height + c.margin.y + c.margin.w
            totalH += cast(f32) ((me.childs.count - 1) * me.spacing)

            var y = me.position.height * 0.5 - totalH * 0.5
            y = Math.max(y, me.padding.y)
            foreach c in me.childs
            {
                var pos = c.position
                pos.y = Math.round(y + c.margin.y)
                c.setPosition(pos)
                y += c.position.height + me.spacing + c.margin.w
            }
        }
        else
        {
            foreach c, i in me.childs
            {
                switch me.layoutKind
                {
                case Top, VertFit:
                    c.dockStyle = .Top
                    if i > 0:
                        c.margin.y = Math.max(c.margin.y, me.spacing)
                case Bottom:
                    c.dockStyle = .Bottom
                    if i > 0:
                        c.margin.w = Math.max(c.margin.w, me.spacing)
                case Left, HorzFit:
                    c.dockStyle = .Left
                    if i > 0:
                        c.margin.x = Math.max(c.margin.x, me.spacing)
                case Right:
                    c.dockStyle = .Right
                    if i > 0:
                        c.margin.z = Math.max(c.margin.z, me.spacing)
                }
            }

            me.applyLayout(evt.oldSize)
        }

        me.wnd.IWnd.onResizeEvent(evt)

        if me.fitToContent
        {
            switch me.layoutKind
            {
            case Top, VertFit:
                let b = me.childs.back()
                let y = b.position.bottom() + me.padding.w
                me.position.height = y

            case Left, HorzFit:
                let b = me.childs.back()
                let x = b.position.right() + me.padding.z
                me.position.width = x
            }
        }
    }
}

impl StackLayoutCtrl
{
    // Create a layout control
    func create(parent: *Wnd, kind: StackLayoutKind = .Top, position: Math.Rectangle = {})->*StackLayoutCtrl
    {
        let res = Wnd.create'StackLayoutCtrl(parent, position)
        res.layoutKind = kind
        return res
    }

    // Recompute layout of all childs
    mtd computeLayout()
    {
        me.sendResizeEvent()
    }
}
