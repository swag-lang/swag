using Core, Pixel

public enum TabBarForm
{
    Flat
    Transparent
}


public enum TabItemForm
{
    Flat
    Transparent
    Round
}

public enum TabBarLayout
{
    Divide
    Fixed
}

public struct TabItem
{
    view:   *Wnd
    icon:   Icon
    name:   RichString
    pos:    f32
    width:  f32
}

public struct Tab
{
    using wnd:              Wnd
    tabBarForm:             TabBarForm = Flat
    tabBarLayout:           TabBarLayout = Divide
    tabItemForm:            TabItemForm = Flat
    tabBarFixedLayoutWidth: f32 = 100

    views:          Array'(*TabItem)
    selectedIdx:    u32
    hotIdx:         u32
    barHeight:      f32
}

impl IWnd for Tab
{
    mtd paintItem(painter: *Painter, v: *TabItem, idx: u32)
    {
        clientRect := getClientRect()
        clientRect.height = barHeight
        labelPos := clientRect
        labelPos.width = v.width
        labelPos.x = v.pos

        theme   := getTheme()
        metrics := getThemeMetrics()
        colors  := getThemeColors()
        rects   := getThemeRects()

        // Item image
        var subRect:    *ThemeImageRect
        var colorImg:   Color
        var colorText:  Color

        switch tabItemForm
        {
        case Flat:
            if idx == selectedIdx
            {
                colorImg = colors.tabItem_FlatSelected
                subRect = &rects.tabItem_FlatSelected
                colorText = colors.tabItem_SelectedText
            }
            else
            {
                colorText = colors.tabItem_UnSelectedText
            }
        case Round:
            if idx == selectedIdx
            {
                colorImg = colors.tabItem_RoundSelected
                subRect = &rects.tabItem_RoundSelected//&rects.tabItem_FlatSelected
                colorText = colors.tabItem_SelectedText
            }
            else
            {
                colorText = colors.tabItem_UnSelectedText
            }
        }

        if subRect
            theme.drawSubRect(painter, labelPos, subRect, colorImg)

        var format: RichStringFormat
        format.palette[0] = colorText
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.flags = .Clip
        format.font = theme.res.fontDefault
        Theme.drawIconText(painter, labelPos, v.icon, &v.name, format, metrics.iconText_Margin)
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc.painter
        theme   := getTheme()
        metrics := getThemeMetrics()
        colors  := getThemeColors()
        rects   := getThemeRects()

        // Background bar
        if tabBarForm != .Transparent
        {
            clientRect := getClientRect()
            clientRect.height = barHeight
            switch tabBarForm
            {
            case Flat:
                painter.fillRect(clientRect, colors.tabBar_FlatBk)
            }
        }

        // Paint all except selected one, which will be painted last
        visit v, idx: views
        {
            if idx == selectedIdx
                continue
            paintItem(painter, v, cast(u32) idx)
        }

        // Paint selected item
        paintItem(painter, views[selectedIdx], selectedIdx)
    }

    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if views.isEmpty()
            return

        computeTabLayout()
        curView := views[selectedIdx].view
        clientRect := getClientRect()
        clientRect.moveTop(barHeight)
        clientRect.applyPadding(padding)
        curView.setPosition(clientRect)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        hit := hitTest(evt.pos)
        if hit != hotIdx
        {
            hotIdx = hit
            invalidate()
        }

        if hit == Swag.U32.Max
            return

        evt.accepted = true
        switch evt.kind
        {
        case .MousePressed:
            if evt.button == .Left
            {
                if hit != Swag.U32.Max
                {
                    select(hit)
                }
            }
        }
    }
}

impl Tab
{
    mtd opDrop()
    {
        visit v: views
            Memory.delete(v)
    }

    mtd hitTest(pos: Math.Point)->u32
    {
        if pos.y >= barHeight
            return Swag.U32.Max

        visit v, idx: views
        {
            if pos.x >= v.pos and pos.x < v.pos + v.width
                return acast idx
        }

        return Swag.U32.Max
    }

    mtd computeTabLayout()
    {
        switch tabBarLayout
        {
        case Divide:
            w := cast(f32) (position.width / views.count)
            x := 0'f32
            visit v: views
            {
                v.pos = x
                v.width = w
                x += w
            }

        case Fixed:
            w := cast(f32) tabBarFixedLayoutWidth
            x := 0'f32
            visit v: views
            {
                v.pos = x
                v.width = w
                x += w
            }
        }
    }
}

public impl Tab
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*Tab
    {
        res := Wnd.create'Tab(parent, "", position, id)
        res.barHeight = 40
        res.sendCreateEvent()
        return res
    }

    // Add a new view
    mtd addView(tabName: string, view: *Wnd, icon: Icon = @{})
    {
        if views.isEmpty()
            view.isHidden = false
        else
            view.isHidden = true
        view.setParent(self)

        item := Memory.new'TabItem()
        item.view = view
        item.name = tabName
        item.icon = icon
        views.add(item)

        sendResizeEvent()
    }

    // Select one tab at the given index
    mtd select(idx: u32)
    {
        if selectedIdx == idx or selectedIdx >= views.count
            return

        views[selectedIdx].view.isHidden = true
        selectedIdx = idx
        views[selectedIdx].view.isHidden = false
        sendResizeEvent()

        invalidate()
    }
}