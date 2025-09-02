#global public
using Core, Pixel

struct ToggleButton
{
    using button:     Button

    isToggled:        bool
    sigChanged:       func(*ToggleButton)

    aniBk:            BlendColor
    aniBorder:        BlendColor
    aniMark:          BlendColor
}

impl IWnd for ToggleButton
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc     = evt.bc
        let theme  = me.getTheme()
        let colors = me.getThemeColors()
        let rects  = me.getThemeRects()

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
        case me.isPressed:
            colorBk     = me.isToggled ? colors.btnToggle_OnPressedBk : colors.btnToggle_OffPressedBk
            colorBorder = me.isToggled ? colors.btnToggle_OnPressedBorder : colors.btnToggle_OffPressedBorder
            colorMark   = me.isToggled ? colors.btnToggle_OnPressedMark : colors.btnToggle_OffPressedMark
        case me.isHot:
            colorBk     = me.isToggled ? colors.btnToggle_OnHotBk : colors.btnToggle_OffHotBk
            colorBorder = me.isToggled ? colors.btnToggle_OnHotBorder : colors.btnToggle_OffHotBorder
            colorMark   = me.isToggled ? colors.btnToggle_OnHotMark : colors.btnToggle_OffHotMark
        default:
            colorBk     = me.isToggled ? colors.btnToggle_OnBk : colors.btnToggle_OffBk
            colorBorder = me.isToggled ? colors.btnToggle_OnBorder : colors.btnToggle_OffBorder
            colorMark   = me.isToggled ? colors.btnToggle_OnMark : colors.btnToggle_OffMark
        }

        subRectBk     = &rects.btnToggle_Bk
        subRectBorder = &rects.btnToggle_Border
        subRectMark   = &rects.btnToggle_Mark

        me.aniBk.apply(me, colorBk)
        me.aniBorder.apply(me, colorBorder)
        me.aniMark.apply(me, colorMark)

        // Box
        var clientRect = me.getClientRect()
        theme.drawSubRect(bc.painter, clientRect, subRectBk, me.aniBk.cur())
        theme.drawSubRect(bc.painter, clientRect, subRectBorder, me.aniBorder.cur())

        // Content
        var posBox = clientRect
        posBox.y     = clientRect.vertCenter() - (posBox.height * 0.5)
        posBox.width = posBox.height
        if me.isToggled:
            posBox.x = clientRect.right() - posBox.width
        theme.drawSubRect(bc.painter, posBox, subRectMark, me.aniMark.cur())
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if me.processMouseEvent(evt)
        {
            me.isToggled = !me.isToggled
            if me.sigChanged:
                me.sigChanged(me)
            me.postCommandEvent(me.id)
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
