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
    paneSize:       f32 = 100
    minPaneSize:    f32 = 5

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
        case MouseLeave:
            if isHot
            {
                isHot = false
                invalidate()
            }

        case MouseMove:
            if moving
            {
                if isVertical()
                {
                    diff := local.y - (rect.y + correctMoving)
                    setPaneSize(paneSize + diff)
                }
                else
                {
                    diff := local.x - (rect.x + correctMoving)
                    setPaneSize(paneSize + diff)
                }
            }
            else
            {
                newHot := rect.contains(local)
                if newHot != isHot
                    invalidate()
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

        setPaneSizePrivate(paneSize)
        size0 := paneSize
        client := getClientRectPadding()

        if isVertical()
        {
            paneRect := Math.Rectangle{client.x, client.y, client.width, size0}
            paneRect.applyPadding(pane0.margin)
            pane0.setPosition(paneRect)
            if pane1
            {
                barSize := getThemeMetrics().splitView_BarSize
                size0 += barSize
                paneRect = Math.Rectangle{client.x, client.y + size0, client.width, client.height - size0}
                paneRect.applyPadding(pane1.margin)
                pane1.setPosition(paneRect)
            }
        }
        else
        {
            paneRect := Math.Rectangle{client.x, client.y, size0, client.height}
            paneRect.applyPadding(pane0.margin)
            pane0.setPosition(paneRect)
            if pane1
            {
                barSize := getThemeMetrics().splitView_BarSize
                size0 += barSize
                paneRect = Math.Rectangle{client.x + size0, client.y, client.width - size0, client.height}
                paneRect.applyPadding(pane1.margin)
                pane1.setPosition(paneRect)
            }
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        frameWnd.IWnd.onPaintEvent(evt)

        painter := evt.bc.painter
        theme   := getTheme()
        rects   := getThemeRects()
        colors  := getThemeColors()

        if evt.kind != .PostChilds
        {
            rect := getBarRect()
            colorBar := isHot ? colors.splitView_BarHot : colors.splitView_Bar

            if isVertical()
            {
                rectBar := isHot ? &rects.splitView_HorzBarHotBk : &rects.splitView_HorzBarBk
                theme.drawSubRect(painter, rect, rectBar, colorBar)
            }
            else
            {
                rectBar := isHot ? &rects.splitView_VertBarHotBk : &rects.splitView_VertBarBk
                theme.drawSubRect(painter, rect, rectBar, colorBar)
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
            result.y += paneSize
            result.height = getThemeMetrics().splitView_BarSize
        }
        else
        {
            result.x += paneSize
            result.width = getThemeMetrics().splitView_BarSize
        }

        return result
    }

    mtd setPaneSizePrivate(newSize: f32)
    {
        paneSize = Math.max(minPaneSize, newSize)
        rect := getClientRectPadding()

        if isVertical()
        {
            size1 := rect.height - (paneSize + getThemeMetrics().splitView_BarSize)
            if size1 < minPaneSize
                paneSize = rect.height - (minPaneSize + getThemeMetrics().splitView_BarSize)
        }
        else
        {
            size1 := rect.width - (paneSize + getThemeMetrics().splitView_BarSize)
            if size1 < minPaneSize
                paneSize = rect.width - (minPaneSize + getThemeMetrics().splitView_BarSize)
        }
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

    // Set the pane size (size of the first view)
    mtd setPaneSize(newSize: f32)
    {
        setPaneSizePrivate(newSize)
        sendResizeEvent()
    }
}