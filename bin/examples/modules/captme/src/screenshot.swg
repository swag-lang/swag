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
    var g_TryStartGrabX: f32
    var g_TryStartGrabY: f32
    var g_StartGrabX:    f32
    var g_StartGrabY:    f32
    var g_EndGrabX:      f32
    var g_EndGrabY:      f32
    var g_RStartGrabX:   f32
    var g_RStartGrabY:   f32
    var g_REndGrabX:     f32
    var g_REndGrabY:     f32
    var g_MouseX:        f32
    var g_MouseY:        f32
    var g_ScreenWidth:   f32
    var g_ScreenHeight:  f32

    var g_StartGrab:    bool
    var g_TryStartGrab: bool
    var g_AutoGrab:     bool
    var g_GrabDone:     bool

    var g_DesktopTexture:     Texture
    var g_GrayDesktopTexture: Texture
    var g_Exit:               bool

    func getGrabRect()->Math.Rectangle
    {
        var result: retval
        result.x = Math.min(g_StartGrabX, g_EndGrabX)
        result.y = Math.min(g_StartGrabY, g_EndGrabY)
        result.width = Math.max(g_StartGrabX, g_EndGrabX) - result.x
        result.height = Math.max(g_StartGrabY, g_EndGrabY) - result.y
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
            g_TryStartGrabX = evt.surfacePos.x
            g_TryStartGrabY = evt.surfacePos.y
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
            g_EndGrabX = evt.surfacePos.x
            g_EndGrabY = evt.surfacePos.y
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
        var grabRect = getGrabRect()

        // Lerp grab rectangle
        if g_AutoGrab
        {
            let dt = 0.1'f32
            g_RStartGrabX = Math.lerp(g_RStartGrabX, grabRect.x, dt)
            g_RStartGrabY = Math.lerp(g_RStartGrabY, grabRect.y, dt)
            g_REndGrabX   = Math.lerp(g_REndGrabX, grabRect.right(), dt)
            g_REndGrabY   = Math.lerp(g_REndGrabY, grabRect.bottom(), dt)
            let xx1 = cast(s32) g_RStartGrabX
            let yy1 = cast(s32) g_RStartGrabY
            let xx2 = cast(s32) g_REndGrabX
            let yy2 = cast(s32) g_REndGrabY
            if cast(s32) grabRect.x != xx1 or 
               cast(s32) grabRect.y != yy1 or 
               cast(s32) grabRect.right() != xx2 or 
               cast(s32) grabRect.bottom() != yy2:
                invalidate()
            grabRect.x = xx1
            grabRect.setRight(xx2)
            grabRect.y = yy1
            grabRect.setBottom(yy2)
        }

        let painter = evt.bc.painter

        // Draw the gray image first
        painter.drawTexture(position, g_GrayDesktopTexture)

        // Draw the normal desktop image inside the grab rectangle
        painter.drawTexture(grabRect, grabRect, g_DesktopTexture)

        // Rectangles
        painter.drawRect(grabRect, Argb.White)
        if g_CaptureOptions.drawCross
        {
            painter.drawLine(g_EndGrabX, 0, g_EndGrabX, g_ScreenHeight, Argb.White)
            painter.drawLine(0, g_EndGrabY, g_ScreenWidth, g_EndGrabY, Argb.White)
        }
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

        let sf  = catch g_App.createSurface(0, 0, cast(s32) g_ScreenWidth, cast(s32) g_ScreenHeight, .Secondary | .TopMost | .NoShadow)
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
    let grabRect = getGrabRect()
    let wGrab = cast(s32) Math.max(grabRect.width, 8)
    let hGrab = cast(s32) Math.max(grabRect.height, 8)
    result = getDesktopImage()
    result.crop(cast(s32) grabRect.x, cast(s32) grabRect.y, wGrab, hGrab)
    return result
}
