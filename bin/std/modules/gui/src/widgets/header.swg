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

        let clientRect = .getClientRect()
        var labelPos   = clientRect
        labelPos.x     = x
        labelPos.width = item.width

        let theme   = .getTheme()
        let metrics = .getThemeMetrics()
        let colors  = .getThemeColors()

        if .hotIdx == idx and !.hotSeparator
        {
            var labelPosBk = labelPos
            labelPosBk.x -= (metrics.header_Separator * 0.5)
            labelPosBk.width += metrics.header_Separator
            painter.fillRect(labelPosBk, .clicked ? colors.headerItem_ClickedBk : colors.headerItem_HotBk)
        }

        labelPos.moveLeft(.marginItems)

        var colorText: Color
        switch
        {
        case bc.isDisabled:
            colorText = colors.headerItem_DisabledText
        case idx == .hotIdx:
            colorText = colors.headerItem_HotText
        default:
            colorText = colors.headerItem_Text
        }

        // Sort mark
        if .sortMark != 0 and .sortColumn == idx
        {
            var icon: Icon = undefined
            if .sortMark == 1 do
                icon = theme.getIcon24(ThemeIcons24.SortArrowDown)
            else do
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
        format.font          = .getFont()
        Theme.drawIconText(painter, labelPos, item.icon, &item.name, format, metrics.iconText_Margin)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter = evt.bc.painter
        let theme   = .getTheme()
        let metrics = .getThemeMetrics()
        let colors  = .getThemeColors()
        let rects   = .getThemeRects()

        // Background bar
        var clientRect = .getClientRect()
        clientRect.offset(.scrollPos)
        switch .form
        {
        case Flat:
            theme.drawSubRect(painter, clientRect, &rects.header_FlatBk, colors.header_Bk)
        case Round:
            theme.drawSubRect(painter, clientRect, &rects.header_RoundBk, colors.header_Bk)
        case Transparent:
            painter.drawLine(clientRect.x, clientRect.bottom() - 1, clientRect.right(), clientRect.bottom() - 1, colors.headerItem_Separator, 1)
        }

        // Clipping, take care of padding
        var clipRect = .getSurfaceRect()
        clipRect.applyPadding(.padding)
        painter.pushClippingSurfaceRect(clipRect)
        defer painter.popClippingRect()

        // Paint all except selected one, which will be painted last
        clientRect = .getClientRectPadding()
        clientRect.offset(.scrollPos)
        var x = .padding.x
        foreach item, idx in .items
        {
            .paintItem(evt.bc, item, cast(u32) idx, x)
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
            if .clicked and .sigClicked
            {
                let (hit, ?) = .hitTest(evt.surfacePos)
                if hit == .hotIdx do
                    .sigClicked(me, .items[.hotIdx].column)
            }

            if .moving or .clicked
            {
                .moving, .clicked = false
                .releaseMouse()
                .invalidate()
            }
        }

        if !.moving and !.clicked
        {
            let (hitr, sep) = .hitTest(evt.surfacePos)
            var hit = hitr

            // Hot, if not a separator, is disabled if item is not clickable
            if hit != Swag.U32.Max
            {
                if !.headerFlags.has(.Clickable) and !sep do
                    hit = Swag.U32.Max
                elif !.items[hit].clickable do
                    hit = Swag.U32.Max
            }

            if hit != .hotIdx
            {
                .hotIdx = hit
                .invalidate()
            }

            if .hotSeparator != sep
            {
                .hotSeparator = sep
                .invalidate()
            }

            if .hotSeparator do
                .cursor = Cursor.from(CursorShape.SizeWE)
            else do
                .cursor.clear()
        }

        if .hotIdx == Swag.U32.Max
        {
            evt.accepted = false
            return
        }

        switch evt.kind
        {
        case MouseMove:
            if .moving
            {
                var pos = .surfaceToLocal(evt.surfacePos)
                pos += .scrollPos.x
                var move = pos.x - .getItemPosition(.hotIdx) + .padding.x
                move -= .correctMoving
                .setItemWidth(.hotIdx, move)
            }

        case MouseLeave:
            if .hotIdx != Swag.U32.Max
            {
                .hotIdx = Swag.U32.Max
                .invalidate()
            }

        case MousePressed:
            if evt.button == .Left and .hotIdx != Swag.U32.Max
            {
                if .hotSeparator
                {
                    .moving = true
                    var pos = .surfaceToLocal(evt.surfacePos)
                    pos += .scrollPos.x
                    .correctMoving = pos.x - .getItemPosition(.hotIdx) + .padding.x
                    .correctMoving -= .getItemWidth(.hotIdx)
                    .captureMouse()
                }
                elif .items[.hotIdx].clickable
                {
                    .clicked = true
                    .captureMouse()
                    .invalidate()
                }
            }
        }
    }
}

