#global public
using Core, Pixel

#[Swag.EnumFlags]
enum SplitViewFlags
{
    None
    Vertical
}

struct SplitView
{
    using frameWnd: FrameWnd
    splitViewFlags  = SplitViewFlags.None
    size:           f32 = 100

    correctMoving:  f32
    isHot:          bool
    moving:         bool
}

impl IWnd for SplitView
{
    mtd onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = true
        local := surfaceToLocal(evt.surfacePos)
        rect  := getBarRect()

        if isHot
            cursor = isVertical() ? Cursor.from(CursorShape.SizeNS) : Cursor.from(CursorShape.SizeWE)
        else
            cursor.clear()

        switch evt.kind
        {
        case MouseMove:
            if moving
            {
                if isVertical()
                {
                    diff := local.y - (rect.y + correctMoving)
                    size += diff
                    sendResizeEvent()
                }
                else
                {
                    diff := local.x - (rect.x + correctMoving)
                    size += diff
                    sendResizeEvent()
                }
            }
            else
            {
                newHot := rect.contains(local)
                if newHot != isHot invalidate()
                isHot = newHot
            }

        case MousePressed:
            if evt.button == .Left and isHot
            {
                moving = true
                surface.app.captureMouse(self)
                if isVertical()
                    correctMoving = local.y - rect.y
                else
                    correctMoving = local.x - rect.x
            }

        case MouseReleased:
            if moving
            {
                moving = false
                surface.app.releaseMouse()
            }
        }
    }

    mtd onResizeEvent(evt: *ResizeEvent)
    {
        pane0 := childs.count > 0 ? childs[0] : null
        pane1 := childs.count > 1 ? childs[1] : null
        if !pane0
            return

        size0 := size
        client := getClientRectPadding()

        if isVertical()
        {
            pane0.setPosition(client.x, client.y, client.width, size0)
            if pane1
            {
                barSize := getThemeMetrics().splitView_BarSize
                size0 += barSize
                pane1.setPosition(client.x, client.y + size0, client.width, client.height - size0)
            }
        }
        else
        {
            pane0.setPosition(client.x, client.y, size0, client.height)
            if pane1
            {
                barSize := getThemeMetrics().splitView_BarSize
                size0 += barSize
                pane1.setPosition(client.x + size0, client.y, client.width - size0, client.height)
            }
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        frameWnd.IWnd.onPaintEvent(evt)

        if evt.kind != .PostChilds
        {
            rect := getBarRect()
            if isVertical()
            {
                evt.bc.painter.fillRect(rect, Argb.Red)
            }
            else
            {
                evt.bc.painter.fillRect(rect, Argb.Red)
            }
        }
    }
}

private impl SplitView
{
    mtd getBarRect()->Math.Rectangle
    {
        var result: retval = getClientRectPadding()
        if isVertical()
        {
            result.y += size
            result.height = getThemeMetrics().splitView_BarSize
        }
        else
        {
            result.x += size
            result.width = getThemeMetrics().splitView_BarSize
        }

        return result
    }
}

impl SplitView
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, vertical = false, id: string = null)->*SplitView
    {
        Debug.assert(parent != null)

        pos := position
        res := Wnd.create'SplitView(parent, null, pos, id)
        res.focusStrategy = .None
        if vertical
            res.splitViewFlags |= .Vertical
        return res
    }


    #[Swag.Inline]
    mtd isVertical() => cast(bool) (splitViewFlags & .Vertical)
}