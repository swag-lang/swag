#global if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui

const W = 1024
const H = 1024

func onPaintEvent(wnd: *Wnd, event: *PaintEvent)
{
    event.painter.paintParams  = .Antialiased
    event.painter.paintQuality = .Normal
    event.painter.clear(Color.fromArgb(0xFFEEEEEE))
    event.painter.fillRoundRect(100, 100, 200, 200, 15, 15, Argb.Red)
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

    widget := Static.create(&surface0.wnd)
    widget.position = @{0, 0, 100, 100}

    surface0.show()
    surface1.show()

    app.run()
}