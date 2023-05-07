#global public
using Core, Pixel, Serialization

struct SurfaceState
{
    position:               Math.Rectangle
    positionNotMaximized:   Math.Rectangle
    isMaximized:            bool
}

struct Surface
{
    using native:   NativeSurface
    app:            *Application
    wnd:            *SurfaceWnd
    painter:        Painter
    rt:             RenderTarget
    userData:       *void

    rc:             RenderingContext
    minSize:        Math.Point
    maxSize:        Math.Point

    flags:          SurfaceFlags = Zero
    dirtyRect:      Math.Rectangle = {Swag.F32.Max, Swag.F32.Max, 0, 0}
    colorDisabled:  Color

    isDirty:        bool
    isHidden:       bool = true
    isDisabled:     bool
    isMinimized:    bool

    using state: SurfaceState
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
    TopMost

    OverlappedWindow = BigCaption | MinimizeBtn | MaximizeBtn | CloseBtn | Border | Sizeable
}

impl Surface
{
    // Change flag surfaces
    mtd setSurfaceFlags(add, remove = SurfaceFlags.Zero)
    {
        // Cannot be changed at runtime
        Debug.assert(!(add & .Secondary) and !(remove & .Secondary))

        flags.add(add)
        flags.remove(remove)
        applySurfaceFlags()
    }

    private mtd applySurfaceFlags()
    {
        if flags.has(.BigCaption)
            wnd.padding.y = app.theme.metrics.wnd_BigCaptionCY
        elif flags.has(.SmallCaption)
            wnd.padding.y = app.theme.metrics.wnd_SmallCaptionCY
        else
            wnd.padding.y = 0

        if flags.has(.Border)
        {
            size := app.theme.metrics.wnd_BorderSize
            wnd.padding.x = size
            wnd.padding.y = Math.max(wnd.padding.y, size)
            wnd.padding.z = size
            wnd.padding.w = size
        }
        else
        {
            wnd.padding.x = 0
            wnd.padding.z = 0
            wnd.padding.w = 0
        }

        wnd.applySurfaceFlags()
        wnd.sendResizeEvent()
    }

    // Creates the surface
    func create(surface: *Surface, x, y, width, height: s32, flags: SurfaceFlags, mainSurface: *Surface) throw
    {
        with surface
        {
            .flags = flags
            .wnd = Memory.new'SurfaceWnd()
            .wnd.isAllocated = true
            .wnd.itf = cast(IWnd) .wnd
            .wnd.type = SurfaceWnd
            .wnd.surface = surface
            .wnd.focusStrategy = .None
            .applySurfaceFlags()
        }

        app := surface.app
        try createNative(app, surface, x, y, width, height, flags, mainSurface)

        surface.minSize = {128, 128}
        w := cast(f32) Math.max(surface.minSize.x, cast(f32) width)
        h := cast(f32) Math.max(surface.minSize.y, cast(f32) height)
        surface.sendResizeEvent(w, h)
    }

    // Destroy the surface
    mtd destroy()
    {
        wnd.destroy()
        hide()
    }

    // Returns inside view
    mtd getView() => wnd.view

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
        invalidateRect({0, 0, position.width, position.height})
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

    // Constraint a window to be inside the closest monitor
    mtd constaintToScreen()
    {
        if isMaximized or isMinimized
            return

        monitors := Env.getMonitors()
        visit m: monitors
        {
            r1 := position
            r1.intersect(m.rect)
            if !r1.isEmpty()
            {
                position.constrainIn(m.rect)
                setPosition(position)
                break
            }
        }
    }

    private mtd createRT()
    {
        if !rt.colorBuffer.valid or rt.colorBuffer.width < rc.width or rt.colorBuffer.height < rc.height
        {
            app.renderer.deleteRenderTarget(&rt)
            var w: s32 = Math.roundUpToPowerOf2(acast rc.width)
            var h: s32 = Math.roundUpToPowerOf2(acast rc.height)
            rt = app.renderer.createRenderTarget(w, h)
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
        dirtyRect = {Swag.F32.Max, Swag.F32.Max, 0, 0}
        isDirty   = false

        var cxt: PaintContext
        cxt.painter = &painter
        cxt.renderer = &app.renderer
        cxt.renderer.begin(rc)
        cxt.painter.begin()

        // Draw shadow around surface
        subRect := &app.theme.rects.surfaceWnd_Shadow
        painter.pushClippingRect(clipRect)
        painter.setColorMaskFull()
        myRect := Math.Rectangle{0, 0, position.width, position.height}
        app.theme.drawSubRect(&painter, myRect, subRect, app.theme.colors.wnd_Shadow, false)

        // Hierarchy paint
        painter.pushClippingRect(wnd.getSurfaceRect())
        painter.setColorMaskColor()
        wnd.paint(&cxt)
        painter.setColorMaskAlpha()
        painter.clear(Argb.Black)
        painter.end()

        // Paint to surface render target
        cxt.renderer.bindRenderTarget(&rt, vp: &myRect)
        cxt.renderer.setScissorRect(clipRect)
        cxt.renderer.clear()
        cxt.renderer.draw(&painter)
        cxt.renderer.unbindRenderTarget()

        // Paint render target to screen
        painter.begin()
        painter.setColorMaskFull()
        painter.setInterpolationMode(.Pixel)
        painter.setBlendingMode(.Copy)

        // If surface is disabled, blend to disabled blit color
        var color: Color
        if isDisabled and app.fadeDisabledSurface
        {
            colorDisabled = Color.getBlend(colorDisabled, app.theme.colors.surfaceDisabled, app.getDt())
            color = colorDisabled
            isDirty = true
            dirtyRect.clear()
        }
        else
        {
            color = Argb.White
            colorDisabled = Argb.White
        }

        dstRect := Math.Rectangle{0, 0, cast(f32) rc.width, cast(f32) rc.height}
        painter.drawTexture(dstRect, dstRect, rt.colorBuffer, color)
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
        wnd.setPosition({sdw, sdw, width - 2 * sdw, height - 2 * sdw})
    }

    mtd setView(view: *Wnd)
    {
        if wnd.view
            wnd.view.destroy()
        wnd.view = view
        wnd.sendResizeEvent()
    }

    private mtd serializeState(ser: *Serializer) throw
    {
        // Save position and surface state
        if wnd.view != null and wnd.view.id != null
        {
            catch ser.serialize(wnd.view.id, &state)
            if !@err and ser.isReading()
            {
                if isMaximized
                {
                    setPosition(positionNotMaximized)
                    isMaximized = false
                    showMaximized()
                }
                else
                {
                    setPosition(position)
                }
            }
        }

        wnd.serializeState(ser)
    }
}