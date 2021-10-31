#global public
using Core, Pixel

#[Swag.EnumFlags]
enum ButtonPaintFlags
{
    Zero = 0
    Pressed
    Hot
    Default
    Disabled
}

struct ButtonPaintStyle
{
    flags:      ButtonPaintFlags = Zero
    palette:    *Palette
}

struct Button
{
    using base:     Widget
    icon:           Icon
    isDefault:      bool
    isPressed:      bool
    isPressing:     bool
    isHot:          bool
    isIn:           bool
    sigPressed:     func(*Button)
}

impl IWnd for Button
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        var style: ButtonPaintStyle

        if isPressed
            style.flags |= .Pressed
        if isHot
            style.flags |= .Hot
        if isDefault
            style.flags |= .Default
        if evt.bc.isDisabled
            style.flags |= .Disabled

        style.palette = getPalette()
        paint(evt.bc, name, icon, position, style)
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

impl Button
{
    func paint(bc: *PaintContext, name: string, icon: Icon, position: Math.Rectangle, style: ButtonPaintStyle)
    {
        flags   := style.flags
        palette := style.palette

        isDefault := flags & .Default

        var subRectBk:      *ImageSubRect
        var subRectBorder:  *ImageSubRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorText:      Color = ?

        if flags & .Disabled
        {
            colorBk       = palette.buttonDisabledBk
            colorBorder   = palette.buttonDisabledBorder
            colorText     = palette.buttonDisabledText
            subRectBk     = &bc.theme.imgMetrics.btnDisabled
            subRectBorder = &bc.theme.imgMetrics.btnDisabledBorder
        }
        else
        {
            colorBk     = isDefault ? palette.buttonDefaultBk : palette.buttonBk
            colorBorder = isDefault ? palette.buttonDefaultBorder : palette.buttonBorder
            colorText   = isDefault ? palette.buttonDefaultText : palette.buttonText

            if flags & .Hot
            {
                colorBk     = isDefault ? palette.buttonDefaultBkHot : palette.buttonBkHot
                colorBorder = isDefault ? palette.buttonDefaultBorderHot : palette.buttonBorderHot
                colorText   = isDefault ? palette.buttonDefaultTextHot : palette.buttonTextHot
            }

            if flags & .Pressed
            {
                colorBk       = isDefault ? palette.buttonDefaultBkPressed : palette.buttonBkPressed
                colorBorder   = isDefault ? palette.buttonDefaultBorderPressed : palette.buttonBorderPressed
                colorText     = isDefault ? palette.buttonDefaultTextPressed : palette.buttonTextPressed
                subRectBk     = &bc.theme.imgMetrics.btnPressed
                subRectBorder = &bc.theme.imgMetrics.btnPressedBorder
            }
            else
            {
                subRectBk     = &bc.theme.imgMetrics.btnHot
                subRectBorder = &bc.theme.imgMetrics.btnHotBorder
            }
        }

        // Background
        bc.theme.drawSubRect(bc.painter, position, subRectBk, colorBk)

        // Border
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

    func create(parent: *Wnd, name: string, position: Math.Rectangle)->*Button
    {
        res := Wnd.create'Button(parent, name, position)

        evt := CreateEvent{}
        res.sendEvent(&evt)

        return res
    }
}