using Core, Pixel

public struct PopupMenuItem
{
    name:       RichString
    rightName:  RichString
    icon:       Icon
    pos:        f32
    size:       f32
    id:         WndId
    popup:      *Menu
    separator:  bool
    disabled:   bool
}

public struct Menu
{
    using wnd:      Wnd
    items:          Array'(*PopupMenuItem)
    hotIdx:         u32 = Swag.U32.Max
    bar:            bool
    endModal:       bool
    endModalResult: WndId
    owner:          *Menu
    subVisible:     *Menu
    timer:          *Timer
    dirtyLayout:    bool
}

impl IWnd for Menu
{
    mtd paintBar(painter: *Painter)
    {
        theme   := getTheme()
        colors  := getThemeColors()
        rects   := getThemeRects()
        metrics := getThemeMetrics()

        if dirtyLayout
        {
            dirtyLayout = false
            computeLayoutBar()
        }

        // Back
        clientRect := getClientRect()
        theme.drawSubRect(painter, clientRect, &rects.menuBar_Bk, colors.menuBar_Bk)
        theme.drawSubRect(painter, clientRect, &rects.menuBar_Border, colors.menuBar_Border)

        var labelPos: Math.Rectangle = ?
        var colorText: Color
        var colorRightText: Color

        visit i, idx: items
        {
            if i.separator
                continue

            labelPos.x = i.pos
            labelPos.y = clientRect.y
            labelPos.width = i.size
            labelPos.height = clientRect.height

            var format: RichStringFormat
            colorText = i.disabled ? colors.popupItem_DisabledText : colors.popupItem_Text
            colorRightText = i.disabled ? colors.popupItem_DisabledText : colors.popupItem_RightText

            // Back selection
            if hotIdx == idx and !i.disabled
            {
                backPos := labelPos
                backPos.x -= metrics.menuBar_SpaceBetween * 0.5
                backPos.width += metrics.menuBar_SpaceBetween
                theme.drawSubRect(painter, backPos, &rects.menuPopup_HotBk, colors.popupItem_HotBk)
                colorText = colors.popupItem_HotText
                colorRightText = colors.popupItem_HotRightText
            }

            // Icon
            if i.icon.isValid()
            {
                i.icon.size = i.size
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.size * 0.5, colorText)
            }

            // Text
            format.palette[0] = colorText
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault
            painter.drawRichString(labelPos, &i.name, format)
        }

        // Border
        clientRect = getClientRect()
        clientRect.inflate(-metrics.menuPopup_ShadowSize)
        theme.drawSubRect(painter, clientRect, &rects.menuPopup_Border, colors.menuPopup_Border)
    }

    mtd paintPopup(painter: *Painter)
    {
        theme   := getTheme()
        colors  := getThemeColors()
        rects   := getThemeRects()
        metrics := getThemeMetrics()

        // Shadow
        subRect := &theme.rects.menuPopup_Shadow
        theme.drawSubRect(painter, @{0, 0, position.width, position.height}, subRect, colors.menuPopup_Shadow)

        // Back
        clientRect := getClientRect()
        clientRect.inflate(-metrics.menuPopup_ShadowSize)
        theme.drawSubRect(painter, clientRect, &rects.menuPopup_Bk, colors.menuPopup_Bk)
        theme.drawSubRect(painter, clientRect, &rects.menuPopup_Border, colors.menuPopup_Border)
        clientRect.applyPadding(padding)

        var labelPos: Math.Rectangle = ?
        var colorText: Color
        var colorRightText: Color

        visit i, idx: items
        {
            labelPos.x = clientRect.x
            labelPos.y = i.pos
            labelPos.width = clientRect.width
            labelPos.height = i.size

            if i.separator
            {
                painter.drawLine(labelPos.x, labelPos.vertCenter(), labelPos.right(), labelPos.vertCenter(), Pen.createSolid(colors.menuPopup_Separator))
                continue
            }

            var format: RichStringFormat
            colorText = i.disabled ? colors.popupItem_DisabledText : colors.popupItem_Text
            colorRightText = i.disabled ? colors.popupItem_DisabledText : colors.popupItem_RightText

            // Back selection
            if hotIdx == idx and !i.disabled
            {
                backPos := labelPos
                backPos.x = metrics.menuPopup_ShadowSize
                backPos.y -= metrics.menuPopup_SpaceLines * 0.5
                backPos.width = position.width - 2 * metrics.menuPopup_ShadowSize
                backPos.height += metrics.menuPopup_SpaceLines
                theme.drawSubRect(painter, backPos, &rects.menuPopup_HotBk, colors.popupItem_HotBk)
                colorText = colors.popupItem_HotText
                colorRightText = colors.popupItem_HotRightText
            }

            // Icon
            if i.icon.isValid()
            {
                i.icon.size = i.size
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.size * 0.5, colorText)
            }

            labelPos.moveLeft(i.size + metrics.menuPopup_IconTextMargin)

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
                icon.size = i.size
                icon.paint(painter, labelPos.right() - icon.size, labelPos.vertCenter() - icon.size * 0.5, colors.popupItem_Text)
            }
        }

        // Border
        clientRect = getClientRect()
        clientRect.inflate(-metrics.menuPopup_ShadowSize)
        theme.drawSubRect(painter, clientRect, &rects.menuPopup_Border, colors.menuPopup_Border)
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        if bar
            paintBar(evt.bc.painter)
        else
            paintPopup(evt.bc.painter)
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

