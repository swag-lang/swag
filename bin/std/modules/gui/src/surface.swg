#global public
using Core

struct Surface
{
    using native:   NativeSurface
    app:            *Application
    wnd:            SurfaceWnd
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

        // Resize embedeed main wnd
        evt := ResizeEvent{kind: EventKind.Resize}
        evt.target = &wnd
        evt.rect.width = rc.width
        evt.rect.height = rc.height
        app.sendEvent(&evt)
    }

    mtd sendCloseEvent()
    {
        evt := Event{kind: EventKind.Close}
        evt.target = &wnd
        app.sendEvent(&evt)
    }
}