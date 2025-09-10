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
    iconColor:           Color
    textColor:           Color

    sigHidePopup:        SigArray'func||(*IconButton)
    sigShowPopup:        SigArray'func||(*IconButton)
    sigCheckChanged:     SigArray'func||(*IconButton)
    sigPaintIcon:        func||(*IconButton, *PaintContext, Math.Rectangle, PaintItemFlags)

    aniBk:               BlendColor
    isChecked:           bool
    popup:               *Wnd
}

impl IWnd for IconButton
{
    mtd hidePopup()
    {
        if !me.popup:
            return

        me.getApp().unregisterHookEvents(me)
        me.sigHidePopup.call(me)
        me.popup.destroy()
        me.popup     = null
        me.isChecked = false
        me.invalidate()
    }

    mtd impl onHookEvent(evt: IEvent)
    {
        if !me.popup:
            return

        if let ptr = evt as MouseEvent where ptr.kind == .MousePressed
        {
            if ptr.target == me or ptr.target == me.popup:
                return
            if ptr.target and me.popup.isParentOf(ptr.target):
                return
            me.hidePopup()
        }
    }

    mtd impl onDestroyEvent(evt: *DestroyEvent)
    {
        me.hidePopup()
    }

    mtd impl onApplyStateEvent(evt: *CommandStateEvent)
    {
        if evt.id != me.id:
            return

        if evt.setFlags.has(.ToolTip)
        {
            me.toolTip = evt.toolTip
        }

        if evt.setFlags.has(.Checked) and me.isChecked != evt.checked
        {
            evt.accepted = true
            me.isChecked = evt.checked
            me.invalidate()
        }

        me.button.IWnd.onApplyStateEvent(evt)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let theme   = me.getTheme()
        let colors  = me.getThemeColors()
        let rects   = me.getThemeRects()
        let metrics = me.getThemeMetrics()

        var subRectBk:    *ThemeImageRect
        var colorBk:      Color = undefined
        var colorIcon:    Color = undefined
        var colorText:    Color = undefined
        var colorChecked: Color = undefined

        switch me.form
        {
        case Square:
            subRectBk = &rects.btnIcon_SquareBk
        case Round:
            subRectBk = &rects.btnIcon_RoundBk
        case RoundSquare:
            subRectBk = &rects.btnIcon_RoundSquareBk
        }

        var paintFlags: PaintItemFlags = .Zero
        switch
        {
        case bc.isDisabled:
            colorBk      = me.isChecked ? colors.btnIcon_CheckedDisabledBk : colors.btnIcon_DisabledBk
            colorIcon    = me.isChecked ? colors.btnIcon_CheckedDisabledIcon : colors.btnIcon_DisabledIcon
            colorText    = me.isChecked ? colors.btnIcon_CheckedDisabledText : colors.btnIcon_DisabledText
            colorChecked = colors.btnIcon_CheckedDisabledMark
        case me.isPressed or (me.isChecked and me.checkedForm == .None):
            colorBk      = me.isChecked ? colors.btnIcon_CheckedPressedBk : colors.btnIcon_PressedBk
            colorIcon    = me.isChecked ? colors.btnIcon_CheckedPressedIcon : colors.btnIcon_PressedIcon
            colorText    = me.isChecked ? colors.btnIcon_CheckedPressedText : colors.btnIcon_PressedText
            colorChecked = colors.btnIcon_CheckedPressedMark
        case me.isHot:
            colorBk      = me.isChecked ? colors.btnIcon_CheckedHotBk : colors.btnIcon_HotBk
            colorIcon    = me.isChecked ? colors.btnIcon_CheckedHotIcon : colors.btnIcon_HotIcon
            colorText    = me.isChecked ? colors.btnIcon_CheckedHotText : colors.btnIcon_HotText
            colorChecked = colors.btnIcon_CheckedHotMark
        default:
            colorBk      = me.isChecked ? colors.btnIcon_CheckedBk : colors.btnIcon_Bk
            colorIcon    = me.isChecked ? colors.btnIcon_CheckedIcon : colors.btnIcon_Icon
            colorText    = me.isChecked ? colors.btnIcon_CheckedText : colors.btnIcon_Text
            colorChecked = colors.btnIcon_CheckedMark
        }

        if bc.isDisabled:
            paintFlags.add(.Disabled)
        if me.isChecked:
            paintFlags.add(.Checked)
        if me.isHot:
            paintFlags.add(.Hot)
        if me.isPressed:
            paintFlags.add(.Pressed)

        if me.iconColor.a:
            colorIcon = me.iconColor
        if me.textColor.a:
            colorText = me.textColor

        // Background
        var clientRect = me.getClientRect()
        me.aniBk.apply(me, colorBk)
        theme.drawSubRect(bc.painter, clientRect, subRectBk, me.aniBk.cur())

        if me.isChecked and me.checkedForm != .None
        {
            switch me.checkedForm
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
                if me.form == .Square:
                    subRectBk = &rects.btnIcon_CheckedSquare
                elif me.form == .Round:
                    subRectBk = &rects.btnIcon_CheckedRound
                else:
                    subRectBk = &rects.btnIcon_CheckedRoundSquare
            }

            theme.drawSubRect(bc.painter, clientRect, subRectBk, colorChecked)
        }

