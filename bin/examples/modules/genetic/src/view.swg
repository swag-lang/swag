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

        painter.drawTexture(10, 10, g_Texture0)
        painter.drawTexture(10, 300, g_Texture1)

        best := genGo()
        img := Image.create(g_Image0.width, g_Image0.height, PixelFormat.RGB8)
        visit idx, cpt: best.genes
        {
            idxPixel := idx * 3
            cptPixel := cpt * 3
            img.pixels[cptPixel] = g_Image0.pixels[idxPixel]
            img.pixels[cptPixel+1] = g_Image0.pixels[idxPixel+1]
            img.pixels[cptPixel+2] = g_Image0.pixels[idxPixel+2]
        }

        surface.app.getRenderer().updateTexture(g_Texture2, img.pixels)
        painter.drawTexture(300, 10, g_Texture2)

        invalidate()
    }
}
