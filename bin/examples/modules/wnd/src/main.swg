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
    case Paint:
        display(wnd.surface)
    default:
        wnd.IWnd.onEvent(event)
    }
}

func messageLoop()
{
    var keyb: Input.Keyboard

    loop
    {
        if !Surface.messageLoop()
            return

        Memory.freeTemp()
        keyb.update()
        g_Surface.invalidate()
    }
}

#main
{
    var app: Gui.Application
    g_Surface = try app.createSurface("Example", 100, 100, W, H, SurfaceFlags.OverlappedWindow, &onEvent)
    g_Surface.show()
    messageLoop()
}