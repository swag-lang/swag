#global public
using Core, Pixel

enum TabForm
{
    Flat
    Round
    Button
    PushButton
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
    PushButton
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
    using item:     Item
    pos:            f32
    width:          f32
    view:           *Wnd
    aniImg:         BlendColor
    aniBorder:      BlendColor
    aniText:        BlendColor
}

struct Tab
{
    using wnd:                Wnd

    barForm                   = TabBarForm.Flat
    itemForm                  = TabItemForm.Flat
    viewForm                  = TabViewForm.Flat
    barLayout                 = TabBarLayout.Divide
    itemFixedLayoutWidth:     f32 = 100
    itemAutoLayoutMargin:     f32 = 25
    itemMinWidth:             f32 = 25

    sigSelChanged:            SigArray'func||(*Tab)

    buttonPopup:              *IconButton
    views:                    ArrayPtr'TabItem
    popup:                    *PopupListCtrl
    selectedIdx:              u32
    firstVisibleIdx:          u32
    hotIdx:                   u32 = Swag.U32.Max
    barHeight:                f32
    mostRightPos:             f32
}

impl IWnd for Tab
{
    mtd paintItem(painter: *Painter, v: *TabItem, idx: u32)
    {
        var clientRect = me.getClientRect()
        clientRect.height = Math.min(me.barHeight, clientRect.height)

        var labelPos = clientRect
        labelPos.width = v.width
        labelPos.x     = v.pos

        let theme   = me.getTheme()
        let metrics = me.getThemeMetrics()
        let colors  = me.getThemeColors()
        let rects   = me.getThemeRects()

        // Item image
        var subRect:       *ThemeImageRect
        var subRectBorder: *ThemeImageRect
        var colorImg:      Color
        var colorText:     Color
        var colorBorder:   Color

        #[Swag.Complete]
        switch me.itemForm
        {
        case Flat:
            if idx == me.selectedIdx
            {
                subRect   = &rects.tabItem_FlatSelected
                colorImg  = colors.tabItem_FlatSelected
                colorText = colors.tabItem_SelectedText
            }
            elif idx == me.hotIdx
            {
                subRect   = &rects.tabItem_FlatSelected
                colorImg  = colors.tabItem_FlatHot
                colorText = colors.tabItem_HotText
            }
            else
            {
                colorText = colors.tabItem_Text
            }

        case Round:
            if idx == me.selectedIdx
            {
                subRect   = &rects.tabItem_RoundSelected
                colorImg  = colors.tabItem_RoundSelected
                colorText = colors.tabItem_SelectedText
            }
            elif idx == me.hotIdx
            {
                subRect   = &rects.tabItem_RoundHot
                colorImg  = colors.tabItem_RoundHot
                colorText = colors.tabItem_HotText
            }
            else
            {
                colorText = colors.tabItem_Text
            }

        case Button:
            if idx == me.selectedIdx
            {
                subRect   = &rects.tabItem_ButtonSelected
                colorImg  = colors.tabItem_ButtonSelected
                colorText = colors.tabItem_SelectedText
            }
            elif idx == me.hotIdx
            {
                subRect   = &rects.tabItem_ButtonHot
                colorImg  = colors.tabItem_ButtonHot
                colorText = colors.tabItem_HotText
            }
            else
            {
                colorText = colors.tabItem_Text
            }

        case PushButton:
            if idx == me.selectedIdx
            {
                subRect       = &rects.tabItem_PushButtonSelected
                subRectBorder = &rects.tabItem_PushButtonBorder
                colorImg      = colors.tabItem_PushButtonSelected
                colorText     = colors.tabItem_PushButtonSelectedText
                colorBorder   = colors.tabItem_PushButtonSelectedBorder
            }
            elif idx == me.hotIdx
            {
                subRect       = &rects.tabItem_PushButtonHot
                subRectBorder = &rects.tabItem_PushButtonBorder
                colorImg      = colors.tabItem_PushButtonHot
                colorText     = colors.tabItem_PushButtonHotText
                colorBorder   = colors.tabItem_PushButtonHotBorder
            }
            else
            {
                subRect       = &rects.tabItem_PushButtonBk
                subRectBorder = &rects.tabItem_PushButtonBorder
                colorImg      = colors.tabItem_PushButtonBk
                colorText     = colors.tabItem_PushButtonText
                colorBorder   = colors.tabItem_PushButtonBorder
            }

        case Transparent:
            break
        }

        v.aniImg.apply(me, colorImg)
        v.aniBorder.apply(me, colorBorder)

        if subRect:
            theme.drawSubRect(painter, labelPos, subRect, v.aniImg.cur())
        if subRectBorder:
            theme.drawSubRect(painter, labelPos, subRectBorder, v.aniBorder.cur())

        var format: RichStringFormat
        v.aniText.apply(me, colorText)
        theme.setTextColors(&format, v.aniText.cur())
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.flags         = .Clip
        format.font          = me.getFont()
        Theme.drawIconText(painter, labelPos, v.icon, &v.name, format, metrics.iconText_Margin)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter = evt.bc.painter
        let theme   = me.getTheme()
        let colors  = me.getThemeColors()
        let rects   = me.getThemeRects()

        // Background bar
        if me.barForm != .Transparent
        {
            var clientRect = me.getClientRect()
            clientRect.height = me.barHeight
            switch me.barForm
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
        var subRect: *ThemeImageRect
        var colorBk: Color

        var clientRect = me.getClientRect()
        clientRect.moveTop(me.barHeight)
        switch me.viewForm
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
        if !me.views.isEmpty()
        {
            clientRect = me.getBarRectItems()
            for idx in me.firstVisibleIdx until cast(u32) me.views.count
            {
                if idx == me.selectedIdx:
                    continue
                let v = me.views[idx]
                if v.pos + v.width > clientRect.right():
                    break
                me.paintItem(painter, v, cast(u32) idx)
            }

            // Paint selected item
            me.paintItem(painter, me.views[me.selectedIdx], me.selectedIdx)
        }
    }

    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if me.views.isEmpty():
            return

        me.computeTabLayout()
        me.ensureVisible(me.selectedIdx)

        let curView = me.views[me.selectedIdx].view
        if curView
        {
            var clientRect = me.getClientRect()
            clientRect.moveTop(me.barHeight)
            clientRect.applyPadding(me.padding)
            curView.setPosition(clientRect)
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        let hit = me.hitTest(evt.surfacePos)
        if hit != me.hotIdx
        {
            me.hotIdx = hit
            me.invalidate()
        }

        if hit == Swag.U32.Max
        {
            evt.accepted = false
            return
        }

        switch evt.kind
        {
        case .MouseLeave:
            if me.hotIdx != Swag.U32.Max
            {
                me.hotIdx = Swag.U32.Max
                me.invalidate()
            }

        case .MousePressed:
            if evt.button == .Left
            {
                if hit != Swag.U32.Max
                {
                    me.select(hit)
                }
            }
        }
    }
}

internal impl Tab
{
    mtd showPopup(x, y: f32)
    {
        me.popup = PopupListCtrl.create(me)
        foreach item in me.views:
            me.popup.addItem(item.name.raw, item.id, item.icon, item.userData0)

        me.popup.sigSelected = mtd||(list, idx)
        {
            if idx != Swag.U32.Max:
                me.select(idx)
            me.hidePopup()
        }

        me.popup.show(x, y)
    }

