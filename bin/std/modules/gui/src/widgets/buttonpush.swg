#global public
using Core, Pixel

struct ButtonPush
{
    using base:     Widget
    icon:           Icon
    isDefault:      bool
    isFlat:         bool
    sigPressed:     func(*ButtonPush)

    isPressed:      bool
    isPressing:     bool
    isHot:          bool
    isIn:           bool
}

impl IWnd for ButtonPush
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        theme   := bc.theme
        colors  := bc.colors
        metrics := bc.metrics

        var subRectBk:      *ImageSubRect
        var subRectBorder:  *ImageSubRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorText:      Color = ?

        switch
        {
        case isDisabled:
            colorBk       = colors.btnPush_DisabledBk
            colorBorder   = colors.btnPush_DisabledBorder
            colorText     = colors.btnPush_DisabledText
            subRectBk     = &metrics.btnPush_Disabled
            subRectBorder = &metrics.btnPush_DisabledBorder
        case isPressed:
            colorBk       = isDefault ? colors.btnPushDefault_PressedBk : colors.btnPush_PressedBk
            colorBorder   = isDefault ? colors.btnPushDefault_PressedBorder : colors.btnPush_PressedBorder
            colorText     = isDefault ? colors.btnPushDefault_PressedText : colors.btnPush_PressedText
            subRectBk     = &metrics.btnPush_Pressed
            subRectBorder = &metrics.btnPush_PressedBorder
        case isHot:
            colorBk       = isDefault ? colors.btnPushDefault_HotBk : colors.btnPush_HotBk
            colorBorder   = isDefault ? colors.btnPushDefault_HotBorder : colors.btnPush_HotBorder
            colorText     = isDefault ? colors.btnPushDefault_HotText : colors.btnPush_HotText
            subRectBk     = &metrics.btnPush_Hot
            subRectBorder = &metrics.btnOush_HotBorder
        default:
            colorBk       = isDefault ? colors.btnPushDefault_Bk : colors.btnPush_Bk
            colorBorder   = isDefault ? colors.btnPushDefault_Border : colors.btnPush_Border
            colorText     = isDefault ? colors.btnPushDefault_Text : colors.btnPush_Text
            subRectBk     = &metrics.btnPush_Normal
            subRectBorder = &metrics.btnPush_NormalBorder
        }

        // Background
        bc.theme.drawSubRect(bc.painter, position, subRectBk, colorBk)

        // Border
        if !isFlat
            bc.theme.drawSubRect(bc.painter, position, subRectBorder, colorBorder)

        // Content
        var labelStyle: LabelPaintStyle
        labelStyle.colorText = colorText
        labelStyle.stringFormat.horzAlignment = .Center
        labelStyle.stringFormat.vertAlignment = .Center
        labelStyle.stringFormat.flags = .Clip
        labelStyle.font = bc.theme.fontDefault.bold
        labelStyle.iconMargin = bc.theme.iconTextMargin
        labelPos := position
        labelPos.inflate(bc.theme.btnMargins)
        Label.paint(bc, labelPos, name, icon, labelStyle)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MouseEnter:
            isHot, isIn = true
            invalidate()

        case MouseLeave:
            isHot, isIn = false
            invalidate()

        case MouseMove:
            if !isPressing
                break
            rect := getClientRect()
            if !rect.contains(evt.pos)
            {
                isPressed = false
                isIn = false
                invalidate()
            }
            else
            {
                isIn = true
                if !isPressed
                {
                    isPressed = true
                    invalidate()
                }
            }

        case MousePressed:
            if evt.button == .Left
            {
                isPressed, isPressing = true
                captureMouse()
                invalidate()
            }

        case MouseReleased:
            if evt.button == .Left
            {
                releaseMouse()
                isPressed, isPressing = false
                if isIn and sigPressed
                    sigPressed(self)
                invalidate()
            }
        }
    }
}

impl ButtonPush
{
    func create(parent: *Wnd, name: string, position: Math.Rectangle)->*ButtonPush
    {
        res := Wnd.create'ButtonPush(parent, name, position)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }
}