#global if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui, Math

const W = 1024
const H = 1024

namespace Res
{
    var image0:         Texture
    var image1:         Texture
}

#main
{
    var app: Gui.Application

    view := Wnd.create'MyView()
    surface := try app.createSurface(100, 100, W, H, view: view)

    assume
    {
        rdr := surface.app.getRenderer()
        dataPath := Path.getDirectoryName(#location.fileName)
        img0 := Image.load(Path.combine(dataPath, "img1.jpg"))
        img1 := Image.load(Path.combine(dataPath, "img2.jpg"))
        Res.image0 = rdr.addImage(img0)
        Res.image1 = rdr.addImage(img1)
    }

    surface.show()
    app.run()
}