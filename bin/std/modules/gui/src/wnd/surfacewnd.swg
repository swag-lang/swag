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
}

impl IWnd for SurfaceWnd
{
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

            wnd.captureMouse()

            if evt.surfacePos.x < 20
                sizingBorder |= .Left
            if evt.surfacePos.y < 20
                sizingBorder |= .Top
            if evt.surfacePos.x > surface.position.width - 20
                sizingBorder |= .Right
            if evt.surfacePos.y > surface.position.height - 20
                sizingBorder |= .Bottom

            if !sizingBorder
                movingSurface = true

        case MouseReleased:
            if evt.button != .Left
                break
            wnd.releaseMouse()
            sizingBorder = .Zero
            movingSurface = false

        case MouseMove:
            hasChanged := false

            if sizingBorder & .Left
            {
                surface.position.x += evt.move.x
                surface.position.width -= evt.move.x
                hasChanged = true
            }

            if sizingBorder & .Top
            {
                surface.position.y += evt.move.y
                surface.position.height -= evt.move.y
                hasChanged = true
            }

            if sizingBorder & .Right
            {
                surface.position.width += evt.move.x
                hasChanged = true
            }

            if sizingBorder & .Bottom
            {
                surface.position.height += evt.move.y
                hasChanged = true
            }

            if movingSurface
            {
                surface.position.offset(evt.move.x, evt.move.y);
                hasChanged = true
            }

            if hasChanged
            {
                surface.setPosition(surface.position)
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
            closeBtn.setRect(pos)
            pos.x -= pos.width
        }

        if maximizeBtn
        {
            maximizeBtn.setRect(pos)
            pos.x -= pos.width
        }

        if minimizeBtn
        {
            minimizeBtn.setRect(pos)
            pos.x -= pos.width
        }

        base.IWnd.onResizeEvent(evt)
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc := evt.bc
        posCaption := position
        theme := bc.theme

        if surface.flags & .Border
        {
            subRect := &bc.theme.imgMetrics.wndBorder
            theme.drawSubRect(bc.painter, position, subRect, theme.tweak.caption)
        }

        if surface.flags & .BigCaption
        {
            posCaption.height = surface.wnd.marginsTopLeft.y
            subRect := &bc.theme.imgMetrics.wndBigCaption
            colorBk := surface.app.isActivated ? theme.tweak.caption : theme.tweak.captionNotActived
            theme.drawSubRect(bc.painter, posCaption, subRect, colorBk)
        }

        if surface.flags & .SmallCaption
        {
            posCaption.height = surface.wnd.marginsTopLeft.y
            subRect := &theme.imgMetrics.wndSmallCaption
            colorBk := surface.app.isActivated ? theme.tweak.caption : theme.tweak.captionNotActived
            theme.drawSubRect(bc.painter, posCaption, subRect, colorBk)
        }

        if surface.flags & (.BigCaption | .SmallCaption)
        {
            posCaption.x += bc.theme.wndCaptionMarginTitle

            if icon.isValid()
            {
                icon.paint(bc.painter, posCaption.x, posCaption.vertCenter() - icon.size * 0.5)
                posCaption.x += icon.size + theme.iconTextMargin
            }

            var sf: Pixel.StringFormat
            sf.vertAlignment = .Center
            colorTxt := surface.app.isActivated ? theme.tweak.captionText : theme.tweak.captionTextNotActivated
            bc.painter.drawString(posCaption, name, theme.fontDefault.regular, colorTxt, sf)
        }
    }
}

impl SurfaceWnd
{
    protected func onMinimize(btn: *ButtonIcon)
    {
        evt := Event.create'SysCommandEvent()
        evt.sysKind = .Minimize
        btn.surface.wnd.postEvent(evt)
    }

    protected func onMaximize(btn: *ButtonIcon)
    {
        evt := Event.create'SysCommandEvent()
        evt.sysKind = btn.surface.isMaximized ? .Restore : .Maximize
        btn.surface.wnd.postEvent(evt)
    }

    protected func onClose(btn: *ButtonIcon)
    {
        btn.surface.destroy()
    }

    mtd create()
    {
        createView'Wnd()

        theme := &surface.app.theme
        if surface.flags & .MinimizeBtn
        {
            minimizeBtn = ButtonIcon.create(self, Icon.from(&theme.icons24, 2), @{0,0,0,0})
            minimizeBtn.sigPressed = &onMinimize
        }

        if surface.flags & .MaximizeBtn
        {
            maximizeBtn = ButtonIcon.create(self, Icon.from(&theme.icons24, 3), @{0,0,0,0})
            maximizeBtn.sigPressed = &onMaximize
        }

        if surface.flags & .CloseBtn
        {
            closeBtn = ButtonIcon.create(self, Icon.from(&theme.icons24, 4), @{0,0,0,0})
            closeBtn.sigPressed = &onClose
            closeBtn.hookOnSysCommandEvent = @(wnd: *Wnd, evt: *SysCommandEvent)->bool
            {
                if evt.sysKind == .SetTheme
                    wnd.style.setStyleSheet(Format.toString("btnIcon_HotBk 0x%{X}", wnd.style.ref.tweak.windowCloseBtnBk.argb))
                return false
            }
        }
    }
}