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
    Zero            = 0
    Checkable
    CheckableToggle
    Popup
    Center
    HideName
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
    using button:        Button

    form                 = IconButtonForm.Square
    iconBtnFlags         = IconButtonFlags.Zero
    checkedForm          = IconButtonCheckedForm.Full
    arrowPos             = IconButtonArrowPos.None
    textPos              = IconButtonTextPos.Bottom
    forceFgColor:        Color

    sigHidePopup:        SigArray'func||(*Self)
    sigShowPopup:        SigArray'func||(*Self)
    sigCheckChanged:     SigArray'func||(*Self)
    sigPaintIcon:        func||(*Self, *PaintContext, Math.Rectangle)

    aniBk:               BlendColor
    isChecked:           bool
    popup:               *Wnd
}

impl IWnd for IconButton
{
    mtd hidePopup()
    {
        if !popup:
            return

        getApp().unregisterHookEvents(self)
        sigHidePopup.call(self)
        popup.destroy()
        popup     = null
        isChecked = false
        invalidate()
    }

    mtd impl onHookEvent(evt: IEvent)
    {
        if !popup:
            return

        if let ptr = evt as MouseEvent where ptr.kind == .MousePressed
        {
            if ptr.target == self or ptr.target == popup:
                return
            if ptr.target and popup.isParentOf(ptr.target):
                return
            hidePopup()
        }
    }

    mtd impl onDestroyEvent(evt: *DestroyEvent)
    {
        hidePopup()
    }

