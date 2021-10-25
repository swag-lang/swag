#global public
using Core, Pixel

#[Swag.EnumFlags]
enum PushButtonPaintFlags
{
    Zero = 0
    Pressed
    Hot
    Default
    Disabled
}

struct PushButtonPaintStyle
{
    flags:      PushButtonPaintFlags = Zero
    palette:    *Palette
}

struct PushButton
{
    using base:     Widget
    isDefault:      bool
    isPressed:      bool
    isPressing:     bool
    isHot:          bool
}

impl IWnd for PushButton
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        var style: PushButtonPaintStyle

        if isPressed
            style.flags |= .Pressed
        if isHot
            style.flags |= .Hot
        if isDefault
            style.flags |= .Default
        if evt.bc.isDisabled
            style.flags |= .Disabled

        style.palette = getPalette()

        paint(evt.bc, name, position, style)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MouseEnter:
            isHot = true
            invalidate()

        case MouseLeave:
            isHot = false
            invalidate()

        case MouseMove:
            if isPressing
            {
                rect := getClientRect()
                if !rect.contains(evt.pos)
                {
                    isPressed = false
                    invalidate()
                }
                else if !isPressed
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
                invalidate()
            }
        }
    }
}

impl PushButton
{
    func paint(bc: *PaintContext, name: string, position: Math.Rectangle, style: PushButtonPaintStyle)
    {
        flags := style.flags
        isDefault := flags & .Default

        var srcRect: Math.Rectangle
        var colorBk: Color = ?
        var colorBorder: Color = ?
        var colorText: Color = ?

        palette := style.palette
        if flags & .Disabled
        {
            colorBk     = palette.buttonDisabledBk
            colorBorder = palette.buttonDisabledBorder
            colorText   = palette.buttonDisabledText
            srcRect = @{1, 57, 27, 27}
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
                colorBk     = isDefault ? palette.buttonDefaultBkPressed : palette.buttonBkPressed
                colorBorder = isDefault ? palette.buttonDefaultBorderPressed : palette.buttonBorderPressed
                colorText   = isDefault ? palette.buttonDefaultTextPressed : palette.buttonTextPressed
                srcRect = @{1, 29, 27, 27}
            }
            else
            {
                srcRect = @{1, 1, 27, 27}
            }
        }

        // Background
        Style.drawTexture(bc.painter, bc.theme.imgWidgets, position, srcRect, 9, colorBk)

        // Border
        srcRect.offset(28, 0)
        Style.drawTexture(bc.painter, bc.theme.imgWidgets, position, srcRect, 9, colorBorder)

        // Content
        var labelStyle: LabelPaintStyle
        labelStyle.colorText = colorText
        labelStyle.stringFormat.horzAlignment = .Center
        labelStyle.stringFormat.vertAlignment = .Center
        labelStyle.font = bc.theme.fontSmall.bold
        Label.paint(bc, position, name, labelStyle)
    }

    func create(parent: *Wnd)->*PushButton
    {
        res := Wnd.create'PushButton(parent)
        return res
    }
}