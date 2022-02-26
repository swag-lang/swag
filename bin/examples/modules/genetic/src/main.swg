#global if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui, Math

const W = 1024
const H = 1024

var g_Image0: Image
var g_Image1: Image
var g_Texture0: Texture
var g_Texture1: Texture
var g_Texture2: Texture

#main
{
    var app: Gui.Application

    view := Wnd.create'MyView()
    surface := try app.createSurface(100, 100, W, H, view: view)

    assume
    {
        rdr := surface.app.getRenderer()
        dataPath := Path.getDirectoryName(#location.fileName)
        g_Image0 = Image.load(Path.combine(dataPath, "img1.jpg"))
        g_Image1 = Image.load(Path.combine(dataPath, "img2.jpg"))

        const SIZE = 96
        g_Image0.resize(SIZE, SIZE)
        g_Image1.resize(SIZE, SIZE)

        g_Texture0 = rdr.addImage(g_Image0)
        g_Texture1 = rdr.addImage(g_Image1)
        g_Texture2 = rdr.addImage(g_Image0)
    }

    genInit()
    surface.show()

    app.run()
}