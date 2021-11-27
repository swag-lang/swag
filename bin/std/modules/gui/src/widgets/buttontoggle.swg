#global public
using Core, Pixel

struct ButtonToggle
{
    using button:   Button
    isToggled:      bool
    sigChanged:     func(*ButtonToggle)
}

impl IWnd for ButtonToggle
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc     := evt.bc
        theme   := getTheme()
        metrics := getThemeMetrics()
        colors  := getThemeColors()
        rects   := getThemeRects()

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var subRectMark:    *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorMark:      Color = ?

        switch
        {
        case isDisabled:
            colorBk       = colors.btnToggle_DisabledBk
            colorBorder   = colors.btnToggle_DisabledBorder
            colorMark     = colors.btnToggle_DisabledMark
        case isPressed:
            colorBk       = isToggled ? colors.btnToggle_OnPressedBk : colors.btnToggle_OffPressedBk
            colorBorder   = isToggled ? colors.btnToggle_OnPressedBorder : colors.btnToggle_OffPressedBorder
            colorMark     = isToggled ? colors.btnToggle_OnPressedMark : colors.btnToggle_OffPressedMark
        case isHot:
            colorBk       = isToggled ? colors.btnToggle_OnHotBk : colors.btnToggle_OffHotBk
            colorBorder   = isToggled ? colors.btnToggle_OnHotBorder : colors.btnToggle_OffHotBorder
            colorMark     = isToggled ? colors.btnToggle_OnHotMark : colors.btnToggle_OffHotMark
        default:
            colorBk       = isToggled ? colors.btnToggle_OnBk : colors.btnToggle_OffBk
            colorBorder   = isToggled ? colors.btnToggle_OnBorder : colors.btnToggle_OffBorder
            colorMark     = isToggled ? colors.btnToggle_OnMark : colors.btnToggle_OffMark
        }

        subRectBk     = &rects.btnToggle_Bk
        subRectBorder = &rects.btnToggle_Border
        subRectMark   = &rects.btnToggle_Mark

        // Box
        clientRect := getClientRect()
        theme.drawSubRect(bc.painter, clientRect, subRectBk, colorBk)
        theme.drawSubRect(bc.painter, clientRect, subRectBorder, colorBorder)

        // Content
        posBox := clientRect
        posBox.y = clientRect.vertCenter() - (posBox.height * 0.5)
        posBox.width = posBox.height
        if isToggled
            posBox.x = clientRect.right() - posBox.width
        theme.drawSubRect(bc.painter, posBox, subRectMark, colorMark)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if processMouseEvent(evt)
        {
            isToggled = !isToggled
            if sigChanged
                sigChanged(self)
        }
    }
}

impl ButtonToggle
{
    func create(parent: *Wnd, name: string, position: Math.Rectangle, id: string = null)->*ButtonToggle
    {
        pos := position
        if pos.height == 0 pos.height = parent.getThemeMetrics().btnToggle_Height
        if pos.width == 0  pos.width  = parent.getThemeMetrics().btnToggle_Width
        res := Wnd.create'ButtonToggle(parent, name, pos, id)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }
}