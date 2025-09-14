#global public
using Core, Pixel

#[Swag.EnumFlags]
enum SplitterFlags
{
    Zero     = 0
    Vertical
    HideBars
}

struct SplitterItem
{
    view:                *Wnd
    size:                f32
    minSize:             f32 = 5
    maxSize:             f32 = 0
    label:               *Label
    tryPreserveSize:     bool
    visible              = true
    aniBar:              BlendColor
}

struct SplitterCtrl
{
    using frameWnd:      FrameWnd
    splitterFlags        = SplitterFlags.Zero
    titleSize:           f32 = 30

    items:               ArrayPtr'SplitterItem
    correctMoving:       f32
    hotIdx               = Swag.U32.Max
    moving:              bool
    numVisiblePanes:     s32
}

impl IWnd for SplitterCtrl
{
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = true
        let local = me.surfaceToLocal(evt.surfacePos)

        switch evt.kind
        {
        case MouseLeave:
            if me.hotIdx != Swag.U32.Max
            {
                me.hotIdx = Swag.U32.Max
                me.invalidate()
            }

        case MouseMove:
            if me.moving
            {
                let rect = me.getBarRect(me.hotIdx)
                if me.isVertical()
                {
                    let diff = local.y - (rect.y + me.correctMoving)
                    me.setPaneSize(me.hotIdx, me.items[me.hotIdx].size + diff)
                }
                else
                {
                    let diff = local.x - (rect.x + me.correctMoving)
                    me.setPaneSize(me.hotIdx, me.items[me.hotIdx].size + diff)
                }

                me.invalidate()
            }
            else
            {
                let newHot = me.hitTest(evt.surfacePos)
                if newHot != me.hotIdx:
                    me.invalidate()
                me.hotIdx = newHot
            }

        case MousePressed:
            if evt.button == .Left and me.hotIdx != Swag.U32.Max
            {
                let rect = me.getBarRect(me.hotIdx)
                me.moving = true
                me.surface.app.captureMouse(me)
                if me.isVertical():
                    me.correctMoving = local.y - rect.y
                else:
                    me.correctMoving = local.x - rect.x
            }

        case MouseReleased:
            if me.moving
            {
                me.moving = false
                me.surface.app.releaseMouse()
            }
        }

        if me.hotIdx != Swag.U32.Max:
            me.cursor = me.isVertical() ? Cursor.from(CursorShape.SizeNS) : Cursor.from(CursorShape.SizeWE)
        else:
            me.cursor.clear()
    }

    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        me.computeLayout()

        let client  = me.getClientRectPadding()
        var x       = client.x
        var y       = client.y
        let barSize = me.getThemeMetrics().splitView_BarSize

        foreach item in me.items
        {
            if !item.visible:
                continue

            if me.isVertical()
            {
                var paneRect = Math.Rectangle{client.x, y, client.width, item.size}
                paneRect.applyPadding(item.view.margin)

                if item.label
                {
                    var labelRect = paneRect
                    labelRect.height = me.titleSize
                    item.label.setPosition(labelRect)

                    paneRect.y += me.titleSize
                    paneRect.height -= me.titleSize
                }

                item.view.setPosition(paneRect)
                y += item.size + barSize
            }
            else
            {
                var paneRect = Math.Rectangle{x, client.y, item.size, client.height}
                paneRect.applyPadding(item.view.margin)

                if item.label
                {
                    var labelRect = paneRect
                    labelRect.width = me.titleSize
                    item.label.setPosition(labelRect)

                    paneRect.x += me.titleSize
                    paneRect.width -= me.titleSize
                }

                item.view.setPosition(paneRect)
                x += item.size + barSize
            }
        }
    }

    mtd impl onPostPaintEvent(evt: *PaintEvent)
    {
        if !me.items.count:
            return

        let painter = evt.bc.painter
        let theme   = me.getTheme()
        let rects   = me.getThemeRects()
        let colors  = me.getThemeColors()

        for idx in me.items.count - 1
        {
            if !me.items[idx].visible:
                continue

            let rect  = me.getBarRect(cast(u32) idx)
            let isHot = me.hotIdx == idx
            if isHot or !me.splitterFlags.has(.HideBars)
            {
                let colorBar = isHot ? colors.splitView_BarHot : colors.splitView_Bar
                me.items[idx].aniBar.apply(me, colorBar)
                if me.isVertical()
                {
                    let rectBar = isHot ? &rects.splitView_HorzBarHotBk : &rects.splitView_HorzBarBk
                    theme.drawSubRect(painter, rect, rectBar, me.items[idx].aniBar.cur())
                }
                else
                {
                    let rectBar = isHot ? &rects.splitView_VertBarHotBk : &rects.splitView_VertBarBk
                    theme.drawSubRect(painter, rect, rectBar, me.items[idx].aniBar.cur())
                }
            }
        }

        me.frameWnd.IWnd.onPostPaintEvent(evt)
    }
}

