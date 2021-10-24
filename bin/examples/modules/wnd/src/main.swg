#global if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui

const W = 1024
const H = 1024

func onPaintEvent(wnd: *Wnd, evt: *PaintEvent)
{
    evt.painter.paintParams  = .Antialiased
    evt.painter.paintQuality = .Normal
    evt.painter.drawTexture(wnd.position, wnd.surface.app.theme.widgetsImg)
}

func onCloseEvent(wnd: *Wnd, event: *CloseEvent)
{
    wnd.postQuitEvent()
}

#main
{
    var app: Gui.Application
    surface0 := try app.createSurface("Example0", 100, 100, W, H)
    surface0.wnd.hookOnCloseEvent = &onCloseEvent

    surface1 := try app.createSurface("Example1", 100+W, 100, W, H, SurfaceFlags.OverlappedWindow | SurfaceFlags.ToolWindow)
    surface1.wnd.hookOnPaintEvent = &onPaintEvent
    surface1.wnd.hookOnCloseEvent = &onCloseEvent

    widget0 := Static.create(&surface0.wnd)
    widget0.position = @{0, 0, 100, 100}

    widget1 := Static.create(&widget0.base.wnd)
    widget1.position = @{10, 10, 80, 80}

    surface0.show()
    surface1.show()

    app.run()
}