impl Menu
{
    mtd opDrop()
    {
        visit i: items
            Memory.delete(i)
    }

    mtd setEndModal(result: WndId)
    {
        p := self
        while p.owner
            p = p.owner
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

        if !bar
            captureMouse()
        else
            releaseMouse()
    }

    mtd deleteTimer()
    {
        if !timer return
        surface.app.deleteTimer(timer)
        timer = null
    }

    mtd showHotPopupBar()
    {
        if hotIdx == Swag.U32.Max
            return
        if !items[hotIdx].popup
            return
        hotItem := items[hotIdx]

        pos := getSurfaceRect()
        surfaceRect := pos
        pos.x += hotItem.pos
        pos.x -= getThemeMetrics().menuPopup_ShadowSize
        pos.x -= getThemeMetrics().menuBar_SpaceBetween * 0.5
        pos.y += pos.height
        pos.y -= getThemeMetrics().menuPopup_ShadowSize
        subVisible = hotItem.popup

        // Be sure popup menu is in the client area of the top view
        subVisible.computeLayoutPopup()
        topView := getTopView()
        pt := topView.surfaceToLocal(@{pos.x, pos.y})
        subPos := subVisible.position
        if pt.x + subPos.width > topView.position.width
            pt.x = topView.position.width - subPos.width
        if pt.y + subPos.height > topView.position.height
            pt.y = topView.position.height - subPos.height
        pt = topView.localToSurface(pt)

        pt = subVisible.parent.surfaceToLocal(pt)
        subVisible.setPosition(@{pt.x, pt.y, subVisible.position.width, subVisible.position.height})
        subVisible.bringToTop()
        subVisible.show()
        subVisible.captureMouse()
    }

    mtd showHotPopupPopup()
    {
        if hotIdx == Swag.U32.Max
            return
        if !items[hotIdx].popup
            return

        hotItem := items[hotIdx]
        pos := getSurfaceRect()
        sdw := getThemeMetrics().menuPopup_ShadowSize
        surfaceRect := pos
        pos.x = pos.right() - sdw
        pos.y += hotItem.pos
        pos.y -= padding.y
        subVisible = hotItem.popup

        // Be sure popup menu is in the client area of the top view
        subVisible.computeLayoutPopup()
        topView := getTopView()
        pt := topView.surfaceToLocal(@{pos.x, pos.y})
        pt -= sdw
        subPos := subVisible.position
        if pt.x + subPos.width > topView.position.width
            pt.x = surfaceRect.x - subPos.width
        if pt.y + subPos.height > topView.position.height
            pt.y = surfaceRect.y - subPos.height
        pt = topView.localToSurface(pt)

        pt = subVisible.parent.surfaceToLocal(pt)
        subVisible.setPosition(@{pt.x, pt.y, subVisible.position.width, subVisible.position.height})
        subVisible.bringToTop()
        subVisible.show()
        subVisible.captureMouse()
    }

    mtd showHotPopup()
    {
        if bar
            showHotPopupBar()
        else
            showHotPopupPopup()
    }

    mtd hitTestBar(pos: Math.Point)->u32
    {
        metrics := getThemeMetrics()
        clientRect := getClientRect()
        sl := metrics.menuBar_SpaceBetween
        visit i, idx: items
        {
            rect := Math.Rectangle{i.pos - sl * 0.5, clientRect.y, i.size + sl, clientRect.height}
            if rect.contains(pos)
                return cast(u32) idx
        }

        return Swag.U32.Max
    }

