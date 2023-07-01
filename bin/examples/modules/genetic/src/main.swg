using Core, Pixel, Gui, Math

const W = 1024
const H = 1024

var g_Image0: Image
var g_Image1: Image
var g_Texture0: Texture
var g_Texture1: Texture
var g_Texture2: Texture

struct MyView
{
    using wnd: Wnd
}

impl IWnd for MyView
{
    mtd impl onKeyEvent(event: *KeyEvent)
    {
        if event.key == .Escape
            event.target.postQuitEvent()
    }

    mtd impl onPaintEvent(event: *PaintEvent)
    {
        painter := event.bc.painter
        painter.clear(Argb.White)

        painter.drawTexture({20, 20, 256, 256}, g_Texture0)
        painter.drawTexture({296, 296, 256, 256}, g_Texture1)

        best := genGo()
        if best surface.app.getRenderer().updateTexture(g_Texture2, best)
        painter.drawTexture({20, 296, 256, 256}, g_Texture2)

        invalidate()
    }
}

#main
{
    var app: Gui.Application

    assume
    {
        view := Wnd.create'MyView()
        surface := app.createSurface(100, 100, W, H, view: view)

        rdr := surface.app.getRenderer()
        dataPath := Path.getDirectoryName(#location.fileName)
        g_Image0 = Image.load(Path.combine(dataPath, "/datas/img1.jpg"))
        g_Image1 = Image.load(Path.combine(dataPath, "/datas/img2.jpg"))

        const SIZE = 64
        g_Image0.resize(SIZE, SIZE, .Bilinear)
        g_Image1.resize(SIZE, SIZE, .Bilinear)

        g_Texture0 = rdr.addImage(g_Image0)
        g_Texture1 = rdr.addImage(g_Image1)
        g_Texture2 = rdr.addImage(g_Image0)
    }

    genInit()
    surface.show()

    app.run()
}