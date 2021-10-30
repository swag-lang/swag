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
            Style.drawTexture(bc.painter, bc.theme.imgWidgets, position, subRect, bc.theme.palette.caption)
        }

        if surface.flags & (.BigCaption | .SmallCaption)
        {
            pos.height = surface.wnd.marginsTopLeft.y
            subRect := &bc.theme.imgMetrics.bigCaption
            Style.drawTexture(bc.painter, bc.theme.imgWidgets, pos, subRect, bc.theme.palette.caption)
        }
    }
}