    mtd hidePopup()
    {
        me.popup.destroy()
        me.popup = null
    }

    mtd getBarRectItems()->Math.Rectangle
    {
        var rect = me.getClientRectPadding()
        if me.buttonPopup:
            rect.setRight(me.buttonPopup.position.x - 4)
        return rect
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        var pos = me.surfaceToLocal(surfacePos)
        if pos.y >= me.barHeight:
            return Swag.U32.Max

        var rect = me.getBarRectItems()
        for idx in me.firstVisibleIdx until cast(u32) me.views.count
        {
            let v = me.views[idx]
            if v.pos + v.width > rect.right():
                break
            if pos.x >= v.pos and pos.x < v.pos + v.width:
                return cast() idx
        }

        return Swag.U32.Max
    }

    mtd computeTabLayout()
    {
        var rect = me.getBarRectItems()

        me.mostRightPos = rect.x
        switch me.barLayout
        {
        case Divide:
            var w = cast(f32) (rect.width / me.views.count)
            w = Math.max(w, me.itemMinWidth)
            foreach v in me.views
            {
                v.width = w
                me.mostRightPos += w
            }

        case Fixed:
            let w = Math.max(me.itemFixedLayoutWidth, me.itemMinWidth)
            foreach v in me.views
            {
                v.width = w
                me.mostRightPos += w
            }

        case Auto:
            foreach v in me.views
            {
                var mes = Painter.measureString(v.item.name.raw, me.getFont().regular)

                // Text
                v.width = mes.width

                // Icon
                if v.item.icon.isValid():
                    v.width += v.item.icon.sizeX + me.getThemeMetrics().iconText_Margin

                // Margin
                v.width += me.itemAutoLayoutMargin

                me.mostRightPos += v.width
            }
        }

        var x = rect.x
        for idx in me.firstVisibleIdx until cast(u32) me.views.count
        {
            let v = me.views[idx]
            v.pos = x
            x += v.width
        }

        me.createDestroyButtonPopup()
    }

