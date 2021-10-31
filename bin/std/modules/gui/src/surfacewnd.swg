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
            posCaption.x += bc.theme.wndCaptionMarginTitle

            if icon.isValid()
            {
                icon.paint(bc.painter, posCaption.x, posCaption.vertCenter() - icon.size * 0.5)
                posCaption.x += icon.size + bc.theme.iconTextMargin
            }

            var sf: Pixel.StringFormat
            sf.vertAlignment = .Center
            bc.painter.drawString(posCaption, name, bc.theme.fontDefault.regular, bc.theme.palette.captionText, sf)
        }
    }
}