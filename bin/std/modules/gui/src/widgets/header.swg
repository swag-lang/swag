#global marked
#global public
using Core, Pixel

enum HeaderForm
{
    Transparent
    Flat
    Round
}

struct HeaderItem
{
    name:          RichString
    icon:          Icon
    width:         f32
    minWidth:      f32
    maxWidth:      f32
    sizeable       = true
    clickable      = true
    iconMargin     = false
    column:        u32
}

#[Swag.EnumFlags]
enum HeaderFlags
{
    Zero      = 0
    Clickable
}

struct Header
{
    using wnd:            Wnd

    form                  = HeaderForm.Flat
    headerFlags           = HeaderFlags.Clickable
    marginItems:          f32 = 5
    sigLayoutChanged:     func||(*Header)
    sigClicked:           func||(*Header, u32)

    items:                ArrayPtr'HeaderItem
    hotIdx                = Swag.U32.Max
    hotSeparator:         bool
    moving:               bool
    clicked:              bool
    correctMoving:        f32
    sortColumn:           u32
    sortMark:             s32
}

impl IWnd for Header
{
    mtd paintItem(bc: PaintContext, item: *HeaderItem, idx: u32, x: f32)
    {
        let painter = bc.painter

        var clientRect = me.getClientRect()
        var labelPos   = clientRect
        labelPos.x     = x
        labelPos.width = item.width

        let theme   = me.getTheme()
        let metrics = me.getThemeMetrics()
        let colors  = me.getThemeColors()

        if me.hotIdx == idx and !me.hotSeparator
        {
            var labelPosBk = labelPos
            labelPosBk.x -= (metrics.header_Separator * 0.5)
            labelPosBk.width += metrics.header_Separator
            painter.fillRect(labelPosBk, me.clicked ? colors.headerItem_ClickedBk : colors.headerItem_HotBk)
        }

        labelPos.moveLeft(me.marginItems)

        var colorText: Color
        switch
        {
        case bc.isDisabled:
            colorText = colors.headerItem_DisabledText
        case idx == me.hotIdx:
            colorText = colors.headerItem_HotText
        default:
            colorText = colors.headerItem_Text
        }

        // Sort mark
        if me.sortMark != 0 and me.sortColumn == idx
        {
            var icon: Icon = undefined
            if me.sortMark == 1:
                icon = theme.getIcon24(ThemeIcons24.SortArrowDown)
            else:
                icon = theme.getIcon24(ThemeIcons24.SortArrowUp)
            if labelPos.width > icon.sizeX
            {
                var sortPos = labelPos
                sortPos.x = Math.max(sortPos.x, sortPos.right() - icon.sizeX)
                sortPos.y = sortPos.vertCenter() - (icon.sizeY * 0.5)
                icon.paint(painter, sortPos.x, sortPos.y, colorText)
                labelPos.width -= icon.sizeX
            }
        }

        var format: RichStringFormat
        theme.setTextColors(&format, colorText)
        format.horzAlignment = .Left
        format.vertAlignment = .Center
        format.flags         = .Clip
        format.font          = me.getFont()
        Theme.drawIconText(painter, labelPos, item.icon, &item.name, format, metrics.iconText_Margin)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter = evt.bc.painter
        let theme   = me.getTheme()
        let metrics = me.getThemeMetrics()
        let colors  = me.getThemeColors()
        let rects   = me.getThemeRects()

        // Background bar
        var clientRect = me.getClientRect()
        clientRect.offset(me.scrollPos)
        switch me.form
        {
        case Flat:
            theme.drawSubRect(painter, clientRect, &rects.header_FlatBk, colors.header_Bk)
        case Round:
            theme.drawSubRect(painter, clientRect, &rects.header_RoundBk, colors.header_Bk)
        case Transparent:
            painter.drawLine(clientRect.x, clientRect.bottom() - 1, clientRect.right(), clientRect.bottom() - 1, colors.headerItem_Separator, 1)
        }

        // Clipping, take care of padding
        var clipRect = me.getSurfaceRect()
        clipRect.applyPadding(me.padding)
        painter.pushClippingSurfaceRect(clipRect)
        defer painter.popClippingRect()

        // Paint all except selected one, which will be painted last
        clientRect = me.getClientRectPadding()
        clientRect.offset(me.scrollPos)
        var x = me.padding.x
        foreach item, idx in me.items
        {
            me.paintItem(evt.bc, item, cast(u32) idx, x)
            x += item.width

            var rect = clientRect
            rect.x = x + (metrics.header_Separator * 0.5)
            painter.drawLine(rect.x, rect.y, rect.x, rect.bottom(), colors.headerItem_Separator, 1)

            x += metrics.header_Separator
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MouseReleased and evt.button == .Left
        {
            if me.clicked and me.sigClicked
            {
                let (hit, ?) = me.hitTest(evt.surfacePos)
                if hit == me.hotIdx:
                    me.sigClicked(me, me.items[me.hotIdx].column)
            }

            if me.moving or me.clicked
            {
                me.moving, me.clicked = false
                me.releaseMouse()
                me.invalidate()
            }
        }

        if !me.moving and !me.clicked
        {
            let (hitr, sep) = me.hitTest(evt.surfacePos)
            var hit = hitr

            // Hot, if not a separator, is disabled if item is not clickable
            if hit != Swag.U32.Max
            {
                if !me.headerFlags.has(.Clickable) and !sep:
                    hit = Swag.U32.Max
                elif !me.items[hit].clickable:
                    hit = Swag.U32.Max
            }

            if hit != me.hotIdx
            {
                me.hotIdx = hit
                me.invalidate()
            }

            if me.hotSeparator != sep
            {
                me.hotSeparator = sep
                me.invalidate()
            }

            if me.hotSeparator:
                me.cursor = Cursor.from(CursorShape.SizeWE)
            else:
                me.cursor.clear()
        }

        if me.hotIdx == Swag.U32.Max
        {
            evt.accepted = false
            return
        }

        switch evt.kind
        {
        case MouseMove:
            if me.moving
            {
                var pos = me.surfaceToLocal(evt.surfacePos)
                pos += me.scrollPos.x
                var move = pos.x - me.getItemPosition(me.hotIdx) + me.padding.x
                move -= me.correctMoving
                me.setItemWidth(me.hotIdx, move)
            }

        case MouseLeave:
            if me.hotIdx != Swag.U32.Max
            {
                me.hotIdx = Swag.U32.Max
                me.invalidate()
            }

        case MousePressed:
            if evt.button == .Left and me.hotIdx != Swag.U32.Max
            {
                if me.hotSeparator
                {
                    me.moving = true
                    var pos = me.surfaceToLocal(evt.surfacePos)
                    pos += me.scrollPos.x
                    me.correctMoving = pos.x - me.getItemPosition(me.hotIdx) + me.padding.x
                    me.correctMoving -= me.getItemWidth(me.hotIdx)
                    me.captureMouse()
                }
                elif me.items[me.hotIdx].clickable
                {
                    me.clicked = true
                    me.captureMouse()
                    me.invalidate()
                }
            }
        }
    }
}

internal impl Header
{
    mtd hitTest(surfacePos: Math.Point)->{ idx: u32, sep: bool }
    {
        let metrics = me.getThemeMetrics()
        var pos     = me.surfaceToLocal(surfacePos)
        pos += me.scrollPos.x
        var x = me.padding.x
        foreach item, idx in me.items
        {
            if pos.x >= x and pos.x < x + item.width:
                return {cast(u32) idx, false}
            x += item.width
            if pos.x >= x and pos.x <= x + metrics.header_Separator and item.sizeable:
                return {cast(u32) idx, true}
            x += metrics.header_Separator
        }

        return {Swag.U32.Max, false}
    }
}

impl Header
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*Header
    {
        Debug.assert(parent != null)

        let metrics = parent.getThemeMetrics()
        var pos     = position
        if pos.height == 0:
            pos.height = metrics.header_Height
        let res = Wnd.create'Header(parent, null, pos, id)
        res.padding       = metrics.header_Padding
        res.focusStrategy = .None
        return res
    }

