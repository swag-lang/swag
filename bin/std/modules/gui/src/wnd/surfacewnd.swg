#global marked
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
        let sdw = me.getThemeMetrics().surfaceWnd_ShadowSize

        var newSizingBorder = SizingBorder.Zero
        if me.surface.flags.has(.Sizeable)
        {
            const SizeBorder = 10
            if surfacePos.x > sdw - SizeBorder and
               surfacePos.x < sdw + SizeBorder and
               surfacePos.y > sdw - SizeBorder and
               surfacePos.y < me.surface.position.height - sdw + SizeBorder
            {
                newSizingBorder |= .Left
            }

            if surfacePos.y > sdw - SizeBorder and
               surfacePos.y < sdw + SizeBorder and
               surfacePos.x > sdw - SizeBorder and
               surfacePos.x < me.surface.position.width - sdw + SizeBorder
            {
                newSizingBorder |= .Top
            }

            if surfacePos.x > me.surface.position.width - sdw - SizeBorder and
               surfacePos.x < me.surface.position.width - sdw + SizeBorder and
               surfacePos.y > sdw - SizeBorder and
               surfacePos.y < me.surface.position.height - sdw + SizeBorder
            {
                newSizingBorder |= .Right
            }

            if surfacePos.y > me.surface.position.height - sdw - SizeBorder and
               surfacePos.y < me.surface.position.height - sdw + SizeBorder and
               surfacePos.x > sdw - SizeBorder and
               surfacePos.x < me.surface.position.width - sdw + SizeBorder
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
            if evt.button != .Left:
                break
            me.surface.bringToTop()
            if me.surface.isMaximized:
                break
            me.captureMouse()
            me.movingSurface = true

        case MouseLeave:
            me.sizingBorder = .Zero

        case MouseReleased:
            if evt.button != .Left:
                break

            me.releaseMouse()
            me.movingSurface = false
            fallthrough

        case MouseMove:
            if me.surface.isMaximized:
                break
            if me.getApp().mouseEnterWnd != me:
                break

            if !me.movingSurface and me.surface.flags.has(.Sizeable)
            {
                let newSizingBorder = me.hitTestBorders(evt.surfacePos)
                if newSizingBorder != me.sizingBorder:
                    me.sizingBorder = newSizingBorder
            }

            switch me.sizingBorder
            {
            case Left, Right:
                me.surface.app.setFrameCursor(Cursor.from(.SizeWE))
            case Top, Bottom:
                me.surface.app.setFrameCursor(Cursor.from(.SizeNS))
            case Right | Bottom, Left | Top:
                me.surface.app.setFrameCursor(Cursor.from(.SizeNWSE))
            case Right | Top, Left | Bottom:
                me.surface.app.setFrameCursor(Cursor.from(.SizeNESW))
            }

            if !me.movingSurface:
                break

            var hasChanged = false
            var pos        = me.surface.position

            if me.sizingBorder & .Left
            {
                pos.x += evt.move.x
                pos.width -= evt.move.x
                hasChanged = true
            }

            if me.sizingBorder & .Top
            {
                pos.y += evt.move.y
                pos.height -= evt.move.y
                hasChanged = true
            }

            if me.sizingBorder & .Right
            {
                pos.width += evt.move.x
                hasChanged = true
            }

            if me.sizingBorder & .Bottom
            {
                pos.height += evt.move.y
                hasChanged = true
            }

            if me.sizingBorder == .Zero
            {
                pos.offset(evt.move.x, evt.move.y)
                hasChanged = true
            }

            if hasChanged
            {
                me.surface.setPosition(pos)
                me.surface.invalidate()
            }
        }
    }

    mtd impl onDestroyEvent(evt: *DestroyEvent)
    {
        me.surface.app.destroySurface(me.surface)
    }

    mtd impl onSysCommandEvent(evt: *SysCommandEvent)
    {
        switch evt.kind
        {
        case Maximize:
            me.surface.showMaximized()
        case Minimize:
            me.surface.showMinimized()
        case Restore:
            me.surface.showNormal()
        case Close:
            me.surface.destroy()
        }
    }

    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        var clientRect = me.getClientRect()

        var pos = clientRect
        pos.width  = me.surface.wnd.padding.y * 1.5
        pos.x      = clientRect.right() - pos.width
        pos.height = me.surface.wnd.padding.y
        pos.y      = 0

        if me.closeBtn
        {
            me.closeBtn.setPosition(pos)
            pos.x -= pos.width
        }

        if me.maximizeBtn
        {
            me.maximizeBtn.setPosition(pos)
            pos.x -= pos.width
        }

        if me.minimizeBtn
        {
            me.minimizeBtn.setPosition(pos)
            pos.x -= pos.width
        }

        me.base.IWnd.onResizeEvent(evt)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        if me.isPendingDestroy:
            return
        let bc     = evt.bc
        let theme  = me.getTheme()
        let colors = me.getThemeColors()
        let rects  = me.getThemeRects()

        var clientRect = me.getClientRect()

        if me.surface.flags.has(.Border)
        {
            let subRect = &rects.wnd_Border
            theme.drawSubRect(bc.painter, clientRect, subRect, colors.wnd_Caption, center: false)
        }

        var posCaption = clientRect
        if me.surface.flags.has(.BigCaption)
        {
            posCaption.height = me.surface.wnd.padding.y
            let subRect = &rects.wnd_Caption
            let colorBk = me.surface.app.isActivated ? colors.wnd_Caption : colors.wnd_CaptionNotActived
            theme.drawSubRect(bc.painter, posCaption, subRect, colorBk)
        }

        if me.surface.flags.has(.SmallCaption)
        {
            posCaption.height = me.surface.wnd.padding.y
            let subRect = &rects.wnd_CaptionSmall
            let colorBk = me.surface.app.isActivated ? colors.wnd_Caption : colors.wnd_CaptionNotActived
            theme.drawSubRect(bc.painter, posCaption, subRect, colorBk)
        }

        if me.surface.flags.has(.BigCaption | .SmallCaption)
        {
            posCaption.x += theme.metrics.wnd_CaptionMarginTitle

            var posClip = posCaption
            if me.surface.flags.has(.MinimizeBtn):
                posClip.setRight(me.minimizeBtn.position.x)
            elif me.surface.flags.has(.MaximizeBtn):
                posClip.setRight(me.maximizeBtn.position.x)
            elif me.surface.flags.has(.CloseBtn):
                posClip.setRight(me.closeBtn.position.x)
            bc.painter.pushClippingRect(posClip)
            defer bc.painter.popClippingRect()

            if me.icon.isValid()
            {
                me.icon.paint(bc.painter, posCaption.x, posCaption.vertCenter() - me.icon.sizeY * 0.5)
                posCaption.x += me.icon.sizeX + theme.metrics.iconText_Margin
            }

            var sf: Pixel.RichStringFormat
            sf.vertAlignment = .Center
            sf.font          = me.getFont()
            sf.palette[0]    = me.surface.app.isActivated ? colors.wnd_CaptionText : colors.wnd_CaptionNotActivatedText
            bc.painter.drawRichString(posCaption, &me.name, sf)
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
        let theme = me.surface.app.theme
        if !theme.intialized:
            return

        if me.surface.flags.has(.MinimizeBtn)
        {
            if !me.minimizeBtn
            {
                with me.minimizeBtn = IconButton.create(me, theme.getIcon24(ThemeIcons24.WndMinimize), {0, 0, 0, 0})
                {
                    .sigPressed += &onMinimize
                }
            }
        }
        elif me.minimizeBtn
        {
            me.minimizeBtn.destroy()
            me.minimizeBtn = null
        }

        if me.surface.flags.has(.MaximizeBtn)
        {
            if !me.maximizeBtn
            {
                with me.maximizeBtn = IconButton.create(me, theme.getIcon24(ThemeIcons24.WndMaximize), {0, 0, 0, 0})
                {
                    .sigPressed += &onMaximize
                }
            }
        }
        elif me.maximizeBtn
        {
            me.maximizeBtn.destroy()
            me.maximizeBtn = null
        }

        if me.surface.flags.has(.CloseBtn)
        {
            if !me.closeBtn
            {
                with me.closeBtn = IconButton.create(me, theme.getIcon24(ThemeIcons24.WndClose), {0, 0, 0, 0})
                {
                    .sigPressed += &onClose
                    .style.addStyleSheetColors(#nameof(ThemeColors.btnIcon_HotBk) ++ " $" ++ #nameof(ThemeColors.wnd_BtnCloseBk))
                    .style.addStyleSheetColors(#nameof(ThemeColors.btnIcon_PressedBk) ++ " $" ++ #nameof(ThemeColors.wnd_BtnCloseHotBk))
                }
            }
        }
        elif me.closeBtn
        {
            me.closeBtn.destroy()
            me.closeBtn = null
        }
    }

    mtd create(userView: #null *Wnd, hook: HookEvent)
    {
        me.computeStyle()

        if userView
        {
            me.view             = userView
            me.view.hookOnEvent = hook
            me.view.setParent(me)
        }
        else
        {
            me.createView'Wnd(hook)
        }

        me.applySurfaceFlags()
    }
}
