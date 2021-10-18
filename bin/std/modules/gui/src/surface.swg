#global public

struct Surface
{
    using native:   NativeSurface
    wnd:            Wnd
    painter:        Pixel.Painter
    rc:             Pixel.RenderingContext
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