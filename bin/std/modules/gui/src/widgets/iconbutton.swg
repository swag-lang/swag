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

enum IconButtonArrowPos
{
    None
    Left
    Top
    Right
    RightUp
    RightDown
    Bottom
}

enum IconButtonTextPos
{
    Left
    Top
    Right
    Bottom
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
    arrowPos        = IconButtonArrowPos.None
    textPos         = IconButtonTextPos.Bottom
    sigHidePopup:       SigArray'closure(*Self)
    sigShowPopup:       SigArray'closure(*Self)
    sigCheckChanged:    SigArray'closure(*Self)
    sigPaintIcon:   closure(*Self, *PaintContext, Math.Rectangle)

    isChecked: bool
    popup:     *Wnd
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
            if evt.target == self or evt.target == popup
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

        if evt.setFlags & .ToolTip
        {
            toolTip = evt.toolTip
        }

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
        var colorText:      Color = ?
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
            colorText    = isChecked ? colors.btnIcon_CheckedDisabledText : colors.btnIcon_DisabledText
            colorChecked = colors.btnIcon_CheckedDisabledMark
        case isPressed or (isChecked and checkedForm == .None):
            colorBk      = isChecked ? colors.btnIcon_CheckedPressedBk : colors.btnIcon_PressedBk
            colorIcon    = isChecked ? colors.btnIcon_CheckedPressedIcon : colors.btnIcon_PressedIcon
            colorText    = isChecked ? colors.btnIcon_CheckedPressedText : colors.btnIcon_PressedText
            colorChecked = colors.btnIcon_CheckedPressedMark
        case isHot:
            colorBk      = isChecked ? colors.btnIcon_CheckedHotBk : colors.btnIcon_HotBk
            colorIcon    = isChecked ? colors.btnIcon_CheckedHotIcon : colors.btnIcon_HotIcon
            colorText    = isChecked ? colors.btnIcon_CheckedHotText : colors.btnIcon_HotText
            colorChecked = colors.btnIcon_CheckedHotMark
        default:
            colorBk      = isChecked ? colors.btnIcon_CheckedBk : colors.btnIcon_Bk
            colorIcon    = isChecked ? colors.btnIcon_CheckedIcon : colors.btnIcon_Icon
            colorText    = isChecked ? colors.btnIcon_CheckedText : colors.btnIcon_Text
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

        pos := getClientRectPadding()
        hasIcon := icon.isValid() or sigPaintIcon != null

        // Arrow
        if arrowPos != .None
        {
            posArrow := pos
            w := metrics.btnIcon_WidthArrow
            h := metrics.btnIcon_HeightArrow

            switch arrowPos
            {
            case Left:
                subRectBk = &rects.btnIcon_ArrowLeft
                posArrow.width = h
                posArrow.height = w
                posArrow.x = pos.x + metrics.btnIcon_MarginArrow
                posArrow.y = pos.vertCenter() - w * 0.5
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.moveLeft(metrics.btnIcon_MarginArrow + w + metrics.btnIcon_SpacingArrowH)

            case Top:
                subRectBk = &rects.btnIcon_ArrowUp
                posArrow.width = w
                posArrow.height = h
                posArrow.x = pos.horzCenter() - w * 0.5
                posArrow.y = pos.y + metrics.btnIcon_MarginArrow
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.moveTop(metrics.btnIcon_MarginArrow + h + metrics.btnIcon_SpacingArrowV)

            case Right:
                subRectBk = &rects.btnIcon_ArrowRight
                posArrow.width = h
                posArrow.height = w
                posArrow.x = pos.right() - w - metrics.btnIcon_MarginArrow
                posArrow.y = pos.vertCenter() - h * 0.5
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.setRight(posArrow.x - metrics.btnIcon_SpacingArrowH)

            case RightUp:
                subRectBk = &rects.btnIcon_ArrowUp
                posArrow.width = w
                posArrow.height = h
                posArrow.x = pos.right() - w - metrics.btnIcon_MarginArrow
                posArrow.y = pos.vertCenter() - h * 0.5
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.setRight(posArrow.x - metrics.btnIcon_SpacingArrowH)

            case RightDown:
                subRectBk = &rects.btnIcon_ArrowDown
                posArrow.width = w
                posArrow.height = h
                posArrow.x = pos.right() - w - metrics.btnIcon_MarginArrow
                posArrow.y = pos.vertCenter() - h * 0.5
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.setRight(posArrow.x - metrics.btnIcon_SpacingArrowH)

            case Bottom:
                subRectBk = &rects.btnIcon_ArrowDown
                posArrow.width = w
                posArrow.height = h
                posArrow.x = pos.horzCenter() - w * 0.5
                posArrow.y = pos.bottom() - h - metrics.btnIcon_MarginArrow
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.setBottom(posArrow.y - metrics.btnIcon_SpacingArrowV)
            }
        }

