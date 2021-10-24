#global public
using Core, Pixel

#[Swag.EnumFlags]
enum PushButtonPaintState
{
    Zero = 0
    Pressed
    Hot
    Default
    Disabled
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
        var state: PushButtonPaintState = Zero
        if isPressed    state |= .Pressed
        if isHot        state |= .Hot
        if isDefault    state |= .Default
        if isDisabled   state |= .Disabled
        paint(evt.bc, name, position, state)
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
    func paint(bc: *PaintContext, name: string, position: Math.Rectangle, state: PushButtonPaintState)
    {
        var srcRect: Math.Rectangle
        palette := &bc.theme.palette
        isDefault := state & .Default

        var colorBk: Color = ?
        var colorBorder: Color = ?
        var colorText: Color = ?

        if state & .Disabled
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

            if state & .Hot
            {
                colorBk     = isDefault ? palette.buttonDefaultBkHot : palette.buttonBkHot
                colorBorder = isDefault ? palette.buttonDefaultBorderHot : palette.buttonBorderHot
                colorText   = isDefault ? palette.buttonDefaultTextHot : palette.buttonTextHot
            }

            if state & .Pressed
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
        sf := StringFormat{}
        sf.horzAlignment = .Center
        sf.vertAlignment = .Center
        bc.painter.drawString(position, name, bc.theme.fontSmall.bold, colorText, sf)
    }

    func create(parent: *Wnd)->*PushButton
    {
        res := Wnd.create'PushButton(parent)
        return res
    }
}