#global public
using Core, Pixel

struct ButtonIcon
{
    using base:     Widget
    icon:           Icon
    sigPressed:     func(*ButtonIcon)

    isPressed:      bool
    isPressing:     bool
    isHot:          bool
    isIn:           bool
}

impl IWnd for ButtonIcon
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc := evt.bc
        palette := getPalette()

        var subRectBk:  *ImageSubRect
        var colorBk:    Color = ?
        var colorIcon:  Color = ?

        if isDisabled
        {
            colorBk    = palette.btnIcon_DisabledBk
            colorIcon  = palette.btnIcon_DisabledIcon
            subRectBk  = &bc.theme.imgMetrics.btnIcon_Disabled
        }
        else if isPressed
        {
            colorBk   = palette.btnIcon_PressedBk
            colorIcon = palette.btnIcon_PressedIcon
            subRectBk = &bc.theme.imgMetrics.btnIcon_Pressed
        }
        else if isHot
        {
            colorBk   = palette.btnIcon_HotBk
            colorIcon = palette.btnIcon_HotIcon
            subRectBk = &bc.theme.imgMetrics.btnIcon_Hot
        }
        else
        {
            colorBk   = palette.btnIcon_Bk
            colorIcon = palette.btnIcon_Icon
            subRectBk = &bc.theme.imgMetrics.btnIcon_Normal
        }

        // Background
        bc.theme.drawSubRect(bc.painter, position, subRectBk, colorBk)

        // Icon
        pos := position
        pos.x = pos.horzCenter() - icon.size * 0.5
        icon.paint(bc.painter, pos.x, pos.vertCenter() - (icon.size * 0.5), colorIcon)
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

impl ButtonIcon
{
    func create(parent: *Wnd, name: string, position: Math.Rectangle)->*ButtonIcon
    {
        res := Wnd.create'ButtonIcon(parent, name, position)

        evt := CreateEvent{}
        res.sendEvent(&evt)

        return res
    }
}