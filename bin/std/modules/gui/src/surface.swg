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
    minSize:        Math.Point
    maxSize:        Math.Point

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
            surface.wnd.padding.y = app.theme.metrics.wnd_BigCaptionCY
        elif flags & .SmallCaption
            surface.wnd.padding.y = app.theme.metrics.wnd_SmallCaptionCY

        if flags & .Border
        {
            size := app.theme.metrics.wnd_BorderSize
            surface.wnd.padding.x = size
            surface.wnd.padding.y = Math.max(surface.wnd.padding.y, size)
            surface.wnd.padding.z = size
            surface.wnd.padding.w = size
        }

        try createNative(app, surface, x, y, width, height, flags, mainSurface)

        surface.minSize = @{256, 256}
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

        // Draw shadow around surface
        subRect := &app.theme.rects.surfaceWnd_Shadow
        painter.colorMask(true, true, true, true)
        painter.clear(Color.fromArgb(0, 0, 0, 0))
        app.theme.drawSubRect(&painter, @{0, 0, position.width, position.height}, subRect, app.theme.colors.wnd_Shadow, false)

        wnd.paint(&cxt)
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