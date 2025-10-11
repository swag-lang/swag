#global public
using Core, Pixel

enum ButtonCheckState
{
    UnChecked
    Checked
    Undefined
}

#[Swag.EnumFlags]
enum CheckButtonFlags
{
    Zero       = 0
    ThreeState
    RightAlign
}

struct CheckButton
{
    using button:        Button

    checked              = ButtonCheckState.UnChecked
    checkButtonFlags     = CheckButtonFlags.Zero
    sigChanged:          SigArray'func||(*CheckButton)
}

impl IWnd for CheckButton
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let painter = bc.painter
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
        case .checked == .Checked or .checked == .Undefined:
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

        switch .checked
        {
        case UnChecked:
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
        let clientRect = .getClientRect()
        var posBox     = clientRect
        posBox.height = metrics.btnCheck_Size
        posBox.y      = clientRect.vertCenter() - (posBox.height * 0.5)
        posBox.width  = posBox.height
        if .checkButtonFlags.has(.RightAlign) do
            posBox.x = clientRect.right() - posBox.width
        theme.drawSubRect(painter, posBox, subRectBk, colorBk)
        theme.drawSubRect(painter, posBox, subRectBorder, colorBorder)
        theme.drawSubRect(painter, posBox, subRectMark, colorMark)

        // Content
        var format: RichStringFormat
        theme.setTextColors(&format, colorText)
        if .checkButtonFlags.has(.RightAlign) do
            format.horzAlignment = .Right
        else do
            format.horzAlignment = .Left
        format.vertAlignment = .Center
        format.flags         = .Clip
        format.font          = .getFont()
        var labelPos = clientRect
        if !.checkButtonFlags.has(.RightAlign) do
            labelPos.x += posBox.width + metrics.btnCheck_Padding
        labelPos.width -= posBox.width + metrics.btnCheck_Padding
        Theme.drawIconText(painter, labelPos, .icon, &.name, format, metrics.iconText_Margin)
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if .processMouseEvent(evt) do
            .toggle()
    }
}

impl CheckButton
{
    #[Swag.Discardable]
    func create(parent: *Wnd, name: string, position: Math.Rectangle = {}, id: WndId = null)->*CheckButton
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0 do
            pos.height = parent.getThemeMetrics().btnPush_Height
        if pos.width == 0 do
            pos.width = parent.getThemeMetrics().btnPush_Width
        return Wnd.create'CheckButton(parent, name, pos, id)
    }

    // Toggle the button state
    public mtd toggle()
    {
        if .checked == .UnChecked and .checkButtonFlags.has(.ThreeState) do
            .checked = .Undefined
        elif .checked == .Checked do
            .checked = .UnChecked
        else do
            .checked = .Checked
        .sigChanged.call(me)
        .postCommandEvent(.id)
    }

    // Returns 'true' if the button is checked
    public mtd const isChecked()->bool => .checked == .Checked
    // Returns 'true' if the button is undefined
    public mtd const isUndefined()->bool => .checked == .Undefined
}
