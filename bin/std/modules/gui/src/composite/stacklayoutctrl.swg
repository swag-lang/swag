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
        if !.childs.count
        {
            .wnd.IWnd.onResizeEvent(evt)
            return
        }

        if .layoutKind == .HorzFit
        {
            var w = .position.width
            w -= .spacing * cast(s32) .childs.count - 1
            w /= cast(s32) .childs.count
            foreach c in .childs
            {
                c.position.width = w
            }
        }
        elif .layoutKind == .VertFit
        {
            var h = .position.height
            h -= .spacing * cast(s32) .childs.count - 1
            h /= cast(s32) .childs.count
            foreach c in .childs
            {
                c.position.height = h
            }
        }

        if .layoutKind == .HorzCenter or .layoutKind == .HorzCenterVertCenter
        {
            var totalW = 0'f32
            foreach c, i in .childs do
                totalW += c.position.width + c.margin.x + c.margin.z
            totalW += cast(f32) ((.childs.count - 1) * .spacing)

            var x = .position.width * 0.5 - totalW * 0.5
            x = Math.max(x, .padding.x)
            let y = .getClientRectPadding().vertCenter()
            foreach c in .childs
            {
                var pos = c.position
                if .layoutKind == .HorzCenterVertCenter do
                    pos.y = y - (pos.height * 0.5)
                pos.x = Math.round(x + c.margin.x)
                c.setPosition(pos)
                x += c.position.width + .spacing + c.margin.z
            }
        }
        elif .layoutKind == .VertCenter
        {
            var totalH = 0'f32
            foreach c, i in .childs do
                totalH += c.position.height + c.margin.y + c.margin.w
            totalH += cast(f32) ((.childs.count - 1) * .spacing)

            var y = .position.height * 0.5 - totalH * 0.5
            y = Math.max(y, .padding.y)
            foreach c in .childs
            {
                var pos = c.position
                pos.y = Math.round(y + c.margin.y)
                c.setPosition(pos)
                y += c.position.height + .spacing + c.margin.w
            }
        }
        else
        {
            foreach c, i in .childs
            {
                switch .layoutKind
                {
                case Top, VertFit:
                    c.dockStyle = .Top
                    if i > 0 do
                        c.margin.y = Math.max(c.margin.y, .spacing)
                case Bottom:
                    c.dockStyle = .Bottom
                    if i > 0 do
                        c.margin.w = Math.max(c.margin.w, .spacing)
                case Left, HorzFit:
                    c.dockStyle = .Left
                    if i > 0 do
                        c.margin.x = Math.max(c.margin.x, .spacing)
                case Right:
                    c.dockStyle = .Right
                    if i > 0 do
                        c.margin.z = Math.max(c.margin.z, .spacing)
                }
            }

            .applyLayout(evt.oldSize)
        }

        .wnd.IWnd.onResizeEvent(evt)

        if .fitToContent
        {
            switch .layoutKind
            {
            case Top, VertFit:
                let b = .childs.back()
                let y = b.position.bottom() + .padding.w
                .position.height = y

            case Left, HorzFit:
                let b = .childs.back()
                let x = b.position.right() + .padding.z
                .position.width = x
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
        .sendResizeEvent()
    }
}
