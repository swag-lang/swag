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
    var g_ScreenWidth:  s32
    var g_ScreenHeight: s32

    var g_Exit: bool
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
    using wnd:        Wnd

    startGrab:        bool
    cancelGrag:       bool
    tryStartGrab:     bool
    autoGrab:         bool
    lerpGrab:         Math.Rectangle
}

impl IWnd for ScreenshotWnd
{
    mtd testAutoGrab()
    {
        let pt = Input.getMousePos()
        let r = getWndRectAtPos(surface, pt.x, pt.y)
        if !r.isEmpty()
        {
            g_StartGrabX = r.x
            g_StartGrabY = r.y
            g_EndGrabX   = r.right()
            g_EndGrabY   = r.bottom()
        }
    }

    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed and evt.key == .Escape:
            g_Exit = true

        if !tryStartGrab and !startGrab
        {
            if evt.key == .Shift or evt.key == .RShift
            {
                if evt.kind == .KeyPressed
                {
                    autoGrab = true
                    testAutoGrab()
                }
                else
                {
                    let pt = Input.getMousePos()
                    g_StartGrabX, g_EndGrabX = pt.x
                    g_StartGrabY, g_EndGrabY = pt.y
                    autoGrab = false
                }
            }
        }

        invalidate()
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MousePressed where evt.button == .Right:
            startGrab = false
            tryStartGrab = false
            cancelGrag = true

        case MousePressed where evt.button == .Left:
            tryStartGrab = true

        case MouseReleased where evt.button == .Left:
            if cancelGrag
            {
                cancelGrag = false
                break
            }

            g_GrabDone = startGrab or autoGrab
            g_Exit = true

        case MouseMove where tryStartGrab:
            g_EndGrabX = evt.surfacePos.x
            g_EndGrabY = evt.surfacePos.y

            if Math.abs(g_EndGrabX - g_StartGrabX) > 1 and
               Math.abs(g_EndGrabY - g_StartGrabY) > 1
            {
                tryStartGrab = false
                startGrab    = true
            }

        case MouseMove where startGrab:
            g_EndGrabX = evt.surfacePos.x
            g_EndGrabY = evt.surfacePos.y
            invalidate()

