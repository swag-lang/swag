#global public
using Core, Pixel

enum ButtonCheckState
{
    Normal
    Checked
    Undefined
}

struct ButtonCheck
{
    using widget:   Widget
    icon:           Icon
    isThreeState:   bool
    isChecked:      ButtonCheckState = Normal
    sigPressed:     func(*ButtonCheck)

    isPressed:      bool
    isPressing:     bool
    isHot:          bool
    isIn:           bool
}

impl IWnd for ButtonCheck
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc     := evt.bc
        theme  := bc.theme
        colors := &bc.theme.colors
        rects  := &bc.theme.rects

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var subRectMark:    *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?
        var colorMark:      Color = ?
        var colorText:      Color = ?

        switch
        {
        case isDisabled:
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

        switch isChecked
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
        posBox := position
        posBox.width = posBox.height
        bc.theme.drawSubRect(bc.painter, posBox, subRectBk, colorBk)
        bc.theme.drawSubRect(bc.painter, posBox, subRectBorder, colorBorder)
        if isChecked != .Normal
            bc.theme.drawSubRect(bc.painter, posBox, subRectMark, colorMark)

        // Content
        var format: RichStringFormat
        format.palette[0] = colorText
        format.horzAlignment = .Left
        format.vertAlignment = .Center
        format.flags = .Clip
        format.font = bc.theme.res.fontDefault
        labelPos := position
        labelPos.x += posBox.width + bc.theme.metrics.btnCheck_Padding
        labelPos.width -= posBox.width + bc.theme.metrics.btnCheck_Padding
        Theme.drawIconText(bc.painter, labelPos, icon, &name, format, bc.theme.metrics.iconText_Margin)
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
                if isChecked == .Normal and isThreeState
                    isChecked = .Undefined
                else if isChecked == .Checked
                    isChecked = .Normal
                else
                    isChecked = .Checked
                invalidate()
            }
        }
    }
}

impl ButtonCheck
{
    func create(parent: *Wnd, name: string, x, y: f32 = 0, width: f32 = 100)->*ButtonCheck
    {
        var pos: Math.Rectangle{x, y, width, parent.getThemeMetrics().btnCheck_Size}
        res := Wnd.create'ButtonCheck(parent, name, pos)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }
}