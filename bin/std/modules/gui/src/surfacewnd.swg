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

        var srcRect: Math.Rectangle = ?

        if surface.flags & .Border
        {
            srcRect = @{130,1,27,27}
            Style.drawTexture(bc.painter, bc.theme.imgWidgets, position, srcRect, 3, bc.theme.palette.caption)
        }

        if surface.flags & (.BigCaption | .SmallCaption)
        {
            pos.height = surface.wnd.marginsTopLeft.y
            srcRect = bc.theme.imgMetrics.bigCaption.rect
            Console.print(srcRect)
            Style.drawTexture(bc.painter, bc.theme.imgWidgets, pos, srcRect, 9, bc.theme.palette.caption)
        }
    }
}