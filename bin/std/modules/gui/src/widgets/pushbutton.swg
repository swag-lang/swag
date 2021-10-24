#global public
using Core, Pixel

#[Swag.EnumFlags]
enum PushButtonPaintState
{
    Zero = 0
    Pressed
    Hot
}

struct PushButton
{
    using base: Widget
    pressed:    bool
    pressing:   bool
    hot:        bool
}

impl IWnd for PushButton
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        var state: PushButtonPaintState = Zero
        if pressed  state |= .Pressed
        if hot      state |= .Hot
        paint(evt.bc, name, position, state)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MouseEnter:
            hot = true
            invalidate()

        case MouseLeave:
            hot = false
            invalidate()

        case MouseMove:
            if pressing
            {
                if !position.contains(evt.pos)
                {
                    pressed = false
                    invalidate()
                }
                else if !pressed
                {
                    pressed = true
                    invalidate()
                }
            }

        case MousePressed:
            if evt.button == .Left
            {
                pressed, pressing = true
                captureMouse()
                invalidate()
            }

        case MouseReleased:
            if evt.button == .Left
            {
                releaseMouse()
                pressed, pressing = false
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

        colorBk     := palette.buttonBk
        colorBorder := palette.buttonBorder
        colorText   := palette.buttonText

        if state & .Hot
        {
            colorBk = palette.buttonBkHot
            colorBorder = palette.buttonBorderHot
            colorText = palette.buttonTextHot
        }

        if state & .Pressed
        {
            colorBk = palette.buttonBkPressed
            colorBorder = palette.buttonBorderPressed
            colorText = palette.buttonTextPressed
            srcRect = @{1, 22, 30, 20}
        }
        else
        {
            srcRect = @{1, 1, 30, 20}
        }

        // Background
        Style.drawTexture(bc.painter, bc.theme.imgWidgets, position, srcRect, 8, 8, colorBk)

        // Border
        srcRect.offset(31, 0)
        Style.drawTexture(bc.painter, bc.theme.imgWidgets, position, srcRect, 8, 8, colorBorder)

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