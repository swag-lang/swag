#global public
using Core, Pixel

struct ButtonRadio
{
    using button:   Button
    isChecked:      bool
    sigChanged:     func(*ButtonRadio)
}

impl IWnd for ButtonRadio
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc     := evt.bc
        theme   := getTheme()
        metrics := getThemeMetrics()
        colors  := getThemeColors()
        rects   := getThemeRects()

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var subRectMark:    *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorMark:      Color = ?
        var colorText:      Color = ?

        switch
        {
        case isDisabled:
            colorBk       = colors.btnCheck_DisabledBk
            colorBorder   = colors.btnCheck_DisabledBorder
            colorMark     = colors.btnCheck_DisabledMark
            colorText     = colors.btnCheck_DisabledText
        case isPressed:
            colorBk       = colors.btnCheck_PressedBk
            colorBorder   = colors.btnCheck_PressedBorder
            colorMark     = colors.btnCheck_PressedMark
            colorText     = colors.btnCheck_PressedText
        case isHot:
            colorBk       = colors.btnCheck_HotBk
            colorBorder   = colors.btnCheck_HotBorder
            colorMark     = colors.btnCheck_HotMark
            colorText     = colors.btnCheck_HotText
        default:
            colorBk       = colors.btnCheck_Bk
            colorBorder   = colors.btnCheck_Border
            colorMark     = colors.btnCheck_Mark
            colorText     = colors.btnCheck_Text
        }

        if isChecked
        {
            subRectBk     = &rects.btnRadio_Checked
            subRectBorder = &rects.btnRadio_CheckedBorder
            subRectMark   = &rects.btnRadio_CheckedMark
        }
        else
        {
            subRectBk     = &rects.btnRadio_Normal
            subRectBorder = &rects.btnRadio_NormalBorder
        }

        // Box
        clientRect := getClientRect()
        posBox := clientRect
        posBox.height = metrics.btnCheck_Size
        posBox.y = clientRect.vertCenter() - (posBox.height * 0.5)
        posBox.width = posBox.height
        theme.drawSubRect(bc.painter, posBox, subRectBk, colorBk)
        theme.drawSubRect(bc.painter, posBox, subRectBorder, colorBorder)
        theme.drawSubRect(bc.painter, posBox, subRectMark, colorMark)

        // Content
        var format: RichStringFormat
        format.palette[0] = colorText
        format.horzAlignment = .Left
        format.vertAlignment = .Center
        format.flags = .Clip
        format.font = theme.res.fontDefault
        labelPos := clientRect
        labelPos.x += posBox.width + metrics.btnCheck_Padding
        labelPos.width -= posBox.width + metrics.btnCheck_Padding
        Theme.drawIconText(bc.painter, labelPos, icon, &name, format, metrics.iconText_Margin)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if processMouseEvent(evt) and !isChecked
        {
            isChecked = true
            if sigChanged
                sigChanged(self)
        }
    }
}

impl ButtonRadio
{
    func create(parent: *Wnd, name: string, position: Math.Rectangle, id: string = null)->*ButtonRadio
    {
        pos := position
        if pos.height == 0 pos.height = parent.getThemeMetrics().btnPush_Height
        if pos.width == 0  pos.width  = parent.getThemeMetrics().btnPush_Width
        res := Wnd.create'ButtonRadio(parent, name, pos, id)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }
}