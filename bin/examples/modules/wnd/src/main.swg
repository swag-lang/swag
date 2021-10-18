#global if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32, Pixel, Gui, Math

const W = 1024
const H = 1024

var g_Surface: *Surface

func display0(surface: *Surface)
{
    surface.painter.paintParams  = .Antialiased
    surface.painter.paintQuality = .Normal

    surface.beginRender()
    surface.painter.clear(Color.fromArgb(0xFFEEEEEE))
    surface.painter.fillRoundRect(100, 100, 200, 200, 15, 15, Argb.Red)
    surface.endRender()
}

func display1(surface: *Surface)
{
    surface.painter.paintParams  = .Antialiased
    surface.painter.paintQuality = .Normal

    surface.beginRender()
    surface.painter.clear(Color.fromArgb(0xFFEEEEEE))
    surface.painter.fillRoundRect(100, 100, 200, 200, 15, 15, Argb.Blue)
    surface.endRender()
}

func onEvent0(wnd: *Wnd, event: *Event)
{
    switch event.kind
    {
    case Close:
        wnd.surface.app.postQuitEvent()
    case Paint:
        display0(wnd.surface)
    default:
        wnd.IWnd.onEvent(event)
    }
}

func onEvent1(wnd: *Wnd, event: *Event)
{
    switch event.kind
    {
    case Close:
        wnd.surface.app.postQuitEvent()
    case Paint:
        display1(wnd.surface)
    default:
        wnd.IWnd.onEvent(event)
    }
}

#main
{
    var app: Gui.Application
    surface0 := try app.createSurface("Example0", 100, 100, W, H, SurfaceFlags.OverlappedWindow, &onEvent0)
    surface1 := try app.createSurface("Example1", 100+W, 100, W, H, SurfaceFlags.OverlappedWindow | SurfaceFlags.ToolWindow, &onEvent1)
    surface0.show()
    surface1.show()

    app.run()
}