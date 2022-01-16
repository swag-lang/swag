#global public
using Core, Pixel

enum TabForm
{
    Flat
    Round
}

enum TabBarForm
{
    Transparent
    Flat
}


enum TabItemForm
{
    Transparent
    Flat
    Round
}

enum TabViewForm
{
    Flat
    Round
}

enum TabBarLayout
{
    Divide
    Fixed
}

struct TabItem
{
    name:   RichString
    icon:   Icon
    pos:    f32
    width:  f32
    view:   *Wnd
}

struct Tab
{
    using wnd:           Wnd
    barForm:             TabBarForm = Flat
    itemForm:            TabItemForm = Flat
    viewForm:            TabViewForm = Flat
    barLayout:           TabBarLayout = Divide
    barFixedLayoutWidth: f32 = 100

    views:          Array'(*TabItem)
    selectedIdx:    u32
    hotIdx:         u32 = Swag.U32.Max
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

        switch itemForm
        {
        case Flat:
            if idx == selectedIdx
            {
                subRect = &rects.tabItem_FlatSelected
                colorImg = colors.tabItem_FlatSelected
                colorText = colors.tabItem_SelectedText
            }
            elif idx == hotIdx
            {
                subRect = &rects.tabItem_FlatSelected
                colorImg = colors.tabItem_FlatHot
                colorText = colors.tabItem_SelectedText
            }
            else
            {
                colorText = colors.tabItem_UnSelectedText
            }
        case Round:
            if idx == selectedIdx
            {
                subRect = &rects.tabItem_RoundSelected
                colorImg = colors.tabItem_RoundSelected
                colorText = colors.tabItem_SelectedText
            }
            elif idx == hotIdx
            {
                subRect = &rects.tabItem_RoundHot
                colorImg = colors.tabItem_RoundHot
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
        theme.setTextColors(&format, colorText)
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
        if barForm != .Transparent
        {
            clientRect := getClientRect()
            clientRect.height = barHeight
            switch barForm
            {
            case Flat:
                painter.fillRect(clientRect, colors.tabBar_FlatBk)
            }
        }

        // Paint border and back
        var subRect:    *ThemeImageRect
        var colorBk:    Color

        clientRect := getClientRect()
        clientRect.moveTop(barHeight)
        switch viewForm
        {
        case Flat:
            subRect = &rects.tabView_FlatBk
            colorBk = colors.tabView_FlatBk
        case Round:
            subRect = &rects.tabView_RoundBk
            colorBk = colors.tabView_RoundBk
        }

        theme.drawSubRect(painter, clientRect, subRect, colorBk)

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
        hit := hitTest(evt.surfacePos)
        if hit != hotIdx
        {
            hotIdx = hit
            invalidate()
        }

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

private impl Tab
{
    mtd hitTest(surfacePos: Math.Point)->u32
    {
        pos := surfaceToLocal(surfacePos)
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
        switch barLayout
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
            w := cast(f32) barFixedLayoutWidth
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

impl Tab
{
    mtd opDrop()
    {
        visit v: views
            Memory.delete(v)
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*Tab
    {
        res := Wnd.create'Tab(parent, "", position, id)
        res.setForm(TabForm.Flat)
        return res
    }

    // Set the tab form
    mtd setForm(tabForm: TabForm)
    {
        switch tabForm
        {
        case Flat:
            barForm = .Flat
            itemForm = .Flat
            viewForm = .Flat
            barHeight = 40
            barLayout = .Divide

        case Round:
            barForm = .Transparent
            itemForm = .Round
            viewForm = .Round
            barHeight = 28
            barLayout = .Fixed
        }
    }

    // Add a new view
    mtd addItem(tabName: string, icon: Icon, view: *Wnd)
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