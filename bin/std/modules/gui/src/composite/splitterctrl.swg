#global public
using Core, Pixel

#[Swag.EnumFlags]
enum SplitterFlags
{
    Zero = 0
    Vertical
    HideBars
}

struct SplitterItem
{
    view:               *Wnd
    size:               f32
    minSize:            f32 = 5
    maxSize:            f32 = 0
    label:              *Label
    tryPreserveSize:    bool
    aniBar:             BlendColor
}

struct SplitterCtrl
{
    using frameWnd: FrameWnd
    splitterFlags   = SplitterFlags.Zero
    titleSize:      f32 = 30

    items:          ArrayPtr'SplitterItem
    correctMoving:  f32
    hotIdx          = Swag.U32.Max
    moving:         bool
}

impl IWnd for SplitterCtrl
{
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = true
        local := surfaceToLocal(evt.surfacePos)

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

                invalidate()
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

        if hotIdx != Swag.U32.Max
            cursor = isVertical() ? Cursor.from(CursorShape.SizeNS) : Cursor.from(CursorShape.SizeWE)
        else
            cursor.clear()
    }

    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
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

                if item.label
                {
                    labelRect := paneRect
                    labelRect.height = titleSize
                    item.label.setPosition(labelRect)

                    paneRect.y += titleSize
                    paneRect.height -= titleSize
                }

                item.view.setPosition(paneRect)
                y += item.size + barSize
            }
            else
            {
                paneRect := Math.Rectangle{x, client.y, item.size, client.height}
                paneRect.applyPadding(item.view.margin)

                if item.label
                {
                    labelRect := paneRect
                    labelRect.width = titleSize
                    item.label.setPosition(labelRect)

                    paneRect.x += titleSize
                    paneRect.width -= titleSize
                }

                item.view.setPosition(paneRect)
                x += item.size + barSize
            }
        }
    }

    mtd impl onPostPaintEvent(evt: *PaintEvent)
    {
        if !items.count
            return

        painter := evt.bc.painter
        theme   := getTheme()
        rects   := getThemeRects()
        colors  := getThemeColors()

        loop idx: items.count - 1
        {
            rect := getBarRect(cast(u32) idx)
            isHot := hotIdx == idx
            if isHot or !splitterFlags.has(.HideBars)
            {
                colorBar := isHot ? colors.splitView_BarHot : colors.splitView_Bar
                items[idx].aniBar.apply(self, colorBar)
                if isVertical()
                {
                    rectBar := isHot ? &rects.splitView_HorzBarHotBk : &rects.splitView_HorzBarBk
                    theme.drawSubRect(painter, rect, rectBar, items[idx].aniBar.cur())
                }
                else
                {
                    rectBar := isHot ? &rects.splitView_VertBarHotBk : &rects.splitView_VertBarBk
                    theme.drawSubRect(painter, rect, rectBar, items[idx].aniBar.cur())
                }
            }
        }

        frameWnd.IWnd.onPostPaintEvent(evt)
    }
}

private impl SplitterCtrl
{
    mtd computeLayout()
    {
        rect    := getClientRectPadding()
        if rect.width <= items.count or rect.height <= items.count
            return

        barSize := getThemeMetrics().splitView_BarSize

        var dispatch: f32
        if items.count > 0
        {
            // Compute the current total sizes of panes
            countNotPreserve, countPreserve := 0
            totalSize := 0'f32
            visit item, idx: items
            {
                totalSize += item.size
                countPreserve += item.tryPreserveSize ? 1 : 0
                countNotPreserve += item.tryPreserveSize ? 0 : 1
            }

            totalSize += cast(f32) ((items.count - 1) * barSize)

            // Compute the size to adapt
            if isVertical()
                dispatch = cast(f32) (rect.height - totalSize)
            else
                dispatch = cast(f32) (rect.width - totalSize)

            // Dispatch the size change first to all panoes not marked as 'tryPreserveSize'
            if countNotPreserve
            {
                diff := cast(f32) Math.round(dispatch / countNotPreserve)
                visit item, idx: items
                {
                    if item.tryPreserveSize
                        continue

                    countNotPreserve -= 1

                    realDiff := countNotPreserve ? diff : dispatch
                    if item.maxSize > 0 and realDiff > 0 and item.size + realDiff > item.maxSize
                        realDiff = item.maxSize - item.size
                    elif realDiff < 0 and item.size + realDiff < item.minSize
                        realDiff = item.minSize - item.size

                    item.size += realDiff
                    dispatch -= realDiff
                }
            }

            // Dispatch the rest between panes that try to preserved their size
            if countPreserve
            {
                diff := cast(f32) Math.round(dispatch / countPreserve)
                visit item, idx: items
                {
                    if !item.tryPreserveSize
                        continue

                    countPreserve -= 1

                    realDiff := countNotPreserve ? diff : dispatch
                    if item.maxSize > 0 and realDiff > 0 and item.size + realDiff > item.maxSize
                        realDiff = item.maxSize - item.size
                    elif realDiff < 0 and item.size + realDiff < item.minSize
                        realDiff = item.minSize - item.size

                    item.size += realDiff
                    dispatch -= realDiff
                }
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

        reqSize := Math.max(curPane.minSize, newSize)
        if curPane.maxSize > 0
            reqSize = Math.min(curPane.maxSize, reqSize)

        diff := reqSize - curPane.size
        if items.count == 1
        {
            curPane.size += diff
            return
        }

        scanLeft := idx == items.count - 1
        i := cast(s32) idx

        loop
        {
            if scanLeft and i == 0
                break
            if !scanLeft and i == items.count - 1
                break

            otherPane := scanLeft ? items[i - 1] : items[i + 1]
            i += scanLeft ? -1 : 1

            realDiff := diff
            if realDiff > 0 and otherPane.size - realDiff < otherPane.minSize
            {
                realDiff = otherPane.size - otherPane.minSize
                otherPane.size = otherPane.minSize
            }
            elif realDiff < 0 and otherPane.maxSize > 0 and otherPane.size - realDiff > otherPane.maxSize
            {
                realDiff = otherPane.maxSize - otherPane.size
                otherPane.size = otherPane.maxSize
            }
            else
            {
                otherPane.size -= realDiff
                break
            }

            diff -= realDiff
        }

        curPane.size += diff
    }
}

impl SplitterCtrl
{
    // Creates a new SplitterCtrl instance
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, vertical = false, id: WndId = null)->*SplitterCtrl
    {
        Debug.assert(parent != null)

        pos := position
        res := Wnd.create'SplitterCtrl(parent, null, pos, id)
        res.focusStrategy = .None
        res.wndFlags |= .PostChildsPaint
        if vertical
            res.splitterFlags.add(.Vertical)
        return res
    }


    #[Swag.Inline]
    mtd isVertical() => splitterFlags.has(.Vertical)

    // Add a new pane
    #[Swag.Discardable]
    mtd addPane(pane: *Wnd, paneSize: f32 = 100, title = false)->*SplitterItem
    {
        pane.setParent(self)

        item := items.newPtr()
        item.view = pane
        if title
        {
            with label := Label.create(self, "")
            {
                .backgroundStyle = .Window
                .style.addStyleSheetColors("wnd_Bk $dlg_BtnBarBk")
            }

            item.label = label
        }

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

        return item
    }

    // Set the pane size (size of the first view)
    mtd setPaneSize(idx: u32, newSize: f32)
    {
        Debug.assert(idx < items.count)
        setPaneSizePrivate(idx, newSize)
        sendResizeEvent()
    }
}