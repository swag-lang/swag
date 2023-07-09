#global public
using Core, Pixel

enum TabForm
{
    Flat
    Round
    Button
}

enum TabBarForm
{
    Transparent
    Flat
    FlatRound
    Round
}


enum TabItemForm
{
    Transparent
    Flat
    Round
    Button
}

enum TabViewForm
{
    None
    Flat
    Round
}

enum TabBarLayout
{
    Divide
    Fixed
    Auto
}

struct TabItem
{
    using item: Item
    pos:        f32
    width:      f32
    view:       *Wnd
    aniImg:     BlendColor
    aniText:    BlendColor
}

struct Tab
{
    using wnd: Wnd

    barForm                 = TabBarForm.Flat
    itemForm                = TabItemForm.Flat
    viewForm                = TabViewForm.Flat
    barLayout               = TabBarLayout.Divide
    itemFixedLayoutWidth:   f32 = 100
    itemAutoLayoutMargin:   f32 = 25
    itemMinWidth:           f32 = 25

    sigSelChanged:      SigArray'closure(self)

    buttonPopup:        *IconButton
    views:              ArrayPtr'TabItem
    popup:              *PopupListCtrl
    selectedIdx:        u32
    firstVisibleIdx:    u32 = 0
    hotIdx:             u32 = Swag.U32.Max
    barHeight:          f32
    mostRightPos:       f32
}

impl IWnd for Tab
{
    mtd paintItem(painter: *Painter, v: *TabItem, idx: u32)
    {
        var clientRect = getClientRect()
        clientRect.height = barHeight
        var labelPos = clientRect
        labelPos.width = v.width
        labelPos.x = v.pos

        let theme = getTheme()
        let metrics = getThemeMetrics()
        let colors = getThemeColors()
        let rects = getThemeRects()

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
        case Button:
            if idx == selectedIdx
            {
                subRect = &rects.tabItem_ButtonSelected
                colorImg = colors.tabItem_ButtonSelected
                colorText = colors.tabItem_SelectedText
            }
            elif idx == hotIdx
            {
                subRect = &rects.tabItem_ButtonHot
                colorImg = colors.tabItem_ButtonHot
                colorText = colors.tabItem_SelectedText
            }
            else
            {
                colorText = colors.tabItem_UnSelectedText
            }
        }

        v.aniImg.apply(self, colorImg)
        if subRect
            theme.drawSubRect(painter, labelPos, subRect, v.aniImg.cur())

        var format: RichStringFormat
        v.aniText.apply(self, colorText)
        theme.setTextColors(&format, v.aniText.cur())
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.flags = .Clip
        format.font = getFont()
        Theme.drawIconText(painter, labelPos, v.icon, &v.name, format, metrics.iconText_Margin)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter = evt.bc.painter
        let theme = getTheme()
        let colors = getThemeColors()
        let rects = getThemeRects()

        // Background bar
        if barForm != .Transparent
        {
            var clientRect = getClientRect()
            clientRect.height = barHeight
            switch barForm
            {
            case Flat:
                painter.fillRect(clientRect, colors.tabBar_FlatBk)
            case FlatRound:
                theme.drawSubRect(painter, clientRect, &rects.tabBar_FlatRoundBk, colors.tabBar_FlatBk)
            case Round:
                theme.drawSubRect(painter, clientRect, &rects.tabBar_RoundBk, colors.tabBar_RoundBk)
            }
        }

        // Paint border and back
        var subRect:    *ThemeImageRect
        var colorBk:    Color

