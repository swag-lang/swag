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
            Theme.drawTexture(bc.painter, bc.theme.imgWidgets, position, subRect, bc.theme.palette.caption)
        }

        if surface.flags & .BigCaption
        {
            pos.height = surface.wnd.marginsTopLeft.y
            subRect := &bc.theme.imgMetrics.bigCaption
            Theme.drawTexture(bc.painter, bc.theme.imgWidgets, pos, subRect, bc.theme.palette.caption)
        }

        if surface.flags & .SmallCaption
        {
            pos.height = surface.wnd.marginsTopLeft.y
            subRect := &bc.theme.imgMetrics.smallCaption
            Theme.drawTexture(bc.painter, bc.theme.imgWidgets, pos, subRect, bc.theme.palette.caption)
        }
    }
}