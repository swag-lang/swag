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

    isHot:          bool

    items:          Array'(*PopupMenuItem)
    selectedIdx:    u32 = Swag.U32.Max
}

public struct ComboBoxPopup
{
    using wnd:  Wnd
    combo:      *ComboBox
    hotIdx:     u32 = Swag.U32.Max
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

        painter.fillRect(getClientRect(), Argb.Red)
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
            showPopup()

        case MouseReleased:
            if evt.button != .Left
                break

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
        swnd := ScrollWnd.create'ComboBoxPopup(&parent.surface.wnd, position, id)
        popup := cast(*ComboBoxPopup) swnd.view
        popup.combo = self

        rect := getSurfaceRect()
        pt := swnd.parent.surfaceToLocal(@{rect.x, rect.y})

        pos := position
        pos.x = pt.x
        pos.y = pt.y + position.height
        pos.height = 100
        swnd.setPosition(pos)
    }

    mtd hidePopup()
    {

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