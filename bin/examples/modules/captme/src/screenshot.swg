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
    var g_StartGrabX:   f32
    var g_StartGrabY:   f32
    var g_EndGrabX:     f32
    var g_EndGrabY:     f32
    var g_ScreenWidth:  f32
    var g_ScreenHeight: f32

    var g_Exit:     bool
    var g_GrabDone: bool

    var g_DesktopTexture:     Texture
    var g_GrayDesktopTexture: Texture

    func getGrabRect()->Math.Rectangle
    {
        var result: retval
        result.x      = Math.min(g_StartGrabX, g_EndGrabX)
        result.y      = Math.min(g_StartGrabY, g_EndGrabY)
        result.width  = Math.max(g_StartGrabX, g_EndGrabX) - result.x
        result.height = Math.max(g_StartGrabY, g_EndGrabY) - result.y
        return result
    }
}

struct ScreenshotWnd
{
    using wnd:         Wnd

    startGrab:         bool
    tryStartGrab:      bool
    autoGrab:          bool
    lerpGrab:          Math.Rectangle
    tryStartGrabX:     f32
    tryStartGrabY:     f32
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
        switch evt.kind
        {
        case MousePressed where evt.button == .Right:
            g_Exit = true

        case MousePressed where evt.button == .Left:
            tryStartGrabX = evt.surfacePos.x
            tryStartGrabY = evt.surfacePos.y
            tryStartGrab  = true

        case MouseReleased where evt.button == .Left:
            if startGrab or tryStartGrab
            {
                startGrab, tryStartGrab = false
                g_GrabDone, g_Exit = true
            }

        case MouseMove:
            cursor = Cursor.from(CursorShape.Cross)

            g_EndGrabX = evt.surfacePos.x
            g_EndGrabY = evt.surfacePos.y
            if tryStartGrab and
               Math.abs(g_EndGrabX - g_StartGrabX) > 1 and
               Math.abs(g_EndGrabY - g_StartGrabY) > 1
            {
                g_StartGrabX = tryStartGrabX
                g_StartGrabY = tryStartGrabY
                tryStartGrab = false
                startGrab    = true
            }

            autoGrab = false
            if !startGrab
            {
                g_StartGrabX = g_EndGrabX
                g_StartGrabY = g_EndGrabY

                let r = getWndRectAtPos(surface, g_EndGrabX, g_EndGrabY)
                if !r.isEmpty()
                {
                    autoGrab     = true
                    g_StartGrabX = r.x
                    g_StartGrabY = r.y
                    g_EndGrabX   = r.right()
                    g_EndGrabY   = r.bottom()
                }
            }

            invalidate()
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        var grabRect = getGrabRect()

        // Lerp grab rectangle
        if autoGrab
        {
            let dt = 0.1'f32
            lerpGrab.x      = Math.lerp(lerpGrab.x, grabRect.x, dt)
            lerpGrab.y      = Math.lerp(lerpGrab.y, grabRect.y, dt)
            lerpGrab.width  = Math.lerp(lerpGrab.width, grabRect.width, dt)
            lerpGrab.height = Math.lerp(lerpGrab.height, grabRect.height, dt)

            var roundRect = lerpGrab
            roundRect.round()

            if !grabRect.isEqualEpsilon(roundRect):
                invalidate()

            grabRect = roundRect
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

        // Dimensions
        let fontR = getFont().regular
        let str   = Format.toString("% x %", grabRect.width, grabRect.height)
        var xx    = grabRect.x
        var yy    = grabRect.y - fontR.height
        if xx < 0 or yy < 0
        {
            xx = grabRect.x + 5
            yy = grabRect.y + 5
        }
        painter.drawString(xx, yy, str, fontR, Argb.White)
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
        g_StartGrabX = r.x
        g_StartGrabY = r.y
        g_EndGrabX   = r.right()
        g_EndGrabY   = r.bottom()
    }
    elif captureMode >= .Screen0 and captureMode <= .Screen3
    {
        let idx = cast(s32) captureMode - cast(s32) CaptureMode.Screen0
        g_StartGrabX = main.monitors[idx].rectReal.x
        g_StartGrabY = main.monitors[idx].rectReal.y
        g_EndGrabX   = g_StartGrabX + main.monitors[idx].rectReal.width
        g_EndGrabY   = g_StartGrabY + main.monitors[idx].rectReal.height
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

        g_Exit     = false
        g_GrabDone = false

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
    let wGrab    = Math.max(grabRect.width, 8)
    let hGrab    = Math.max(grabRect.height, 8)
    result = getDesktopImage()
    result.crop(cast(s32) grabRect.x, cast(s32) grabRect.y, cast(s32) wGrab, cast(s32) hGrab)
    return result
}