internal impl SplitterCtrl
{
    mtd computeLayout()
    {
        let rect = me.getClientRectPadding()
        if rect.width <= me.items.count or rect.height <= me.items.count:
            return

        let barSize = me.getThemeMetrics().splitView_BarSize

        var dispatch: f32
        if me.items.count > 0
        {
            // Compute the current total sizes of panes
            var countNotPreserve, countPreserve = 0
            var totalSize = 0'f32
            foreach item, idx in me.items
            {
                if !item.visible:
                    continue

                totalSize += item.size
                countPreserve += item.tryPreserveSize ? 1 : 0
                countNotPreserve += item.tryPreserveSize ? 0 : 1
            }

            totalSize += cast(f32) ((me.numVisiblePanes - 1) * barSize)

            // Compute the size to adapt
            if me.isVertical():
                dispatch = cast(f32) (rect.height - totalSize)
            else:
                dispatch = cast(f32) (rect.width - totalSize)

            // Dispatch the size change first to all panes not marked as 'tryPreserveSize'
            if countNotPreserve
            {
                let diff = cast(f32) Math.round(dispatch / countNotPreserve)
                foreach item, idx in me.items
                {
                    if item.tryPreserveSize:
                        continue
                    if !item.visible:
                        continue

                    countNotPreserve -= 1

                    var realDiff = countNotPreserve ? diff : dispatch
                    if item.maxSize > 0 and realDiff > 0 and item.size + realDiff > item.maxSize:
                        realDiff = item.maxSize - item.size
                    elif realDiff < 0 and item.size + realDiff < item.minSize:
                        realDiff = item.minSize - item.size

                    item.size += realDiff
                    dispatch -= realDiff
                }
            }

            // Dispatch the rest between panes that try to preserved their size
            if countPreserve
            {
                let diff = cast(f32) Math.round(dispatch / countPreserve)
                foreach item, idx in me.items
                {
                    if !item.tryPreserveSize:
                        continue
                    if !item.visible:
                        continue

                    countPreserve -= 1

                    var realDiff = countNotPreserve ? diff : dispatch
                    if item.maxSize > 0 and realDiff > 0 and item.size + realDiff > item.maxSize:
                        realDiff = item.maxSize - item.size
                    elif realDiff < 0 and item.size + realDiff < item.minSize:
                        realDiff = item.minSize - item.size

                    item.size += realDiff
                    dispatch -= realDiff
                }
            }
        }

        foreach item, idx in me.items
        {
            if !item.visible:
                continue

            me.setPaneSizePrivate(cast(u32) idx, item.size)
        }
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        let client = me.getClientRectPadding()
        let local  = me.surfaceToLocal(surfacePos)

        var x       = client.x
        var y       = client.y
        let barSize = me.getThemeMetrics().splitView_BarSize

        foreach item, idx in me.items
        {
            if !item.visible:
                continue
            var rect = client
            if me.isVertical()
            {
                rect.y      = y + item.size
                rect.height = barSize
                if rect.contains(local):
                    return cast(u32) idx
                y = rect.bottom()
            }
            else
            {
                rect.x     = x + item.size
                rect.width = barSize
                if rect.contains(local):
                    return cast(u32) idx
                x = rect.right()
            }
        }

        return Swag.U32.Max
    }

