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

        paint(evt.bc, name, position, style)
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
    func paint(bc: *PaintContext, name: string, position: Math.Rectangle, style: ButtonPaintStyle)
    {
        flags   := style.flags
        palette := style.palette

        isDefault := flags & .Default

        var srcRect:     Math.Rectangle = ?
        var colorBk:     Color = ?
        var colorBorder: Color = ?
        var colorText:   Color = ?

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
        labelStyle.stringFormat.flags = .Clip
        labelStyle.font = bc.theme.fontSmall.bold
        labelPos := position
        labelPos.inflate(-5)
        Label.paint(bc, labelPos, name, labelStyle)

        var icon: Icon
        icon.set(&bc.theme.icons24, 0, 16)
        icon.draw(bc.painter, position.x, position.y)
        icon.set(&bc.theme.icons24, 1, 16)
        icon.draw(bc.painter, position.x + 16, position.y)
    }

    func create(parent: *Wnd, name: string, position: Math.Rectangle)->*Button
    {
        res := Wnd.create'Button(parent, name, position)

        evt := CreateEvent{}
        res.sendEvent(&evt)

        return res
    }
}