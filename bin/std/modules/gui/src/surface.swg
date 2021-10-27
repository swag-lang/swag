#global public
using Core

struct Surface
{
    using native:   NativeSurface
    app:            *Application
    wnd:            FrameWnd
    painter:        Pixel.Painter
    rc:             Pixel.RenderingContext
    position:       Math.Rectangle
    dirty:          bool
}

#[Swag.EnumFlags]
enum SurfaceFlags
{
    Zero = 0
    SecondaryWindow
}

impl Surface
{
    mtd invalidate()
    {
        invalidateRect(@{0, 0, position.width, position.height})
    }

    mtd invalidateRect(rect: Math.Rectangle)
    {
        dirty = true
    }

    mtd paint()
    {
        if !dirty
            return
        dirty = false

        var cxt: PaintContext
        cxt.theme = &app.theme
        cxt.painter = &painter
        cxt.renderer = &app.renderer
        cxt.renderer.begin(rc)
        cxt.painter.begin()
        cxt.clipRect = @{0, 0, rc.width, rc.height}

        wnd.paint(&cxt)

        cxt.painter.end()
        cxt.renderer.draw(&painter)
        cxt.renderer.end()
    }

    mtd sendResizeEvent(width, height: s32)
    {
        rc.width = width
        rc.height = height
        position.width = width
        position.height = width
        wnd.resize(width, height)
    }

    mtd sendCloseEvent()
    {
        evt := CloseEvent{}
        wnd.sendEvent(&evt)
    }
}