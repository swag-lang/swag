#global public
using Core, Pixel

struct PopupList
{
    using frameWnd: FrameWnd
    heightItem:     f32 = 30
    minWidthPopup:  f32 = 100
    maxHeightPopup: f32 = 300

    sigSelected:    closure(*Self, u32)

    items:          ArrayPtr'Item
    selectedIdx     = Swag.U32.Max
    scrollWnd:      *ScrollWnd
    hasIcons:       bool
}

struct PopupListView
{
    using wnd:  Wnd
    list:       *PopupList
    hotIdx:     u32 = Swag.U32.Max
}

impl IWnd for PopupListView
{
    mtd onFocusEvent(evt: *FocusEvent)
    {
        if evt.kind == .KillFocus and evt.other != parent
        {
            if list.sigSelected
                list.sigSelected(list, Swag.U32.Max)
            else
                list.destroy()
        }
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        var hit: u32

        pos := surfaceToLocal(evt.surfacePos)
        if evt.kind == .MouseLeave
            hit = Swag.U32.Max
        else
            hit = cast(u32) ((pos.y + scrollPos.y) / list.heightItem)
        if hit != hotIdx
        {
            hotIdx = hit
            invalidate()
        }

        if evt.kind == .MousePressed and evt.button == .Left
        {
            if list.sigSelected
                list.sigSelected(list, hotIdx)
            else
                list.destroy()
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
        visit i, idx: list.items
        {
            labelPos.x = clientRect.x
            labelPos.y = y
            labelPos.width = clientRect.width
            labelPos.height = list.heightItem

            var format: RichStringFormat
            colorText = colors.comboBoxItem_Text

            // Back selection
            if hotIdx == idx
            {
                theme.drawSubRect(painter, labelPos, &rects.selectionBox_Bk, colors.comboBoxItem_HotBk)
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

            if list.hasIcons
            {
                labelPos.moveLeft(fntHeight + metrics.comboBox_IconTextMargin)
            }

            if idx == list.selectedIdx
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
            if idx == list.selectedIdx
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

            y += list.heightItem
        }
    }
}

impl IWnd for PopupList
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
        subRect := &theme.rects.popupList_Shadow
        theme.drawSubRect(painter, clientRect, subRect, colors.popupList_Shadow)

        // Border
        clientRect.inflate(-metrics.popupList_ShadowSize)
        subRect = &rects.wnd_Border
        theme.drawSubRect(bc.painter, clientRect, subRect, colors.popupList_Border, center: false)
    }
}

impl PopupList
{
    // Create the popup list, but do not display it
    func create(owner: *Wnd)->*PopupList
    {
        metrics := owner.getThemeMetrics()

        popup := Wnd.create'PopupList(&owner.parent.surface.wnd, owner.position, owner.id)

        with popup
        {
            setAllPaddings(metrics.popupList_BorderSize + metrics.popupList_ShadowSize)
            scrollWnd = createView'ScrollWnd()
            scrollWnd.dockStyle = .Center
            scrollWnd.focusStrategy = .None
            scrollWnd.scrollWndFlags |= .SmallBar
            popupView := scrollWnd.createView'PopupListView()
            popupView.list = popup
        }

        return popup
    }

    // Add a new item in the list
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

    // Show popup
    mtd show(x, y: f32, width: f32 = 10)
    {
        metrics := getThemeMetrics()

        scrollWnd.setScrollSize(0, cast(f32) items.count * heightItem)

        pt := parent.surfaceToLocal(@{x, y})
        pos := Math.Rectangle{pt.x, pt.y, 0, 0}
        pos.width = Math.max(width, minWidthPopup)
        pos.height = Math.min(scrollWnd.scrollSize.y, maxHeightPopup)
        pos.height += 2 * metrics.popupList_BorderSize
        pos.inflate(metrics.popupList_ShadowSize)
        setPosition(pos)

        scrollWnd.view.setFocus()
    }
}
