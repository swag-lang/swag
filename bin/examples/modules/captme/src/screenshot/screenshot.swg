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

var g_StartGrabX:         f32
var g_StartGrabY:         f32
var g_EndGrabX:           f32
var g_EndGrabY:           f32
var g_ScreenWidth:        s32
var g_ScreenHeight:       s32
var g_Exit:               bool
var g_GrabDone:           bool
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

func waitForCapture()->bool
{
    g_Exit     = false
    g_GrabDone = false
    while !g_Exit do
        g_App.runFrame()
    return g_GrabDone
}

func performScreenCapture(main: *MainWnd)->{ img: Image, sf: *Surface } throw
{
    g_App.disableHotKeys()
    defer g_App.enableHotKeys()

    var result: retval
    result.img       = getDesktopImage()
    g_DesktopTexture = g_App.renderer.addImage(result.img)
    defer g_App.renderer.removeTexture(&g_DesktopTexture)

    let sx = Env.getMetric(.DesktopX)
    let sy = Env.getMetric(.DesktopY)

    result.sf = catch g_App.createSurface(sx, sy, g_ScreenWidth, g_ScreenHeight, .Secondary | .TopMost | .NoShadow)
    let captureWnd = Wnd.create'CaptureRectWnd(result.sf.wnd)
    captureWnd.main   = main
    captureWnd.cursor = Cursor.from(CursorShape.Cross)
    captureWnd.setPosition(0, 0, result.sf.position.width, result.sf.position.height)
    captureWnd.SetCapture()

    result.sf.setView(captureWnd)
    result.sf.show()
    result.sf.bringToTop()

    if !waitForCapture()
    {
        result.sf.destroy()
        result.sf = null
    }

    return result
}

// In place edition of capture image
func performInPlaceEdit(img: Image, surface: *Surface, main: *MainWnd)->#null *Capture throw
{
    if !surface do
        return null

    let grabRect = getGrabRect()
    with let capture = Memory.new'Capture()
    {
        .width          = cast(s32) grabRect.width
        .height         = cast(s32) grabRect.height
        .backImg        = img
        .backImagePos.x = -grabRect.x
        .backImagePos.y = -grabRect.y
        .inPlaceEdit    = true
    }

    if !main.captureOptions.inPlaceEdit
    {
        InPlaceEditWnd.crop(capture)
        surface.destroy()
        return capture
    }

    let inPlaceWnd = InPlaceEditWnd.create(capture, main, surface.wnd)
    surface.setView(inPlaceWnd)

    inPlaceWnd.newMain.fileDlgState = main.fileDlgStateInPlace
    defer main.fileDlgStateInPlace = inPlaceWnd.newMain.fileDlgState

    if !waitForCapture()
    {
        surface.destroy()
        return null
    }

    inPlaceWnd.editWnd.editView.capture = null
    surface.destroy()
    return capture
}

func captureRect(main: *MainWnd)->#null *Capture throw
{
    let (img, sf) = catch performScreenCapture(main)
    return catch performInPlaceEdit(img, sf, main)
}

func setGrabBounds(main: *MainWnd, captureMode: CaptureMode)->bool
{
    switch captureMode
    {
    case .FullScreen:
        g_StartGrabX, g_StartGrabY = 0
        g_EndGrabX = g_ScreenWidth * Env.getDPIScale()
        g_EndGrabY = g_ScreenHeight * Env.getDPIScale()

    case .ActiveWindow:
        let r = getBackgroundWndRect()
        if r.isEmpty() do
            return false
        g_StartGrabX = r.x
        g_StartGrabY = r.y
        g_EndGrabX   = r.right()
        g_EndGrabY   = r.bottom()

    case .Screen0 to .Screen3:
        let idx  = cast(s32) captureMode - cast(s32) CaptureMode.Screen0
        let rect = main.monitors[idx].rectReal
        g_StartGrabX = rect.x
        g_StartGrabY = rect.y
        g_EndGrabX   = rect.x + rect.width
        g_EndGrabY   = rect.y + rect.height

    case .LastArea:
        if g_EndGrabX == g_StartGrabX or g_EndGrabY == g_StartGrabY do
            return false

    case .Area:
        return false
    }

    return true
}

func screenShot(main: *MainWnd, captureMode: CaptureMode) throw
{
    // Hide editor before capture
    let surface   = main.surface
    let wasHidden = surface.isHidden
    if main.captureOptions.hideEditorBefore and !surface.isHidden and !surface.isMinimized do
        surface.hide()

    // Force a delay
    if main.captureOptions.delay do
        Threading.Thread.sleep(main.captureOptions.delay * 1000)

    g_ScreenWidth  = Env.getMetric(.DesktopWidth)
    g_ScreenHeight = Env.getMetric(.DesktopHeight)

    // Capture
    var capture: *Capture
    if !setGrabBounds(main, captureMode) do
        capture = captureRect(main)
    else
    {
        let grabRect = getGrabRect()
        let wGrab    = Math.max(grabRect.width, 8)
        let hGrab    = Math.max(grabRect.height, 8)

        var img = getDesktopImage()
        img.crop(cast(s32) grabRect.x, cast(s32) grabRect.y, cast(s32) wGrab, cast(s32) hGrab)

        with capture = Memory.new'Capture()
        {
            .width, .widthOrg = img.width
            .height, .heightOrg = img.height
            .backImg = img
        }

        if main.captureOptions.sendToClipboard do
            Clipboard.addImage(capture.backImg)
        if !main.captureOptions.sendToEditor do
            capture = null
    }

    if (capture or !wasHidden)
    {
        if surface.isMinimized do
            surface.showNormal()
        else do
            surface.show()
    }

    if capture do
        main.newCapture(capture.backImg, null, capture)
}
