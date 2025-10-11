using Core, Pixel, Serialization

#[Swag.EnumFlags]
public enum SizingBorder
{
    Zero   = 0
    Left
    Top
    Right
    Bottom
}

public struct SurfaceWnd
{
    using base:        FrameWnd
    icon:              Icon
    minimizeBtn:       *IconButton
    maximizeBtn:       *IconButton
    closeBtn:          *IconButton
    movingSurface:     bool
    sizingBorder:      SizingBorder = Zero
}

impl IWnd for SurfaceWnd
{
    mtd hitTestBorders(surfacePos: Math.Point)->SizingBorder
    {
        let sdw = .getThemeMetrics().surfaceWnd_ShadowSize

        var newSizingBorder = SizingBorder.Zero
        if .surface.flags.has(.Sizeable)
        {
            const SizeBorder = 10
            if surfacePos.x > sdw - SizeBorder and
               surfacePos.x < sdw + SizeBorder and
               surfacePos.y > sdw - SizeBorder and
               surfacePos.y < .surface.position.height - sdw + SizeBorder
            {
                newSizingBorder |= .Left
            }

            if surfacePos.y > sdw - SizeBorder and
               surfacePos.y < sdw + SizeBorder and
               surfacePos.x > sdw - SizeBorder and
               surfacePos.x < .surface.position.width - sdw + SizeBorder
            {
                newSizingBorder |= .Top
            }

            if surfacePos.x > .surface.position.width - sdw - SizeBorder and
               surfacePos.x < .surface.position.width - sdw + SizeBorder and
               surfacePos.y > sdw - SizeBorder and
               surfacePos.y < .surface.position.height - sdw + SizeBorder
            {
                newSizingBorder |= .Right
            }

            if surfacePos.y > .surface.position.height - sdw - SizeBorder and
               surfacePos.y < .surface.position.height - sdw + SizeBorder and
               surfacePos.x > sdw - SizeBorder and
               surfacePos.x < .surface.position.width - sdw + SizeBorder
            {
                newSizingBorder |= .Bottom
            }
        }

        return newSizingBorder
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MousePressed:
            if evt.button != .Left do
                break
            .surface.bringToTop()
            if .surface.isMaximized do
                break
            .captureMouse()
            .movingSurface = true

        case MouseLeave:
            .sizingBorder = .Zero

        case MouseReleased:
            if evt.button != .Left do
                break

            .releaseMouse()
            .movingSurface = false
            fallthrough

        case MouseMove:
            if .surface.isMaximized do
                break
            if .getApp().mouseEnterWnd != me do
                break

            if !.movingSurface and .surface.flags.has(.Sizeable)
            {
                let newSizingBorder = .hitTestBorders(evt.surfacePos)
                if newSizingBorder != .sizingBorder do
                    .sizingBorder = newSizingBorder
            }

            switch .sizingBorder
            {
            case Left, Right:
                .surface.app.setFrameCursor(Cursor.from(.SizeWE))
            case Top, Bottom:
                .surface.app.setFrameCursor(Cursor.from(.SizeNS))
            case Right | Bottom, Left | Top:
                .surface.app.setFrameCursor(Cursor.from(.SizeNWSE))
            case Right | Top, Left | Bottom:
                .surface.app.setFrameCursor(Cursor.from(.SizeNESW))
            }

            if !.movingSurface do
                break

            var hasChanged = false
            var pos        = .surface.position

            if .sizingBorder & .Left
            {
                pos.x += evt.move.x
                pos.width -= evt.move.x
                hasChanged = true
            }

            if .sizingBorder & .Top
            {
                pos.y += evt.move.y
                pos.height -= evt.move.y
                hasChanged = true
            }

            if .sizingBorder & .Right
            {
                pos.width += evt.move.x
                hasChanged = true
            }

            if .sizingBorder & .Bottom
            {
                pos.height += evt.move.y
                hasChanged = true
            }

            if .sizingBorder == .Zero
            {
                pos.offset(evt.move.x, evt.move.y)
                hasChanged = true
            }

            if hasChanged
            {
                .surface.setPosition(pos)
                .surface.invalidate()
            }
        }
    }

    mtd impl onDestroyEvent(evt: *DestroyEvent)
    {
        .surface.app.destroySurface(.surface)
    }

    mtd impl onSysCommandEvent(evt: *SysCommandEvent)
    {
        switch evt.kind
        {
        case Maximize:
            .surface.showMaximized()
        case Minimize:
            .surface.showMinimized()
        case Restore:
            .surface.showNormal()
        case Close:
            .surface.destroy()
        }
    }

    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        let clientRect = .getClientRect()

        var pos = clientRect
        pos.width  = .surface.wnd.padding.y * 1.5
        pos.x      = clientRect.right() - pos.width
        pos.height = .surface.wnd.padding.y
        pos.y      = 0

        if .closeBtn
        {
            .closeBtn.setPosition(pos)
            pos.x -= pos.width
        }

        if .maximizeBtn
        {
            .maximizeBtn.setPosition(pos)
            pos.x -= pos.width
        }

        if .minimizeBtn
        {
            .minimizeBtn.setPosition(pos)
            pos.x -= pos.width
        }

        .base.IWnd.onResizeEvent(evt)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        if .isPendingDestroy do
            return
        let bc     = evt.bc
        let theme  = .getTheme()
        let colors = .getThemeColors()
        let rects  = .getThemeRects()

        let clientRect = .getClientRect()

        if .surface.flags.has(.Border)
        {
            let subRect = &rects.wnd_Border
            theme.drawSubRect(bc.painter, clientRect, subRect, colors.wnd_CaptionBk, center: false)
        }

        var posCaption = clientRect
        if .surface.flags.has(.BigCaption)
        {
            posCaption.height = .surface.wnd.padding.y
            let subRect = &rects.wnd_CaptionBk
            let colorBk = .surface.app.isActivated ? colors.wnd_CaptionBk : colors.wnd_CaptionBkNotActived
            theme.drawSubRect(bc.painter, posCaption, subRect, colorBk)
        }

        if .surface.flags.has(.SmallCaption)
        {
            posCaption.height = .surface.wnd.padding.y
            let subRect = &rects.wnd_CaptionSmall
            let colorBk = .surface.app.isActivated ? colors.wnd_CaptionBk : colors.wnd_CaptionBkNotActived
            theme.drawSubRect(bc.painter, posCaption, subRect, colorBk)
        }

        if .surface.flags.has(.BigCaption | .SmallCaption)
        {
            posCaption.x += theme.metrics.wnd_CaptionMarginTitle

            var posClip = posCaption
            if .surface.flags.has(.MinimizeBtn) do
                posClip.setRight(.minimizeBtn.position.x)
            elif .surface.flags.has(.MaximizeBtn) do
                posClip.setRight(.maximizeBtn.position.x)
            elif .surface.flags.has(.CloseBtn) do
                posClip.setRight(.closeBtn.position.x)
            bc.painter.pushClippingRect(posClip)
            defer bc.painter.popClippingRect()

            if .icon.isValid()
            {
                .icon.paint(bc.painter, posCaption.x, posCaption.vertCenter() - .icon.sizeY * 0.5)
                posCaption.x += .icon.sizeX + theme.metrics.iconText_Margin
            }

            var sf: Pixel.RichStringFormat
            sf.vertAlignment = .Center
            sf.font          = .getFont()
            sf.palette[0]    = .surface.app.isActivated ? colors.wnd_CaptionText : colors.wnd_CaptionNotActivatedText
            bc.painter.drawRichString(posCaption, &.name, sf)
        }
    }
}

