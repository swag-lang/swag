#global public
using Core, Pixel

enum IconButtonForm
{
    Square
    Round
}

struct IconButton
{
    using button: Button

    form            = IconButtonForm.Square
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

        switch form
        {
        case Square:
            subRectBk = &rects.btnIcon_SquareBk
        case Round:
            subRectBk = &rects.btnIcon_RoundBk
        }

        switch
        {
        case bc.isDisabled:
            colorBk    = colors.btnIcon_DisabledBk
            colorIcon  = colors.btnIcon_DisabledIcon
        case isPressed:
            colorBk   = colors.btnIcon_PressedBk
            colorIcon = colors.btnIcon_PressedIcon
        case isHot:
            colorBk   = colors.btnIcon_HotBk
            colorIcon = colors.btnIcon_HotIcon
        default:
            colorBk   = colors.btnIcon_Bk
            colorIcon = colors.btnIcon_Icon
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
        if processMouseEvent(evt)
            postCommandEvent(id)
    }
}

impl IconButton
{
    #[Swag.Discardable]
    func create(parent: *Wnd, icon: Icon, position: Math.Rectangle, id: string = null)->*IconButton
    {
        Debug.assert(parent != null)
        res := Wnd.create'IconButton(parent, null, position, id)
        res.icon = icon
        return res
    }

    // Set the button form
    mtd setForm(form: IconButtonForm)
    {
        self.form = #self.form
        invalidate()
    }
}