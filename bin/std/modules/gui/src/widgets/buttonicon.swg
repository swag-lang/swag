#global public
using Core, Pixel

struct ButtonIcon
{
    using widget:   Widget
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
        bc     := evt.bc
        theme  := bc.theme
        colors := &bc.theme.colors
        rects  := &bc.theme.rects

        var subRectBk:  *ThemeImageRect
        var colorBk:    Color = ?
        var colorIcon:  Color = ?

        switch
        {
        case isDisabled:
            colorBk    = colors.btnIcon_DisabledBk
            colorIcon  = colors.btnIcon_DisabledIcon
            subRectBk  = &rects.btnIcon_Disabled
        case isPressed:
            colorBk   = colors.btnIcon_PressedBk
            colorIcon = colors.btnIcon_PressedIcon
            subRectBk = &rects.btnIcon_Pressed
        case isHot:
            colorBk   = colors.btnIcon_HotBk
            colorIcon = colors.btnIcon_HotIcon
            subRectBk = &rects.btnIcon_Hot
        default:
            colorBk   = colors.btnIcon_Bk
            colorIcon = colors.btnIcon_Icon
            subRectBk = &rects.btnIcon_Normal
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
    func create(parent: *Wnd, icon: Icon, position: Math.Rectangle)->*ButtonIcon
    {
        res := Wnd.create'ButtonIcon(parent, null, position)
        res.icon = icon
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }
}