        var clientRect = getClientRect()
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
        if !views.isEmpty()
        {
            clientRect = getBarRectItems()
            loop idx: firstVisibleIdx..cast(u32) views.count-1
            {
                if idx == selectedIdx
                    continue
                let v = views[idx]
                if v.pos + v.width > clientRect.right()
                    break
                paintItem(painter, v, cast(u32) idx)
            }

            // Paint selected item
            paintItem(painter, views[selectedIdx], selectedIdx)
        }
    }

    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if views.isEmpty()
            return

        computeTabLayout()
        ensureVisible(selectedIdx)

        let curView = views[selectedIdx].view
        if curView
        {
            var clientRect = getClientRect()
            clientRect.moveTop(barHeight)
            clientRect.applyPadding(padding)
            curView.setPosition(clientRect)
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        let hit = hitTest(evt.surfacePos)
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
    mtd showPopup(x, y: f32)
    {
        popup = PopupListCtrl.create(self)
        visit item: views
            popup.addItem(item.name.raw, item.id, item.icon, item.userData0)

        popup.sigSelected = closure|self|(list, idx) {
            if idx != Swag.U32.Max
                self.select(idx)
            self.hidePopup()
        }

        popup.show(x, y)
    }

    mtd hidePopup()
    {
        popup.destroy()
        popup = null
    }

    mtd getBarRectItems()->Math.Rectangle
    {
        var rect = getClientRectPadding()
        if buttonPopup
            rect.setRight(buttonPopup.position.x - 4)
        return rect
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        var pos = surfaceToLocal(surfacePos)
        if pos.y >= barHeight
            return Swag.U32.Max

        var rect = getBarRectItems()
        loop idx: firstVisibleIdx..cast(u32) views.count - 1
        {
            let v = views[idx]
            if v.pos + v.width > rect.right()
                break
            if pos.x >= v.pos and pos.x < v.pos + v.width
                return acast idx
        }

        return Swag.U32.Max
    }

    mtd computeTabLayout()
    {
        var rect = getBarRectItems()

        mostRightPos = rect.x
        switch barLayout
        {
        case Divide:
            var w = cast(f32) (rect.width / views.count)
            w = Math.max(w, itemMinWidth)
            visit v: views
            {
                v.width = w
                mostRightPos += w
            }

        case Fixed:
            let w = Math.max(itemFixedLayoutWidth, itemMinWidth)
            visit v: views
            {
                v.width = w
                mostRightPos += w
            }

        case Auto:
            visit v: views
            {
                var mes = Painter.measureString(v.item.name.raw, getFont().regular)

                // Text
                v.width = mes.width

                // Icon
                if v.item.icon.isValid()
                    v.width += v.item.icon.sizeX + getThemeMetrics().iconText_Margin

                // Margin
                v.width += itemAutoLayoutMargin

                mostRightPos += v.width
            }
        }

        var x = rect.x
        loop idx: firstVisibleIdx..cast(u32) views.count - 1
        {
            let v = views[idx]
            v.pos = x
            x += v.width
        }

        createDestroyButtonPopup()
    }

    mtd createDestroyButtonPopup()
    {
        var barRect = getClientRectPadding()
        if mostRightPos > barRect.right()
        {
            // Create a button if there are too many items for the given size
            if !buttonPopup
            {
                buttonPopup = IconButton.create(self, getTheme().getIcon24(ThemeIcons24.SortArrowDown), {})
                buttonPopup.form = .Round
                buttonPopup.sigPressed += closure|self|(btn)
                {
                    var rect = btn.getSurfaceRect()
                    self.showPopup(rect.x, rect.y + btn.position.height)
                }
            }

            let x = barRect.right() - 32
            buttonPopup.setPosition({x, (barHeight * 0.5) - 12, 24, 24})
        }
        else if buttonPopup
        {
            firstVisibleIdx = 0
            buttonPopup.destroy()
            buttonPopup = null
            computeTabLayout()
        }
    }

    mtd ensureVisible(idx: u32)
    {
        var rect = getBarRectItems()
        if idx < firstVisibleIdx or views[idx].pos + views[idx].width >= rect.right()
        {
            firstVisibleIdx = idx
            var x = rect.right() - views[firstVisibleIdx].width
            while firstVisibleIdx
            {
                x -= views[firstVisibleIdx - 1].width
                if x < rect.x
                    break
                firstVisibleIdx -= 1
            }

            computeTabLayout()
        }
    }
}

impl Tab
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*Tab
    {
        Debug.assert(parent != null)
        let res = Wnd.create'Tab(parent, null, position, id)
        res.focusStrategy = .None
        res.setForm(TabForm.Flat)
        return res
    }

    // Set the tab form
    mtd setForm(tabForm: TabForm)
    {
        switch tabForm
        {
        case Flat:
            barForm   = .Flat
            itemForm  = .Flat
            viewForm  = .Flat
            barLayout = .Divide
            barHeight = getThemeMetrics().tabBar_FlatHeight

        case Round:
            barForm   = .Transparent
            itemForm  = .Round
            viewForm  = .Round
            barLayout = .Fixed
            barHeight = getThemeMetrics().tabBar_RoundHeight

        case Button:
            viewForm  = .None
            barForm   = .Round
            itemForm  = .Button
            barLayout = .Auto
            barHeight = getThemeMetrics().tabBar_ButtonHeight
        }
    }

    // Add a new view
    mtd addItem(tabName: string, icon: Icon, view: *Wnd = null)
    {
        if view
        {
            if views.isEmpty()
                view.wndFlags &= ~.Hidden
            else
                view.wndFlags |= .Hidden
            view.setParent(self)
        }

        let item = views.newPtr()
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

        if views[selectedIdx].view
            views[selectedIdx].view.wndFlags |= .Hidden
        selectedIdx = idx
        if views[selectedIdx].view
            views[selectedIdx].view.wndFlags &= ~.Hidden

        sigSelChanged.call(self)

        sendResizeEvent()
        invalidate()
    }
}