    mtd impl onApplyStateEvent(evt: *CommandStateEvent)
    {
        if evt.id != id:
            return

        if evt.setFlags.has(.ToolTip)
        {
            toolTip = evt.toolTip
        }

        if evt.setFlags.has(.Checked) and isChecked != evt.checked
        {
            evt.accepted = true
            isChecked    = evt.checked
            invalidate()
        }

        button.IWnd.onApplyStateEvent(evt)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let theme   = getTheme()
        let colors  = getThemeColors()
        let rects   = getThemeRects()
        let metrics = getThemeMetrics()

        var subRectBk:    *ThemeImageRect
        var colorBk:      Color = undefined
        var colorIcon:    Color = undefined
        var colorText:    Color = undefined
        var colorChecked: Color = undefined

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

        if forceFgColor.a
        {
            colorIcon = forceFgColor
            colorText = forceFgColor
        }

        // Background
        var clientRect = getClientRect()
        aniBk.apply(self, colorBk)
        theme.drawSubRect(bc.painter, clientRect, subRectBk, aniBk.cur())

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

        var pos     = getClientRectPadding()
        let hasIcon = icon.isValid() or sigPaintIcon != null

        if iconSizeX:
            icon.sizeX = iconSizeX
        if iconSizeY:
            icon.sizeY = iconSizeY

        // Center everything
        var marginArrow = metrics.btnIcon_MarginArrow
        if iconBtnFlags.has(.Center)
        {
            var nw, nh = 0'f32
            marginArrow = 0

            #[Swag.Complete]
            switch arrowPos
            {
            case Left, Right, RightUp, RightDown:
                nw += metrics.btnIcon_WidthArrow + metrics.btnIcon_SpacingArrowH
            case Top, Bottom:
                nh += metrics.btnIcon_HeightArrow + metrics.btnIcon_SpacingArrowV
            case None:
                break
            }

            let fontR = getFont().regular
            var sm    = Painter.measureString(name.raw.toString(), fontR)
            nw += sm.width
            nh += fontR.height

            if hasIcon
            {
                #[Swag.Complete]
                switch textPos
                {
                case Left, Right:
                    nw += metrics.btnIcon_SpacingTextH
                    nw += icon.sizeX
                case Top, Bottom:
                    nh += metrics.btnIcon_SpacingTextV
                    nh += icon.sizeY
                }
            }

            pos.x      = pos.horzCenter() - nw * 0.5
            pos.y      = pos.vertCenter() - nh * 0.5
            pos.width  = nw
            pos.height = nh
        }

        // Arrow
        if arrowPos != .None
        {
            var posArrow = pos
            let w        = metrics.btnIcon_WidthArrow
            let h        = metrics.btnIcon_HeightArrow

            switch arrowPos
            {
            case Left:
                subRectBk       = &rects.btnIcon_ArrowLeft
                posArrow.width  = h
                posArrow.height = w
                posArrow.x      = pos.x + marginArrow
                posArrow.y      = pos.vertCenter() - w * 0.5
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.moveLeft(marginArrow + w + metrics.btnIcon_SpacingArrowH)

            case Top:
                subRectBk       = &rects.btnIcon_ArrowUp
                posArrow.width  = w
                posArrow.height = h
                posArrow.x      = pos.horzCenter() - w * 0.5
                posArrow.y      = pos.y + marginArrow
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.moveTop(marginArrow + h + metrics.btnIcon_SpacingArrowV)

            case Right:
                subRectBk       = &rects.btnIcon_ArrowRight
                posArrow.width  = h
                posArrow.height = w
                posArrow.x      = pos.right() - w - marginArrow
                posArrow.y      = pos.vertCenter() - h * 0.5
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.setRight(posArrow.x - metrics.btnIcon_SpacingArrowH)

            case RightUp:
                subRectBk       = &rects.btnIcon_ArrowUp
                posArrow.width  = w
                posArrow.height = h
                posArrow.x      = pos.right() - w - marginArrow
                posArrow.y      = pos.vertCenter() - h * 0.5
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.setRight(posArrow.x - metrics.btnIcon_SpacingArrowH)

            case RightDown:
                subRectBk       = &rects.btnIcon_ArrowDown
                posArrow.width  = w
                posArrow.height = h
                posArrow.x      = pos.right() - w - marginArrow
                posArrow.y      = pos.vertCenter() - h * 0.5
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.setRight(posArrow.x - metrics.btnIcon_SpacingArrowH)

            case Bottom:
                subRectBk       = &rects.btnIcon_ArrowDown
                posArrow.width  = w
                posArrow.height = h
                posArrow.x      = pos.horzCenter() - w * 0.5
                posArrow.y      = pos.bottom() - h - marginArrow
                theme.drawSubRect(bc.painter, posArrow, subRectBk, colorIcon)
                pos.setBottom(posArrow.y - metrics.btnIcon_SpacingArrowV)
            }
        }

        // Name
        if !name.isEmpty() and !iconBtnFlags.has(.HideName)
        {
            var sf: RichStringFormat
            theme.setTextColors(&sf, colorText)
            sf.font          = getFont()
            sf.horzAlignment = .Center
            sf.vertAlignment = .Center
            sf.flags         = .DontAdaptSingleLineHeight
            switch textPos
            {
            case Left:
                sf.horzAlignment = .Left
                if !hasIcon and (arrowPos == .Top or arrowPos == .Bottom):
                    sf.horzAlignment = .Center
                bc.painter.drawRichString(pos, &name, sf)
                pos.moveLeft(name.boundRect.width + metrics.btnIcon_SpacingTextH)

                if !icon.isValid():
                    icon.sizeX = pos.width
                if pos.width < icon.sizeX
                {
                    let ratio = icon.sizeX / icon.sizeY
                    icon.sizeX = pos.width * ratio
                    icon.sizeY = pos.width
                }
                else
                {
                    pos.x     = pos.right() - icon.sizeX
                    pos.width = icon.sizeX
                }

            case Right:
                sf.horzAlignment = .Right
                if !hasIcon and (arrowPos == .Top or arrowPos == .Bottom):
                    sf.horzAlignment = .Center
                bc.painter.drawRichString(pos, &name, sf)
                pos.width -= name.boundRect.width + metrics.btnIcon_SpacingTextH

                if !icon.isValid():
                    icon.sizeX = pos.width
                if pos.width < icon.sizeX
                {
                    let ratio = icon.sizeX / icon.sizeY
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
                pos.moveTop(sf.font.regular.height + metrics.btnIcon_SpacingTextV)

                if !icon.isValid():
                    icon.sizeY = pos.height
                if pos.height < icon.sizeY
                {
                    let ratio = icon.sizeX / icon.sizeY
                    icon.sizeX = pos.height * ratio
                    icon.sizeY = pos.height
                }
                else
                {
                    pos.y      = pos.bottom() - icon.sizeY
                    pos.height = icon.sizeY
                }

            case Bottom:
                sf.vertAlignment = .Bottom
                bc.painter.drawRichString(pos, &name, sf)

                pos.height -= sf.font.regular.height + metrics.btnIcon_SpacingTextV
                if !icon.isValid():
                    icon.sizeY = pos.height
                if pos.height < icon.sizeY
                {
                    let ratio = icon.sizeX / icon.sizeY
                    icon.sizeX = pos.height * ratio
                    icon.sizeY = pos.height
                }
                else:
                    pos.height = icon.sizeY
            }
        }

        // Icon
        if sigPaintIcon
        {
            sigPaintIcon(self, bc, pos)
        }
        elif name.isEmpty() or iconBtnFlags.has(.HideName)
        {
            pos.x = pos.horzCenter() - icon.sizeX * 0.5
            pos.y = pos.vertCenter() - icon.sizeY * 0.5
            icon.paint(bc.painter, pos.x, pos.y, colorIcon)
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

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if processMouseEvent(evt)
        {
            if iconBtnFlags.has(.CheckableToggle)
            {
                isChecked = !isChecked
                postCommandEvent(id)
                sigCheckChanged.call(self)
            }
            elif iconBtnFlags.has(.Checkable)
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
            if iconBtnFlags.has(.Popup)
            {
                if !popup
                {
                    sigShowPopup.call(self)
                    if popup:
                        getApp().registerHookEvents(self)
                }
            }
        }
    }
}

impl IconButton
{
    #[Swag.Discardable]
    func create(parent: *Wnd, icon: Icon, position: Math.Rectangle = {}, id: WndId = null)->*IconButton
    {
        Debug.assert(parent != null)
        var pos = position
        if pos.height == 0:
            pos.height = parent.getThemeMetrics().btnPush_Height
        if pos.width == 0:
            pos.width = parent.getThemeMetrics().btnPush_Width
        let res = Wnd.create'IconButton(parent, null, pos, id)
        res.icon    = icon
        res.padding = res.getThemeMetrics().btnIcon_Padding
        if id != null:
            res.updateCommandState()
        return res
    }

    // Set the check state of the button (if the button is checkable)
    mtd setCheck(checked: bool)
    {
        if !iconBtnFlags.has(.Checkable):
            return
        if isChecked == checked:
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

    // Creates a popup window linked to the button position.
    mtd createAnchorPopup(w, h: f32)->*FrameWnd
    {
        var pos     = getSurfaceRect()
        let topView = getTopView()
        var pt      = topView.surfaceToLocal({pos.x, pos.bottom()})
        pt.y += getThemeMetrics().frameWnd_AnchorHeight

        var sdw = getThemeMetrics().frameWnd_SmallShadowSize

        with let frame = Wnd.create'FrameWnd(topView, {pt.x, pt.y, w, h})
        {
            .owner = self
            .setFrameForm(.Round)
            .setFrameFlags(.SmallShadow)
            .style.addStyleSheetColors("frameWnd_Border $frameWnd_PopupBorder")
            .fitPosInParent(sdw.z, sdw.w)
            .anchor    = .Top
            .anchorPos = pos.horzCenter() - frame.getSurfaceRect().x
        }

        popup = frame
        return frame
    }
}
