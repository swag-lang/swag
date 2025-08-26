#global marked
#global public
using Core, Pixel

struct RadioButton
{
    using button:        Button
    checked:             bool
    radioButtonFlags     = RadioButtonFlags.Zero
    sigChanged:          SigArray'func||(*RadioButton)
}

#[Swag.EnumFlags]
enum RadioButtonFlags
{
    Zero       = 0
    RightAlign
}

impl IWnd for RadioButton
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let theme   = me.getTheme()
        let metrics = me.getThemeMetrics()
        let colors  = me.getThemeColors()
        let rects   = me.getThemeRects()

        var subRectBk:     *ThemeImageRect
        var subRectBorder: *ThemeImageRect
        var subRectMark:   *ThemeImageRect
        var colorBk:       Color = undefined
        var colorBorder:   Color = undefined
        var colorMark:     Color = undefined
        var colorText:     Color = undefined

        switch
        {
        case bc.isDisabled:
            colorBk     = colors.btnCheck_DisabledBk
            colorBorder = colors.btnCheck_DisabledBorder
            colorMark   = colors.btnCheck_DisabledMark
            colorText   = colors.btnCheck_DisabledText
        case me.isPressed:
            colorBk     = colors.btnCheck_PressedBk
            colorBorder = colors.btnCheck_PressedBorder
            colorMark   = colors.btnCheck_PressedMark
            colorText   = colors.btnCheck_PressedText
        case me.isHot:
            colorBk     = colors.btnCheck_HotBk
            colorBorder = colors.btnCheck_HotBorder
            colorMark   = colors.btnCheck_HotMark
            colorText   = colors.btnCheck_HotText
        case me.checked:
            colorBk     = colors.btnCheck_CheckedBk
            colorText   = colors.btnCheck_CheckedText
            colorBorder = colors.btnCheck_CheckedBorder
            colorMark   = colors.btnCheck_CheckedMark
        default:
            colorBk     = colors.btnCheck_Bk
            colorBorder = colors.btnCheck_Border
            colorMark   = colors.btnCheck_Mark
            colorText   = colors.btnCheck_Text
        }

        if me.checked
        {
            subRectBk     = &rects.btnRadio_Checked
            subRectBorder = &rects.btnRadio_CheckedBorder
            subRectMark   = &rects.btnRadio_CheckedMark
        }
        else
        {
            subRectBk     = &rects.btnRadio_Normal
            subRectBorder = &rects.btnRadio_NormalBorder
        }

        // Box
        var clientRect = me.getClientRect()
        var posBox     = clientRect
        posBox.height = metrics.btnCheck_Size
        posBox.y      = clientRect.vertCenter() - (posBox.height * 0.5)
        posBox.width  = posBox.height
        if me.radioButtonFlags.has(.RightAlign):
            posBox.x = clientRect.right() - posBox.width
        theme.drawSubRect(bc.painter, posBox, subRectBk, colorBk)
        theme.drawSubRect(bc.painter, posBox, subRectBorder, colorBorder)
        theme.drawSubRect(bc.painter, posBox, subRectMark, colorMark)

        // Content
        var format: RichStringFormat
        theme.setTextColors(&format, colorText)
        if me.radioButtonFlags.has(.RightAlign):
            format.horzAlignment = .Right
        else:
            format.horzAlignment = .Left
        format.vertAlignment = .Center
        format.flags         = .Clip
        format.font          = me.getFont()
        var labelPos = clientRect
        if !me.radioButtonFlags.has(.RightAlign):
            labelPos.x += posBox.width + metrics.btnCheck_Padding
        labelPos.width -= posBox.width + metrics.btnCheck_Padding
        Theme.drawIconText(bc.painter, labelPos, me.icon, &me.name, format, metrics.iconText_Margin)
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if me.processMouseEvent(evt) and !me.checked
        {
            me.checked = true
            me.sigChanged.call(me)
            me.postCommandEvent(me.id)
        }
    }
}

impl RadioButton
{
    #[Swag.Discardable]
    func create(parent: *Wnd, name: string, position: Math.Rectangle = {}, id: WndId = null)->*RadioButton
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0:
            pos.height = parent.getThemeMetrics().btnPush_Height
        if pos.width == 0:
            pos.width = parent.getThemeMetrics().btnPush_Width
        return Wnd.create'RadioButton(parent, name, pos, id)
    }

    // Set the checked state
    mtd setChecked(checked: bool)
    {
        if me.checked == #me.checked:
            return
        me.checked = #me.checked
        me.invalidate()
    }
}
