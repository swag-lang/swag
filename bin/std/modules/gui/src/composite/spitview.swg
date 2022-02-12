#global public
using Core, Pixel

#[Swag.EnumFlags]
enum SplitViewFlags
{
    None
    Vertical
}

struct SplitViewItem
{
    view:       *Wnd
    size:       f32
    minSize:    f32 = 5
}

struct SplitView
{
    using frameWnd: FrameWnd
    splitViewFlags  = SplitViewFlags.None

    items:          ArrayPtr'SplitViewItem
    correctMoving:  f32
    hotIdx          = Swag.U32.Max
    moving:         bool
}

impl IWnd for SplitView
{
    mtd onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = true
        local := surfaceToLocal(evt.surfacePos)

        if hotIdx != Swag.U32.Max
            cursor = isVertical() ? Cursor.from(CursorShape.SizeNS) : Cursor.from(CursorShape.SizeWE)
        else
            cursor.clear()

        switch evt.kind
        {
        case MouseLeave:
            if hotIdx != Swag.U32.Max
            {
                hotIdx = Swag.U32.Max
                invalidate()
            }

        case MouseMove:
            if moving
            {
                rect := getBarRect(hotIdx)
                if isVertical()
                {
                    diff := local.y - (rect.y + correctMoving)
                    setPaneSize(hotIdx, items[hotIdx].size + diff)
                }
                else
                {
                    diff := local.x - (rect.x + correctMoving)
                    setPaneSize(hotIdx, items[hotIdx].size + diff)
                }
            }
            else
            {
                newHot := hitTest(evt.surfacePos)
                if newHot != hotIdx
                    invalidate()
                hotIdx = newHot
            }

        case MousePressed:
            if evt.button == .Left and hotIdx != Swag.U32.Max
            {
                rect := getBarRect(hotIdx)
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

        computeLayout()

        client := getClientRectPadding()
        x := client.x
        y := client.y
        barSize := getThemeMetrics().splitView_BarSize

        visit item: items
        {
            if isVertical()
            {
                paneRect := Math.Rectangle{client.x, y, client.width, item.size}
                paneRect.applyPadding(item.view.margin)
                item.view.setPosition(paneRect)
                y += item.size + barSize
            }
            else
            {
                paneRect := Math.Rectangle{x, client.y, item.size, client.height}
                paneRect.applyPadding(item.view.margin)
                item.view.setPosition(paneRect)
                x += item.size + barSize
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
            loop idx: items
            {
                rect := getBarRect(cast(u32) idx)
                isHot := hotIdx == idx
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
}

private impl SplitView
{
    mtd computeLayout()
    {
        rect    := getClientRectPadding()
        barSize := getThemeMetrics().splitView_BarSize

        // Compute the current total sizes of panes
        var dispatch: f32
        if items.count > 0
        {
            totalSize := 0'f32
            visit item, idx: items
                totalSize += item.size
            totalSize += cast(f32) ((items.count - 1) * barSize)
            if isVertical()
                dispatch = cast(f32) (totalSize - rect.height)
            else
                dispatch = cast(f32) (totalSize - rect.width)

            diff := cast(f32) Math.round(dispatch / items.count)
            visit item, idx: items
            {
                if idx == items.count - 1
                    item.size -= dispatch
                else
                    item.size -= diff
                dispatch -= diff
            }
        }

        visit item, idx: items
        {
            setPaneSizePrivate(cast(u32) idx, item.size)
        }
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        client := getClientRectPadding()
        local  := surfaceToLocal(surfacePos)

        x := client.x
        y := client.y
        barSize := getThemeMetrics().splitView_BarSize

        visit v, idx: items
        {
            rect := client
            if isVertical()
            {
                rect.y = y + v.size
                rect.height = barSize
                if rect.contains(local)
                    return cast(u32) idx
                y = rect.bottom()
            }
            else
            {
                rect.x = x + v.size
                rect.width = barSize
                if rect.contains(local)
                    return cast(u32) idx
                x = rect.right()
            }
        }

        return Swag.U32.Max
    }

    mtd getBarRect(idx: u32)->Math.Rectangle
    {
        var result: retval = getClientRectPadding()

        barSize := getThemeMetrics().splitView_BarSize
        loop i: 0..idx
        {
            if isVertical()
            {
                result.y += items[i].size
                if i != idx
                    result.y += barSize
                result.height = barSize
            }
            else
            {
                result.x += items[i].size
                if i != idx
                    result.x += barSize
                result.width = barSize
            }
        }

        return result
    }

    mtd setPaneSizePrivate(idx: u32, newSize: f32)
    {
        curPane := items[idx]

        reqSize  := Math.max(curPane.minSize, newSize)
        diff     := reqSize - curPane.size
        if items.count == 1
        {
            curPane.size += diff
            return
        }

        otherPane := idx == items.count - 1 ? items[idx - 1] : items[idx + 1]
        if otherPane.size - diff < otherPane.minSize
        {
            diff = otherPane.size - otherPane.minSize
            otherPane.size = otherPane.minSize
        }
        else
        {
            otherPane.size -= diff
        }

        curPane.size += diff
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

    // Add a new pane
    mtd addPane(pane: *Wnd, paneSize: f32 = 100)
    {
        pane.setParent(self)

        item := items.newPtr()
        item.view = pane
        items.add(item)

        if items.count == 1
        {
            rect := getClientRectPadding()
            if isVertical()
                item.size = rect.height
            else
                item.size = rect.width
        }
        else
        {
            setPaneSize(cast(u32) items.count - 1, paneSize)
        }
    }

    // Set the pane size (size of the first view)
    mtd setPaneSize(idx: u32, newSize: f32)
    {
        Debug.assert(idx < items.count)
        setPaneSizePrivate(idx, newSize)
        sendResizeEvent()
    }
}