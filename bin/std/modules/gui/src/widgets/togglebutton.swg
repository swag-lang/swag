#global public
using Core, Pixel

struct ToggleButton
{
    using button:     Button

    isToggled:        bool
    sigChanged:       func(*Self)

    aniBk:            BlendColor
    aniBorder:        BlendColor
    aniMark:          BlendColor
}

impl IWnd for ToggleButton
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc     = evt.bc
        let theme  = getTheme()
        let colors = getThemeColors()
        let rects  = getThemeRects()

        var subRectBk:     *ThemeImageRect
        var subRectBorder: *ThemeImageRect
        var subRectMark:   *ThemeImageRect
        var colorBk:       Color = undefined
        var colorBorder:   Color = undefined
        var colorMark:     Color = undefined

        switch
        {
        case bc.isDisabled:
            colorBk     = colors.btnToggle_DisabledBk
            colorBorder = colors.btnToggle_DisabledBorder
            colorMark   = colors.btnToggle_DisabledMark
        case isPressed:
            colorBk     = isToggled ? colors.btnToggle_OnPressedBk : colors.btnToggle_OffPressedBk
            colorBorder = isToggled ? colors.btnToggle_OnPressedBorder : colors.btnToggle_OffPressedBorder
            colorMark   = isToggled ? colors.btnToggle_OnPressedMark : colors.btnToggle_OffPressedMark
        case isHot:
            colorBk     = isToggled ? colors.btnToggle_OnHotBk : colors.btnToggle_OffHotBk
            colorBorder = isToggled ? colors.btnToggle_OnHotBorder : colors.btnToggle_OffHotBorder
            colorMark   = isToggled ? colors.btnToggle_OnHotMark : colors.btnToggle_OffHotMark
        default:
            colorBk     = isToggled ? colors.btnToggle_OnBk : colors.btnToggle_OffBk
            colorBorder = isToggled ? colors.btnToggle_OnBorder : colors.btnToggle_OffBorder
            colorMark   = isToggled ? colors.btnToggle_OnMark : colors.btnToggle_OffMark
        }

        subRectBk     = &rects.btnToggle_Bk
        subRectBorder = &rects.btnToggle_Border
        subRectMark   = &rects.btnToggle_Mark

        aniBk.apply(self, colorBk)
        aniBorder.apply(self, colorBorder)
        aniMark.apply(self, colorMark)

        // Box
        var clientRect = getClientRect()
        theme.drawSubRect(bc.painter, clientRect, subRectBk, aniBk.cur())
        theme.drawSubRect(bc.painter, clientRect, subRectBorder, aniBorder.cur())

        // Content
        var posBox = clientRect
        posBox.y     = clientRect.vertCenter() - (posBox.height * 0.5)
        posBox.width = posBox.height
        if isToggled:
            posBox.x = clientRect.right() - posBox.width
        theme.drawSubRect(bc.painter, posBox, subRectMark, aniMark.cur())
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if processMouseEvent(evt)
        {
            isToggled = !isToggled
            if sigChanged:
                sigChanged(self)
            postCommandEvent(id)
        }
    }
}

impl ToggleButton
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*ToggleButton
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0:
            pos.height = parent.getThemeMetrics().btnToggle_Height
        if pos.width == 0:
            pos.width = parent.getThemeMetrics().btnToggle_Width
        return Wnd.create'ToggleButton(parent, null, pos, id)
    }
}
