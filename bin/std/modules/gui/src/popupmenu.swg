using Core, Pixel

public struct PopupMenuItem
{
    name:   RichString
    icon:   Icon
}

public struct PopupMenu
{
    using wnd:  Wnd
    items:      Array'(*PopupMenuItem)
}

impl IWnd for PopupMenu
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        colors := getThemeColors()

        clientRect := getClientRect()
        evt.bc.painter.fillRect(clientRect, colors.popupMenu_Bk)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = true
        if evt.kind == .MousePressed
        {
            destroy()
        }
    }
}

impl PopupMenu
{
    mtd opDrop()
    {
        visit i: items
            Memory.delete(i)
    }
}

public impl PopupMenu
{
    #[Swag.Discardable]
    func create(parent: *Wnd, id: string = null)->*PopupMenu
    {
        res := Wnd.create'PopupMenu(&parent.surface.wnd, null, @{}, id)
        res.sendCreateEvent()
        res.isHidden = true
        return res
    }

    mtd addItem(itemName: string, icon: Icon)
    {
        item := Memory.new'PopupMenuItem()
        item.name = itemName
        item.icon = icon
        items.add(item)
    }

    mtd doModal(surfacePos: Math.Point)
    {
        pt := parent.surfaceToLocal(surfacePos)
        setPosition(@{pt.x, pt.y, 100, 100})
        show()
        captureMouse()
    }
}