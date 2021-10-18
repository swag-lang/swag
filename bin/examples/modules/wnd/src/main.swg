#global if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32, Pixel, Gui, Math

const W = 1024
const H = 1024

var g_Surface: *Surface

func display(surface: *Surface)
{
    surface.painter.paintParams  = .Antialiased
    surface.painter.paintQuality = .Normal

    surface.beginRender()
    surface.painter.clear(Color.fromArgb(0xFFEEEEEE))
    surface.endRender()
}

func onEvent(wnd: *Wnd, event: *Event)
{
    switch event.kind
    {
    case Close:
        wnd.surface.app.postQuitEvent()
    case Paint:
        display(wnd.surface)
    default:
        wnd.IWnd.onEvent(event)
    }
}

#main
{
    var app: Gui.Application
    surface := try app.createSurface("Example", 100, 100, W, H, SurfaceFlags.OverlappedWindow, &onEvent)
    surface.show()
    app.run()
}