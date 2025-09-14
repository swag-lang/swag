#global public
using Core, Pixel, Serialization

struct SurfaceState
{
    position:                 Math.Rectangle
    positionNotMaximized:     Math.Rectangle
    isMaximized:              bool
}

struct Surface
{
    using native:      NativeSurface
    app:               *Application
    wnd:               *SurfaceWnd
    painter:           Painter
    rt:                RenderTarget
    userData:          *void
    surfaceID:         String

    rc:                RenderingContext
    minSize:           Math.Point
    maxSize:           Math.Point

    flags:             SurfaceFlags = Zero
    dirtyRect:         Math.Rectangle = {Swag.F32.Max, Swag.F32.Max, 0, 0}
    colorDisabled:     Color

    isDirty:           bool
    isHidden:          bool = true
    isDisabled:        bool
    isMinimized:       bool
    using state:       SurfaceState
}

#[Swag.EnumFlags]
enum SurfaceFlags
{
    Zero             = 0

    BigCaption
    SmallCaption
    MinimizeBtn
    MaximizeBtn
    CloseBtn
    Border
    Sizeable

    Secondary
    TopMost
    NoShadow

    OverlappedWindow = BigCaption | MinimizeBtn | MaximizeBtn | CloseBtn | Border | Sizeable
}

impl Surface
{
    // Change flag surfaces
    mtd setSurfaceFlags(add, remove = SurfaceFlags.Zero)
    {
        // Cannot be changed at runtime
        Debug.assert(!(add & .Secondary) and !(remove & .Secondary))

        me.flags.add(add)
        me.flags.remove(remove)
        me.applySurfaceFlags()
    }

    internal mtd applySurfaceFlags()
    {
        if me.flags.has(.BigCaption):
            me.wnd.padding.y = me.app.theme.metrics.wnd_BigCaptionCY
        elif me.flags.has(.SmallCaption):
            me.wnd.padding.y = me.app.theme.metrics.wnd_SmallCaptionCY
        else:
            me.wnd.padding.y = 0

        if me.flags.has(.Border)
        {
            let size = me.app.theme.metrics.wnd_BorderSize
            me.wnd.padding.x = size
            me.wnd.padding.y = Math.max(me.wnd.padding.y, size)
            me.wnd.padding.z = size
            me.wnd.padding.w = size
        }
        else
        {
            me.wnd.padding.x = 0
            me.wnd.padding.z = 0
            me.wnd.padding.w = 0
        }

        me.applyNativeSurfaceFlags()
        me.wnd.applySurfaceFlags()
        me.wnd.sendResizeEvent()
    }

    // Creates the surface
    func create(surface: *Surface, x, y, width, height: s32, flags: SurfaceFlags, mainSurface: *Surface) throw
    {
        with surface
        {
            .flags             = flags
            .wnd               = Memory.new'SurfaceWnd()
            .wnd.isAllocated   = true
            .wnd.itf           = cast(IWnd) .wnd
            .wnd.type          = SurfaceWnd
            .wnd.surface       = surface
            .wnd.focusStrategy = .None
            .wnd.computeStyle()
            .applySurfaceFlags()
        }

        let app = surface.app
        try createNative(app, surface, x, y, width, height, flags, mainSurface)

        surface.minSize = {128, 128}
        let w = cast(f32) Math.max(surface.minSize.x, cast(f32) width)
        let h = cast(f32) Math.max(surface.minSize.y, cast(f32) height)

        surface.sendResizeEvent(w, h)
    }

    // Destroy the surface
    mtd destroy()
    {
        me.wnd.destroy()
        me.hide()
    }

    // Returns inside view
    mtd getView()->*Wnd
    {
        return me.wnd.view
    }

    // Set the surface icon
    mtd setIcon(icon: Icon)
    {
        me.wnd.icon = icon
        me.invalidate()
    }

