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
        res := Wnd.create'PopupMenu(parent.surface.wnd.view, null, @{}, id)
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

    mtd display(surfacePos: Math.Point)
    {
        pt := parent.surfaceToLocal(surfacePos)
        position.x = pt.x
        position.y = pt.y
        position.width = 100
        position.height = 100

        invalidate()
        show()
        discard surface.app.doModalLoop(surface)
    }
}