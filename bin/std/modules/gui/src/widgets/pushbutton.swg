#global public
using Core, Pixel

enum PushButtonForm
{
    Normal
    Default
    Flat
}

struct PushButton
{
    using button:   Button
    form:           PushButtonForm = Normal
}

impl IWnd for PushButton
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        theme   := getTheme()
        metrics := getThemeMetrics()
        colors  := getThemeColors()
        rects   := getThemeRects()

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorText:      Color = ?

        if form == .Default
        {
            switch
            {
            case isDisabled:
                colorBk       = colors.btnPushDefault_DisabledBk
                colorBorder   = colors.btnPushDefault_DisabledBorder
                colorText     = colors.btnPushDefault_DisabledText
                subRectBk     = &rects.btnPush_Disabled
                subRectBorder = &rects.btnPush_DisabledBorder
            case isPressed:
                colorBk       = colors.btnPushDefault_PressedBk
                colorBorder   = colors.btnPushDefault_PressedBorder
                colorText     = colors.btnPushDefault_PressedText
                subRectBk     = &rects.btnPush_Pressed
                subRectBorder = &rects.btnPush_PressedBorder
            case isHot:
                colorBk       = colors.btnPushDefault_HotBk
                colorBorder   = colors.btnPushDefault_HotBorder
                colorText     = colors.btnPushDefault_HotText
                subRectBk     = &rects.btnPush_Hot
                subRectBorder = &rects.btnPush_HotBorder
            default:
                colorBk       = colors.btnPushDefault_Bk
                colorBorder   = colors.btnPushDefault_Border
                colorText     = colors.btnPushDefault_Text
                subRectBk     = &rects.btnPush_Normal
                subRectBorder = &rects.btnPush_NormalBorder
            }
        }
        elif form == .Flat
        {
            switch
            {
            case isDisabled:
                colorBk       = colors.btnPushFlat_DisabledBk
                colorBorder   = colors.btnPushFlat_DisabledBorder
                colorText     = colors.btnPushFlat_DisabledText
                subRectBk     = &rects.btnPush_Disabled
                subRectBorder = &rects.btnPush_DisabledBorder
            case isPressed:
                colorBk       = colors.btnPushFlat_PressedBk
                colorBorder   = colors.btnPushFlat_PressedBorder
                colorText     = colors.btnPushFlat_PressedText
                subRectBk     = &rects.btnPush_Pressed
                subRectBorder = &rects.btnPush_PressedBorder
            case isHot:
                colorBk       = colors.btnPushFlat_HotBk
                colorBorder   = colors.btnPushFlat_HotBorder
                colorText     = colors.btnPushFlat_HotText
                subRectBk     = &rects.btnPush_Hot
                subRectBorder = &rects.btnPush_HotBorder
            default:
                colorBk       = colors.btnPushFlat_Bk
                colorBorder   = colors.btnPushFlat_Border
                colorText     = colors.btnPushFlat_Text
                subRectBk     = &rects.btnPush_Normal
                subRectBorder = &rects.btnPush_NormalBorder
            }
        }
        else
        {
            switch
            {
            case isDisabled:
                colorBk       = colors.btnPush_DisabledBk
                colorBorder   = colors.btnPush_DisabledBorder
                colorText     = colors.btnPush_DisabledText
                subRectBk     = &rects.btnPush_Disabled
                subRectBorder = &rects.btnPush_DisabledBorder

            case isPressed:
                colorBk       = colors.btnPush_PressedBk
                colorBorder   = colors.btnPush_PressedBorder
                colorText     = colors.btnPush_PressedText
                subRectBk     = &rects.btnPush_Pressed
                subRectBorder = &rects.btnPush_PressedBorder

            case isHot:
                colorBk       = colors.btnPush_HotBk
                colorBorder   = colors.btnPush_HotBorder
                colorText     = colors.btnPush_HotText
                subRectBk     = &rects.btnPush_Hot
                subRectBorder = &rects.btnPush_HotBorder

            default:
                colorBk       = colors.btnPush_Bk
                colorBorder   = colors.btnPush_Border
                colorText     = colors.btnPush_Text
                subRectBk     = &rects.btnPush_Normal
                subRectBorder = &rects.btnPush_NormalBorder
            }
        }

        // Background
        clientRect := getClientRect()
        theme.drawSubRect(bc.painter, clientRect, subRectBk, colorBk)

        // Border
        theme.drawSubRect(bc.painter, clientRect, subRectBorder, colorBorder)

        // Content
        var format: RichStringFormat
        theme.setTextColors(&format, colorText)
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
        if processMouseEvent(evt)
            postCommandEvent(id)
    }
}

impl PushButton
{
    #[Swag.Discardable]
    func create(parent: *Wnd, name: string, position: Math.Rectangle, id: string = null)->*PushButton
    {
        res := Wnd.create'PushButton(parent, name, position, id)
        if res.position.height == 0
            res.position.height = res.getThemeMetrics().btnPush_Height
        if res.position.width == 0
            res.position.width  = res.getThemeMetrics().btnPush_Width
        return res
    }
}