#global public

struct Surface
{
    using native:   NativeSurface
    wnd:            SurfaceWnd
    proc:           func(*Surface, SurfaceMessage)
    painter:        Pixel.Painter
    rc:             Pixel.RenderingContext
}

enum SurfaceMessageKind
{
    None
    Resize
    Paint
}

struct SurfaceMessage
{
    kind:   SurfaceMessageKind = .None
    param0: u64
    param1: u64
}

#[Swag.EnumFlags]
enum SurfaceFlags
{
    Caption
    SysMenu
    MinimizeBox
    MaximizeBox

    DlgFrame
    SizeFrame

    OverlappedWindow
    PopupWindow
    PaletteWindow
}

impl Surface
{
    mtd beginRender()
    {
        g_Renderer.begin(rc)
        painter.begin()
    }

    mtd endRender()
    {
        painter.end()
        g_Renderer.draw(&painter)
        g_Renderer.end()
    }
}