        case MouseMove:
            if !autoGrab
            {
                g_StartGrabX, g_EndGrabX = evt.surfacePos.x
                g_StartGrabY, g_EndGrabY = evt.surfacePos.y
            }
            else
            {
                testAutoGrab()
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
        else
        {
            lerpGrab = grabRect
        }

        let painter = evt.bc.painter

        // Draw the gray image first
        painter.drawTexture(position, g_DesktopTexture)

        // Draw the normal desktop image inside the grab rectangle
        let col: Color = {0, 0, 0, 200}
        if !grabRect.isEmpty()
        {
            painter.fillRect(0, 0, grabRect.x, position.height, col)
            painter.fillRect(grabRect.x, 0, grabRect.width, grabRect.y, col)
            painter.fillRect(grabRect.right(), 0, position.width - grabRect.right(), position.bottom(), col)
            painter.fillRect(grabRect.x, grabRect.bottom(), grabRect.width, position.bottom() - grabRect.bottom(), col)
        }
        else
        {
            painter.fillRect(position, col)
        }

        painter.drawRect(grabRect, Argb.White)

        // Rectangles
        if g_CaptureOptions.drawCross and !autoGrab
        {
            painter.drawLine(g_EndGrabX, 0, g_EndGrabX, cast() g_ScreenHeight, Argb.White)
            painter.drawLine(0, g_EndGrabY, cast() g_ScreenWidth, g_EndGrabY, Argb.White)
        }

        // Dimensions
        if grabRect.width > 0 and grabRect.height > 0
        {
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
}

struct InPlaceEditWnd
{
    using wnd:  Wnd
    editWnd:    *EditWnd
}

impl InPlaceEditWnd
{
    func create(capture: *Capture, main: *MainWnd, parent: *Wnd)->*InPlaceEditWnd
    {
        with let frame = Wnd.create'InPlaceEditWnd(parent)
        {
            .editWnd = EditWnd.create(main, frame)
            .editWnd.dockStyle = .Center
            .editWnd.editView.dockStyle = .Center
            .editWnd.editView.viewMargin = 0
            .editWnd.editView.setCapture(capture)
            .editWnd.editView.setZoom(1)
            .editWnd.editView.setFocus()
            .editWnd.editView.inPlaceEdit = true
        }

        return frame
    }
}

impl IWnd for InPlaceEditWnd
{
    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed and evt.key == .Escape:
            g_Exit = true
    }
}

func captureRect(main: *MainWnd)->#null *Capture throw
{
    // Capture
    ////////////////////////////////

    g_App.disableHotKeys()
    defer g_App.enableHotKeys()

    var img = getDesktopImage()
    var imgBase = img
    g_DesktopTexture = g_App.renderer.addImage(img)
    defer g_App.renderer.removeTexture(&g_DesktopTexture)

    //img.grayScale()
    //img.fade(Argb.Black, 0.6)
    g_GrayDesktopTexture = g_App.renderer.addImage(img)
    defer g_App.renderer.removeTexture(&g_GrayDesktopTexture)

    g_Exit     = false
    g_GrabDone = false

    let sx = Env.getMetric(.DesktopX)
    let sy = Env.getMetric(.DesktopY)
    let sf  = catch g_App.createSurface(sx, sy, g_ScreenWidth, g_ScreenHeight, .Secondary | .TopMost | .NoShadow)
    let captureWnd = Wnd.create'ScreenshotWnd(sf.wnd)
    captureWnd.cursor = Cursor.from(CursorShape.Cross)
    captureWnd.setPosition(0, 0, sf.position.width, sf.position.height)
    captureWnd.SetCapture()

    sf.setView(captureWnd)
    sf.show()
    sf.bringToTop()

    while !g_Exit:
        g_App.runFrame()

    if !g_GrabDone
    {
        sf.destroy()
        return null
    }

    // In place edition of capture image
    ////////////////////////////////

    var grabRect = getGrabRect()
    let wGrab    = Math.max(grabRect.width, 8)
    let hGrab    = Math.max(grabRect.height, 8)
    with let capture = Memory.new'Capture()
    {
        .width = cast(s32) wGrab
        .height = cast(s32) hGrab
        .backImg = imgBase
        .backImagePos.x = -grabRect.x
        .backImagePos.y = -grabRect.y
        .inPlaceEdit = true
    }

    let inPlaceMain = InPlaceEditWnd.create(capture, main, sf.wnd)
    sf.setView(inPlaceMain)
    captureWnd.destroy()

/*
    g_Exit = false
    g_GrabDone = false
    while !g_Exit:
        g_App.runFrame()

    if !g_GrabDone
    {
        sf.destroy()
        return null
    }*/

    if capture
    {
        with capture
        {
            grabRect.x = -.backImagePos.x
            grabRect.y = -.backImagePos.y
            grabRect.width = .width
            grabRect.height = .height

            imgBase.crop(grabRect)
            .backImg, .backImgOrg = imgBase

            .width, .widthOrg = cast(s32) .backImg.width
            .height, .heightOrg = cast(s32) .backImg.height
            .backImagePos = {}

            .inPlaceEdit = false
            .dirtyBackImg = true
        }
    }

    inPlaceMain.editWnd.editView.capture = null
    sf.destroy()
    return capture
}

func screenShot(main: *MainWnd, captureMode: CaptureMode)->#null *Capture throw
{
    startScreenshot()
    defer endScreenshot()

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
            return null
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
        return captureRect(main)
    }

    // Grab
    let grabRect = getGrabRect()
    let wGrab    = Math.max(grabRect.width, 8)
    let hGrab    = Math.max(grabRect.height, 8)
    var img      = getDesktopImage()
    img.crop(cast(s32) grabRect.x, cast(s32) grabRect.y, cast(s32) wGrab, cast(s32) hGrab)

    with let capture = Memory.new'Capture()
    {
        .width, .widthOrg = img.width
        .height, .heightOrg = img.height
        .backImg, .backImgOrg = img
    }

    return capture
}
