using Core, Pixel

public struct PopupMenuItem
{
    name:   RichString
    icon:   Icon
    pos:    f32
    height: f32
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
        painter := evt.bc.painter
        theme   := getTheme()
        colors  := getThemeColors()
        rects   := getThemeRects()
        metrics := getThemeMetrics()

        clientRect := getClientRect()
        theme.drawSubRect(painter, clientRect, &rects.popupMenu_Bk, colors.popupMenu_Bk)
        theme.drawSubRect(painter, clientRect, &rects.popupMenu_Border, colors.popupMenu_Border)

        var labelPos: Math.Rectangle
        y := padding.y
        visit i: items
        {
            var format: RichStringFormat
            format.palette[0] = colors.popupItem_Text
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault
            labelPos.x = padding.x
            labelPos.y = i.pos
            labelPos.width = position.width - padding.x - padding.z
            labelPos.height = i.height
            Theme.drawIconText(painter, labelPos, i.icon, &i.name, format, metrics.iconText_Margin)
            y += labelPos.height
        }
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

    mtd computeLayout()
    {
        theme   := getTheme()
        metrics := getThemeMetrics()

        position.width = 0
        position.height = 0
        visit i: items
        {
            var fake: Pixel.Painter{fake: true}
            var rsf:  Pixel.RichStringFormat
            rsf.font = surface.app.theme.res.fontDefault
            fake.drawRichString(@{0, 0, 512, 0}, &i.name, rsf)

            i.pos    = position.height + padding.y
            i.height = i.name.boundRect.height

            w := i.name.boundRect.width
            if i.icon.isValid()
            {
                w += i.icon.size
                w += metrics.iconText_Margin
            }

            position.width = Math.max(position.width, w)
            position.height += i.height + metrics.popupMenu_SpaceLines
        }

        position.width  += padding.x + padding.z
        position.height += padding.y + padding.w
    }
}

public impl PopupMenu
{
    #[Swag.Discardable]
    func create(parent: *Wnd, id: string = null)->*PopupMenu
    {
        res := Wnd.create'PopupMenu(&parent.surface.wnd, null, @{}, id)
        res.sendCreateEvent()
        res.padding = res.getThemeMetrics().popupMenu_Padding
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
        computeLayout()
        pt := parent.surfaceToLocal(surfacePos)
        setPosition(@{pt.x, pt.y, position.width, position.height})
        show()
        captureMouse()
    }
}