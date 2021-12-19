using Core, Pixel

public struct ComboBoxItem
{
    name:       RichString
    icon:       Icon
    pos:        f32
    size:       f32
    id:         WndId
}

public struct ComboBox
{
    using wnd: Wnd
    minWidthPopup:  f32 = 100
    maxHeightPopup: f32 = 200

    isHot:          bool
    popup:          *ComboBoxPopup

    items:          Array'(*ComboBoxItem)
    selectedIdx:    u32 = Swag.U32.Max
}

public struct ComboBoxPopup
{
    using frameWnd: FrameWnd
    combo: *ComboBox
}

public struct ComboBoxPopupView
{
    using wnd: Wnd
    combo:  *ComboBox
    hotIdx: u32 = Swag.U32.Max
}

impl IWnd for ComboBoxPopupView
{
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
            labelPos.y = y; y += 30
            labelPos.width = clientRect.width
            labelPos.height = 30

            var format: RichStringFormat
            colorText = colors.popupItem_Text

            // Back selection
            if hotIdx == idx
            {
                backPos := labelPos
                backPos.x = metrics.menuPopup_ShadowSize
                backPos.y -= metrics.menuPopup_SpaceLines * 0.5
                backPos.width = position.width - 2 * metrics.menuPopup_ShadowSize
                backPos.height += metrics.menuPopup_SpaceLines
                theme.drawSubRect(painter, backPos, &rects.menuPopup_HotBk, colors.popupItem_HotBk)
                colorText = colors.popupItem_HotText
            }

            // Icon
            if i.icon.isValid()
            {
                i.icon.size = i.size
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.size * 0.5, colorText)
            }

            labelPos.moveLeft(i.size + metrics.menuPopup_IconTextMargin)

            // Text
            format.palette[0] = colorText
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault
            painter.drawRichString(labelPos, &i.name, format)
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
        subRect := &theme.rects.menuPopup_Shadow
        theme.drawSubRect(painter, clientRect, subRect, colors.menuPopup_Shadow)

        // Border
        clientRect.inflate(-metrics.menuPopup_ShadowSize)
        subRect = &rects.wnd_Border
        theme.drawSubRect(bc.painter, clientRect, subRect, colors.wnd_Caption, center: false)
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
        case popup != null:
            colorBorder = colors.comboBox_SelectedBorder
            colorBk = colors.comboBox_SelectedBk
        case isHot:
            colorBorder = colors.comboBox_HotBorder
            colorBk = colors.comboBox_HotBk
        default:
            colorBorder = colors.comboBox_Border
            colorBk = colors.comboBox_Bk
        }

        clientRect := getClientRect()
        theme.drawSubRect(painter, clientRect, subRectBk, colorBk)
        theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder)
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
        popup.padding = metrics.wnd_BorderSize + metrics.menuPopup_ShadowSize
        scrollWnd := ScrollWnd.create'ComboBoxPopupView(popup, @{})
        scrollWnd.dockStyle = .Center
        scrollWnd.setScrollSize(0, 400)
        view := cast(*ComboBoxPopupView) scrollWnd.view
        view.combo = self
        popup.sendCreateEvent()

        pos := getSurfaceRect()
        pt := popup.parent.surfaceToLocal(@{pos.x, pos.y})
        pos.x = pt.x
        pos.y = pt.y + position.height
        pos.width = Math.max(position.width, minWidthPopup)
        pos.height = Math.min(100, maxHeightPopup)
        pos.inflate(metrics.menuPopup_ShadowSize)
        popup.setPosition(pos)
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
            id   = #scopefct.id
        }

        items.add(item)
    }
}