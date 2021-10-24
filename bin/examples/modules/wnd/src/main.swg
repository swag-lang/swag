#global if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui

const W = 1024
const H = 1024

func onPaintEvent(wnd: *Wnd, evt: *PaintEvent)
{
    evt.bc.painter.paintParams  = .Antialiased
    evt.bc.painter.paintQuality = .Normal
    evt.bc.painter.drawTexture(wnd.position, wnd.surface.app.theme.imgWidgets)
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

    widget0 := PushButton.create(&surface0.wnd)
    widget0.name = "Ok"
    widget0.position = @{5, 5, 100, 30}

    widget1 := PushButton.create(&surface0.wnd)
    widget1.name = "Cancel"
    widget1.position = @{5, 40, 100, 30}

    surface0.show()
    surface1.show()

    app.run()
}