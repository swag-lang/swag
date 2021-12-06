using Core, Pixel

public struct PopupMenuItem
{
    name:   RichString
    icon:   Icon
    pos:    f32
    height: f32
    id:     WndId
}

public struct PopupMenu
{
    using wnd:      Wnd
    items:          Array'(*PopupMenuItem)
    hotIdx:         u32 = Swag.U32.Max
    endModal:       bool
    endModalResult: WndId
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

        // Shadow
        subRect := &theme.rects.popupMenu_Shadow
        theme.drawSubRect(painter, @{0, 0, position.width, position.height}, subRect, colors.popupMenu_Shadow)

        // Back
        clientRect := getClientRect()
        clientRect.inflate(-metrics.popupMenu_ShadowSize)
        theme.drawSubRect(painter, clientRect, &rects.popupMenu_Bk, colors.popupMenu_Bk)
        theme.drawSubRect(painter, clientRect, &rects.popupMenu_Border, colors.popupMenu_Border)
        clientRect.applyPadding(padding)

        var labelPos: Math.Rectangle
        visit i, idx: items
        {
            labelPos.x = clientRect.x
            labelPos.y = i.pos
            labelPos.width = clientRect.width
            labelPos.height = i.height

            var format: RichStringFormat
            format.palette[0] = colors.popupItem_Text

            if hotIdx == idx
            {
                backPos := labelPos
                backPos.x = metrics.popupMenu_ShadowSize
                backPos.y -= metrics.popupMenu_SpaceLines * 0.5
                backPos.width = position.width - 2 * metrics.popupMenu_ShadowSize
                backPos.height += metrics.popupMenu_SpaceLines
                painter.fillRect(backPos, colors.popupItem_HotBk)
                format.palette[0] = colors.popupItem_HotText
            }

            // Icon
            if i.icon.isValid()
            {
                i.icon.size = i.height
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.size * 0.5)
            }

            labelPos.moveLeft(i.height + metrics.popupMenu_IconTextMargin)

            // Text
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault
            painter.drawRichString(labelPos, &i.name, format)
        }

        // Border
        clientRect = getClientRect()
        clientRect.inflate(-metrics.popupMenu_ShadowSize)
        theme.drawSubRect(painter, clientRect, &rects.popupMenu_Border, colors.popupMenu_Border)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        hit := hitTest(evt.pos)
        if hit != hotIdx
        {
            hotIdx = hit
            invalidate()
        }

        evt.accepted = true
        if evt.kind == .MousePressed
        {
            endModal = true
            if hotIdx != Swag.U32.Max
                endModalResult = items[hotIdx].id
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

    mtd hitTest(pos: Math.Point)->u32
    {
        metrics := getThemeMetrics()
        clientRect := getClientRect()
        sl := metrics.popupMenu_SpaceLines
        visit i, idx: items
        {
            rect := Math.Rectangle{clientRect.x, i.pos - sl * 0.5, clientRect.width, i.height + sl}
            if rect.contains(pos)
                return cast(u32) idx
        }

        return Swag.U32.Max
    }

    mtd computeLayout()
    {
        theme   := getTheme()
        metrics := getThemeMetrics()

        clientRect := getClientRect()
        clientRect.inflate(-metrics.popupMenu_ShadowSize)
        clientRect.applyPadding(padding)

        y := clientRect.y

        position.width = 0
        position.height = clientRect.y

        var fake: Pixel.Painter{fake: true}
        var rsf:  Pixel.RichStringFormat
        rsf.font = surface.app.theme.res.fontDefault

        visit i, idx: items
        {
            fake.drawRichString(@{0, 0, 512, 0}, &i.name, rsf)

            if idx
                position.height += metrics.popupMenu_SpaceLines
            i.pos    = position.height
            i.height = rsf.font.regular.height

            w := i.name.boundRect.width
            position.width = Math.max(position.width, w)
            position.height += i.height
        }

        // Add shadow size
        position.width  += 2 * metrics.popupMenu_ShadowSize
        position.width  += 2 * (rsf.font.regular.height + metrics.popupMenu_IconTextMargin)
        position.height += metrics.popupMenu_ShadowSize

        // Add padding
        position.width  += padding.x + padding.z
        position.height += padding.w
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

    // Add a new item
    mtd addItem(itemName: string, id: WndId, icon: Icon = @{})
    {
        item := Memory.new'PopupMenuItem()
        item.name = itemName
        item.icon = icon
        item.id = #scopefct.id
        items.add(item)
    }

    // Make the popup menu modal
    // Will return the selected id
    mtd doModal(surfacePos: Math.Point)->WndId
    {
        computeLayout()
        pt := parent.surfaceToLocal(surfacePos)
        pt -= getThemeMetrics().popupMenu_ShadowSize
        setPosition(@{pt.x, pt.y, position.width, position.height})
        show()
        captureMouse()

        while !endModal
            surface.app.runFrame()

        destroy()
        return endModalResult
    }
}