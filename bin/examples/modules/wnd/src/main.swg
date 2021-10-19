#global if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32, Pixel, Gui, Math

const W = 1024
const H = 1024

var g_Surface: *Surface

func display0(event: *PaintEvent)
{
    event.painter.paintParams  = .Antialiased
    event.painter.paintQuality = .Normal

    event.painter.begin()
    event.painter.clear(Color.fromArgb(0xFFEEEEEE))
    event.painter.fillRoundRect(100, 100, 200, 200, 15, 15, Argb.Red)
    event.painter.end()

    event.renderer.draw(event.painter)
}

func onEvent0(wnd: *Wnd, event: *Event)
{
    switch event.kind
    {
    case Close:
        wnd.surface.app.postQuitEvent()
    case Paint:
        display0(cast(*PaintEvent) event)
    default:
        wnd.IWnd.onEvent(event)
    }
}

#main
{
    var app: Gui.Application
    surface0 := try app.createSurface("Example0", 100, 100, W, H, SurfaceFlags.OverlappedWindow, &onEvent0)
    surface1 := try app.createSurface("Example1", 100+W, 100, W, H, SurfaceFlags.OverlappedWindow | SurfaceFlags.ToolWindow)
    surface0.show()
    surface1.show()

    app.run()
}