    mtd hitTestPopup(pos: Math.Point)->u32
    {
        metrics := getThemeMetrics()
        clientRect := getClientRect()
        sl := metrics.menuPopup_SpaceLines
        visit i, idx: items
        {
            rect := Math.Rectangle{clientRect.x, i.pos - sl * 0.5, clientRect.width, i.size + sl}
            if rect.contains(pos)
                return cast(u32) idx
        }

        return Swag.U32.Max
    }

    mtd hitTest(pos: Math.Point)->u32
    {
        return bar ? hitTestBar(pos) : hitTestPopup(pos)
    }

    mtd computeLayoutBar()
    {
        theme   := getTheme()
        metrics := getThemeMetrics()

        clientRect := getClientRectPadding()

        x := clientRect.x

        var fake: Pixel.Painter{fake: true}
        var rsf:  Pixel.RichStringFormat
        rsf.font = surface.app.theme.res.fontDefault

        visit i, idx: items
        {
            i.pos = x + metrics.menuBar_SpaceBetween * 0.5
            fake.drawRichString(@{0, 0, 512, 0}, &i.name, rsf)
            i.size = i.name.boundRect.width
            x = i.pos + i.size + metrics.menuBar_SpaceBetween * 0.5
        }
    }

    mtd computeLayoutPopup()
    {
        theme   := getTheme()
        metrics := getThemeMetrics()

        clientRect := getClientRect()
        clientRect.inflate(-metrics.menuPopup_ShadowSize)
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
                position.height += metrics.menuPopup_SpaceLines
            i.pos = position.height

            if i.separator
            {
                i.size = metrics.menuPopup_SeparatorHeight
                position.height += i.size
                continue
            }

            i.size = rsf.font.regular.height

            fake.drawRichString(@{0, 0, 512, 0}, &i.name, rsf)
            w := i.name.boundRect.width

            if !i.rightName.isEmpty()
            {
                fake.drawRichString(@{0, 0, 512, 0}, &i.rightName, rsf)
                w += i.name.boundRect.width
            }

            position.width = Math.max(position.width, w)
            position.height += i.size
        }

        // Add shadow size
        position.width  += 2 * metrics.menuPopup_ShadowSize
        position.width  += 2 * (rsf.font.regular.height + metrics.menuPopup_IconTextMargin)
        position.height += metrics.menuPopup_ShadowSize

        // Add padding
        position.width  += padding.x + padding.z
        position.height += padding.w
    }
}

public impl Menu
{
    #[Swag.Discardable]
    func createPopup(parent: *Wnd, id: string = null)->*Menu
    {
        res := Wnd.create'Menu(&parent.surface.wnd, null, @{}, id)
        res.sendCreateEvent()
        res.padding = res.getThemeMetrics().menuPopup_Padding
        res.isHidden = true
        return res
    }

    #[Swag.Discardable]
    func createBar(parent: *Wnd, id: string = null)->*Menu
    {
        res := Wnd.create'Menu(&parent.surface.wnd, null, @{}, id)
        res.sendCreateEvent()
        res.padding = res.getThemeMetrics().menuPopup_Padding
        res.bar = true
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
        dirtyLayout = true
    }

    // Add a new item
    mtd addPopup(itemName: string, popup: *Menu, icon: Icon = @{})
    {
        item := Memory.new'PopupMenuItem()
        item.name = itemName
        item.icon = icon
        item.popup = popup
        popup.owner = self
        items.add(item)
        dirtyLayout = true
    }

    // Add a separator
    mtd addSeparator()
    {
        item := Memory.new'PopupMenuItem()
        item.separator = true
        items.add(item)
        dirtyLayout = true
    }

    // Make the popup menu modal
    // Will return the selected id
    mtd doModal(surfacePos: Math.Point)->WndId
    {
        Debug.assert(!bar)

        computeLayoutPopup()

        // Be sure popup menu is in the client area of the top view
        topView := getTopView()
        pt := topView.surfaceToLocal(surfacePos)
        pt -= getThemeMetrics().menuPopup_ShadowSize
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

        endModal = false
        endModalResult = null
        while !endModal
            surface.app.runFrame()

        hideSubVisible()
        hide()

        return endModalResult
    }
}