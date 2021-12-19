#global public
using Core, Pixel

struct IconButton
{
    using button:   Button
}

impl IWnd for IconButton
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc     := evt.bc
        theme  := getTheme()
        colors := getThemeColors()
        rects  := getThemeRects()

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
        clientRect := getClientRect()
        theme.drawSubRect(bc.painter, clientRect, subRectBk, colorBk)

        // Icon
        pos := clientRect
        pos.x = pos.horzCenter() - icon.size * 0.5
        icon.paint(bc.painter, pos.x, pos.vertCenter() - (icon.size * 0.5), colorIcon)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        discard processMouseEvent(evt)
    }
}

impl IconButton
{
    #[Swag.Discardable]
    func create(parent: *Wnd, icon: Icon, position: Math.Rectangle, id: string = null)->*IconButton
    {
        res := Wnd.create'IconButton(parent, null, position, id)
        res.icon = icon
        res.sendCreateEvent()
        return res
    }
}