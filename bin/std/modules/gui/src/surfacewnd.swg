using Core, Pixel

public struct SurfaceWnd
{
    using base:     FrameWnd
    icon:           Icon
    minimizeBtn:    *Button
    maximizeBtn:    *Button
    closeBtn:       *Button
}

impl IWnd for SurfaceWnd
{
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
    mtd create()
    {
        createView'Wnd()

        if surface.flags & .MinimizeBtn
        {
            minimizeBtn = Button.create(self, null, @{0,0,0,0})
            minimizeBtn.icon.set(&surface.app.theme.icons24, 2, 16)
        }

        if surface.flags & .MaximizeBtn
        {
            maximizeBtn = Button.create(self, null, @{0,0,0,0})
            maximizeBtn.icon.set(&surface.app.theme.icons24, 3, 16)
        }

        if surface.flags & .CloseBtn
        {
            closeBtn = Button.create(self, null, @{0,0,0,0})
            closeBtn.icon.set(&surface.app.theme.icons24, 4, 16)
        }
    }
}