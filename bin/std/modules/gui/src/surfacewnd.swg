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
        pos := position

        if surface.flags & .Border
        {
            subRect := &bc.theme.imgMetrics.wndBorder
            bc.theme.drawSubRect(bc.painter, position, subRect, bc.theme.palette.caption)
        }

        if surface.flags & .BigCaption
        {
            pos.height = surface.wnd.marginsTopLeft.y
            subRect := &bc.theme.imgMetrics.bigCaption
            bc.theme.drawSubRect(bc.painter, pos, subRect, bc.theme.palette.caption)
        }

        if surface.flags & .SmallCaption
        {
            pos.height = surface.wnd.marginsTopLeft.y
            subRect := &bc.theme.imgMetrics.smallCaption
            bc.theme.drawSubRect(bc.painter, pos, subRect, bc.theme.palette.caption)
        }
    }
}