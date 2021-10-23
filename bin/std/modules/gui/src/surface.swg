#global public
using Core

struct Surface
{
    using native:   NativeSurface
    app:            *Application
    wnd:            Wnd
    painter:        Pixel.Painter
    rc:             Pixel.RenderingContext
    pos:            Math.Rectangle
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

impl Surface
{
    mtd sendPaintEvent()
    {
        var cxt: PaintContext
        cxt.painter = &painter
        cxt.renderer = &app.renderer
        cxt.renderer.begin(rc)
        cxt.painter.begin()

        wnd.paint(&cxt)

        cxt.painter.end()
        cxt.renderer.draw(&painter)
        cxt.renderer.end()
    }

    mtd sendResizeEvent(width, height: s32)
    {
        rc.width = width
        rc.height = height
        pos.width = width
        pos.height = width
        wnd.resize(width, height)
    }

    mtd sendCloseEvent()
    {
        evt := CloseEvent{}
        wnd.sendEvent(&evt)
    }
}