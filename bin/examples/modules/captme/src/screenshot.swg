using Core, Win32, Gdi32, Pixel

enum CaptureMode
{
    Area
    LastArea
    FullScreen
    Screen0
    Screen1
    Screen2
    Screen3
    ActiveWindow
}

private
{
    var g_TryStartGrabX: s32
    var g_TryStartGrabY: s32
    var g_StartGrabX:    s32
    var g_StartGrabY:    s32
    var g_EndGrabX:      s32
    var g_EndGrabY:      s32
    var g_RStartGrabX:   f32
    var g_RStartGrabY:   f32
    var g_REndGrabX:     f32
    var g_REndGrabY:     f32
    var g_MouseX:        s32
    var g_MouseY:        s32

    var g_ScreenWidth:  s32
    var g_ScreenHeight: s32

    var g_StartGrab:    bool
    var g_TryStartGrab: bool
    var g_AutoGrab:     bool
    var g_GrabDone:     bool

    var g_DesktopTexture:     Texture
    var g_GrayDesktopTexture: Texture
    var g_Exit:               bool

    func getGrabRect()->{ x1: s32, y1: s32, x2: s32, y2: s32 }
    {
        var result: retval
        result.x1 = Math.min(g_StartGrabX, g_EndGrabX)
        result.y1 = Math.min(g_StartGrabY, g_EndGrabY)
        result.x2 = Math.max(g_StartGrabX, g_EndGrabX)
        result.y2 = Math.max(g_StartGrabY, g_EndGrabY)
        return result
    }
}

struct ScreenshotWnd
{
    using wnd: Wnd
}

impl IWnd for ScreenshotWnd
{
    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed and evt.key == .Escape:
            g_Exit = true
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MousePressed and evt.button == .Right
        {
            g_Exit = true
        }
        elif evt.kind == .MousePressed and evt.button == .Left
        {
            g_TryStartGrabX = cast(s32) evt.surfacePos.x
            g_TryStartGrabY = cast(s32) evt.surfacePos.y
            g_TryStartGrab  = true
        }
        elif evt.kind == .MouseReleased and evt.button == .Left
        {
            if g_StartGrab or g_TryStartGrab
            {
                g_StartGrab    = false
                g_TryStartGrab = false
                g_GrabDone     = true
                g_Exit         = true
            }
        }
        elif evt.kind == .MouseMove
        {
            g_EndGrabX = cast(s32) evt.surfacePos.x
            g_EndGrabY = cast(s32) evt.surfacePos.y
            g_MouseX   = g_EndGrabX
            g_MouseY   = g_EndGrabY
            if g_TryStartGrab and
               Math.abs(g_EndGrabX - g_StartGrabX) > 1 and
               Math.abs(g_EndGrabY - g_StartGrabY) > 1
            {
                g_StartGrabX   = g_TryStartGrabX
                g_StartGrabY   = g_TryStartGrabY
                g_TryStartGrab = false
                g_StartGrab    = true
            }

            g_AutoGrab = false
            if !g_StartGrab
            {
                g_StartGrabX = g_EndGrabX
                g_StartGrabY = g_EndGrabY

                let r = getWndRectAtPos(surface, g_EndGrabX, g_EndGrabY)
                if !r.isEmpty()
                {
                    g_AutoGrab   = true
                    g_StartGrabX = cast() r.x
                    g_StartGrabY = cast() r.y
                    g_EndGrabX   = cast() r.right()
                    g_EndGrabY   = cast() r.bottom()
                }
            }

            invalidate()
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        var (x1, y1, x2, y2) = getGrabRect()

        // Lerp grab rectangle
        if g_AutoGrab
        {
            let dt = 0.1'f32
            g_RStartGrabX = Math.lerp(g_RStartGrabX, cast(f32) x1, dt)
            g_RStartGrabY = Math.lerp(g_RStartGrabY, cast(f32) y1, dt)
            g_REndGrabX   = Math.lerp(g_REndGrabX, cast(f32) x2, dt)
            g_REndGrabY   = Math.lerp(g_REndGrabY, cast(f32) y2, dt)
            let xx1 = cast(s32) g_RStartGrabX
            let yy1 = cast(s32) g_RStartGrabY
            let xx2 = cast(s32) g_REndGrabX
            let yy2 = cast(s32) g_REndGrabY
            if x1 != xx1 or y1 != yy1 or x2 != xx2 or y2 != yy2:
                invalidate()
            x1 = xx1
            x2 = xx2
            y1 = yy1
            y2 = yy2
        }

        let painter = evt.bc.painter

        // Draw the gray image first
        painter.drawTexture(position, g_GrayDesktopTexture)

        // Draw the normal desktop image inside the grab rectangle
        painter.drawTexture({x1, y1, x2 - x1, y2 - y1}, {x1, y1, x2 - x1, y2 - y1}, g_DesktopTexture)
    }
}

