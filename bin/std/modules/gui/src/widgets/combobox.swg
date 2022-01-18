#global public
using Core, Pixel

struct ComboBox
{
    using wnd: Wnd

    minWidthPopup:  f32 = 100
    maxHeightPopup: f32 = 300
    heightItem:     f32 = 30
    sigChanged:     func(*ComboBox, WndId, *void)

    editBox:        *EditBox
    isHot:          bool
    hasIcons:       bool
    popup:          *ComboBoxPopup
    items:          Array'(*ComboBoxItem)
    selectedIdx:    u32 = Swag.U32.Max
}

struct ComboBoxItem
{
    name:       RichString
    icon:       Icon
    id:         WndId
    userData:   *void
}

struct ComboBoxPopup
{
    using frameWnd: FrameWnd
}

struct ComboBoxPopupView
{
    using wnd:  Wnd
    combo:      *ComboBox
    hotIdx:     u32 = Swag.U32.Max
}

impl IWnd for ComboBoxPopupView
{
    mtd onFocusEvent(evt: *FocusEvent)
    {
        if evt.kind == .KillFocus and evt.other != parent
        {
            combo.hidePopup()
        }
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        var hit: u32

        pos := surfaceToLocal(evt.surfacePos)
        if evt.kind == .MouseLeave
            hit = Swag.U32.Max
        else
            hit = cast(u32) ((pos.y + scrollPos.y) / combo.heightItem)
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
        clientRect.offset(scrollPos)
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
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if editBox
        {
            clientRect := getClientRect()
            clientRect.width -= getThemeMetrics().comboBox_RightMargin
            editBox.setPosition(clientRect)
        }
    }

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
        if !editBox and selectedIdx < items.count
        {
            fntHeight := theme.res.fontDefault.regular.height

            var format: RichStringFormat
            item := items[selectedIdx]
            itemRect := clientRect
            itemRect.width -= metrics.comboBox_RightMargin
            itemRect.moveLeft(metrics.comboBox_Padding)

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

private impl ComboBox
{
    mtd showPopup()
    {
        Debug.assert(!popup)
        metrics := getThemeMetrics()

        popup = Wnd.create'ComboBoxPopup(&parent.surface.wnd, position, id)
        popup.setAllPaddings(metrics.comboBox_BorderSize + metrics.comboBox_ShadowSize)

        scrollWnd := popup.createView'ScrollWnd()
        scrollWnd.dockStyle = .Center
        scrollWnd.focusStrategy = .None
        scrollWnd.smallBarSize = true

        view := scrollWnd.createView'ComboBoxPopupView()
        view.combo = self

        scrollWnd.setScrollSize(0, cast(f32) items.count * heightItem)

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

impl ComboBox
{
    mtd opDrop()
    {
        removeAllItems()
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null, editBox = false)->*ComboBox
    {
        Debug.assert(parent != null)
        pos := position
        if pos.height == 0
            pos.height = parent.getThemeMetrics().comboBox_Height
        if pos.width == 0
            pos.width = parent.getThemeMetrics().comboBox_Width
        res := Wnd.create'ComboBox(parent, null, pos, id)

        if editBox
        {
            res.editBox = EditBox.create(res, null, @{})
            res.editBox.setForm(EditBoxForm.Transparent)
        }

        return res
    }

    // Remove all items
    mtd removeAllItems()
    {
        visit i: items
            Memory.delete(i)
        items.clear()
    }

    // Add a new item
    mtd addItem(name: string, id: WndId, icon: Icon = @{}, userData: *void = null)
    {
        item := Memory.new'ComboBoxItem()
        with item
        {
            name     = #self.name
            icon     = #self.icon
            id       = #self.id
            userData = #self.userData
        }

        if item.icon.isValid()
            hasIcons = true
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

        // Notify
        if selectedIdx < items.count
        {
            if sigChanged
                sigChanged(self, items[selectedIdx].id, items[selectedIdx].userData)
            postCommandEvent(items[selectedIdx].id)
        }
        elif sigChanged
        {
            sigChanged(self, null, null)
        }

        if editBox
        {
            if selectedIdx < items.count
                editBox.setText(items[selectedIdx].name.raw)
            else
                editBox.setText("")
        }

        invalidate()
    }
}