    mtd getBarRect(idx: u32)->Math.Rectangle
    {
        var result: retval = me.getClientRectPadding()

        let barSize = me.getThemeMetrics().splitView_BarSize
        for i in 0 to idx
        {
            if me.isVertical()
            {
                result.y += me.items[i].size
                if i != idx:
                    result.y += barSize
                result.height = barSize
            }
            else
            {
                result.x += me.items[i].size
                if i != idx:
                    result.x += barSize
                result.width = barSize
            }
        }

        return result
    }

    mtd setPaneSizePrivate(idx: u32, newSize: f32)
    {
        let curPane = me.items[idx]

        var reqSize = Math.max(curPane.minSize, newSize)
        if curPane.maxSize > 0:
            reqSize = Math.min(curPane.maxSize, reqSize)

        var diff = reqSize - curPane.size
        if me.items.count == 1
        {
            curPane.size += diff
            return
        }

        let scanLeft = idx == me.items.count - 1
        var i        = cast(s32) idx

        for
        {
            if scanLeft and i == 0:
                break
            if !scanLeft and i == me.items.count - 1:
                break

            let otherPane = scanLeft ? me.items[i - 1] : me.items[i + 1]
            i += scanLeft ? -1 : 1

            var realDiff = diff
            if realDiff > 0 and otherPane.size - realDiff < otherPane.minSize
            {
                realDiff       = otherPane.size - otherPane.minSize
                otherPane.size = otherPane.minSize
            }
            elif realDiff < 0 and otherPane.maxSize > 0 and otherPane.size - realDiff > otherPane.maxSize
            {
                realDiff       = otherPane.maxSize - otherPane.size
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

        let pos = position
        let res = Wnd.create'SplitterCtrl(parent, null, pos, id)
        res.focusStrategy = .None
        res.wndFlags |= .PostChildsPaint
        if vertical:
            res.splitterFlags.add(.Vertical)
        return res
    }

    #[Swag.Inline]
    mtd isVertical() => me.splitterFlags.has(.Vertical)

    // Add a new pane
    #[Swag.Discardable]
    mtd addPane(pane: *Wnd, paneSize: f32 = 100, title = false)->*SplitterItem
    {
        pane.setParent(me)
        me.numVisiblePanes += 1

        let item = me.items.newPtr()
        item.view = pane
        if title
        {
            with let label = Label.create(me, "")
            {
                .backgroundStyle = .Window
                .style.addStyleSheetColors("wnd_Bk $splitView_TitleBk")
            }

            item.label = label
        }

        me.items.add(item)

        if me.items.count == 1
        {
            let rect = me.getClientRectPadding()
            if me.isVertical():
                item.size = rect.height
            else:
                item.size = rect.width
        }
        else
        {
            me.setPaneSize(cast(u32) me.items.count - 1, paneSize)
        }

        return item
    }

    // Set the pane size (size of the first view)
    mtd setPaneSize(idx: u32, newSize: f32)
    {
        Debug.assert(idx < me.items.count)
        me.setPaneSizePrivate(idx, newSize)
        me.sendResizeEvent()
    }

    // Show or hide a given pane.
    mtd showPane(idx: u32, show: bool)
    {
        Debug.assert(idx < me.items.count)
        let pane = me.items[idx]

        if pane.visible == show:
            return

        me.numVisiblePanes += show ? 1 : -1
        pane.visible = show
        if pane.view:
            pane.view.show(show)

        if show
        {
            let mem = pane.size
            pane.size = 0
            me.setPaneSize(idx, mem)
        }

        me.sendResizeEvent()
    }

    // Returns true if the given pane is visible.
    mtd isPaneVisible(idx: u32)->bool
    {
        Debug.assert(idx < me.items.count)
        return me.items[idx].visible
    }
}