impl SurfaceWnd
{
    func onMinimize(btn: *Button)
    {
        let evt = Event.create'SysCommandEvent()
        evt.kind = .Minimize
        btn.surface.wnd.postEvent(evt)
    }

    func onMaximize(btn: *Button)
    {
        let evt = Event.create'SysCommandEvent()
        evt.kind = btn.surface.isMaximized ? .Restore : .Maximize
        btn.surface.wnd.postEvent(evt)
    }

    func onClose(btn: *Button)
    {
        let evt = Event.create'SysCommandEvent()
        evt.kind = .Close
        btn.surface.wnd.view.postEvent(evt)
    }

    internal mtd applySurfaceFlags()
    {
        let theme = .surface.app.theme
        if !theme.intialized do
            return

        if .surface.flags.has(.MinimizeBtn)
        {
            if !.minimizeBtn
            {
                with me.minimizeBtn = IconButton.create(me, theme.getIcon24(ThemeIcons24.WndMinimize), {0, 0, 0, 0})
                {
                    .sigPressed += &onMinimize
                }
            }
        }
        elif .minimizeBtn
        {
            .minimizeBtn.destroy()
            .minimizeBtn = null
        }

        if .surface.flags.has(.MaximizeBtn)
        {
            if !.maximizeBtn
            {
                with me.maximizeBtn = IconButton.create(me, theme.getIcon24(ThemeIcons24.WndMaximize), {0, 0, 0, 0})
                {
                    .sigPressed += &onMaximize
                }
            }
        }
        elif .maximizeBtn
        {
            .maximizeBtn.destroy()
            .maximizeBtn = null
        }

        if .surface.flags.has(.CloseBtn)
        {
            if !.closeBtn
            {
                with me.closeBtn = IconButton.create(me, theme.getIcon24(ThemeIcons24.WndClose), {0, 0, 0, 0})
                {
                    .sigPressed += &onClose
                    .style.addStyleSheetColors(#nameof(ThemeColors.btnIcon_HotBk) ++ " $" ++ #nameof(ThemeColors.wnd_BtnCloseBk))
                    .style.addStyleSheetColors(#nameof(ThemeColors.btnIcon_PressedBk) ++ " $" ++ #nameof(ThemeColors.wnd_BtnCloseHotBk))
                    .style.addStyleSheetColors(#nameof(ThemeColors.btnIcon_HotIcon) ++ " $" ++ #nameof(ThemeColors.wnd_BtnCloseHotIcon))
                    .style.addStyleSheetColors(#nameof(ThemeColors.btnIcon_PressedIcon) ++ " $" ++ #nameof(ThemeColors.wnd_BtnClosePressedIcon))
                }
            }
        }
        elif .closeBtn
        {
            .closeBtn.destroy()
            .closeBtn = null
        }
    }

    mtd create(userView: #null *Wnd, hook: HookEvent)
    {
        .computeStyle()

        if userView
        {
            .view             = userView
            .view.hookOnEvent = hook
            .view.setParent(me)
        }
        else
        {
            .createView'Wnd(hook)
        }

        .applySurfaceFlags()
    }
}
