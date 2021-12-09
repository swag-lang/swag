using Core, Pixel

public struct PopupMenuItem
{
    name:       RichString
    rightName:  RichString
    icon:       Icon
    pos:        f32
    height:     f32
    id:         WndId
    separator:  bool
    disabled:   bool
    popup:      *PopupMenu
}

public struct PopupMenu
{
    using wnd:      Wnd
    items:          Array'(*PopupMenuItem)
    hotIdx:         u32 = Swag.U32.Max
    endModal:       bool
    endModalResult: WndId
    owner:          *PopupMenu
    subVisible:     *PopupMenu
    timer:          *Timer
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

        var labelPos: Math.Rectangle = ?
        var colorText: Color
        var colorRightText: Color

        visit i, idx: items
        {
            labelPos.x = clientRect.x
            labelPos.y = i.pos
            labelPos.width = clientRect.width
            labelPos.height = i.height

            if i.separator
            {
                painter.drawLine(labelPos.x, labelPos.vertCenter(), labelPos.right(), labelPos.vertCenter(), Pen.createSolid(colors.popupMenu_Separator))
                continue
            }

            var format: RichStringFormat
            colorText = i.disabled ? colors.popupItem_DisabledText : colors.popupItem_Text
            colorRightText = i.disabled ? colors.popupItem_DisabledText : colors.popupItem_RightText

            // Back selection
            if hotIdx == idx and !i.disabled
            {
                backPos := labelPos
                backPos.x = metrics.popupMenu_ShadowSize
                backPos.y -= metrics.popupMenu_SpaceLines * 0.5
                backPos.width = position.width - 2 * metrics.popupMenu_ShadowSize
                backPos.height += metrics.popupMenu_SpaceLines
                painter.fillRect(backPos, colors.popupItem_HotBk)
                colorText = colors.popupItem_HotText
                colorRightText = colors.popupItem_HotRightText
            }

            // Icon
            if i.icon.isValid()
            {
                i.icon.size = i.height
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.size * 0.5, colorText)
            }

            labelPos.moveLeft(i.height + metrics.popupMenu_IconTextMargin)

            // Text
            format.palette[0] = colorText
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault
            painter.drawRichString(labelPos, &i.name, format)

            // Text right
            if !i.rightName.isEmpty()
            {
                format.palette[0] = colorRightText
                format.horzAlignment = .Right
                format.vertAlignment = .Center
                format.flags = .Clip
                format.font = theme.res.fontDefault
                painter.drawRichString(labelPos, &i.rightName, format)
            }