        var pos     = me.getClientRectPadding()
        let hasIcon = me.icon.isValid() or me.sigPaintIcon != null

        if me.iconSizeX:
            me.icon.sizeX = me.iconSizeX
        if me.iconSizeY:
            me.icon.sizeY = me.iconSizeY

        // Center everything
        var marginArrow = metrics.btnIcon_MarginArrow
        if me.iconBtnFlags.has(.Center)
        {
            var nw, nh = 0'f32
            marginArrow = 0

            #[Swag.Complete]
            switch me.arrowPos
            {
            case Left, Right, RightUp, RightDown:
                nw += metrics.btnIcon_WidthArrow + metrics.btnIcon_SpacingArrowH
            case Top, Bottom:
                nh += metrics.btnIcon_HeightArrow + metrics.btnIcon_SpacingArrowV
            case None:
                break
            }

            let fontR = me.getFont().regular
            var sm    = Painter.measureString(me.name.raw.toString(), fontR)
            nw += sm.width
            nh += fontR.height

            if hasIcon
            {
                #[Swag.Complete]
                switch me.textPos
                {
                case Left, Right:
                    nw += metrics.btnIcon_SpacingTextH
                    nw += me.icon.sizeX
                case Top, Bottom:
                    nh += metrics.btnIcon_SpacingTextV
                    nh += me.icon.sizeY
                }
            }

            pos.x      = pos.horzCenter() - nw * 0.5
            pos.y      = pos.vertCenter() - nh * 0.5
            pos.width  = nw
            pos.height = nh
        }

