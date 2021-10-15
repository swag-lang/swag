#global if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32, Pixel, Gui, Math

const W = 1024
const H = 1024

var g_Painter:      Painter
var g_Renderer:     RenderOglWin32
var g_Wnd:          Surface

func display()
{
    g_Renderer.begin()

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
        g_Renderer.resize(cast(s32) msg.param0, cast(s32) msg.param1)
    }
}

#main
{
    try
    {
        g_Wnd = Surface.create("Example", W, H, SurfaceFlags.OverlappedWindow, &winProc)
        g_Renderer.init(g_Wnd.hWnd, W, H)
        g_Wnd.show()
        messageLoop()
        g_Renderer.drop()
    }
}