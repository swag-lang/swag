using Core, Pixel

public struct PushButton
{
    using button:   Button
    isDefault:      bool
}

impl IWnd for PushButton
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
            if isDefault
            {
                colorBk       = colors.btnPushDefault_PressedBk
                colorBorder   = colors.btnPushDefault_PressedBorder
                colorText     = colors.btnPushDefault_PressedText
            }
            else
            {
                colorBk       = colors.btnPush_PressedBk
                colorBorder   = colors.btnPush_PressedBorder
                colorText     = colors.btnPush_PressedText
            }

            subRectBk     = &rects.btnPush_Pressed
            subRectBorder = &rects.btnPush_PressedBorder

        case isHot:
            if isDefault
            {
                colorBk       = colors.btnPushDefault_HotBk
                colorBorder   = colors.btnPushDefault_HotBorder
                colorText     = colors.btnPushDefault_HotText
            }
            else
            {
                colorBk       = colors.btnPush_HotBk
                colorBorder   = colors.btnPush_HotBorder
                colorText     = colors.btnPush_HotText
            }

            subRectBk     = &rects.btnPush_Hot
            subRectBorder = &rects.btnOush_HotBorder

        default:
            if isDefault
            {
                colorBk       = colors.btnPushDefault_Bk
                colorBorder   = colors.btnPushDefault_Border
                colorText     = colors.btnPushDefault_Text
            }
            else
            {
                colorBk       = colors.btnPush_Bk
                colorBorder   = colors.btnPush_Border
                colorText     = colors.btnPush_Text
            }

            subRectBk     = &rects.btnPush_Normal
            subRectBorder = &rects.btnPush_NormalBorder
        }

        // Background
        clientRect := getClientRect()
        theme.drawSubRect(bc.painter, clientRect, subRectBk, colorBk)

        // Border
        theme.drawSubRect(bc.painter, clientRect, subRectBorder, colorBorder)

        // Content
        var format: RichStringFormat
        format.palette[0] = colorText
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.flags = .Clip
        format.font = theme.res.fontDefault
        format.fontStyle = .Bold
        labelPos := clientRect
        labelPos.inflate(-metrics.btnPush_Padding)
        Theme.drawIconText(bc.painter, labelPos, icon, &name, format, metrics.iconText_Margin)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        discard processMouseEvent(evt)
    }
}

impl PushButton
{
    public func create(parent: *Wnd, name: string, position: Math.Rectangle, id: string = null)->*PushButton
    {
        res := Wnd.create'PushButton(parent, name, position, id)
        if res.position.height == 0
            res.position.height = res.getThemeMetrics().btnPush_Height
        if res.position.width == 0
            res.position.width  = res.getThemeMetrics().btnPush_Width

        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }
}