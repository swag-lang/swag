#global public
using Core, Pixel

struct PopupListCtrl
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
    list:       *PopupListCtrl
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

        surfaceRect := getSurfaceRect()
        mouseInRect := surfaceRect.contains(evt.surfacePos)

        pos := surfaceToLocal(evt.surfacePos)
        if evt.kind == .MouseLeave or !mouseInRect
            hit = Swag.U32.Max
        else
            hit = cast(u32) ((pos.y + scrollPos.y) / list.heightItem)
        if hit != hotIdx
        {
            hotIdx = hit
            invalidate()
        }

        if evt.kind == .MousePressed and evt.button == .Left and mouseInRect
        {
            if list.sigSelected
                list.sigSelected(list, hotIdx)
            releaseMouse()
            list.destroy()
            return
        }

        surfaceRect = parent.getSurfaceRect()
        mouseInRect = surfaceRect.contains(evt.surfacePos)
        if evt.kind == .MousePressed and evt.button == .Left and !mouseInRect
        {
            if list.sigSelected
                list.sigSelected(list, Swag.U32.Max)
            releaseMouse()
            list.destroy()
            return
        }

        if evt.kind == .MouseLeave
            evt.accepted = true
        else
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
                i.icon.sizeX = fntHeight
                i.icon.sizeY = fntHeight
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.sizeY * 0.5, colorText)
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

impl PopupListCtrl
{
    // Create the popup list, but do not display it
    func create(owner: *Wnd)->*PopupListCtrl
    {
        metrics := owner.getThemeMetrics()

        popup := Wnd.create'PopupListCtrl(owner.parent.surface.wnd, owner.position, owner.id)

        with popup
        {
            popup.setFrameFlags(FrameWndFlags.SmallShadow)
            setFrameForm(FrameWndForm.Square)
            scrollWnd = createView'ScrollWnd()
            scrollWnd.dockStyle = .Center
            scrollWnd.focusStrategy = .None
            scrollWnd.scrollWndFlags |= .SmallBar
            popupView := scrollWnd.createView'PopupListView()
            popupView.list = popup
        }

        return popup
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
        pos.height += padding.y + padding.w

        // Be sure popup is in the client area of the top view
        topView := getTopView()
        if pos.x + pos.width > topView.position.width
            pos.x = topView.position.width - pos.width
        if pos.y + pos.height > topView.position.height
            pos.y = topView.position.height - pos.height

        setPosition(pos)

        //scrollWnd.view.setFocus()
        scrollWnd.view.captureMouse()
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
}
