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
        let local = .surfaceToLocal(evt.surfacePos)

        switch evt.kind
        {
        case MouseLeave:
            if .hotIdx != Swag.U32.Max
            {
                .hotIdx = Swag.U32.Max
                .invalidate()
            }

        case MouseMove:
            if .moving
            {
                let rect = .getBarRect(.hotIdx)
                if .isVertical()
                {
                    let diff = local.y - (rect.y + .correctMoving)
                    .setPaneSize(.hotIdx, .items[.hotIdx].size + diff)
                }
                else
                {
                    let diff = local.x - (rect.x + .correctMoving)
                    .setPaneSize(.hotIdx, .items[.hotIdx].size + diff)
                }

                .invalidate()
            }
            else
            {
                let newHot = .hitTest(evt.surfacePos)
                if newHot != .hotIdx do
                    .invalidate()
                .hotIdx = newHot
            }

        case MousePressed:
            if evt.button == .Left and .hotIdx != Swag.U32.Max
            {
                let rect = .getBarRect(.hotIdx)
                .moving = true
                .surface.app.captureMouse(me)
                if .isVertical() do
                    .correctMoving = local.y - rect.y
                else do
                    .correctMoving = local.x - rect.x
            }

        case MouseReleased:
            if .moving
            {
                .moving = false
                .surface.app.releaseMouse()
            }
        }

        if .hotIdx != Swag.U32.Max do
            .cursor = .isVertical() ? Cursor.from(CursorShape.SizeNS) : Cursor.from(CursorShape.SizeWE)
        else do
            .cursor.clear()
    }

    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        .computeLayout()

        let client  = .getClientRectPadding()
        var x       = client.x
        var y       = client.y
        let barSize = .getThemeMetrics().splitView_BarSize

        foreach item in .items
        {
            if !item.visible do
                continue

            if .isVertical()
            {
                var paneRect = Math.Rectangle{client.x, y, client.width, item.size}
                paneRect.applyPadding(item.view.margin)

                if item.label
                {
                    var labelRect = paneRect
                    labelRect.height = .titleSize
                    item.label.setPosition(labelRect)

                    paneRect.y += .titleSize
                    paneRect.height -= .titleSize
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
                    labelRect.width = .titleSize
                    item.label.setPosition(labelRect)

                    paneRect.x += .titleSize
                    paneRect.width -= .titleSize
                }

                item.view.setPosition(paneRect)
                x += item.size + barSize
            }
        }
    }

    mtd impl onPostPaintEvent(evt: *PaintEvent)
    {
        if !.items.count do
            return

        let painter = evt.bc.painter
        let theme   = .getTheme()
        let rects   = .getThemeRects()
        let colors  = .getThemeColors()

        for idx in .items.count - 1
        {
            if !.items[idx].visible do
                continue

            let rect  = .getBarRect(cast(u32) idx)
            let isHot = .hotIdx == idx
            if isHot or !.splitterFlags.has(.HideBars)
            {
                let colorBar = isHot ? colors.splitView_BarHot : colors.splitView_Bar
                .items[idx].aniBar.apply(me, colorBar)
                if .isVertical()
                {
                    let rectBar = isHot ? &rects.splitView_HorzBarHotBk : &rects.splitView_HorzBarBk
                    theme.drawSubRect(painter, rect, rectBar, .items[idx].aniBar.cur())
                }
                else
                {
                    let rectBar = isHot ? &rects.splitView_VertBarHotBk : &rects.splitView_VertBarBk
                    theme.drawSubRect(painter, rect, rectBar, .items[idx].aniBar.cur())
                }
            }
        }

        .frameWnd.IWnd.onPostPaintEvent(evt)
    }
}

