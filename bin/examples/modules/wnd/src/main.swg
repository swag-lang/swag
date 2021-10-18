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

func onEvent(wnd: *Wnd, event: *Event)
{
    switch event.kind
    {
    case Paint, Resize:
        display(wnd.surface)
    }
}

#main
{
    try
    {
        g_Surface = Surface.create("Example", W, H, SurfaceFlags.OverlappedWindow)
        g_Surface.wnd.hookOnEvent = &onEvent
        g_Surface.show()
        messageLoop()
    }
}