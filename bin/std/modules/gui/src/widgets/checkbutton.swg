#global public
using Core, Pixel

enum ButtonCheckState
{
    Normal
    Checked
    Undefined
}

#[Swag.EnumFlags]
enum CheckButtonFlags
{
    None
    ThreeState
}

struct CheckButton
{
    using button: Button

    checked             = ButtonCheckState.Normal
    checkButtonFlags    = CheckButtonFlags.None
    sigChanged:         func(*Self)
}

impl IWnd for CheckButton
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
        var colorText:      Color = ?

        switch
        {
        case bc.isDisabled:
            colorBk       = colors.btnCheck_DisabledBk
            colorBorder   = colors.btnCheck_DisabledBorder
            colorMark     = colors.btnCheck_DisabledMark
            colorText     = colors.btnCheck_DisabledText
        case isPressed:
            colorBk       = colors.btnCheck_PressedBk
            colorBorder   = colors.btnCheck_PressedBorder
            colorMark     = colors.btnCheck_PressedMark
            colorText     = colors.btnCheck_PressedText
        case isHot:
            colorBk       = colors.btnCheck_HotBk
            colorBorder   = colors.btnCheck_HotBorder
            colorMark     = colors.btnCheck_HotMark
            colorText     = colors.btnCheck_HotText
        default:
            colorBk       = colors.btnCheck_Bk
            colorBorder   = colors.btnCheck_Border
            colorMark     = colors.btnCheck_Mark
            colorText     = colors.btnCheck_Text
        }

        switch checked
        {
        case Normal:
            subRectBk     = &rects.btnCheck_Normal
            subRectBorder = &rects.btnCheck_NormalBorder
        case Checked:
            subRectBk     = &rects.btnCheck_Checked
            subRectBorder = &rects.btnCheck_CheckedBorder
            subRectMark   = &rects.btnCheck_CheckedMark
        case Undefined:
            subRectBk     = &rects.btnCheck_Undefined
            subRectBorder = &rects.btnCheck_UndefinedBorder
            subRectMark   = &rects.btnCheck_UndefinedMark
        }

        // Box
        clientRect := getClientRect()
        posBox := clientRect
        posBox.height = metrics.btnCheck_Size
        posBox.y = clientRect.vertCenter() - (posBox.height * 0.5)
        posBox.width = posBox.height
        theme.drawSubRect(bc.painter, posBox, subRectBk, colorBk)
        theme.drawSubRect(bc.painter, posBox, subRectBorder, colorBorder)
        theme.drawSubRect(bc.painter, posBox, subRectMark, colorMark)

        // Content
        var format: RichStringFormat
        theme.setTextColors(&format, colorText)
        format.horzAlignment = .Left
        format.vertAlignment = .Center
        format.flags = .Clip
        format.font = theme.res.fontDefault
        labelPos := clientRect
        labelPos.x += posBox.width + metrics.btnCheck_Padding
        labelPos.width -= posBox.width + metrics.btnCheck_Padding
        Theme.drawIconText(bc.painter, labelPos, icon, &name, format, metrics.iconText_Margin)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if processMouseEvent(evt)
        {
            if checked == .Normal and (checkButtonFlags & .ThreeState)
                checked = .Undefined
            elif checked == .Checked
                checked = .Normal
            else
                checked = .Checked
            if sigChanged
                sigChanged(self)
            postCommandEvent(id)
        }
    }
}

impl CheckButton
{
    #[Swag.Discardable]
    func create(parent: *Wnd, name: string, position: Math.Rectangle, id: string = null)->*CheckButton
    {
        Debug.assert(parent != null)
        pos := position
        if pos.height == 0
            pos.height = parent.getThemeMetrics().btnPush_Height
        if pos.width == 0
            pos.width = parent.getThemeMetrics().btnPush_Width
        return Wnd.create'CheckButton(parent, name, pos, id)
    }
}