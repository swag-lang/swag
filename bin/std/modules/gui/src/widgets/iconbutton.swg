#global public
using Core, Pixel

enum IconButtonForm
{
    Square
    Round
    RoundSquare
}

#[Swag.EnumFlags]
enum IconButtonFlags
{
    None
    Checkable
    CheckableToggle
    Popup
}

enum IconButtonCheckedForm
{
    None
    Bottom
    Top
    Left
    Right
    Full
}

struct IconButton
{
    using button: Button

    form            = IconButtonForm.Square
    iconBtnFlags    = IconButtonFlags.None
    checkedForm     = IconButtonCheckedForm.Full
    sigHidePopup:   SigArray'closure(*Self)
    sigShowPopup:   SigArray'closure(*Self)

    isChecked:          bool
    marginBottomText:   f32 = 4.0
    marginTopIcon:      f32
    marginBottomIcon:   f32
    popup:              *Wnd
}

impl IWnd for IconButton
{
    mtd hidePopup()
    {
        if !popup
            return

        getApp().unregisterHookEvents(self)
        sigHidePopup.call(self)
        popup.destroy()
        popup = null
        isChecked = false
        invalidate()
    }

    mtd onHookEvent(evt: *Event)
    {
        if !popup
            return

        if evt.kind == .MousePressed
        {
            if evt.target == self
                return
            if evt.target and popup.isParentOf(evt.target)
                return
            hidePopup()
        }
    }

    mtd onDestroyEvent(evt: *DestroyEvent)
    {
        hidePopup()
    }

    mtd onCommandStateEvent(evt: *CommandUIEvent)
    {
        if evt.id != id
            return

        if evt.setFlags & .Checked and isChecked != evt.checked
        {
            evt.accepted = true
            isChecked = evt.checked
            invalidate()
        }

        button.IWnd.onCommandStateEvent(evt)
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc      := evt.bc
        theme   := getTheme()
        colors  := getThemeColors()
        rects   := getThemeRects()
        metrics := getThemeMetrics()

        var subRectBk:      *ThemeImageRect
        var colorBk:        Color = ?
        var colorIcon:      Color = ?
        var colorChecked:   Color = ?

        switch form
        {
        case Square:
            subRectBk = &rects.btnIcon_SquareBk
        case Round:
            subRectBk = &rects.btnIcon_RoundBk
        case RoundSquare:
            subRectBk = &rects.btnIcon_RoundSquareBk
        }

        switch
        {
        case bc.isDisabled:
            colorBk      = isChecked ? colors.btnIcon_CheckedDisabledBk : colors.btnIcon_DisabledBk
            colorIcon    = isChecked ? colors.btnIcon_CheckedDisabledIcon : colors.btnIcon_DisabledIcon
            colorChecked = colors.btnIcon_CheckedDisabledMark
        case isPressed or (isChecked and checkedForm == .None):
            colorBk      = isChecked ? colors.btnIcon_CheckedPressedBk : colors.btnIcon_PressedBk
            colorIcon    = isChecked ? colors.btnIcon_CheckedPressedIcon : colors.btnIcon_PressedIcon
            colorChecked = colors.btnIcon_CheckedPressedMark
        case isHot:
            colorBk      = isChecked ? colors.btnIcon_CheckedHotBk : colors.btnIcon_HotBk
            colorIcon    = isChecked ? colors.btnIcon_CheckedHotIcon : colors.btnIcon_HotIcon
            colorChecked = colors.btnIcon_CheckedHotMark
        default:
            colorBk      = isChecked ? colors.btnIcon_CheckedBk : colors.btnIcon_Bk
            colorIcon    = isChecked ? colors.btnIcon_CheckedIcon : colors.btnIcon_Icon
            colorChecked = colors.btnIcon_CheckedMark
        }

        // Background
        clientRect := getClientRect()
        theme.drawSubRect(bc.painter, clientRect, subRectBk, colorBk)

        if isChecked and checkedForm != .None
        {
            switch checkedForm
            {
            case Bottom:
                subRectBk = &rects.btnIcon_CheckedBottom
            case Top:
                subRectBk = &rects.btnIcon_CheckedTop
            case Left:
                subRectBk = &rects.btnIcon_CheckedLeft
            case Right:
                subRectBk = &rects.btnIcon_CheckedRight
            case Full:
                subRectBk = &rects.btnIcon_CheckedFull
            }

            theme.drawSubRect(bc.painter, clientRect, subRectBk, colorChecked)
        }

        pos := clientRect

        // Arrow
        if iconBtnFlags & .Popup
        {
            subRectBk = &rects.btnIcon_ArrowDown
            posArrow := pos
            posArrow.x = clientRect.horzCenter() - metrics.btnIcon_WidthArrow * 0.5
            posArrow.width = metrics.btnIcon_WidthArrow
            posArrow.height = metrics.btnIcon_HeightArrow
            posArrow.y = clientRect.bottom() - metrics.btnIcon_HeightArrow - metrics.btnIcon_MarginArrow
            theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
            pos.setBottom(posArrow.y)
        }

        // Name
        if !name.isEmpty()
        {
            var sf: RichStringFormat
            theme.setTextColors(&sf, colorIcon)
            sf.horzAlignment = .Center
            sf.vertAlignment = .Bottom
            sf.font = theme.res.fontDefault
            pos1 := pos
            pos1.height -= marginBottomText
            bc.painter.drawRichString(pos1, &name, sf)
        }

        // Icon
        pos.moveTop(marginTopIcon)
        pos.height -= marginBottomIcon
        pos.x = pos.horzCenter() - icon.sizeX * 0.5
        pos.y = pos.vertCenter() - icon.sizeY * 0.5
        icon.paint(bc.painter, pos.x, pos.y, colorIcon)

    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if processMouseEvent(evt)
        {
            if iconBtnFlags & .CheckableToggle
            {
                isChecked = !isChecked
                postCommandEvent(id)
            }
            elif iconBtnFlags & .Checkable
            {
                if !isChecked
                {
                    isChecked = true
                    postCommandEvent(id)
                }
            }
            else
            {
                postCommandEvent(id)
            }

            // Display/Hide popup
            if iconBtnFlags & .Popup
            {
                if popup
                {
                    hidePopup()
                }
                else
                {
                    sigShowPopup.call(self)
                    if popup
                        getApp().registerHookEvents(self)
                }
            }
        }
    }
}

impl IconButton
{
    #[Swag.Discardable]
    func create(parent: *Wnd, icon: Icon = @{}, position: Math.Rectangle = @{}, id: WndId = null)->*IconButton
    {
        Debug.assert(parent != null)
        res := Wnd.create'IconButton(parent, null, position, id)
        res.icon = icon
        return res
    }

    // Set the check state of the button (if the button is checkable)
    mtd setCheck(checked: bool)
    {
        if !(iconBtnFlags & .Checkable)
            return
        if isChecked == checked
            return
        isChecked = checked
        postCommandEvent(id)
        invalidate()
    }

    // Set the button form
    mtd setForm(form: IconButtonForm)
    {
        self.form = #self.form
        invalidate()
    }
}