using Core, Gui, Pixel

struct MyView
{
    using wnd: Wnd
}

impl IWnd for MyView
{
    mtd onPaintEvent(event: *PaintEvent)
    {
        painter := event.bc.painter
        painter.clear(Argb.White)

        painter.drawTexture(@{20, 20, 256, 256}, g_Texture0)
        painter.drawTexture(@{296, 296, 256, 256}, g_Texture1)

        best := genGo()
        surface.app.getRenderer().updateTexture(g_Texture2, best.genes.buffer)
        painter.drawTexture(@{20, 296, 256, 256}, g_Texture2)

        invalidate()
    }
}
