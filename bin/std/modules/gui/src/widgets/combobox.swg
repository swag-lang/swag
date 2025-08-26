#global marked
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
    using base:      Item
    isSeparator:     bool
}

struct ComboBox
{
    using wnd:          Wnd

    kind                = ComboBoxKind.Select
    minWidthPopup:      f32 = 100
    maxHeightPopup:     f32 = 300
    sigChanged:         SigArray'func||(*ComboBox, u32)

    editBox:            *EditBox
    isHot:              bool
    popup:              *PopupListCtrl
    items:              ArrayPtr'ComboBoxItem
    selectedIdx         = Swag.U32.Max
    maxIconSize:        s32

    aniBk:              BlendColor
    aniBorder:          BlendColor
    aniText:            BlendColor
}

impl IWnd for ComboBox
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if me.editBox
        {
            let metrics    = me.getThemeMetrics()
            var clientRect = me.getClientRect()
            clientRect.width -= metrics.comboBox_RightMargin
            clientRect.moveLeft(metrics.comboBox_Padding)
            me.editBox.setPosition(clientRect)
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let painter = evt.bc.painter

        let theme   = me.getTheme()
        let colors  = me.getThemeColors()
        let metrics = me.getThemeMetrics()
        let rects   = me.getThemeRects()

        var subRectBk:     *ThemeImageRect
        var subRectBorder: *ThemeImageRect
        var colorBk:       Color = undefined
        var colorBorder:   Color = undefined
        var colorText:     Color = undefined

        subRectBk     = &rects.edit_SquareBk
        subRectBorder = &rects.edit_SquareBorder

        switch
        {
        case bc.isDisabled:
            colorBorder = colors.comboBox_DisabledBorder
            colorBk     = colors.comboBox_DisabledBk
            colorText   = colors.comboBox_DisabledText
        case me.popup != null:
            colorBorder = colors.comboBox_SelectedBorder
            colorBk     = colors.comboBox_SelectedBk
            colorText   = colors.comboBox_SelectedText
        case me.isHot:
            colorBorder = colors.comboBox_HotBorder
            colorBk     = colors.comboBox_HotBk
            colorText   = colors.comboBox_HotText
        default:
            colorBorder = colors.comboBox_Border
            colorBk     = colors.comboBox_Bk
            colorText   = colors.comboBox_Text
        }

        me.aniBk.apply(me, colorBk)
        me.aniBorder.apply(me, colorBorder)
        me.aniText.apply(me, colorText)

        var clientRect = me.getClientRect()
        theme.drawSubRect(painter, clientRect, subRectBk, me.aniBk.cur())
        theme.drawSubRect(painter, clientRect, subRectBorder, me.aniBorder.cur())

        // Item
        if !me.editBox and me.selectedIdx < me.items.count
        {
            var format:  RichStringFormat
            let item     = me.items[me.selectedIdx]
            var itemRect = clientRect
            itemRect.width -= metrics.comboBox_RightMargin
            itemRect.moveLeft(metrics.comboBox_Padding)

            if item.icon.isValid():
                item.icon.paint(painter, itemRect.x, itemRect.vertCenter() - item.icon.sizeY * 0.5, me.aniText.cur())
            if me.maxIconSize:
                itemRect.moveLeft(me.maxIconSize + metrics.comboBox_IconTextMargin)

            theme.setTextColors(&format, me.aniText.cur())
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = me.getFont()
            painter.drawRichString(itemRect, &item.name, format)
        }

        var icon = theme.getIcon24(ThemeIcons24.BottomPopupArrow)
        icon.paint(painter, clientRect.right() - metrics.comboBox_RightMargin, clientRect.vertCenter() - icon.sizeY * 0.5, colorText)
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false
            return

        case MousePressed:
            if evt.button != .Left:
                break
            if me.popup:
                me.hidePopup()
            else:
                me.showPopup()

        case MouseEnter:
            me.isHot = true
            me.invalidate()

        case MouseLeave:
            me.isHot = false
            me.invalidate()
        }
    }
}

internal impl ComboBox
{
    mtd showPopup()
    {
        me.popup                = PopupListCtrl.create(me)
        me.popup.minWidthPopup  = me.minWidthPopup
        me.popup.maxHeightPopup = me.maxHeightPopup
        me.popup.simFitY        = true
        me.popup.selectedIdx    = me.selectedIdx

        foreach item in me.items
        {
            if item.isSeparator:
                me.popup.addSeparator()
            else:
                me.popup.addItem(item.name.raw, item.id, item.icon, item.userData0, item.userData1)
        }

        me.popup.sigSelected = mtd||(list, idx)
        {
            if idx != Swag.U32.Max:
                me.selectItem(idx)
            me.hidePopup()
        }

        var rect = me.getSurfaceRect()
        me.popup.show(rect.x, rect.bottom(), rect.width)
    }

    mtd hidePopup()
    {
        if !me.popup:
            return
        me.popup.destroy()
        me.popup = null
    }
}

impl ComboBox
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null, kind = ComboBoxKind.Select)->*ComboBox
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0:
            pos.height = parent.getThemeMetrics().comboBox_Height
        if pos.width == 0:
            pos.width = parent.getThemeMetrics().comboBox_Width
        let res = Wnd.create'ComboBox(parent, null, pos, id)

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
        me.maxIconSize = 0
        me.items.clear()
        me.invalidate()
    }

    // Add a new item
    #[Swag.Discardable]
    mtd addItem(name: #null string, id: WndId = null, icon: Icon = {}, userData0: *void = null, userData1: *void = null)->*ComboBoxItem
    {
        let item = me.items.newPtr()
        with item
        {
            .name      = #me.name
            .icon      = #me.icon
            .id        = #me.id
            .userData0 = #me.userData0
            .userData1 = #me.userData1
        }

        if item.icon.isValid():
            me.maxIconSize = Math.max(me.maxIconSize, cast(s32) item.icon.sizeX)
        me.items.add(item)
        return item
    }

    // Add a separator
    mtd addSeparator()
    {
        let item = me.items.newPtr()
        item.isSeparator = true
        me.items.add(item)
    }

    // Returns the selected item
    mtd getSelectedItem()->*ComboBoxItem
    {
        Debug.assert(me.selectedIdx < me.items.count)
        return me.items[me.selectedIdx]
    }

    // Set the selected item index
    mtd selectItem(idx: u32)
    {
        var newIdx = me.selectedIdx
        if idx >= me.items.count:
            newIdx = Swag.U32.Max
        else:
            newIdx = idx
        if newIdx == me.selectedIdx:
            return

        me.selectedIdx = newIdx

        // Notify
        if me.selectedIdx < me.items.count
        {
            me.sigChanged.call(me, me.selectedIdx)
            me.postCommandEvent(me.items[me.selectedIdx].id)
        }
        else
        {
            me.sigChanged.call(me, Swag.U32.Max)
        }

        if me.editBox
        {
            if me.selectedIdx < me.items.count:
                me.editBox.setText(me.items[me.selectedIdx].name.raw)
            else:
                me.editBox.setText("")
        }

        me.invalidate()
    }
}
