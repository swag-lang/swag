#global public
using Core, Pixel

struct ComboBox
{
    using wnd: Wnd

    minWidthPopup:  f32 = 100
    maxHeightPopup: f32 = 300
    sigChanged:     func(*Self, WndId, *void)

    editBox:        *EditBox
    isHot:          bool
    hasIcons:       bool
    popup:          *PopupList
    items:          ArrayPtr'Item
    selectedIdx     = Swag.U32.Max
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
        popup = PopupList.create(self)
        popup.minWidthPopup = minWidthPopup
        popup.maxHeightPopup = maxHeightPopup

        visit item: items
            popup.addItem(item.name.raw, item.id, item.icon, item.userData)

        popup.sigSelected = @|self|(list, idx) {
            if idx != Swag.U32.Max
                self.selectItem(idx)
            self.hidePopup()
        }

        popup.show(getSurfaceRect())
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
        items.clear()
    }

    // Add a new item
    mtd addItem(name: string, id: WndId, icon: Icon = @{}, userData: *void = null)
    {
        item := items.newPtr()
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