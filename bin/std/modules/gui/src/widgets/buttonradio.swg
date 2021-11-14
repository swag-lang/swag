#global public
using Core, Pixel

struct ButtonRadio
{
    using button:   Button
    isChecked:      bool
}

impl IWnd for ButtonRadio
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc     := evt.bc
        theme  := bc.theme
        colors := &bc.theme.colors
        rects  := &bc.theme.rects

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
        posBox := position
        posBox.height = getThemeMetrics().btnCheck_Size
        posBox.y = position.vertCenter() - (posBox.height * 0.5)
        posBox.width = posBox.height
        bc.theme.drawSubRect(bc.painter, posBox, subRectBk, colorBk)
        bc.theme.drawSubRect(bc.painter, posBox, subRectBorder, colorBorder)
        bc.theme.drawSubRect(bc.painter, posBox, subRectMark, colorMark)

        // Content
        var format: RichStringFormat
        format.palette[0] = colorText
        format.horzAlignment = .Left
        format.vertAlignment = .Center
        format.flags = .Clip
        format.font = bc.theme.res.fontDefault
        labelPos := position
        labelPos.x += posBox.width + bc.theme.metrics.btnCheck_Padding
        labelPos.width -= posBox.width + bc.theme.metrics.btnCheck_Padding
        Theme.drawIconText(bc.painter, labelPos, icon, &name, format, bc.theme.metrics.iconText_Margin)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if processMouseEvent(evt)
        {
            isChecked = true
        }
    }
}

impl ButtonRadio
{
    func create(parent: *Wnd, name: string, position: Math.Rectangle)->*ButtonRadio
    {
        res := Wnd.create'ButtonRadio(parent, name, position)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }
}