    // Set the surface title
    mtd setTitle(name: #null string)
    {
        me.wnd.name = name
        me.invalidate()
    }

    // Invalidate the full surface to be painted
    mtd invalidate()
    {
        me.invalidateRect({0, 0, me.position.width, me.position.height})
    }

    // Invalidate one part of the surface to be painted
    mtd invalidateRect(rect: Math.Rectangle)
    {
        if !me.isDirty
        {
            me.dirtyRect = rect
            me.isDirty   = true
            return
        }

        let r = me.dirtyRect.right()
        let b = me.dirtyRect.bottom()
        me.dirtyRect.x = Math.min(me.dirtyRect.x, rect.x)
        me.dirtyRect.y = Math.min(me.dirtyRect.y, rect.y)
        me.dirtyRect.setRight(Math.max(r, rect.right()))
        me.dirtyRect.setBottom(Math.max(b, rect.bottom()))
    }

    // Center this surface relative to 'from'
    mtd center(from: *Surface)
    {
        var pos = me.position
        pos.x = from.position.horzCenter()
        pos.y = from.position.vertCenter()
        pos.x -= me.position.width * 0.5
        pos.y -= me.position.height * 0.5
        me.setPosition(pos)
    }

    // Constraint a window position to be inside the closest monitor
    mtd constaintPositionToScreen(topLeft, bottomRight = true, margin = 0'f32)
    {
        if me.isMaximized or me.isMinimized:
            return

        let monitor = Env.getMonitorForPoint({me.position.x, me.position.y})
        var np      = me.position

        if topLeft
        {
            np.x = Math.max(np.x, monitor.rect.x + margin)
            np.x = Math.min(np.x, monitor.rect.right() - margin)
            np.y = Math.max(np.y, monitor.rect.y + margin)
            np.y = Math.min(np.y, monitor.rect.bottom() - margin)
        }

        if bottomRight
        {
            np.x = Math.min(np.x, monitor.rect.right() - margin - me.position.width)
            np.y = Math.min(np.y, monitor.rect.bottom() - margin - me.position.height)
        }

        me.setPosition(np)
    }

    internal mtd createRT()
    {
        if !me.rt.colorBuffer.valid or me.rt.colorBuffer.width < me.rc.width or me.rt.colorBuffer.height < me.rc.height
        {
            me.app.renderer.deleteRenderTarget(&me.rt)
            let w = Math.roundUpToPowerOf2(cast(u32) me.rc.width)
            let h = Math.roundUpToPowerOf2(cast(u32) me.rc.height)
            me.rt = me.app.renderer.createRenderTarget(w, h)
            me.invalidate()
        }
    }

    // Main surface paint function
    mtd paint()
    {
        me.createRT()

        if !me.isDirty:
            return
        let clipRect = me.dirtyRect
        me.dirtyRect = {Swag.F32.Max, Swag.F32.Max, 0, 0}
        me.isDirty   = false

        with var cxt: PaintContext
        {
            .painter  = &me.painter
            .renderer = &me.app.renderer
            .renderer.begin(me.rc)
            .painter.begin()
        }

        // Draw shadow around surface
        var myRect = Math.Rectangle{0, 0, me.position.width, me.position.height}
        if !me.flags.has(.NoShadow)
        {
            let subRect = &me.app.theme.rects.surfaceWnd_Shadow
            me.painter.pushClippingRect(clipRect)
            me.painter.setColorMaskFull()
            me.app.theme.drawSubRect(&me.painter, myRect, subRect, me.app.theme.colors.wnd_Shadow, false)
        }

        // Hierarchy paint
        me.painter.pushClippingRect(me.wnd.getSurfaceRect())
        me.painter.setColorMaskColor()
        me.wnd.paint(&cxt)

        if me.app.options.showClipRect
        {
            var dbgColor = Random.shared().nextU32() & 0x00FFFFFF
            dbgColor |= 0x55000000
            me.painter.fillRect(clipRect, Color.fromArgb(dbgColor))
        }

        me.painter.setColorMaskAlpha()
        me.painter.clear(Argb.Black)
        me.painter.end()

        // Paint to surface render target
        with cxt
        {
            .renderer.bindRenderTarget(&me.rt, vp: &myRect)
            .renderer.setScissorRect(clipRect)
            .renderer.clear()
            .renderer.draw(&me.painter)
            .renderer.unbindRenderTarget()
        }

        // Paint render target to screen
        me.painter.begin()
        me.painter.setColorMaskFull()
        me.painter.setInterpolationMode(.Pixel)
        me.painter.setBlendingMode(.Copy)

        // If surface is disabled, blend to disabled blit color
        var color: Color
        if me.isDisabled and me.app.fadeDisabledSurface
        {
            me.colorDisabled = Color.getBlend(me.colorDisabled, me.app.theme.colors.surfaceDisabled, me.app.getDt())
            color            = me.colorDisabled
            me.isDirty       = true
            me.dirtyRect.clear()
        }
        else
        {
            color            = Argb.White
            me.colorDisabled = Argb.White
        }

        let dstRect = Math.Rectangle{0, 0, cast(f32) me.rc.width, cast(f32) me.rc.height}
        me.painter.drawTexture(dstRect, dstRect, me.rt.colorBuffer, color)
        me.painter.end()

        cxt.renderer.draw(&me.painter)
        cxt.renderer.end()
    }

    internal mtd sendResizeEvent(width, height: f32)
    {
        me.rc.width        = cast(s32) width
        me.rc.height       = cast(s32) height
        me.position.width  = width
        me.position.height = height

        let sdw = me.flags.has(.NoShadow) ? 0 : me.app.theme.metrics.surfaceWnd_ShadowSize
        me.wnd.setPosition({sdw, sdw, width - 2 * sdw, height - 2 * sdw})
    }

    mtd setView(view: *Wnd)
    {
        if me.wnd.view:
            me.wnd.view.destroy()
        me.wnd.view = view
        me.wnd.sendResizeEvent()
    }

    internal mtd serializeState(ser: *Serializer) throw
    {
        // Save position and surface state
        if me.wnd.view != null and me.wnd.view.id != null
        {
            catch ser.serialize(me.wnd.view.id, &me.state)
            if @err == null and ser.isReading()
            {
                if me.isMaximized
                {
                    me.setPosition(me.positionNotMaximized)
                    me.isMaximized = false
                    me.showMaximized()
                }
                else
                {
                    me.setPosition(me.position)
                }
            }
        }

        me.wnd.serializeState(ser)
    }
}
