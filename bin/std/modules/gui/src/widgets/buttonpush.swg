#global public
using Core, Pixel

enum ButtonStyle
{
    Normal
    Default
    Flat
}

struct ButtonPush
{
    using button:   Button
    btnStyle:       ButtonStyle = Normal
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
            if btnStyle == .Flat
            {
                colorBk       = colors.btnPushFlat_DisabledBk
                colorBorder   = colors.btnPushFlat_DisabledBorder
                colorText     = colors.btnPushFlat_DisabledText
            }
            else
            {
                colorBk       = colors.btnPush_DisabledBk
                colorBorder   = colors.btnPush_DisabledBorder
                colorText     = colors.btnPush_DisabledText
            }

            subRectBk     = &rects.btnPush_Disabled
            subRectBorder = &rects.btnPush_DisabledBorder

        case isPressed:
            if btnStyle == .Flat
            {
                colorBk       = colors.btnPushFlat_PressedBk
                colorBorder   = colors.btnPushFlat_PressedBorder
                colorText     = colors.btnPushFlat_PressedText
            }
            else if btnStyle == .Default
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
            if btnStyle == .Flat
            {
                colorBk       = colors.btnPushFlat_HotBk
                colorBorder   = colors.btnPushFlat_HotBorder
                colorText     = colors.btnPushFlat_HotText
            }
            else if btnStyle == .Default
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
            if btnStyle == .Flat
            {
                colorBk       = colors.btnPushFlat_Bk
                colorBorder   = colors.btnPushFlat_Border
                colorText     = colors.btnPushFlat_Text
            }
            else if btnStyle == .Default
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
        bc.theme.drawSubRect(bc.painter, clientRect, subRectBk, colorBk)

        // Border
        bc.theme.drawSubRect(bc.painter, clientRect, subRectBorder, colorBorder)

        // Content
        var format: RichStringFormat
        format.palette[0] = colorText
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.flags = .Clip
        format.font = bc.theme.res.fontDefault
        format.fontStyle = .Bold
        labelPos := clientRect
        labelPos.inflate(-bc.theme.metrics.btnPush_Padding)
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