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
        let theme   = .getTheme()
        let metrics = .getThemeMetrics()
        let colors  = .getThemeColors()
        let rects   = .getThemeRects()

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
        case .isPressed:
            colorBk     = colors.btnCheck_PressedBk
            colorBorder = colors.btnCheck_PressedBorder
            colorMark   = colors.btnCheck_PressedMark
            colorText   = colors.btnCheck_PressedText
        case .isHot:
            colorBk     = colors.btnCheck_HotBk
            colorBorder = colors.btnCheck_HotBorder
            colorMark   = colors.btnCheck_HotMark
            colorText   = colors.btnCheck_HotText
        case .checked:
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

        if .checked
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
        let clientRect = .getClientRect()
        var posBox     = clientRect
        posBox.height = metrics.btnCheck_Size
        posBox.y      = clientRect.vertCenter() - (posBox.height * 0.5)
        posBox.width  = posBox.height
        if .radioButtonFlags.has(.RightAlign) do
            posBox.x = clientRect.right() - posBox.width
        theme.drawSubRect(bc.painter, posBox, subRectBk, colorBk)
        theme.drawSubRect(bc.painter, posBox, subRectBorder, colorBorder)
        theme.drawSubRect(bc.painter, posBox, subRectMark, colorMark)

        // Content
        var format: RichStringFormat
        theme.setTextColors(&format, colorText)
        if .radioButtonFlags.has(.RightAlign) do
            format.horzAlignment = .Right
        else do
            format.horzAlignment = .Left
        format.vertAlignment = .Center
        format.flags         = .Clip
        format.font          = .getFont()
        var labelPos = clientRect
        if !.radioButtonFlags.has(.RightAlign) do
            labelPos.x += posBox.width + metrics.btnCheck_Padding
        labelPos.width -= posBox.width + metrics.btnCheck_Padding
        Theme.drawIconText(bc.painter, labelPos, .icon, &.name, format, metrics.iconText_Margin)
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if .processMouseEvent(evt) and !.checked
        {
            .checked = true
            .sigChanged.call(me)
            .postCommandEvent(.id)
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
        if pos.height == 0 do
            pos.height = parent.getThemeMetrics().btnPush_Height
        if pos.width == 0 do
            pos.width = parent.getThemeMetrics().btnPush_Width
        return Wnd.create'RadioButton(parent, name, pos, id)
    }

    // Set the checked state
    mtd setChecked(checked: bool)
    {
        if .checked == checked do
            return
        .checked = checked
        .invalidate()
    }
}
