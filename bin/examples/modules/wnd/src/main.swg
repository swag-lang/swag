#global if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32, Pixel, Gui, Math

const W = 1024
const H = 1024

var g_Painter:      Painter
var g_Renderer:     RenderOglWin32
var g_RC:           RenderingContext

func display()
{
    g_Renderer.begin(g_RC)

    g_Painter.paintParams  = .Antialiased
    g_Painter.paintQuality = .Normal

    g_Painter.begin()
    g_Painter.clear(Color.fromArgb(0xFFEEEEEE))
    g_Painter.end()
    g_Renderer.draw(&g_Painter)

    g_Renderer.end()
}

func messageLoop()
{
    var keyb: Input.Keyboard

    loop
    {
        if !Surface.messageLoop()
            return

        Memory.freeTemp()
        display()
        keyb.update()
    }
}

func winProc(wnd: Surface, msg: SurfaceMessage)
{
    switch msg.kind
    {
    case Paint:
        display()
    case Resize:
        g_RC.width = cast(s32) msg.param0
        g_RC.height = cast(s32) msg.param1
    }
}

#main
{
    try
    {
        surface := Surface.create("Example", W, H, SurfaceFlags.OverlappedWindow, &winProc)
        g_RC = g_Renderer.createContext(surface.hWnd)
        surface.show()
        messageLoop()
        g_Renderer.drop()
    }
}