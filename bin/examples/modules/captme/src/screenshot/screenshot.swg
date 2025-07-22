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

    g_Exit     = false
    g_GrabDone = false

    let sx = Env.getMetric(.DesktopX)
    let sy = Env.getMetric(.DesktopY)
    let sf  = catch g_App.createSurface(sx, sy, g_ScreenWidth, g_ScreenHeight, .Secondary | .TopMost | .NoShadow)
    let captureWnd = Wnd.create'CaptureRectWnd(sf.wnd)
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

    g_Exit = false
    g_GrabDone = false
    while !g_Exit:
        g_App.runFrame()

    if !g_GrabDone
    {
        sf.destroy()
        return null
    }

    inPlaceMain.editWnd.editView.capture = null
    sf.destroy()
    return capture
}

func screenShot(main: *MainWnd, captureMode: CaptureMode)->#null *Capture throw
{
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
        let capture = captureRect(main)
        return capture
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
