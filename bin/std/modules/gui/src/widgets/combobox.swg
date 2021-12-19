using Core, Pixel

public struct ComboBox
{
    using wnd: Wnd

    minWidthPopup:  f32 = 100
    maxHeightPopup: f32 = 300
    heightItem:     f32 = 30
    onChanged:      func(*ComboBox, u32)

    isHot:          bool
    hasIcons:       bool
    popup:          *ComboBoxPopup
    items:          Array'(*ComboBoxItem)
    selectedIdx:    u32 = Swag.U32.Max
}

public struct ComboBoxItem
{
    name:   RichString
    icon:   Icon
    pos:    f32
    size:   f32
    id:     WndId
}

public struct ComboBoxPopup
{
    using frameWnd: FrameWnd
}

public struct ComboBoxPopupView
{
    using wnd:  Wnd
    combo:      *ComboBox
    hotIdx:     u32 = Swag.U32.Max
}

impl IWnd for ComboBoxPopupView
{
    mtd onFocusEvent(evt: *FocusEvent)
    {
        if evt.kind != .SetFocus
        {
            combo.hidePopup()
        }
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        hit := cast(u32) ((evt.pos.y + scrollPos.y) / combo.heightItem)
        if hit != hotIdx
        {
            hotIdx = hit
            invalidate()
        }

        if evt.kind == .MousePressed and evt.button == .Left
        {
            combo.selectItem(hotIdx)
            combo.hidePopup()
            return
        }

        evt.accepted = false
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        painter := evt.bc.painter

        theme   := getTheme()
        colors  := getThemeColors()
        metrics := getThemeMetrics()
        rects   := getThemeRects()

        clientRect := getClientRect()
        painter.fillRect(clientRect, colors.comboBox_SelectedBk)

        var labelPos: Math.Rectangle = ?
        var colorText: Color

        y := 0'f32
        visit i, idx: combo.items
        {
            labelPos.x = clientRect.x
            labelPos.y = y
            labelPos.width = clientRect.width
            labelPos.height = combo.heightItem

            var format: RichStringFormat
            colorText = colors.comboBoxItem_Text

            // Back selection
            if hotIdx == idx
            {
                theme.drawSubRect(painter, labelPos, &rects.comboBoxItem_HotBk, colors.comboBoxItem_HotBk)
                colorText = colors.comboBoxItem_HotText
            }

            labelPos.moveLeft(metrics.comboBox_Padding)

            // Icon
            fntHeight := theme.res.fontDefault.regular.height
            if i.icon.isValid()
            {
                i.icon.size = fntHeight
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.size * 0.5, colorText)
            }

            if combo.hasIcons
            {
                labelPos.moveLeft(fntHeight + metrics.comboBox_IconTextMargin)
            }

            if idx == combo.selectedIdx
            {
                labelPos.setRight(labelPos.right() - fntHeight - metrics.comboBox_IconTextMargin)
            }

            // Text
            theme.setTextColors(&format, colorText)
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault
            painter.drawRichString(labelPos, &i.name, format)

            // Check mark
            if idx == combo.selectedIdx
            {
                labelPos.x = labelPos.right()
                labelPos.y = labelPos.vertCenter() - (fntHeight * 0.5)
                labelPos.width = fntHeight
                labelPos.height = fntHeight
                if hotIdx == idx
                    colorText = colors.comboBoxItem_HotCheckMark
                else
                    colorText = colors.comboBoxItem_CheckMark
                theme.drawSubRect(painter, labelPos, &rects.btnCheck_CheckedMark, colorText)
            }

            y += combo.heightItem
        }
    }
}

impl IWnd for ComboBoxPopup
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        painter := evt.bc.painter

        theme   := getTheme()
        colors  := getThemeColors()
        metrics := getThemeMetrics()
        rects   := getThemeRects()

        // Shadow
        clientRect := getClientRect()
        subRect := &theme.rects.comboBox_Shadow
        theme.drawSubRect(painter, clientRect, subRect, colors.comboBox_Shadow)

        // Border
        clientRect.inflate(-metrics.comboBox_ShadowSize)
        subRect = &rects.wnd_Border
        theme.drawSubRect(bc.painter, clientRect, subRect, colors.comboBox_Border, center: false)
    }
}

