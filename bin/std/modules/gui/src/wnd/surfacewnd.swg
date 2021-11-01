using Core, Pixel

public struct SurfaceWnd
{
    using base:     FrameWnd
    icon:           Icon
    minimizeBtn:    *ButtonIcon
    maximizeBtn:    *ButtonIcon
    closeBtn:       *ButtonIcon
}

impl IWnd for SurfaceWnd
{
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
        pos.width = surface.wnd.marginsTopLeft.y
        pos.x = position.right() - pos.width - 16
        pos.height = surface.wnd.marginsTopLeft.y
        pos.y = 0

        if closeBtn
        {
            closeBtn.setRect(pos)
            pos.x -= pos.width * 1.5
        }

        if maximizeBtn
        {
            maximizeBtn.setRect(pos)
            pos.x -= pos.width * 1.5
        }

        if minimizeBtn
        {
            minimizeBtn.setRect(pos)
            pos.x -= pos.width * 1.5
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
            theme.drawSubRect(bc.painter, position, subRect, theme.palette.caption)
        }

        if surface.flags & .BigCaption
        {
            posCaption.height = surface.wnd.marginsTopLeft.y
            subRect := &bc.theme.imgMetrics.wndBigCaption
            colorBk := surface.app.isActivated ? theme.palette.caption : theme.palette.captionNotActived
            theme.drawSubRect(bc.painter, posCaption, subRect, colorBk)
        }

        if surface.flags & .SmallCaption
        {
            posCaption.height = surface.wnd.marginsTopLeft.y
            subRect := &theme.imgMetrics.wndSmallCaption
            colorBk := surface.app.isActivated ? theme.palette.caption : theme.palette.captionNotActived
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
            colorTxt := surface.app.isActivated ? theme.palette.captionText : theme.palette.captionTextNotActivated
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

        if surface.flags & .MinimizeBtn
        {
            minimizeBtn = ButtonIcon.create(self, Icon.from(&surface.app.theme.icons24, 2), @{0,0,0,0})
            minimizeBtn.sigPressed = &onMinimize
        }

        if surface.flags & .MaximizeBtn
        {
            maximizeBtn = ButtonIcon.create(self, Icon.from(&surface.app.theme.icons24, 3), @{0,0,0,0})
            maximizeBtn.sigPressed = &onMaximize
        }

        if surface.flags & .CloseBtn
        {
            closeBtn = ButtonIcon.create(self, Icon.from(&surface.app.theme.icons24, 4), @{0,0,0,0})
            closeBtn.sigPressed = &onClose
        }
    }
}