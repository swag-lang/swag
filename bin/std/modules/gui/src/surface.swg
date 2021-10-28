#global public
using Core

struct Surface
{
    using native:   NativeSurface
    app:            *Application
    wnd:            SurfaceWnd
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
    // Creates the surface
    func create(app: *Application, result: *Surface, x, y, width, height: s32, flags: SurfaceFlags, mainSurface: *Surface) throw
    {
        try NativeSurface.createNative(app, result, x, y, width, height, flags, mainSurface)
        result.wnd.marginsTopLeft.y = 50
        result.wnd.sendResizeEvent()
    }

    // Invalidate the full surface to be painted
    mtd invalidate()
    {
        invalidateRect(@{0, 0, position.width, position.height})
    }

    // Invalidate one part of the surface to be painted
    mtd invalidateRect(rect: Math.Rectangle)
    {
        dirty = true
    }

    // Main surface paint function
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

    protected mtd sendResizeEvent(width, height: s32)
    {
        rc.width = width
        rc.height = height
        position.width = width
        position.height = width
        wnd.resize(width, height)
    }

    protected mtd sendCloseEvent()
    {
        evt := CloseEvent{}
        wnd.sendEvent(&evt)
    }
}