        // Arrow
        if me.arrowPos != .None
        {
            var posArrow = pos
            let w        = metrics.btnIcon_WidthArrow
            let h        = metrics.btnIcon_HeightArrow

            switch me.arrowPos
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
        if !me.name.isEmpty() and !me.iconBtnFlags.has(.HideName)
        {
            var sf: RichStringFormat
            theme.setTextColors(&sf, colorText)
            sf.font          = me.getFont()
            sf.horzAlignment = .Center
            sf.vertAlignment = .Center
            sf.flags         = .DontAdaptSingleLineHeight
            switch me.textPos
            {
            case Left:
                sf.horzAlignment = .Left
                if !hasIcon and (me.arrowPos == .Top or me.arrowPos == .Bottom):
                    sf.horzAlignment = .Center
                bc.painter.drawRichString(pos, &me.name, sf)
                pos.moveLeft(me.name.boundRect.width + metrics.btnIcon_SpacingTextH)

                if !me.icon.isValid():
                    me.icon.sizeX = pos.width
                if pos.width < me.icon.sizeX
                {
                    let ratio = me.icon.sizeX / me.icon.sizeY
                    me.icon.sizeX = pos.width * ratio
                    me.icon.sizeY = pos.width
                }
                else
                {
                    pos.x     = pos.right() - me.icon.sizeX
                    pos.width = me.icon.sizeX
                }

            case Right:
                sf.horzAlignment = .Right
                if !hasIcon and (me.arrowPos == .Top or me.arrowPos == .Bottom):
                    sf.horzAlignment = .Center
                bc.painter.drawRichString(pos, &me.name, sf)
                pos.width -= me.name.boundRect.width + metrics.btnIcon_SpacingTextH

                if !me.icon.isValid():
                    me.icon.sizeX = pos.width
                if pos.width < me.icon.sizeX
                {
                    let ratio = me.icon.sizeX / me.icon.sizeY
                    me.icon.sizeX = pos.width * ratio
                    me.icon.sizeY = pos.width
                }
                else
                {
                    pos.width = me.icon.sizeX
                }

            case Top:
                sf.vertAlignment = .Top
                bc.painter.drawRichString(pos, &me.name, sf)
                pos.moveTop(sf.font.regular.height + metrics.btnIcon_SpacingTextV)

                if !me.icon.isValid():
                    me.icon.sizeY = pos.height
                if pos.height < me.icon.sizeY
                {
                    let ratio = me.icon.sizeX / me.icon.sizeY
                    me.icon.sizeX = pos.height * ratio
                    me.icon.sizeY = pos.height
                }
                else
                {
                    pos.y      = pos.bottom() - me.icon.sizeY
                    pos.height = me.icon.sizeY
                }

            case Bottom:
                sf.vertAlignment = .Bottom
                bc.painter.drawRichString(pos, &me.name, sf)

                pos.height -= sf.font.regular.height + metrics.btnIcon_SpacingTextV
                if !me.icon.isValid():
                    me.icon.sizeY = pos.height
                if pos.height < me.icon.sizeY
                {
                    let ratio = me.icon.sizeX / me.icon.sizeY
                    me.icon.sizeX = pos.height * ratio
                    me.icon.sizeY = pos.height
                }
                else:
                    pos.height = me.icon.sizeY
            }
        }

        // Icon
        if me.sigPaintIcon
        {
            me.sigPaintIcon(me, bc, pos, paintFlags)
        }
        elif me.name.isEmpty() or me.iconBtnFlags.has(.HideName)
        {
            pos.x = pos.horzCenter() - me.icon.sizeX * 0.5
            pos.y = pos.vertCenter() - me.icon.sizeY * 0.5
            me.icon.paint(bc.painter, pos.x, pos.y, colorIcon)
        }
        else
        {
            switch me.textPos
            {
            case Left:
                pos.x = pos.right() - me.icon.sizeX
                pos.y = pos.vertCenter() - me.icon.sizeY * 0.5
                me.icon.paint(bc.painter, pos.x, pos.y, colorIcon)
            case Right:
                pos.y = pos.vertCenter() - me.icon.sizeY * 0.5
                me.icon.paint(bc.painter, pos.x, pos.y, colorIcon)
            case Top:
                pos.x = pos.horzCenter() - me.icon.sizeX * 0.5
                pos.y = pos.bottom() - me.icon.sizeY
                me.icon.paint(bc.painter, pos.x, pos.y, colorIcon)
            case Bottom:
                pos.x = pos.horzCenter() - me.icon.sizeX * 0.5
                me.icon.paint(bc.painter, pos.x, pos.y, colorIcon)
            }
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if me.processMouseEvent(evt)
        {
            if me.iconBtnFlags.has(.CheckableToggle)
            {
                me.isChecked = !me.isChecked
                me.postCommandEvent(me.id)
                me.sigCheckChanged.call(me)
            }
            elif me.iconBtnFlags.has(.Checkable)
            {
                if !me.isChecked
                {
                    me.isChecked = true
                    me.postCommandEvent(me.id)
                    me.sigCheckChanged.call(me)
                }
            }
            else
            {
                me.postCommandEvent(me.id)
            }

            // Display/Hide popup
            if me.iconBtnFlags.has(.Popup)
            {
                if !me.popup
                {
                    me.sigShowPopup.call(me)
                    if me.popup:
                        me.getApp().registerHookEvents(me)
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
        if !me.iconBtnFlags.has(.Checkable):
            return
        if me.isChecked == checked:
            return
        me.isChecked = checked
        me.postCommandEvent(me.id)
        me.invalidate()
    }

    // Set the button form
    mtd setForm(form: IconButtonForm)
    {
        me.form = form
        me.invalidate()
    }

    // Creates a popup window linked to the button position.
    mtd createAnchorPopup(w, h: f32)->*FrameWnd
    {
        var pos     = me.getSurfaceRect()
        let topView = me.getTopView()
        var pt      = topView.surfaceToLocal({pos.x, pos.bottom()})
        pt.y += me.getThemeMetrics().frameWnd_AnchorHeight

        var sdw = me.getThemeMetrics().frameWnd_SmallShadowSize

        with let frame = Wnd.create'FrameWnd(topView, {pt.x, pt.y, w, h})
        {
            .owner = me
            .setFrameForm(.Round)
            .setFrameFlags(.SmallShadow)
            .style.addStyleSheetColors("frameWnd_Border $frameWnd_PopupBorder")
            .fitPosInParent(sdw.z, sdw.w)
            .anchor    = .Top
            .anchorPos = pos.horzCenter() - frame.getSurfaceRect().x
        }

        me.popup = frame
        return frame
    }
}
