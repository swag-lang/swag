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

    items:          Array'(*PopupMenuItem)
    selectedIdx:    u32 = Swag.U32.Max
}

public struct ComboBoxPopup
{
    using frameWnd: FrameWnd

    combo:          *ComboBox
    hotIdx:         u32 = Swag.U32.Max
}

impl IWnd for ComboBoxPopup
{
    mtd onCreateEvent(evt: *CreateEvent)
    {
        padding = getThemeMetrics().wnd_BorderSize + getThemeMetrics().menuPopup_ShadowSize
        scrollWnd := ScrollWnd.create'Wnd(self, @{})
        scrollWnd.dockStyle = .Center
        scrollWnd.setScrollSize(0, 400)
    }

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

        if bc.isDisabled
            colorBorder = colors.edit_DisabledBorder
        else if hasFocus()
            colorBorder = colors.edit_FocusBorder
        else if isHot
            colorBorder = colors.edit_HotBorder
        else
            colorBorder = colors.edit_Border

        colorBk = colors.edit_Bk

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
        popup = Wnd.create'ComboBoxPopup(&parent.surface.wnd, position, id)
        popup.combo = self
        popup.sendCreateEvent()

        pos := getSurfaceRect()
        pt := popup.parent.surfaceToLocal(@{pos.x, pos.y})

        pos = position
        pos.x = pt.x
        pos.y = pt.y + position.height
        pos.inflate(getThemeMetrics().menuPopup_ShadowSize)
        pos.width = Math.max(pos.width, minWidthPopup)
        pos.height = Math.min(100, maxHeightPopup)
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
}