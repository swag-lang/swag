using Core, Pixel

public struct SurfaceWnd
{
    using base: FrameWnd
}

impl IWnd for SurfaceWnd
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc := evt.bc
        posCaption := position

        if surface.flags & .Border
        {
            subRect := &bc.theme.imgMetrics.wndBorder
            bc.theme.drawSubRect(bc.painter, position, subRect, bc.theme.palette.caption)
        }

        if surface.flags & .BigCaption
        {
            posCaption.height = surface.wnd.marginsTopLeft.y
            subRect := &bc.theme.imgMetrics.wndBigCaption
            bc.theme.drawSubRect(bc.painter, posCaption, subRect, bc.theme.palette.caption)
        }

        if surface.flags & .SmallCaption
        {
            posCaption.height = surface.wnd.marginsTopLeft.y
            subRect := &bc.theme.imgMetrics.wndSmallCaption
            bc.theme.drawSubRect(bc.painter, posCaption, subRect, bc.theme.palette.caption)
        }

        if surface.flags & (.BigCaption | .SmallCaption)
        {
            var sf: Pixel.StringFormat
            sf.vertAlignment = .Center
            posCaption.x += bc.theme.wndCaptionMarginTitle
            bc.painter.drawString(posCaption, "toto", bc.theme.fontDefault.regular, bc.theme.palette.captionText, sf)
        }
    }
}