func screenShot(main: *MainWnd, captureMode: CaptureMode)->Image throw
{
    startScreenshot()
    defer endScreenshot()

    var result: retval

    g_ScreenWidth  = Env.getMetric(.DesktopWidth)
    g_ScreenHeight = Env.getMetric(.DesktopHeight)

    if captureMode == .FullScreen
    {
        g_StartGrabX, g_StartGrabY = 0
        g_EndGrabX = g_ScreenWidth
        g_EndGrabY = g_ScreenHeight
    }
    elif captureMode == .ActiveWindow
    {
        let r = getBackgroundWndRect()
        if r.isEmpty():
            return result
        g_StartGrabX = cast() r.x
        g_StartGrabY = cast() r.y
        g_EndGrabX   = cast() r.right()
        g_EndGrabY   = cast() r.bottom()
    }
    elif captureMode >= .Screen0 and captureMode <= .Screen3
    {
        let idx = cast(s32) captureMode - cast(s32) CaptureMode.Screen0
        g_StartGrabX = cast(s32) main.monitors[idx].rectReal.x
        g_StartGrabY = cast(s32) main.monitors[idx].rectReal.y
        g_EndGrabX   = g_StartGrabX + cast(s32) main.monitors[idx].rectReal.width
        g_EndGrabY   = g_StartGrabY + cast(s32) main.monitors[idx].rectReal.height
    }
    elif captureMode == .LastArea and g_EndGrabX != g_StartGrabX and g_EndGrabY != g_StartGrabY
    {
    }
    else
    {
        var img = getDesktopImage()
        g_DesktopTexture = g_App.renderer.addImage(img)
        defer g_App.renderer.removeTexture(&g_DesktopTexture)
        img.grayScale()
        img.fade(Argb.Black, 0.6)
        g_GrayDesktopTexture = g_App.renderer.addImage(img)
        defer g_App.renderer.removeTexture(&g_GrayDesktopTexture)

        g_Exit        = false
        g_StartGrab   = false
        g_GrabDone    = false
        g_RStartGrabX = 0
        g_RStartGrabY = 0
        g_REndGrabX   = 0
        g_REndGrabY   = 0

        let sf  = catch g_App.createSurface(0, 0, g_ScreenWidth, g_ScreenHeight, .Secondary | .TopMost | .NoShadow)
        let wnd = Wnd.create'ScreenshotWnd(sf.wnd)
        wnd.setPosition(0, 0, sf.position.width, sf.position.height)
        sf.setView(wnd)
        sf.show()
        sf.bringToTop()

        while !g_Exit:
            g_App.runFrame()

        sf.destroy()

        if !g_GrabDone:
            return result
    }

    // Grab
    let (x1, y1, x2, y2) = getGrabRect()
    let wGrab = Math.max(x2 - x1, 8)
    let hGrab = Math.max(y2 - y1, 8)
    result = getDesktopImage()
    result.crop(x1, y1, wGrab, hGrab)
    return result
}
