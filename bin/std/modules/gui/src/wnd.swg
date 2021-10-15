using Core, Pixel

interface IWnd
{
    paint: func(self, PaintContext)
}

struct Wnd
{
    rect: Math.Rectangle
}

impl IWnd for Wnd
{
    mtd paint(context: PaintContext)
    {
        context.painter.fillRect(rect.x, rect.y, rect.width, rect.height, Argb.Red)
    }
}