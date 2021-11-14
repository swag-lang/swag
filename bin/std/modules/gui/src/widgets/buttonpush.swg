#global public
using Core, Pixel

struct ButtonPush
{
    using button:   Button
    isDefault:      bool
    isFlat:         bool
}

impl IWnd for ButtonPush
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc     := evt.bc
        theme  := bc.theme
        colors := &bc.theme.colors
        rects  := &bc.theme.rects

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorText:      Color = ?

        switch
        {
        case isDisabled:
            colorBk       = colors.btnPush_DisabledBk
            colorBorder   = colors.btnPush_DisabledBorder
            colorText     = colors.btnPush_DisabledText
            subRectBk     = &rects.btnPush_Disabled
            subRectBorder = &rects.btnPush_DisabledBorder
        case isPressed:
            colorBk       = isDefault ? colors.btnPushDefault_PressedBk : colors.btnPush_PressedBk
            colorBorder   = isDefault ? colors.btnPushDefault_PressedBorder : colors.btnPush_PressedBorder
            colorText     = isDefault ? colors.btnPushDefault_PressedText : colors.btnPush_PressedText
            subRectBk     = &rects.btnPush_Pressed
            subRectBorder = &rects.btnPush_PressedBorder
        case isHot:
            colorBk       = isDefault ? colors.btnPushDefault_HotBk : colors.btnPush_HotBk
            colorBorder   = isDefault ? colors.btnPushDefault_HotBorder : colors.btnPush_HotBorder
            colorText     = isDefault ? colors.btnPushDefault_HotText : colors.btnPush_HotText
            subRectBk     = &rects.btnPush_Hot
            subRectBorder = &rects.btnOush_HotBorder
        default:
            colorBk       = isDefault ? colors.btnPushDefault_Bk : colors.btnPush_Bk
            colorBorder   = isDefault ? colors.btnPushDefault_Border : colors.btnPush_Border
            colorText     = isDefault ? colors.btnPushDefault_Text : colors.btnPush_Text
            subRectBk     = &rects.btnPush_Normal
            subRectBorder = &rects.btnPush_NormalBorder
        }

        // Background
        bc.theme.drawSubRect(bc.painter, position, subRectBk, colorBk)

        // Border
        if !isFlat
            bc.theme.drawSubRect(bc.painter, position, subRectBorder, colorBorder)

        // Content
        var format: RichStringFormat
        format.palette[0] = colorText
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.flags = .Clip
        format.font = bc.theme.res.fontDefault
        format.fontStyle = .Bold
        labelPos := position
        labelPos.inflate(bc.theme.metrics.btn_Margins)
        Theme.drawIconText(bc.painter, labelPos, icon, &name, format, bc.theme.metrics.iconText_Margin)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        discard processMouseEvent(evt)
    }
}

impl ButtonPush
{
    func create(parent: *Wnd, name: string, position: Math.Rectangle, id: string = null)->*ButtonPush
    {
        pos := position
        if pos.height == 0 pos.height = parent.getThemeMetrics().btnPush_Height
        if pos.width == 0  pos.width  = parent.getThemeMetrics().btnPush_Width
        res := Wnd.create'ButtonPush(parent, name, pos, id)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }
}