#global public
using Core, Pixel

enum ComboBoxKind
{
    Select
    Edit
    CheckBox
}

struct ComboBoxItem
{
    using base: Item
    isSeparator: bool
}

struct ComboBox
{
    using wnd: Wnd

    kind            = ComboBoxKind.Select
    minWidthPopup:  f32 = 100
    maxHeightPopup: f32 = 300
    sigChanged:     SigArray'closure(*Self, u32)

    editBox:        *EditBox
    isHot:          bool
    popup:          *PopupListCtrl
    items:          ArrayPtr'ComboBoxItem
    selectedIdx     = Swag.U32.Max
    maxIconSize:    s32

    aniBk:      BlendColor
    aniBorder:  BlendColor
    aniText:    BlendColor
}

impl IWnd for ComboBox
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if editBox
        {
            metrics := getThemeMetrics()
            clientRect := getClientRect()
            clientRect.width -= metrics.comboBox_RightMargin
            clientRect.moveLeft(metrics.comboBox_Padding)
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
        var colorBk:        Color = undefined
        var colorBorder:    Color = undefined
        var colorText:      Color = undefined

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

        aniBk.apply(self, colorBk)
        aniBorder.apply(self, colorBorder)
        aniText.apply(self, colorText)

        clientRect := getClientRect()
        theme.drawSubRect(painter, clientRect, subRectBk, aniBk.cur())
        theme.drawSubRect(painter, clientRect, subRectBorder, aniBorder.cur())

        // Item
        if !editBox and selectedIdx < items.count
        {
            var format: RichStringFormat
            item := items[selectedIdx]
            itemRect := clientRect
            itemRect.width -= metrics.comboBox_RightMargin
            itemRect.moveLeft(metrics.comboBox_Padding)

            if item.icon.isValid()
                item.icon.paint(painter, itemRect.x, itemRect.vertCenter() - item.icon.sizeY * 0.5, aniText.cur())
            if maxIconSize
                itemRect.moveLeft(maxIconSize + metrics.comboBox_IconTextMargin)

            theme.setTextColors(&format, aniText.cur())
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = getFont()
            painter.drawRichString(itemRect, &item.name, format)
        }

        icon := theme.getIcon24(ThemeIcons24.BottomPopupArrow)
        icon.paint(painter, clientRect.right() - metrics.comboBox_RightMargin, clientRect.vertCenter() - icon.sizeY * 0.5, colorText)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false
            return

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
        popup = PopupListCtrl.create(self)
        popup.minWidthPopup = minWidthPopup
        popup.maxHeightPopup = maxHeightPopup
        popup.simFitY = true
        popup.selectedIdx = selectedIdx

        visit item: items
        {
            if item.isSeparator
                popup.addSeparator()
            else
                popup.addItem(item.name.raw, item.id, item.icon, item.userData0, item.userData1)
        }

        popup.sigSelected = closure|self|(list, idx) {
            if idx != Swag.U32.Max
                self.selectItem(idx)
            self.hidePopup()
        }

        rect := getSurfaceRect()
        popup.show(rect.x, rect.bottom(), rect.width)
    }

    mtd hidePopup()
    {
        if !popup
            return
        popup.destroy()
        popup = null
    }
}

impl ComboBox
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null, kind = ComboBoxKind.Select)->*ComboBox
    {
        Debug.assert(parent != null)
        pos := position
        if pos.height == 0
            pos.height = parent.getThemeMetrics().comboBox_Height
        if pos.width == 0
            pos.width = parent.getThemeMetrics().comboBox_Width
        res := Wnd.create'ComboBox(parent, null, pos, id)

        res.kind = kind
        if res.kind == .Edit
        {
            res.editBox = EditBox.create(res, null, {})
            res.editBox.setForm(EditBoxForm.Transparent)
        }

        res.sendResizeEvent()
        return res
    }

    // Remove all items
    mtd clear()
    {
        maxIconSize = 0
        items.clear()
        invalidate()
    }

    // Add a new item
    #[Swag.Discardable]
    mtd addItem(name: string, id: WndId = null, icon: Icon = {}, userData0: *void = null, userData1: *void = null)->*ComboBoxItem
    {
        item := items.newPtr()
        with item
        {
            .name      = #self.name
            .icon      = #self.icon
            .id        = #self.id
            .userData0 = #self.userData0
            .userData1 = #self.userData1
        }

        if item.icon.isValid()
            maxIconSize = Math.max(maxIconSize, cast(s32) item.icon.sizeX)
        items.add(item)
        return item
    }

    // Add a separator
    mtd addSeparator()
    {
        item := items.newPtr()
        item.isSeparator = true
        items.add(item)
    }

    // Returns the selected item
    mtd getSelectedItem()->*ComboBoxItem
    {
        if selectedIdx >= items.count
            return null
        return items[selectedIdx]
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
            sigChanged.call(self, selectedIdx)
            postCommandEvent(items[selectedIdx].id)
        }
        else
        {
            sigChanged.call(self, Swag.U32.Max)
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