internal impl SplitterCtrl
{
    mtd computeLayout()
    {
        let rect = .getClientRectPadding()
        if rect.width <= .items.count or rect.height <= .items.count do
            return

        let barSize = .getThemeMetrics().splitView_BarSize

        var dispatch: f32
        if .items.count > 0
        {
            // Compute the current total sizes of panes
            var countNotPreserve, countPreserve = 0
            var totalSize = 0'f32
            foreach item, idx in .items
            {
                if !item.visible do
                    continue

                totalSize += item.size
                countPreserve += item.tryPreserveSize ? 1 : 0
                countNotPreserve += item.tryPreserveSize ? 0 : 1
            }

            totalSize += cast(f32) ((.numVisiblePanes - 1) * barSize)

            // Compute the size to adapt
            if .isVertical() do
                dispatch = cast(f32) (rect.height - totalSize)
            else do
                dispatch = cast(f32) (rect.width - totalSize)

            // Dispatch the size change first to all panes not marked as 'tryPreserveSize'
            if countNotPreserve
            {
                let diff = cast(f32) Math.round(dispatch / countNotPreserve)
                foreach item, idx in .items
                {
                    if item.tryPreserveSize do
                        continue
                    if !item.visible do
                        continue

                    countNotPreserve -= 1

                    var realDiff = countNotPreserve ? diff : dispatch
                    if item.maxSize > 0 and realDiff > 0 and item.size + realDiff > item.maxSize do
                        realDiff = item.maxSize - item.size
                    elif realDiff < 0 and item.size + realDiff < item.minSize do
                        realDiff = item.minSize - item.size

                    item.size += realDiff
                    dispatch -= realDiff
                }
            }

            // Dispatch the rest between panes that try to preserved their size
            if countPreserve
            {
                let diff = cast(f32) Math.round(dispatch / countPreserve)
                foreach item, idx in .items
                {
                    if !item.tryPreserveSize do
                        continue
                    if !item.visible do
                        continue

                    countPreserve -= 1

                    var realDiff = countNotPreserve ? diff : dispatch
                    if item.maxSize > 0 and realDiff > 0 and item.size + realDiff > item.maxSize do
                        realDiff = item.maxSize - item.size
                    elif realDiff < 0 and item.size + realDiff < item.minSize do
                        realDiff = item.minSize - item.size

                    item.size += realDiff
                    dispatch -= realDiff
                }
            }
        }

        foreach item, idx in .items
        {
            if !item.visible do
                continue

            .setPaneSizePrivate(cast(u32) idx, item.size)
        }
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        let client = .getClientRectPadding()
        let local  = .surfaceToLocal(surfacePos)

        var x       = client.x
        var y       = client.y
        let barSize = .getThemeMetrics().splitView_BarSize

        foreach item, idx in .items
        {
            if !item.visible do
                continue
            var rect = client
            if .isVertical()
            {
                rect.y      = y + item.size
                rect.height = barSize
                if rect.contains(local) do
                    return cast(u32) idx
                y = rect.bottom()
            }
            else
            {
                rect.x     = x + item.size
                rect.width = barSize
                if rect.contains(local) do
                    return cast(u32) idx
                x = rect.right()
            }
        }

        return Swag.U32.Max
    }

    mtd getBarRect(idx: u32)->Math.Rectangle
    {
        var result: retval = .getClientRectPadding()

        let barSize = .getThemeMetrics().splitView_BarSize
        for i in 0 to idx
        {
            if .isVertical()
            {
                result.y += .items[i].size
                if i != idx do
                    result.y += barSize
                result.height = barSize
            }
            else
            {
                result.x += .items[i].size
                if i != idx do
                    result.x += barSize
                result.width = barSize
            }
        }

        return result
    }

    mtd setPaneSizePrivate(idx: u32, newSize: f32)
    {
        let curPane = .items[idx]

        var reqSize = Math.max(curPane.minSize, newSize)
        if curPane.maxSize > 0 do
            reqSize = Math.min(curPane.maxSize, reqSize)

        var diff = reqSize - curPane.size
        if .items.count == 1
        {
            curPane.size += diff
            return
        }

        let scanLeft = idx == .items.count - 1
        var i        = cast(s32) idx

        for
        {
            if scanLeft and i == 0 do
                break
            if !scanLeft and i == .items.count - 1 do
                break

            let otherPane = scanLeft ? .items[i - 1] : .items[i + 1]
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
        if vertical do
            res.splitterFlags.add(.Vertical)
        return res
    }

    #[Swag.Inline]
    mtd isVertical() => .splitterFlags.has(.Vertical)

    // Add a new pane
    #[Swag.Discardable]
    mtd addPane(pane: *Wnd, paneSize: f32 = 100, title = false)->*SplitterItem
    {
        pane.setParent(me)
        .numVisiblePanes += 1

        let item = .items.newPtr()
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

        .items.add(item)

        if .items.count == 1
        {
            let rect = .getClientRectPadding()
            if .isVertical() do
                item.size = rect.height
            else do
                item.size = rect.width
        }
        else
        {
            .setPaneSize(cast(u32) .items.count - 1, paneSize)
        }

        return item
    }

    // Set the pane size (size of the first view)
    mtd setPaneSize(idx: u32, newSize: f32)
    {
        Debug.assert(idx < .items.count)
        .setPaneSizePrivate(idx, newSize)
        .sendResizeEvent()
    }

    // Show or hide a given pane.
    mtd showPane(idx: u32, show: bool)
    {
        Debug.assert(idx < .items.count)
        let pane = .items[idx]

        if pane.visible == show do
            return

        .numVisiblePanes += show ? 1 : -1
        pane.visible = show
        if pane.view do
            pane.view.show(show)

        if show
        {
            let mem = pane.size
            pane.size = 0
            .setPaneSize(idx, mem)
        }

        .sendResizeEvent()
    }

    // Returns true if the given pane is visible.
    mtd isPaneVisible(idx: u32)->bool
    {
        Debug.assert(idx < .items.count)
        return .items[idx].visible
    }
}