impl IWnd for ComboBox
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        painter := evt.bc.painter

        theme   := getTheme()
        colors  := getThemeColors()
        metrics := getThemeMetrics()
        rects   := getThemeRects()

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorText:      Color = ?

        subRectBk = &rects.edit_SquareBk
        subRectBorder = &rects.edit_SquareBorder

        switch
        {
        case bc.isDisabled:
            colorBorder = colors.comboBox_DisabledBorder
            colorBk = colors.comboBox_DisabledBk
            colorText = colors.comboBox_DisabledText
        case popup != null:
            colorBorder = colors.comboBox_SelectedBorder
            colorBk = colors.comboBox_SelectedBk
            colorText = colors.comboBox_SelectedText
        case isHot:
            colorBorder = colors.comboBox_HotBorder
            colorBk = colors.comboBox_HotBk
            colorText = colors.comboBox_HotText
        default:
            colorBorder = colors.comboBox_Border
            colorBk = colors.comboBox_Bk
            colorText = colors.comboBox_Text
        }

        clientRect := getClientRect()
        theme.drawSubRect(painter, clientRect, subRectBk, colorBk)
        theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder)

        // Item
        if selectedIdx < items.count
        {
            fntHeight := theme.res.fontDefault.regular.height

            var format: RichStringFormat
            item := items[selectedIdx]
            itemRect := clientRect
            itemRect.width -= metrics.comboBox_RightMargin
            itemRect.moveLeft(metrics.comboBox_LeftMargin)

            if item.icon.isValid()
            {
                item.icon.size = fntHeight
                item.icon.paint(painter, itemRect.x, itemRect.vertCenter() - item.icon.size * 0.5, colorText)
            }

            if hasIcons
            {
                itemRect.moveLeft(fntHeight)
                itemRect.moveLeft(metrics.comboBox_IconTextMargin)
            }

            theme.setTextColors(&format, colorText)
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault
            painter.drawRichString(itemRect, &item.name, format)
        }

        icon := theme.getIcon24(ThemeIcons24.BottomPopupArrow)
        icon.paint(painter, clientRect.right() - metrics.comboBox_RightMargin, clientRect.vertCenter() - icon.size * 0.5, colorText)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        evt.accepted = true
        switch evt.kind
        {
        case MousePressed:
            if evt.button != .Left
                break
            if popup
                hidePopup()
            else
                showPopup()

        case MouseEnter:
            isHot = true
            invalidate()

        case MouseLeave:
            isHot = false
            invalidate()
        }
    }
}

impl ComboBox
{
    mtd showPopup()
    {
        Debug.assert(!popup)
        metrics := getThemeMetrics()

        popup = Wnd.create'ComboBoxPopup(&parent.surface.wnd, position, id)
        popup.padding = metrics.comboBox_BorderSize + metrics.comboBox_ShadowSize
        scrollWnd := ScrollWnd.create'ComboBoxPopupView(popup, @{})
        scrollWnd.dockStyle = .Center
        scrollWnd.setScrollSize(0, cast(f32) items.count * heightItem)
        view := cast(*ComboBoxPopupView) scrollWnd.view
        view.combo = self
        popup.sendCreateEvent()

        pos := getSurfaceRect()
        pt := popup.parent.surfaceToLocal(@{pos.x, pos.y})
        pos.x = pt.x
        pos.y = pt.y + position.height
        pos.width = Math.max(position.width, minWidthPopup)
        pos.height = Math.min(scrollWnd.scrollSize.y, maxHeightPopup)
        pos.height += 2 * metrics.comboBox_BorderSize
        pos.inflate(metrics.comboBox_ShadowSize)
        popup.setPosition(pos)

        scrollWnd.view.setFocus()
    }

    mtd hidePopup()
    {
        Debug.assert(popup != null)
        popup.destroy()
        popup = null
    }
}

public impl ComboBox
{
    #[Swag.Discardable]
    public func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*ComboBox
    {
        pos := position
        if pos.height == 0 pos.height = parent.getThemeMetrics().comboBox_Height
        if pos.width == 0  pos.width  = parent.getThemeMetrics().comboBox_Width
        res := Wnd.create'ComboBox(parent, null, pos, id)
        res.sendCreateEvent()
        return res
    }

    // Add a new item
    mtd addItem(name: string, id: WndId, icon: Icon = @{})
    {
        item := Memory.new'ComboBoxItem()
        with item
        {
            name = #scopefct.name
            icon = #scopefct.icon
            if icon.isValid()
                hasIcons = true
            id   = #scopefct.id
        }

        items.add(item)
    }

    // Set the selected item index
    mtd selectItem(idx: u32)
    {
        newIdx := selectedIdx
        if idx >= items.count
            newIdx = Swag.U32.Max
        else
            newIdx = idx
        if newIdx == selectedIdx
            return

        selectedIdx = newIdx
        if onChanged
            onChanged(self, selectedIdx)

        invalidate()
    }
}