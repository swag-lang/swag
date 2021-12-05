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

        subRect := &theme.rects.popupMenu_Shadow
        painter.colorMask(true, true, true, true)
        theme.drawSubRect(painter, @{0, 0, position.width, position.height}, subRect, colors.popupMenu_Shadow)

        clientRect := getClientRect()
        clientRect.inflate(-metrics.popupMenu_ShadowSize)
        theme.drawSubRect(painter, clientRect, &rects.popupMenu_Bk, colors.popupMenu_Bk)
        theme.drawSubRect(painter, clientRect, &rects.popupMenu_Border, colors.popupMenu_Border)
        clientRect.applyPadding(padding)

        var labelPos: Math.Rectangle
        visit i: items
        {
            var format: RichStringFormat
            format.palette[0] = colors.popupItem_Text
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault
            labelPos.x = clientRect.x
            labelPos.y = i.pos
            labelPos.width = clientRect.width
            labelPos.height = i.height
            Theme.drawIconText(painter, labelPos, i.icon, &i.name, format, metrics.iconText_Margin)
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

        y := padding.y + metrics.popupMenu_ShadowSize

        position.width = 0
        position.height = 0

        visit i: items
        {
            var fake: Pixel.Painter{fake: true}
            var rsf:  Pixel.RichStringFormat
            rsf.font = surface.app.theme.res.fontDefault
            fake.drawRichString(@{0, 0, 512, 0}, &i.name, rsf)

            i.pos    = y
            i.height = i.name.boundRect.height

            w := i.name.boundRect.width
            if i.icon.isValid()
            {
                w += i.icon.size
                w += metrics.iconText_Margin
            }

            position.width = Math.max(position.width, w)
            position.height += i.height + metrics.popupMenu_SpaceLines
            y += i.height + metrics.popupMenu_SpaceLines
        }

        // Add shadow size
        position.width  += 2 * metrics.popupMenu_ShadowSize
        position.height += 2 * metrics.popupMenu_ShadowSize

        // Add padding
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
        pt -= getThemeMetrics().popupMenu_ShadowSize
        setPosition(@{pt.x, pt.y, position.width, position.height})
        show()
        captureMouse()
    }
}