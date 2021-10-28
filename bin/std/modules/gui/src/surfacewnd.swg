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
        pos.height = 30
        var srcRect: Math.Rectangle = ?
        srcRect = @{57,1,72,27}
        Style.drawTexture(bc.painter, bc.theme.imgWidgets, pos, srcRect, 9, Argb.White)
        //evt.bc.painter.drawTexture(pos, srcRect, bc.theme.imgWidgets)
        //evt.bc.painter.fillRect(position, Argb.Red)
    }
}