    mtd createDestroyButtonPopup()
    {
        var barRect = me.getClientRectPadding()
        if me.mostRightPos > barRect.right()
        {
            // Create a button if there are too many items for the given size
            if !me.buttonPopup
            {
                me.buttonPopup      = IconButton.create(me, me.getTheme().getIcon24(ThemeIcons24.SortArrowDown), {})
                me.buttonPopup.form = .Round
                me.buttonPopup.sigPressed += mtd||(btn)
                {
                    var rect = btn.getSurfaceRect()
                    me.showPopup(rect.x, rect.y + btn.position.height)
                }
            }

            let x = barRect.right() - 32
            me.buttonPopup.setPosition({x, (me.barHeight * 0.5) - 12, 24, 24})
        }
        elif me.buttonPopup
        {
            me.firstVisibleIdx = 0
            me.buttonPopup.destroy()
            me.buttonPopup = null
            me.computeTabLayout()
        }
    }

    mtd ensureVisible(idx: u32)
    {
        var rect = me.getBarRectItems()
        if idx < me.firstVisibleIdx or me.views[idx].pos + me.views[idx].width >= rect.right()
        {
            me.firstVisibleIdx = idx
            var x = rect.right() - me.views[me.firstVisibleIdx].width
            while me.firstVisibleIdx
            {
                x -= me.views[me.firstVisibleIdx - 1].width
                if x < rect.x:
                    break
                me.firstVisibleIdx -= 1
            }

            me.computeTabLayout()
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
        res.setForm(.Flat)
        return res
    }

    // Set the tab form
    mtd setForm(tabForm: TabForm)
    {
        #[Swag.Complete]
        switch tabForm
        {
        case Flat:
            me.barForm   = .Flat
            me.itemForm  = .Flat
            me.viewForm  = .Flat
            me.barLayout = .Divide
            me.barHeight = me.getThemeMetrics().tabBar_FlatHeight

        case Round:
            me.barForm   = .Transparent
            me.itemForm  = .Round
            me.viewForm  = .Round
            me.barLayout = .Fixed
            me.barHeight = me.getThemeMetrics().tabBar_RoundHeight

        case Button:
            me.viewForm  = .None
            me.barForm   = .Round
            me.itemForm  = .Button
            me.barLayout = .Auto
            me.barHeight = me.getThemeMetrics().tabBar_ButtonHeight

        case PushButton:
            me.viewForm  = .None
            me.barForm   = .Transparent
            me.itemForm  = .PushButton
            me.barLayout = .Auto
            me.barHeight = me.getThemeMetrics().tabBar_ButtonHeight
        }
    }

    // Add a new view
    #[Swag.Discardable]
    mtd addItem(tabName: string, icon: Icon = {}, view: *Wnd = null)->*TabItem
    {
        if view
        {
            if me.views.isEmpty():
                view.wndFlags &= ~.Hidden
            else:
                view.wndFlags |= .Hidden
            view.setParent(me)
        }

        let item = me.views.newPtr()
        item.view = view
        item.name = tabName
        item.icon = icon
        me.views.add(item)

        me.sendResizeEvent()
        return item
    }

    // Remove all items
    mtd clear()
    {
        me.views.clear()
    }

    // Select one tab at the given index
    mtd select(idx: u32)
    {
        if me.selectedIdx == idx or me.selectedIdx >= me.views.count:
            return

        if me.views[me.selectedIdx].view:
            me.views[me.selectedIdx].view.wndFlags |= .Hidden
        me.selectedIdx = idx
        if me.views[me.selectedIdx].view:
            me.views[me.selectedIdx].view.wndFlags &= ~.Hidden

        me.sigSelChanged.call(me)

        me.sendResizeEvent()
        me.invalidate()
    }

    // Returns the current selected item, or 'null'
    mtd getSelectedItem()->*TabItem
    {
        Debug.assert(me.selectedIdx < me.views.count)
        return me.views[me.selectedIdx]
    }
}
