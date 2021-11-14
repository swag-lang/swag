using Core, Pixel

#[Swag.EnumFlags]
public enum SizingBorder{ Zero = 0; Left; Top; Right; Bottom; }

public struct SurfaceWnd
{
    using base:     FrameWnd
    icon:           Icon
    minimizeBtn:    *ButtonIcon
    maximizeBtn:    *ButtonIcon
    closeBtn:       *ButtonIcon
    movingSurface:  bool
    sizingBorder:   SizingBorder = Zero
    cursorSet:      bool
}

impl IWnd for SurfaceWnd
{
    mtd hitTestBorders(surfacePos: Math.Point)->SizingBorder
    {
        newSizingBorder := SizingBorder.Zero
        if surface.flags & .Sizeable
        {
            const SizeBorder = 10
            if surfacePos.x < SizeBorder
                newSizingBorder |= .Left
            if surfacePos.y < SizeBorder
                newSizingBorder |= .Top
            if surfacePos.x > surface.position.width - SizeBorder
                newSizingBorder |= .Right
            if surfacePos.y > surface.position.height - SizeBorder
                newSizingBorder |= .Bottom
        }

        return newSizingBorder
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MousePressed:
            if evt.button != .Left
                break
            surface.bringToTop()
            if surface.isMaximized
                break
            captureMouse()
            movingSurface = true

        case MouseReleased:
            if evt.button != .Left
                break
            releaseMouse()
            sizingBorder = .Zero
            movingSurface = false

        case MouseLeave:
            sizingBorder = .Zero
            if cursorSet
            {
                cursorSet = false
                surface.app.popCursor()
            }

        case MouseMove:
            if surface.isMaximized
                break

            if !movingSurface and surface.flags & .Sizeable
            {
                newSizingBorder := hitTestBorders(evt.surfacePos)
                if newSizingBorder != sizingBorder
                {
                    sizingBorder = newSizingBorder
                    if cursorSet
                    {
                        cursorSet = false
                        surface.app.popCursor()
                    }

                    switch newSizingBorder
                    {
                    case Left, Right:
                        cursorSet = true
                        surface.app.pushCursor(Cursor.from(CursorShape.SizeWE))
                    case Top, Bottom:
                        cursorSet = true
                        surface.app.pushCursor(Cursor.from(CursorShape.SizeNS))
                    case Right|Bottom, Left|Top:
                        cursorSet = true
                        surface.app.pushCursor(Cursor.from(CursorShape.SizeNWSE))
                    case Right|Top, Left|Bottom:
                        cursorSet = true
                        surface.app.pushCursor(Cursor.from(CursorShape.SizeNESW))
                    }
                }
            }
            else if !movingSurface and cursorSet
            {
                cursorSet = false
                surface.app.popCursor()
            }

            if !movingSurface
                break

            hasChanged := false
            pos := surface.position

            if sizingBorder & .Left
            {
                pos.x += evt.move.x
                pos.width -= evt.move.x
                hasChanged = true
            }

            if sizingBorder & .Top
            {
                pos.y += evt.move.y
                pos.height -= evt.move.y
                hasChanged = true
            }

            if sizingBorder & .Right
            {
                pos.width += evt.move.x
                hasChanged = true
            }

            if sizingBorder & .Bottom
            {
                pos.height += evt.move.y
                hasChanged = true
            }

            if sizingBorder == .Zero
            {
                pos.offset(evt.move.x, evt.move.y);
                hasChanged = true
            }

            if hasChanged
            {
                surface.setPosition(pos)
                surface.invalidate()
            }
        }
    }

    mtd onDestroyEvent(evt: *DestroyEvent)
    {
        surface.app.destroySurface(surface)
    }

    mtd onSysCommandEvent(evt: *SysCommandEvent)
    {
        switch evt.sysKind
        {
        case Maximize:
            surface.showMaximized()
        case Minimize:
            surface.showMinimized()
        case Restore:
            surface.showNormal()
        case Close:
            surface.destroy()
        }
    }

    mtd onResizeEvent(evt: *ResizeEvent)
    {
        pos := position
        pos.width = surface.wnd.marginsTopLeft.y * 1.5
        pos.x = position.right() - pos.width
        pos.height = surface.wnd.marginsTopLeft.y
        pos.y = 0

        if closeBtn
        {
            closeBtn.setPosition(pos)
            pos.x -= pos.width
        }

        if maximizeBtn
        {
            maximizeBtn.setPosition(pos)
            pos.x -= pos.width
        }

        if minimizeBtn
        {
            minimizeBtn.setPosition(pos)
            pos.x -= pos.width
        }

        base.IWnd.onResizeEvent(evt)
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc     := evt.bc
        theme  := bc.theme
        colors := &bc.theme.colors
        rects  := &bc.theme.rects

        if surface.flags & .Border
        {
            subRect := &rects.wnd_Border
            theme.drawSubRect(bc.painter, position, subRect, colors.wnd_Caption)
        }

        posCaption := position
        if surface.flags & .BigCaption
        {
            posCaption.height = surface.wnd.marginsTopLeft.y
            subRect := &rects.wnd_Caption
            colorBk := surface.app.isActivated ? colors.wnd_Caption : colors.wnd_CaptionNotActived
            theme.drawSubRect(bc.painter, posCaption, subRect, colorBk)
        }

        if surface.flags & .SmallCaption
        {
            posCaption.height = surface.wnd.marginsTopLeft.y
            subRect := &rects.wnd_CaptionSmall
            colorBk := surface.app.isActivated ? colors.wnd_Caption : colors.wnd_CaptionNotActived
            theme.drawSubRect(bc.painter, posCaption, subRect, colorBk)
        }

        if surface.flags & (.BigCaption | .SmallCaption)
        {
            posCaption.x += bc.theme.metrics.wnd_CaptionMarginTitle

            if icon.isValid()
            {
                icon.paint(bc.painter, posCaption.x, posCaption.vertCenter() - icon.size * 0.5)
                posCaption.x += icon.size + theme.metrics.iconText_Margin
            }

            var sf: Pixel.RichStringFormat
            sf.vertAlignment = .Center
            sf.font.regular = theme.res.fontDefault.regular
            sf.palette[0] = surface.app.isActivated ? colors.wnd_CaptionText : colors.wnd_CaptionNotActivatedText
            bc.painter.drawRichString(posCaption, &name, sf)
        }
    }
}