            // Right icon
            if i.popup
            {
                icon := theme.getIcon24(ThemeIcons24.RightPopupArrow)
                icon.size = i.height
                icon.paint(painter, labelPos.right() - icon.size, labelPos.vertCenter() - icon.size * 0.5, colors.popupItem_Text)
            }
        }

        // Border
        clientRect = getClientRect()
        clientRect.inflate(-metrics.popupMenu_ShadowSize)
        theme.drawSubRect(painter, clientRect, &rects.popupMenu_Border, colors.popupMenu_Border)
    }

    mtd onTimerEvent(evt: *TimerEvent)
    {
        hideSubVisible()
        deleteTimer()
        showHotPopup()
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        hit := hitTest(evt.pos)
        if hit != hotIdx
        {
            if !timer
                timer = surface.app.addTimer(300, self)
            timer.restart()
            hotIdx = hit
            invalidate()

            if hit != Swag.U32.Max
            {
                p := owner
                while p
                {
                    p.deleteTimer()
                    p = p.owner
                }
            }
        }

        evt.accepted = true
        if hotIdx == Swag.U32.Max and owner
        {
            evt.pos = owner.surfaceToLocal(evt.surfacePos)
            owner.onMouseEvent(evt)
            return
        }

        if evt.kind != .MousePressed or evt.button != .Left
            return

        if hotIdx != Swag.U32.Max
        {
            hotItem := items[hotIdx]
            if hotItem.popup
            {
                hideSubVisible()
                deleteTimer()
                showHotPopup()
            }
            else if !hotItem.separator and !hotItem.disabled
            {
                setEndModal(hotItem.id)
            }
        }
        else
        {
            setEndModal(null)
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

    mtd setEndModal(result: WndId)
    {
        p := owner
        if !p
            p = self
        else
            while p.owner p = p.owner
        p.endModal = true
        p.endModalResult = result
    }

    mtd hideSubVisible()
    {
        while subVisible
        {
            subVisible.hide()
            subVisible = subVisible.subVisible
        }

        captureMouse()
    }

    mtd deleteTimer()
    {
        if !timer return
        surface.app.deleteTimer(timer)
        timer = null
    }

    mtd showHotPopup()
    {
        if hotIdx == Swag.U32.Max
            return
        if !items[hotIdx].popup
            return

        hotItem := items[hotIdx]
        pos := getSurfaceRect()
        surfaceRect := pos
        pos.x = pos.right() - getThemeMetrics().popupMenu_ShadowSize
        pos.y += hotItem.pos
        pos.y -= padding.y
        subVisible = hotItem.popup

        // Be sure popup menu is in the client area of the top view
        subVisible.computeLayout()
        topView := getTopView()
        pt := topView.surfaceToLocal(@{pos.x, pos.y})
        subPos := subVisible.position
        if pt.x + subPos.width > topView.position.width
            pt.x = surfaceRect.x - subPos.width
        if pt.y + subPos.height > topView.position.height
            pt.y = surfaceRect.y - subPos.height
        pt = topView.localToSurface(pt)

        discard subVisible.doModal(pt)
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
            if idx
                position.height += metrics.popupMenu_SpaceLines
            i.pos = position.height

            if i.separator
            {
                i.height = metrics.popupMenu_SeparatorHeight
                position.height += i.height
                continue
            }

            i.height = rsf.font.regular.height

            fake.drawRichString(@{0, 0, 512, 0}, &i.name, rsf)
            w := i.name.boundRect.width

            if !i.rightName.isEmpty()
            {
                fake.drawRichString(@{0, 0, 512, 0}, &i.rightName, rsf)
                w += i.name.boundRect.width
            }

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
    mtd addItem(itemName: string, id: WndId, icon: Icon = @{}, rightName: string = null, disabled = false)
    {
        item := Memory.new'PopupMenuItem()
        item.name = itemName
        item.rightName = rightName
        item.icon = icon
        item.id = #scopefct.id
        item.disabled = disabled
        items.add(item)
    }

    // Add a new item
    mtd addPopup(itemName: string, popup: *PopupMenu, icon: Icon = @{})
    {
        item := Memory.new'PopupMenuItem()
        item.name = itemName
        item.icon = icon
        item.popup = popup
        popup.owner = self
        items.add(item)
    }

    // Add a separator
    mtd addSeparator()
    {
        item := Memory.new'PopupMenuItem()
        item.separator = true
        items.add(item)
    }

    // Make the popup menu modal
    // Will return the selected id
    mtd doModal(surfacePos: Math.Point)->WndId
    {
        computeLayout()

        // Be sure popup menu is in the client area of the top view
        topView := getTopView()
        pt := topView.surfaceToLocal(surfacePos)
        pt -= getThemeMetrics().popupMenu_ShadowSize
        if pt.x + position.width > topView.position.width
            pt.x = topView.position.width - position.width
        if pt.y + position.height > topView.position.height
            pt.y = topView.position.height - position.height
        pt = topView.localToSurface(pt)
        pt = parent.surfaceToLocal(pt)

        setPosition(@{pt.x, pt.y, position.width, position.height})

        bringToTop()
        show()
        captureMouse()

        if owner
            return endModalResult

        endModal = false
        endModalResult = null
        while !endModal
            surface.app.runFrame()

        hideSubVisible()
        hide()
        releaseMouse()

        return endModalResult
    }
}