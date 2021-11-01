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
        palette := getPalette()

        var subRectBk:      *ImageSubRect
        var subRectBorder:  *ImageSubRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorText:      Color = ?

        switch
        {
        case isDisabled:
            colorBk       = palette.btnPush_DisabledBk
            colorBorder   = palette.btnPush_DisabledBorder
            colorText     = palette.btnPush_DisabledText
            subRectBk     = &theme.imgMetrics.btnPush_Disabled
            subRectBorder = &theme.imgMetrics.btnPush_DisabledBorder
        case isPressed:
            colorBk       = isDefault ? palette.btnPushDefault_PressedBk : palette.btnPush_PressedBk
            colorBorder   = isDefault ? palette.btnPushDefault_PressedBorder : palette.btnPush_PressedBorder
            colorText     = isDefault ? palette.btnPushDefault_PressedText : palette.btnPush_PressedText
            subRectBk     = &theme.imgMetrics.btnPush_Pressed
            subRectBorder = &theme.imgMetrics.btnPush_PressedBorder
        case isHot:
            colorBk       = isDefault ? palette.btnPushDefault_HotBk : palette.btnPush_HotBk
            colorBorder   = isDefault ? palette.btnPushDefault_HotBorder : palette.btnPush_HotBorder
            colorText     = isDefault ? palette.btnPushDefault_HotText : palette.btnPush_HotText
            subRectBk     = &theme.imgMetrics.btnPush_Hot
            subRectBorder = &theme.imgMetrics.btnOush_HotBorder
        default:
            colorBk       = isDefault ? palette.btnPushDefault_Bk : palette.btnPush_Bk
            colorBorder   = isDefault ? palette.btnPushDefault_Border : palette.btnPush_Border
            colorText     = isDefault ? palette.btnPushDefault_Text : palette.btnPush_Text
            subRectBk     = &theme.imgMetrics.btnPush_Normal
            subRectBorder = &theme.imgMetrics.btnPush_NormalBorder
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