        // Name
        if !name.isEmpty()
        {
            var sf: RichStringFormat
            theme.setTextColors(&sf, colorText)
            sf.font = theme.res.fontDefault
            sf.horzAlignment = .Center
            sf.vertAlignment = .Center
            switch textPos
            {
            case Left:
                sf.horzAlignment = .Left
                if !hasIcon and (arrowPos == .Top or arrowPos == .Bottom)
                    sf.horzAlignment = .Center
                bc.painter.drawRichString(pos, &name, sf)
                pos.moveLeft(name.boundRect.width + metrics.btnIcon_SpacingText)

                if !icon.isValid()
                    icon.sizeX = pos.width
                if pos.width < icon.sizeX
                {
                    ratio := icon.sizeX / icon.sizeY
                    icon.sizeX = pos.width * ratio
                    icon.sizeY = pos.width
                }
                else
                {
                    pos.x = pos.right() - icon.sizeX
                    pos.width = icon.sizeX
                }

            case Right:
                sf.horzAlignment = .Right
                if !hasIcon and (arrowPos == .Top or arrowPos == .Bottom)
                    sf.horzAlignment = .Center
                bc.painter.drawRichString(pos, &name, sf)
                pos.width -= name.boundRect.width + metrics.btnIcon_SpacingText

                if !icon.isValid()
                    icon.sizeX = pos.width
                if pos.width < icon.sizeX
                {
                    ratio := icon.sizeX / icon.sizeY
                    icon.sizeX = pos.width * ratio
                    icon.sizeY = pos.width
                }
                else
                {
                    pos.width = icon.sizeX
                }

            case Top:
                sf.vertAlignment = .Top
                bc.painter.drawRichString(pos, &name, sf)
                pos.moveTop(sf.font.regular.height + metrics.btnIcon_SpacingText)

                if !icon.isValid()
                    icon.sizeY = pos.height
                if pos.height < icon.sizeY
                {
                    ratio := icon.sizeX / icon.sizeY
                    icon.sizeX = pos.height * ratio
                    icon.sizeY = pos.height
                }
                else
                {
                    pos.y = pos.bottom() - icon.sizeY
                    pos.height = icon.sizeY
                }

            case Bottom:
                sf.vertAlignment = .Bottom
                bc.painter.drawRichString(pos, &name, sf)

                pos.height -= sf.font.regular.height + metrics.btnIcon_SpacingText
                if !icon.isValid()
                    icon.sizeY = pos.height
                if pos.height < icon.sizeY
                {
                    ratio := icon.sizeX / icon.sizeY
                    icon.sizeX = pos.height * ratio
                    icon.sizeY = pos.height
                }
                else
                    pos.height = icon.sizeY
            }
        }

        // Icon
        if sigPaintIcon
        {
            sigPaintIcon(self, bc, pos)
        }
        else
        {
            switch textPos
            {
            case Left:
                pos.x = pos.right() - icon.sizeX
                pos.y = pos.vertCenter() - icon.sizeY * 0.5
                icon.paint(bc.painter, pos.x, pos.y, colorIcon)
            case Right:
                pos.y = pos.vertCenter() - icon.sizeY * 0.5
                icon.paint(bc.painter, pos.x, pos.y, colorIcon)
            case Top:
                pos.x = pos.horzCenter() - icon.sizeX * 0.5
                pos.y = pos.bottom() - icon.sizeY
                icon.paint(bc.painter, pos.x, pos.y, colorIcon)
            case Bottom:
                pos.x = pos.horzCenter() - icon.sizeX * 0.5
                icon.paint(bc.painter, pos.x, pos.y, colorIcon)
            }
        }
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if processMouseEvent(evt)
        {
            if iconBtnFlags & .CheckableToggle
            {
                isChecked = !isChecked
                postCommandEvent(id)
                sigCheckChanged.call(self)
            }
            elif iconBtnFlags & .Checkable
            {
                if !isChecked
                {
                    isChecked = true
                    postCommandEvent(id)
                    sigCheckChanged.call(self)
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
        res.padding = res.getThemeMetrics().btnIcon_Padding
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