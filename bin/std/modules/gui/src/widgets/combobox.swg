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
        if .editBox
        {
            let metrics    = .getThemeMetrics()
            var clientRect = .getClientRect()
            clientRect.width -= metrics.comboBox_RightMargin
            clientRect.moveLeft(metrics.comboBox_Padding)
            .editBox.setPosition(clientRect)
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let painter = evt.bc.painter

        let theme   = .getTheme()
        let colors  = .getThemeColors()
        let metrics = .getThemeMetrics()
        let rects   = .getThemeRects()

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
        case .popup != null:
            colorBorder = colors.comboBox_SelectedBorder
            colorBk     = colors.comboBox_SelectedBk
            colorText   = colors.comboBox_SelectedText
        case .isHot:
            colorBorder = colors.comboBox_HotBorder
            colorBk     = colors.comboBox_HotBk
            colorText   = colors.comboBox_HotText
        default:
            colorBorder = colors.comboBox_Border
            colorBk     = colors.comboBox_Bk
            colorText   = colors.comboBox_Text
        }

        .aniBk.apply(me, colorBk)
        .aniBorder.apply(me, colorBorder)
        .aniText.apply(me, colorText)

        let clientRect = .getClientRect()
        theme.drawSubRect(painter, clientRect, subRectBk, .aniBk.cur())
        theme.drawSubRect(painter, clientRect, subRectBorder, .aniBorder.cur())

        // Item
        if !.editBox and .selectedIdx < .items.count
        {
            var format:  RichStringFormat
            let item     = .items[.selectedIdx]
            var itemRect = clientRect
            itemRect.width -= metrics.comboBox_RightMargin
            itemRect.moveLeft(metrics.comboBox_Padding)

            if item.icon.isValid()
            {
                let iconColor = item.icon.color.a ? item.icon.color : .aniText.cur()
                item.icon.paint(painter, itemRect.x, itemRect.vertCenter() - item.icon.sizeY * 0.5, iconColor)
            }

            if .maxIconSize do
                itemRect.moveLeft(.maxIconSize + metrics.comboBox_IconTextMargin)

            theme.setTextColors(&format, .aniText.cur())
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = .getFont()
            painter.drawRichString(itemRect, &item.name, format)
        }

        let icon = theme.getIcon24(ThemeIcons24.BottomPopupArrow)
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
            if evt.button != .Left do
                break
            if .popup do
                .hidePopup()
            else do
                .showPopup()

        case MouseEnter:
            .isHot = true
            .invalidate()

        case MouseLeave:
            .isHot = false
            .invalidate()
        }
    }
}

internal impl ComboBox
{
    mtd showPopup()
    {
        .popup                = PopupListCtrl.create(me)
        .popup.minWidthPopup  = .minWidthPopup
        .popup.maxHeightPopup = .maxHeightPopup
        .popup.simFitY        = true
        .popup.selectedIdx    = .selectedIdx

        foreach item in .items
        {
            if item.isSeparator do
                .popup.addSeparator()
            else do
                .popup.addItem(item.name.raw, item.id, item.icon, item.userData0, item.userData1)
        }

        .popup.sigSelected = mtd||(list, idx)
        {
            if idx != Swag.U32.Max do
                .selectItem(idx)
            .hidePopup()
        }

        let rect = .getSurfaceRect()
        .popup.show(rect.x, rect.bottom(), rect.width)
    }

    mtd hidePopup()
    {
        if !.popup do
            return
        .popup.destroy()
        .popup = null
    }
}

impl ComboBox
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null, kind = ComboBoxKind.Select)->*ComboBox
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0 do
            pos.height = parent.getThemeMetrics().comboBox_Height
        if pos.width == 0 do
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
        .maxIconSize = 0
        .items.clear()
        .invalidate()
    }

    // Add a new item
    #[Swag.Discardable]
    mtd addItem(name: #null string, id: WndId = null, icon: Icon = {}, userData0: *void = null, userData1: *void = null)->*ComboBoxItem
    {
        let item = .items.newPtr()
        with item
        {
            .name      = name
            .icon      = icon
            .id        = id
            .userData0 = userData0
            .userData1 = userData1
        }

        if item.icon.isValid() do
            .maxIconSize = Math.max(.maxIconSize, cast(s32) item.icon.sizeX)
        .items.add(item)
        return item
    }

    // Add a separator
    mtd addSeparator()
    {
        let item = .items.newPtr()
        item.isSeparator = true
        .items.add(item)
    }

    // Returns the selected item
    mtd getSelectedItem()->*ComboBoxItem
    {
        Debug.assert(.selectedIdx < .items.count)
        return .items[.selectedIdx]
    }

    // Set the selected item index
    mtd selectItem(idx: u32)
    {
        var newIdx = .selectedIdx
        if idx >= .items.count do
            newIdx = Swag.U32.Max
        else do
            newIdx = idx
        if newIdx == .selectedIdx do
            return

        .selectedIdx = newIdx

        // Notify
        if .selectedIdx < .items.count
        {
            .sigChanged.call(me, .selectedIdx)
            .postCommandEvent(.items[.selectedIdx].id)
        }
        else
        {
            .sigChanged.call(me, Swag.U32.Max)
        }

        if .editBox
        {
            if .selectedIdx < .items.count do
                .editBox.setText(.items[.selectedIdx].name.raw)
            else do
                .editBox.setText("")
        }

        .invalidate()
    }
}
