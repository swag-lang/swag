#global public
using Core, Pixel

struct Surface
{
    using native:   NativeSurface
    app:            *Application
    wnd:            SurfaceWnd
    painter:        Painter

    rc:             RenderingContext
    position:       Math.Rectangle

    flags:          SurfaceFlags = Zero

    isDirty:        bool
    isHidden:       bool = true
    isDisabled:     bool
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
    Sizeable

    Secondary

    OverlappedWindow = BigCaption | MinimizeBtn | MaximizeBtn | CloseBtn | Border | Sizeable
}

impl Surface
{
    // Creates the surface
    func create(surface: *Surface, view: *Wnd, x, y, width, height: s32, flags: SurfaceFlags, mainSurface: *Surface) throw
    {
        surface.flags = flags
        surface.wnd.itf = cast(IWnd) surface.wnd
        surface.wnd.surface = surface
        surface.wnd.focusStrategy = .None
        surface.wnd.create(view)
        app := surface.app

        if flags & .BigCaption
            surface.wnd.marginsTopLeft.y = app.theme.metrics.wnd_BigCaptionCY
        else if flags & .SmallCaption
            surface.wnd.marginsTopLeft.y = app.theme.metrics.wnd_SmallCaptionCY

        if flags & .Border
        {
            size := app.theme.metrics.wnd_BorderSize
            surface.wnd.marginsTopLeft.x = size
            surface.wnd.marginsTopLeft.y = Math.max(surface.wnd.marginsTopLeft.y, size)
            surface.wnd.marginsBottomRight.x = size
            surface.wnd.marginsBottomRight.y = size
        }

        try createNative(app, surface, x, y, width, height, flags, mainSurface)

        surface.wnd.minSize = @{128, 128}
        surface.sendResizeEvent(cast(f32) width, cast(f32) height)
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

    // Center this surface relative to 'from'
    mtd center(from: *Surface)
    {
        pos := position
        pos.x = from.position.horzCenter()
        pos.y = from.position.vertCenter()
        pos.x -= position.width * 0.5
        pos.y -= position.height * 0.5
        setPosition(pos)
    }

    // Main surface paint function
    mtd paint()
    {
        if !isDirty
            return
        isDirty = false

        var cxt: PaintContext
        cxt.painter = &painter
        cxt.renderer = &app.renderer
        cxt.renderer.begin(rc)
        cxt.painter.begin()
        cxt.clipRect = @{0, 0, rc.width, rc.height}

        subRect := &app.theme.rects.surfaceWnd_Shadow
        painter.colorMask(true, true, true, true)
        app.theme.drawSubRect(&painter, @{0, 0, position.width, position.height}, subRect, Color.fromArgb(140,0,0,0))

        wnd.paint(&cxt)

        //painter.colorMask(false, false, false, true)
        //painter.clear(Color.fromArgb(155, 0, 0, 0))

        painter.end()

        cxt.renderer.draw(&painter)
        cxt.renderer.end()
    }

    private mtd sendResizeEvent(width, height: f32)
    {
        rc.width = cast(s32) width
        rc.height = cast(s32) height
        position.width = width
        position.height = height

        sdw := app.theme.metrics.surfaceWnd_ShadowSize
        wnd.setPosition(@{sdw, sdw, width - 2 * sdw, height - 2 * sdw})
    }
}