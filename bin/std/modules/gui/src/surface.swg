#global public

struct Surface
{
    using native:   NativeSurface
    app:            *Application
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
        app.renderer.begin(rc)
        painter.begin()
    }

    mtd endRender()
    {
        painter.end()
        app.renderer.draw(&painter)
        app.renderer.end()
    }
}