internal impl Header
{
    mtd hitTest(surfacePos: Math.Point)->{ idx: u32, sep: bool }
    {
        let metrics = .getThemeMetrics()
        var pos     = .surfaceToLocal(surfacePos)
        pos += .scrollPos.x
        var x = .padding.x
        foreach item, idx in .items
        {
            if pos.x >= x and pos.x < x + item.width do
                return {cast(u32) idx, false}
            x += item.width
            if pos.x >= x and pos.x <= x + metrics.header_Separator and item.sizeable do
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
        if pos.height == 0 do
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
        let item = .items.newPtr()
        item.name   = tabName
        item.icon   = icon
        item.width  = width
        item.column = cast() .items.count
        .items.add(item)
        return item
    }

    // Returns the given item, by index
    mtd getItem(idx: u32)->*HeaderItem
    {
        Debug.assert(idx < .items.count)
        return .items[idx]
    }

    // Returns the given item, by column
    mtd getItemByCol(column: u32)->*HeaderItem
    {
        Debug.assert(column < .items.count)

        foreach item in .items
        {
            if item.column == column do
                return item
        }

        unreachable
    }

    // Get an item width by index
    mtd getItemWidth(idx: u32)->f32
    {
        Debug.assert(idx < .items.count)
        return .getItemWidthByCol(.items[idx].column)
    }

    // Get an item width by column
    mtd getItemWidthByCol(column: u32)->f32
    {
        Debug.assert(column < .items.count)

        foreach item in .items
        {
            if item.column == column do
                return item.width
        }

        return 0
    }

    // Get an item position by index
    mtd getItemPosition(idx: u32)->f32
    {
        Debug.assert(idx < cast(u32) .items.count)
        return .getItemPositionByCol(.items[idx].column)
    }

    // Get an item position by column
    mtd getItemPositionByCol(column: u32)->f32
    {
        Debug.assert(column < .items.count)

        var x = .padding.x
        let m = .getThemeMetrics().header_Separator
        foreach item, i in .items
        {
            if item.column == column do
                return x
            x += item.width + m
        }

        return 0
    }

    // Get the header total width
    mtd getTotalItemsWidth()->f32
    {
        var w = .getItemPosition(cast(u32) .items.count - 1)
        w += .items.back().width
        w += .getThemeMetrics().header_Separator
        return w
    }

    // Set an item width by index
    mtd setItemWidth(idx: u32, width: f32)
    {
        Debug.assert(idx < .items.count)
        .setItemWidthByCol(.items[idx].column, width)
    }

    // Set an item width by column
    mtd setItemWidthByCol(column: u32, width: f32)
    {
        Debug.assert(column < .items.count)

        let item = .getItemByCol(column)
        var move = Math.max(width, item.minWidth)
        if item.maxWidth > 0 do
            move = Math.min(move, item.maxWidth)
        if item.width == move do
            return
        item.width = move
        if .sigLayoutChanged do
            .sigLayoutChanged(me)
        .invalidate()
    }
}
