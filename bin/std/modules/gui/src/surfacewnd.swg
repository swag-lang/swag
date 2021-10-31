using Core, Pixel

public struct SurfaceWnd
{
    using base: FrameWnd
    icon:       Icon
}

impl IWnd for SurfaceWnd
{
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