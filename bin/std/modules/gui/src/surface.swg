#global public
using Core, Pixel

struct Surface
{
    using native:   NativeSurface
    app:            *Application
    wnd:            *SurfaceWnd
    painter:        Painter
    rt:             RenderTarget

    rc:             RenderingContext
    position:       Math.Rectangle
    minSize:        Math.Point
    maxSize:        Math.Point

    flags:          SurfaceFlags = Zero
    dirtyRect:      Math.Rectangle = @{Swag.F32.Max, Swag.F32.Max, 0, 0}

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
        surface.wnd = Memory.new'SurfaceWnd()
        surface.wnd.itf = cast(IWnd) surface.wnd
        surface.wnd.surface = surface
        surface.wnd.focusStrategy = .None
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
        w := cast(f32) Math.max(surface.minSize.x, cast(f32) width)
        h := cast(f32) Math.max(surface.minSize.y, cast(f32) height)
        surface.sendResizeEvent(w, h)
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
        if !isDirty
        {
            dirtyRect = rect
            isDirty = true
            return
        }

        r := dirtyRect.right()
        b := dirtyRect.bottom()
        dirtyRect.x = Math.min(dirtyRect.x, rect.x)
        dirtyRect.y = Math.min(dirtyRect.y, rect.y)
        dirtyRect.setRight(Math.max(r, rect.right()))
        dirtyRect.setBottom(Math.max(b, rect.bottom()))
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

    private mtd createRT()
    {
        if !rt.colorBuffer.valid or rt.colorBuffer.width != rc.width or rt.colorBuffer.height != rc.height
        {
            app.renderer.deleteRenderTarget(&rt)
            rt = app.renderer.createRenderTarget(rc.width, rc.height)
            invalidate()
        }
    }

    // Main surface paint function
    mtd paint()
    {
        createRT()

        if !isDirty
            return
        clipRect := dirtyRect
        dirtyRect = @{Swag.F32.Max, Swag.F32.Max, 0, 0}
        isDirty   = false

        var cxt: PaintContext
        cxt.painter = &painter
        cxt.renderer = &app.renderer
        cxt.painter.begin()

        // Draw shadow around surface
        subRect := &app.theme.rects.surfaceWnd_Shadow
        painter.pushClippingRect(clipRect)
        painter.colorMask(true, true, true, true)
        app.theme.drawSubRect(&painter, @{0, 0, position.width, position.height}, subRect, app.theme.colors.wnd_Shadow, false)
        painter.colorMask(true, true, true, false)

        // Hierarchy paint
        painter.pushClippingRect(wnd.getSurfaceRect())
        wnd.paint(&cxt)
        painter.colorMask(false, false, false, true)
        painter.clear(Color.fromArgb(255, 0, 0, 0))
        painter.end()

        cxt.renderer.begin(rc)

        // Paint to surface render target
        cxt.renderer.bindRenderTarget(rt)
        cxt.renderer.setScissorRect(clipRect)
        cxt.renderer.clear()
        cxt.renderer.draw(&painter)
        cxt.renderer.unbindRenderTarget()

        // Paint render target to screen
        painter.begin()
        painter.colorMask(true, true, true, true)
        painter.setInterpolationMode(.Pixel)
        painter.setBlendingMode(.Copy)
        painter.drawTexture(0, 0, cast(f32) rc.width, cast(f32) rc.height, rt.colorBuffer)
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