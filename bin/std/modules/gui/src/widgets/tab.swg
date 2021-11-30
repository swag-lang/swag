using Core, Pixel

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
    using wnd:      Wnd

    views:          Array'(*TabItem)
    selectedIdx:    u32
    barHeight:      f32
}

impl IWnd for Tab
{
    mtd paintItem(painter: *Painter, v: *TabItem)
    {
        clientRect := getClientRect()
        clientRect.height = barHeight

        theme   := getTheme()
        metrics := getThemeMetrics()

        var format: RichStringFormat
        format.palette[0] = Argb.White
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.flags = .Clip
        format.font = theme.res.fontDefault
        labelPos := clientRect
        labelPos.width = v.width
        labelPos.x = v.pos
        Theme.drawIconText(painter, labelPos, v.icon, &v.name, format, metrics.iconText_Margin)
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        // Paint all except selected one, which will be painted last
        visit v, idx: views
        {
            if idx == selectedIdx
                continue
            paintItem(evt.bc.painter, v)
        }

        // Paint selected item
        paintItem(evt.bc.painter, views[selectedIdx])
    }

    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if views.isEmpty()
            return

        computeTabLayout()
        curView := views[selectedIdx].view
        clientRect := getClientRect()
        clientRect.moveTop(barHeight)
        curView.setPosition(clientRect)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = true
        if evt.kind == .MousePressed and evt.button == .Left
        {
            hit := hitTest(evt.pos)
            if hit != Swag.U32.Max
            {
                select(hit)
            }
        }
    }
}

impl Tab
{
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
        w := cast(f32) (position.width / views.count)
        x := 0'f32

        visit v: views
        {
            v.pos = x
            v.width = w
            x += w
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