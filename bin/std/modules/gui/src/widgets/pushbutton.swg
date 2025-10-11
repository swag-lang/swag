#global public
using Core, Pixel

enum PushButtonForm
{
    Normal
    Default
    Flat
    Strong
}

struct PushButton
{
    using button:     Button

    form              = PushButtonForm.Normal

    aniBk:            BlendColor
    aniBorder:        BlendColor
    aniText:          BlendColor
}

impl IWnd for PushButton
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let theme   = .getTheme()
        let metrics = .getThemeMetrics()
        let colors  = .getThemeColors()
        let rects   = .getThemeRects()

        var subRectBk:     *ThemeImageRect
        var subRectBorder: *ThemeImageRect
        var colorBk:       Color = undefined
        var colorBorder:   Color = undefined
        var colorText:     Color = undefined

        if .form == .Default
        {
            switch
            {
            case bc.isDisabled:
                colorBk       = colors.btnPushDefault_DisabledBk
                colorBorder   = colors.btnPushDefault_DisabledBorder
                colorText     = colors.btnPushDefault_DisabledText
                subRectBk     = &rects.btnPush_Disabled
                subRectBorder = &rects.btnPush_DisabledBorder
            case .isPressed:
                colorBk       = colors.btnPushDefault_PressedBk
                colorBorder   = colors.btnPushDefault_PressedBorder
                colorText     = colors.btnPushDefault_PressedText
                subRectBk     = &rects.btnPush_Pressed
                subRectBorder = &rects.btnPush_PressedBorder
            case .isHot:
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
        elif .form == .Flat
        {
            switch
            {
            case bc.isDisabled:
                colorBk       = colors.btnPushFlat_DisabledBk
                colorBorder   = colors.btnPushFlat_DisabledBorder
                colorText     = colors.btnPushFlat_DisabledText
                subRectBk     = &rects.btnPush_Disabled
                subRectBorder = &rects.btnPush_DisabledBorder
            case .isPressed:
                colorBk       = colors.btnPushFlat_PressedBk
                colorBorder   = colors.btnPushFlat_PressedBorder
                colorText     = colors.btnPushFlat_PressedText
                subRectBk     = &rects.btnPush_Pressed
                subRectBorder = &rects.btnPush_PressedBorder
            case .isHot:
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
            case bc.isDisabled:
                colorBk       = colors.btnPush_DisabledBk
                colorBorder   = colors.btnPush_DisabledBorder
                colorText     = colors.btnPush_DisabledText
                subRectBk     = &rects.btnPush_Disabled
                subRectBorder = &rects.btnPush_DisabledBorder

            case .isPressed:
                colorBk       = colors.btnPush_PressedBk
                colorBorder   = colors.btnPush_PressedBorder
                colorText     = colors.btnPush_PressedText
                subRectBk     = &rects.btnPush_Pressed
                subRectBorder = &rects.btnPush_PressedBorder

            case .isHot:
                colorBk       = colors.btnPush_HotBk
                colorBorder   = colors.btnPush_HotBorder
                colorText     = colors.btnPush_HotText
                subRectBk     = &rects.btnPush_Hot
                subRectBorder = &rects.btnPush_HotBorder

            default:
                if .form == .Strong
                {
                    colorBk     = colors.btnPush_StrongBk
                    colorBorder = colors.btnPush_StrongBorder
                    colorText   = colors.btnPush_StrongText
                }
                else
                {
                    colorBk     = colors.btnPush_Bk
                    colorBorder = colors.btnPush_Border
                    colorText   = colors.btnPush_Text
                }

                subRectBk     = &rects.btnPush_Normal
                subRectBorder = &rects.btnPush_NormalBorder
            }
        }

        // Background
        let clientRect = .getClientRect()
        .aniBk.apply(me, colorBk)
        theme.drawSubRect(bc.painter, clientRect, subRectBk, .aniBk.cur())

        // Border
        .aniBorder.apply(me, colorBorder)
        theme.drawSubRect(bc.painter, clientRect, subRectBorder, .aniBorder.cur())

        // Content
        var format: RichStringFormat
        .aniText.apply(me, colorText)
        theme.setTextColors(&format, .aniText.cur())
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.flags         = .Clip
        format.font          = .getFont()
        if .form == .Strong do
            format.fontStyle = .Bold
        var labelPos = clientRect
        labelPos.inflate(-metrics.btnPush_Padding)
        Theme.drawIconText(bc.painter, labelPos, .icon, &.name, format, metrics.iconText_Margin)
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if .processMouseEvent(evt) do
            .postCommandEvent(.id)
    }
}

impl PushButton
{
    #[Swag.Discardable]
    func create(parent: *Wnd, name: string, position: Math.Rectangle = {}, id: WndId = null)->*PushButton
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0 do
            pos.height = parent.getThemeMetrics().btnPush_Height
        if pos.width == 0 do
            pos.width = parent.getThemeMetrics().btnPush_Width
        let res = Wnd.create'PushButton(parent, name, pos, id)
        if id != null do
            res.updateCommandState()
        return res
    }
}
