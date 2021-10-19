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
    ToolWindow

    DlgFrame
    SizeFrame

    OverlappedWindow
    PopupWindow
    PaletteWindow
}
