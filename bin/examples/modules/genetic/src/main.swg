using Core, Pixel, Gui, Core.Math

const W = 1024
const H = 512

var g_Image0:   Image
var g_Image1:   Image
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
        if event.key == .Escape do
            event.target.postQuitEvent()
    }

    mtd impl onPaintEvent(event: *PaintEvent)
    {
        let painter = event.bc.painter
        painter.clear(Argb.White)

        painter.drawTexture({20, 20, 256, 256}, g_Texture0)

        let best = genGo()
        if best do
            .surface.app.getRenderer().updateTexture(g_Texture2, best)
        painter.drawTexture({300, 20, 256, 256}, g_Texture2)

        painter.drawTexture({580, 20, 256, 256}, g_Texture1)

        painter.drawString(20, 300, Format.toString("gen %", g_NumGen), g_Font, Argb.Black)
        painter.drawString(20, 340, Format.toString("score %", g_BestScore), g_Font, Argb.Black)

        .invalidate()
    }
}

#main
{
    var app: Gui.Application
    if Env.hasArg("swag.test") do
        app.maxRunFrame = @bytecode ? 2 : 10

    assume
    {
        let view    = Wnd.create'MyView()
        let surface = app.createSurface(100, 100, W, H, view: view)

        let rdr      = surface.app.getRenderer()
        let dataPath = Path.getDirectoryName(#curlocation.fileName)
        g_Image0 = Image.load(Path.combine(dataPath, "/datas/img1.jpg"))
        g_Image1 = Image.load(Path.combine(dataPath, "/datas/img2.jpg"))
        g_Font   = view.getTheme().createDefaultFont(30)

        g_Image0.grayScale()
        g_Image1.grayScale()

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
