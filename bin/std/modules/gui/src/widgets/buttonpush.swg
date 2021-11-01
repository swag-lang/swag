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
        bc := evt.bc
        palette := getPalette()

        var subRectBk:      *ImageSubRect
        var subRectBorder:  *ImageSubRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorText:      Color = ?

        if isDisabled
        {
            colorBk       = palette.buttonDisabledBk
            colorBorder   = palette.buttonDisabledBorder
            colorText     = palette.buttonDisabledText
            subRectBk     = &bc.theme.imgMetrics.btnPush_Disabled
            subRectBorder = &bc.theme.imgMetrics.btnPush_DisabledBorder
        }
        else
        {
            if isPressed
            {
                colorBk       = isDefault ? palette.buttonDefaultBkPressed : palette.buttonBkPressed
                colorBorder   = isDefault ? palette.buttonDefaultBorderPressed : palette.buttonBorderPressed
                colorText     = isDefault ? palette.buttonDefaultTextPressed : palette.buttonTextPressed
                subRectBk     = &bc.theme.imgMetrics.btnPush_Pressed
                subRectBorder = &bc.theme.imgMetrics.btnPush_PressedBorder
            }
            else if isHot
            {
                colorBk       = isDefault ? palette.buttonDefaultBkHot : palette.buttonBkHot
                colorBorder   = isDefault ? palette.buttonDefaultBorderHot : palette.buttonBorderHot
                colorText     = isDefault ? palette.buttonDefaultTextHot : palette.buttonTextHot
                subRectBk     = &bc.theme.imgMetrics.btnPush_Hot
                subRectBorder = &bc.theme.imgMetrics.btnOush_HotBorder
            }
            else
            {
                colorBk       = isDefault ? palette.buttonDefaultBk : palette.buttonBk
                colorBorder   = isDefault ? palette.buttonDefaultBorder : palette.buttonBorder
                colorText     = isDefault ? palette.buttonDefaultText : palette.buttonText
                subRectBk     = &bc.theme.imgMetrics.btnPush_Normal
                subRectBorder = &bc.theme.imgMetrics.btnPush_NormalBorder
            }
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