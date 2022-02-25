#global if #os == Swag.TargetOs.Windows
using Gui, Pixel

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

        painter.drawTexture(10, 10, Res.image0)
        painter.drawTexture(10, 300, Res.image1)
    }
}