impl SurfaceWnd
{
    func onMinimize(btn: *Button)
    {
        evt := Event.create'SysCommandEvent()
        evt.sysKind = .Minimize
        btn.surface.wnd.postEvent(evt)
    }

    func onMaximize(btn: *Button)
    {
        evt := Event.create'SysCommandEvent()
        evt.sysKind = btn.surface.isMaximized ? .Restore : .Maximize
        btn.surface.wnd.postEvent(evt)
    }

    func onClose(btn: *Button)
    {
        btn.surface.destroy()
    }

    mtd create(userView: *Wnd)
    {
        if userView
        {
            view = userView
            view.parent = self
            view.surface = surface
            childs.add(view)
        }
        else
            createView'Wnd()

        theme := surface.app.theme
        if surface.flags & .MinimizeBtn
        {
            minimizeBtn = ButtonIcon.create(self, theme.getIcon24(ThemeIcons24.WndMinimize), @{0,0,0,0})
            minimizeBtn.sigPressed = &onMinimize
        }

        if surface.flags & .MaximizeBtn
        {
            maximizeBtn = ButtonIcon.create(self, theme.getIcon24(ThemeIcons24.WndMaximize), @{0,0,0,0})
            maximizeBtn.sigPressed = &onMaximize
        }

        if surface.flags & .CloseBtn
        {
            closeBtn = ButtonIcon.create(self, theme.getIcon24(ThemeIcons24.WndClose), @{0,0,0,0})
            closeBtn.sigPressed = &onClose
            using ThemeColors
            closeBtn.style.addStyleSheetColors(@nameof(btnIcon_HotBk)     ~" $"~ @nameof(wnd_BtnCloseBk))
            closeBtn.style.addStyleSheetColors(@nameof(btnIcon_PressedBk) ~" $"~ @nameof(wnd_BtnCloseHotBk))
        }
    }
}