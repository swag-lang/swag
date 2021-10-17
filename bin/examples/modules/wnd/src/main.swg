#global if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32, Pixel, Gui, Math

const W = 1024
const H = 1024

var g_Surface: Surface

func display(wnd: *Surface)
{
    wnd.painter.paintParams  = .Antialiased
    wnd.painter.paintQuality = .Normal

    wnd.beginRender()
    wnd.painter.clear(Color.fromArgb(0xFFEEEEEE))
    wnd.endRender()
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

func winProc(wnd: *Surface, msg: SurfaceMessage)
{
    switch msg.kind
    {
    case Paint, Resize:
        display(wnd)
    }
}

#main
{
    try
    {
        g_Surface = Surface.create("Example", W, H, SurfaceFlags.OverlappedWindow, &winProc)
        g_Surface.show()
        messageLoop()
    }
}