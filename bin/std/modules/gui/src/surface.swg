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
    flags:          SurfaceFlags = Zero
    isDirty:        bool
}

#[Swag.EnumFlags]
enum SurfaceFlags
{
    Zero = 0
    BigCaption
    SmallCaption
    MinimizeBtn
    MaximizeBtn
    CloseBtn
    Border
    SecondaryWindow
}

impl Surface
{
    // Creates the surface
    func create(app: *Application, surface: *Surface, x, y, width, height: s32, flags: SurfaceFlags, mainSurface: *Surface) throw
    {
        surface.flags = flags
        surface.wnd.position = @{0, 0, width, height}
        surface.wnd.itf = cast(IWnd) surface.wnd
        surface.wnd.surface = surface
        surface.wnd.create()

        if flags & .BigCaption
            surface.wnd.marginsTopLeft.y = app.theme.wndBigCaptionCY
        else if flags & .SmallCaption
            surface.wnd.marginsTopLeft.y = app.theme.wndSmallCaptionCY

        if flags & .Border
        {
            size := app.theme.wndBorderSize
            surface.wnd.marginsTopLeft.x = size
            surface.wnd.marginsTopLeft.y = Math.max(surface.wnd.marginsTopLeft.y, size)
            surface.wnd.marginsBottomRight.x = size
            surface.wnd.marginsBottomRight.y = size
        }

        try createNative(app, surface, x, y, width, height, flags, mainSurface)
        surface.wnd.sendResizeEvent()
    }

    // Destroy the surface
    mtd destroy()
    {
        wnd.destroy()
    }

    // Set the surface icon
    mtd setIcon(icon: Icon)
    {
        wnd.icon = icon
        invalidate()
    }

    // Set the surface title
    mtd setTitle(name: string)
    {
        wnd.name = name
        invalidate()
    }

    // Invalidate the full surface to be painted
    mtd invalidate()
    {
        invalidateRect(@{0, 0, position.width, position.height})
    }

    // Invalidate one part of the surface to be painted
    mtd invalidateRect(rect: Math.Rectangle)
    {
        isDirty = true
    }

    // Main surface paint function
    mtd paint()
    {
        if !isDirty
            return
        isDirty = false

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

    protected mtd sendResizeEvent(width, height: f32)
    {
        rc.width = cast(s32) width
        rc.height = cast(s32) height
        position.width = width
        position.height = width
        wnd.resize(width, height)
    }
}