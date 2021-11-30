using Core, Pixel

public struct TabItem
{
    view:   *Wnd
    name:   String
    pos:    f32
    width:  f32
}

public struct Tab
{
    using wnd:      Wnd

    views:          Array'TabItem
    selectedIdx:    s32
    barHeight:      f32 = 30
}

impl IWnd for Tab
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc.painter
        visit *v: views
        {
            painter.fillRect(@{v.pos, 0, v.width, barHeight}, Argb.Red)
        }
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
    }
}

impl Tab
{
    mtd computeTabLayout()
    {
        w := cast(f32) (position.width / views.count)
        x := 0'f32

        visit *v: views
        {
            v.pos = x
            v.width = w
            x += w
        }
    }
}

public impl Tab
{
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*Tab
    {
        res := Wnd.create'Tab(parent, "", position, id)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }

    // Add a new view
    mtd addView(tabName: string, view: *Wnd)
    {
        if views.isEmpty()
            view.isHidden = false
        else
            view.isHidden = true
        view.setParent(self)

        var item: TabItem
        item.view = view
        item.name = tabName
        views.add(item)

        sendResizeEvent()
    }
}