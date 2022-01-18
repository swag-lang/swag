#global public
using Core, Pixel

enum HeaderForm
{
    Transparent
    Flat
    FlatRound
}

struct HeaderItem
{
    name:   RichString
    icon:   Icon
    width:  f32
}

struct Header
{
    using wnd:  Wnd
    form:       HeaderForm = Flat

    items:          Array'(*HeaderItem)
    hotIdx:         u32 = Swag.U32.Max
    hotSeparator:   bool
}

impl IWnd for Header
{
    mtd paintItem(bc: PaintContext, item: *HeaderItem, idx: u32, x: f32)
    {
        painter := bc.painter

        clientRect := getClientRect()
        labelPos := clientRect
        labelPos.width = item.width
        labelPos.x = x

        theme   := getTheme()
        metrics := getThemeMetrics()
        colors  := getThemeColors()
        rects   := getThemeRects()

        var colorText: Color
        switch
        {
        case bc.isDisabled:
            colorText = colors.headerItem_DisabledText
        case idx == hotIdx:
            colorText = colors.headerItem_HotText
        default:
            colorText = colors.headerItem_Text
        }

        var format: RichStringFormat
        theme.setTextColors(&format, colorText)
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.flags = .Clip
        format.font = theme.res.fontDefault
        Theme.drawIconText(painter, labelPos, item.icon, &item.name, format, metrics.iconText_Margin)
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc.painter
        theme   := getTheme()
        metrics := getThemeMetrics()
        colors  := getThemeColors()
        rects   := getThemeRects()

        // Background bar
        if form != .Transparent
        {
            clientRect := getClientRect()
            switch form
            {
            case Flat:
                painter.fillRect(clientRect, colors.header_Bk)
            case FlatRound:
                theme.drawSubRect(painter, clientRect, &rects.header_FlatRoundBk, colors.header_Bk)
            }
        }

        // Paint all except selected one, which will be painted last
        x := padding.x
        visit item, idx: items
        {
            paintItem(evt.bc, item, cast(u32) idx, x)
            x += item.width + metrics.header_Separator
        }
    }

   mtd onMouseEvent(evt: *MouseEvent)
    {
        (hit, sep) := hitTest(evt.surfacePos)
        if hit != hotIdx
        {
            hotIdx = hit
            invalidate()
        }

        if hotSeparator != sep
        {
            hotSeparator = sep
            invalidate()
        }

        if hotSeparator
            cursor = Cursor.from(CursorShape.SizeWE)
        else
            cursor.clear()

        if hit == Swag.U32.Max
        {
            evt.accepted = false
            return
        }

        switch evt.kind
        {
        case .MouseLeave:
            if hotIdx != Swag.U32.Max
            {
                hotIdx = Swag.U32.Max
                invalidate()
            }

        case .MousePressed:
            if evt.button == .Left and hit != Swag.U32.Max
            {
                if hotSeparator
                {
                }
            }
        }
    }
}

private impl Header
{
    mtd hitTest(surfacePos: Math.Point)->{idx: u32, sep: bool}
    {
        metrics := getThemeMetrics()
        pos := surfaceToLocal(surfacePos)
        x := padding.x
        visit item, idx: items
        {
            if pos.x >= x and pos.x < x + item.width
                return @{cast(u32) idx, false}
            x += item.width
            if pos.x >= x and pos.x <= x + metrics.header_Separator
                return @{cast(u32) idx, true}
            x += metrics.header_Separator
        }

        return @{Swag.U32.Max, false}
    }
}

impl Header
{
    mtd opDrop()
    {
        visit v: items
            Memory.delete(v)
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*Header
    {
        Debug.assert(parent != null)
        pos := position
        if pos.height == 0
            pos.height = parent.getThemeMetrics().header_Height
        res := Wnd.create'Header(parent, null, pos, id)
        res.padding = parent.getThemeMetrics().header_Padding
        res.focusStrategy = .None
        return res
    }

    // Add a new view
    mtd addItem(tabName: string, width = 50, icon: Icon = @{})
    {
        item := Memory.new'HeaderItem()
        item.name = tabName
        item.icon = icon
        item.width = width
        items.add(item)
    }
}