    // Add a new view
    #[Swag.Discardable]
    mtd addItem(tabName: string, width = 100'f32, icon: Icon = {})->*HeaderItem
    {
        let item = me.items.newPtr()
        item.name   = tabName
        item.icon   = icon
        item.width  = width
        item.column = cast() me.items.count
        me.items.add(item)
        return item
    }

    // Returns the given item, by index
    mtd getItem(idx: u32)->*HeaderItem
    {
        Debug.assert(idx < me.items.count)
        return me.items[idx]
    }

    // Returns the given item, by column
    mtd getItemByCol(column: u32)->*HeaderItem
    {
        Debug.assert(column < me.items.count)

        foreach item in me.items
        {
            if item.column == column:
                return item
        }

        unreachable
    }

    // Get an item width by index
    mtd getItemWidth(idx: u32)->f32
    {
        Debug.assert(idx < me.items.count)
        return me.getItemWidthByCol(me.items[idx].column)
    }

    // Get an item width by column
    mtd getItemWidthByCol(column: u32)->f32
    {
        Debug.assert(column < me.items.count)

        foreach item in me.items
        {
            if item.column == column:
                return item.width
        }

        return 0
    }

    // Get an item position by index
    mtd getItemPosition(idx: u32)->f32
    {
        Debug.assert(idx < cast(u32) me.items.count)
        return me.getItemPositionByCol(me.items[idx].column)
    }

    // Get an item position by column
    mtd getItemPositionByCol(column: u32)->f32
    {
        Debug.assert(column < me.items.count)

        var x = me.padding.x
        let m = me.getThemeMetrics().header_Separator
        foreach item, i in me.items
        {
            if item.column == column:
                return x
            x += item.width + m
        }

        return 0
    }

    // Get the header total width
    mtd getTotalItemsWidth()->f32
    {
        var w = me.getItemPosition(cast(u32) me.items.count - 1)
        w += me.items.back().width
        w += me.getThemeMetrics().header_Separator
        return w
    }

    // Set an item width by index
    mtd setItemWidth(idx: u32, width: f32)
    {
        Debug.assert(idx < me.items.count)
        me.setItemWidthByCol(me.items[idx].column, width)
    }

    // Set an item width by column
    mtd setItemWidthByCol(column: u32, width: f32)
    {
        Debug.assert(column < me.items.count)

        let item = me.getItemByCol(column)
        var move = Math.max(width, item.minWidth)
        if item.maxWidth > 0:
            move = Math.min(move, item.maxWidth)
        if item.width == move:
            return
        item.width = move
        if me.sigLayoutChanged:
            me.sigLayoutChanged(me)
        me.invalidate()
    }
}
