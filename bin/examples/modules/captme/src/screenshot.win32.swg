#global if #os == Swag.TargetOs.Windows
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

private {

var g_TryStartGrabX:    s32
var g_TryStartGrabY:    s32
var g_StartGrabX:       s32
var g_StartGrabY:       s32
var g_EndGrabX:         s32
var g_EndGrabY:         s32
var g_RStartGrabX:      f32
var g_RStartGrabY:      f32
var g_REndGrabX:        f32
var g_REndGrabY:        f32
var g_MouseX:           s32
var g_MouseY:           s32

var g_ScreenWidth:       s32
var g_ScreenHeight:      s32

var g_StartGrab:    bool
var g_TryStartGrab: bool
var g_AutoGrab:     bool
var g_GrabDone:     bool
var g_InitDone:     bool

var g_BlackBrush:   HBRUSH
var g_RedBrush:     HBRUSH
var g_PenRect:      HPEN

var g_MemDC:             HDC
var g_MemBitmap:         HBITMAP
var g_MemDesktopDC:      HDC
var g_MemDesktopBitmap:  HBITMAP
var g_MemDesktop1DC:     HDC
var g_MemDesktop1Bitmap: HBITMAP

func getGrabRect()->{x1: s32, y1: s32, x2: s32, y2: s32}
{
    var result: retval
    result.x1 = Math.min(g_StartGrabX, g_EndGrabX)
    result.y1 = Math.min(g_StartGrabY, g_EndGrabY)
    result.x2 = Math.max(g_StartGrabX, g_EndGrabX)
    result.y2 = Math.max(g_StartGrabY, g_EndGrabY)
    return result
}

var g_HitTest:  POINT
var g_Hit:      HWND
var g_Ignore:   HWND
func enumProc(hWnd: HWND, _lParam: LPARAM)->BOOL
{
    if !IsWindowVisible(hWnd)
        return TRUE

    if hWnd == g_Ignore
        return TRUE

    var r: RECT
    GetWindowRect(hWnd, &r)
    if  g_HitTest.x >= r.left and
        g_HitTest.x < r.right and
        g_HitTest.y >= r.top and
        g_HitTest.y < r.bottom
    {
        g_Hit = hWnd
        return FALSE
    }

    return TRUE
}

func getWndAtPos(hWnd: HWND, pt: POINT)->HWND
{
    g_HitTest = pt
    g_Hit     = null
    g_Ignore  = hWnd
    EnumWindows(acast &enumProc, 0)
    if !g_Hit
        return null

    loop
    {
        var pt1 = pt
        ScreenToClient(g_Hit, &pt1)
        let child = ChildWindowFromPointEx(g_Hit, pt1, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT)
        if !child or child == g_Hit
            break
        g_Hit = child
    }

    return g_Hit
}

func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    switch msg
    {
    case WM_KEYDOWN:
        if wParam == VK_ESCAPE
            PostQuitMessage(0)
        return 0

    case WM_RBUTTONDOWN:
        PostQuitMessage(0)

    case WM_LBUTTONDOWN:
        g_TryStartGrabX = GET_X_LPARAM(lParam)
        g_TryStartGrabY = GET_Y_LPARAM(lParam)
        g_TryStartGrab = true

    case WM_LBUTTONUP:
        if g_StartGrab or g_TryStartGrab
        {
            g_StartGrab = false
            g_TryStartGrab = false
            g_GrabDone = true
            InvalidateRect(hWnd, null, FALSE)
            PostQuitMessage(0)
        }

    case WM_MOUSEMOVE:
        g_EndGrabX = GET_X_LPARAM(lParam)
        g_EndGrabY = GET_Y_LPARAM(lParam)
        g_MouseX = g_EndGrabX
        g_MouseY = g_EndGrabY
        if g_TryStartGrab and
           Math.abs(g_EndGrabX - g_StartGrabX) > 1 and
           Math.abs(g_EndGrabY - g_StartGrabY) > 1
        {
            g_StartGrabX = g_TryStartGrabX
            g_StartGrabY = g_TryStartGrabY
            g_TryStartGrab = false
            g_StartGrab = true
        }

        g_AutoGrab = false
        if !g_StartGrab
        {
            g_StartGrabX = g_EndGrabX
            g_StartGrabY = g_EndGrabY

            var pp: POINT
            pp.x = g_EndGrabX
            pp.y = g_EndGrabY
            ClientToScreen(hWnd, &pp)

            let under = getWndAtPos(hWnd, pp)
            if under
            {
                var r: RECT
                g_AutoGrab = true
                GetWindowRect(under, &r)
                g_StartGrabX = r.left
                g_StartGrabY = r.top
                g_EndGrabX = r.right-1
                g_EndGrabY = r.bottom-1
            }
        }

        InvalidateRect(hWnd, null, FALSE)

    case WM_ERASEBKGND:
        return TRUE

    case WM_PAINT:
        var ps: PAINTSTRUCT
        let hDC = BeginPaint(hWnd, &ps)

        var rc1: RECT
        GetClientRect(hWnd, &rc1)

        // Blit the "disabled" desktop
        catch BitBlt(g_MemDC, 0, 0, g_ScreenWidth, g_ScreenHeight, g_MemDesktop1DC, 0, 0, SRCCOPY | CAPTUREBLT)

        var (x1, y1, x2, y2) = getGrabRect()

        // Lerp
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
            if x1 != xx1 or y1 != yy1 or x2 != xx2 or y2 != yy2
                InvalidateRect(hWnd, null, FALSE)
            x1 = xx1; x2 = xx2; y1 = yy1; y2 = yy2
        }

        // Blit the real desktop inside the selected area
        catch BitBlt(g_MemDC, x1, y1, x2-x1, y2-y1, g_MemDesktopDC, x1, y1, SRCCOPY | CAPTUREBLT)

        if g_StartGrab or g_AutoGrab
        {
            // Border
            SetBkMode(g_MemDC, TRANSPARENT)
            SelectObject(g_MemDC, acast g_PenRect)
            SelectObject(g_MemDC, GetStockObject(NULL_BRUSH))
            Rectangle(g_MemDC, x1, y1, x2, y2)

            // Mouse cross
            Rectangle(g_MemDC, g_MouseX, 0, g_MouseX+1, g_ScreenHeight)
            Rectangle(g_MemDC, 0, g_MouseY, g_ScreenWidth, g_MouseY+1)

            // Capture size
            var str = Format.toString("% x %", x2 - x1, y2 - y1)
            SetBkMode(g_MemDC, OPAQUE)
            SetBkColor(g_MemDC, RGB(0, 0, 0))
            SetTextColor(g_MemDC, RGB(255, 255, 255))
            var xx = x1
            var yy = y1 - 20
            if xx < 0 or yy < 0
            {
                xx = x1 + 5
                yy = y1 + 5
            }
            TextOutA(g_MemDC, xx, yy, @dataof(str), acast @countof(str))
        }

        // Blit result to window dc
        catch BitBlt(hDC, 0, 0, g_ScreenWidth, g_ScreenHeight, g_MemDC, 0, 0, SRCCOPY | CAPTUREBLT)

        EndPaint(hWnd, &ps)
        return 0
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam)
}

func createNative(w, h: s32)->HWND throw
{
    let className = @dataof(Utf16.toZeroTerminated("CaptMe.CaptureSurface"))

    var wc: WNDCLASSW
    wc.hInstance     = GetModuleHandleW(null)
    wc.cbWndExtra    = @sizeof(*void);
    wc.style         = CS_HREDRAW | CS_VREDRAW
    wc.lpfnWndProc   = acast @mkcallback(&winProc)
    wc.hCursor       = LoadCursorA(null, MAKEINTRESOURCEA(IDC_CROSS))
    wc.lpszClassName = className
    catch RegisterClassW(wc)

    let hWnd = cast(const *void) CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        className,
        null,
        WS_POPUP | WS_VISIBLE,
        0,
        0,
        w,
        h,
        null,
        null,
        GetModuleHandleW(null),
        null
    )

    UpdateWindow(hWnd)
    InvalidateRect(hWnd, null, FALSE)
    return hWnd
}
}

func screenShot(main: *MainWnd, captureMode: CaptureMode)->Image throw
{
    var result: retval

    if !g_InitDone
    {
        g_InitDone   = true
        g_BlackBrush = cast(HBRUSH) GetStockObject(BLACK_BRUSH)
        g_RedBrush   = cast(HBRUSH) catch CreateSolidBrush(RGB(255, 0, 0))
        g_PenRect    = CreatePen(PS_DOT, 1, RGB(255,255,0))
    }

    g_StartGrab    = false
    g_GrabDone     = false
    g_ScreenWidth  = GetSystemMetrics(SM_CXMAXTRACK)
    g_ScreenHeight = GetSystemMetrics(SM_CYMAXTRACK)

    let hDesktopWnd = GetDesktopWindow()
    let hDesktopDC = GetDC(hDesktopWnd)

    // Make a copy of the desktop
    g_MemDesktopDC = CreateCompatibleDC(hDesktopDC)
    g_MemDesktopBitmap = CreateCompatibleBitmap(hDesktopDC, g_ScreenWidth, g_ScreenHeight)
    SelectObject(g_MemDesktopDC, acast g_MemDesktopBitmap)
    BitBlt(g_MemDesktopDC, 0, 0, g_ScreenWidth, g_ScreenHeight, hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT)

    if captureMode == .FullScreen
    {
        g_StartGrabX, g_StartGrabY = 0
        g_EndGrabX = g_ScreenWidth
        g_EndGrabY = g_ScreenHeight
    }
    elif captureMode == .ActiveWindow
    {
        g_Hit = GetForegroundWindow()
        if !g_Hit
            return result
        var r: RECT
        GetWindowRect(g_Hit, &r)
        g_StartGrabX = r.left
        g_StartGrabY = r.top
        g_EndGrabX   = r.right
        g_EndGrabY   = r.bottom
    }
    elif captureMode >= .Screen0 and captureMode <= .Screen3
    {
        let idx = cast(s32) captureMode - cast(s32) CaptureMode.Screen0
        g_StartGrabX = cast(s32) main.monitors[idx].rect.x
        g_StartGrabY = cast(s32) main.monitors[idx].rect.y
        g_EndGrabX = g_StartGrabX + cast(s32) main.monitors[idx].rect.width
        g_EndGrabY = g_StartGrabY + cast(s32) main.monitors[idx].rect.height
    }
    elif captureMode == .LastArea and g_EndGrabX != g_StartGrabX and g_EndGrabY != g_StartGrabY
    {
    }
    else
    {
        // Make a grayscale copy of the desktop
        g_MemDesktop1DC = CreateCompatibleDC(hDesktopDC)
        g_MemDesktop1Bitmap = CreateCompatibleBitmap(hDesktopDC, g_ScreenWidth, g_ScreenHeight)
        SelectObject(g_MemDesktop1DC, acast g_MemDesktop1Bitmap)
        BitBlt(g_MemDesktop1DC, 0, 0, g_ScreenWidth, g_ScreenHeight, g_MemDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT)
        var img = Image.from(g_MemDesktop1Bitmap)
        img.grayScale()
        img.fade(Argb.Black, 0.6)
        img.flip()
        SetBitmapBits(g_MemDesktop1Bitmap, acast img.size, img.pixels.buffer)

        // Create a working memdc
        g_MemDC = CreateCompatibleDC(hDesktopDC)
        g_MemBitmap = CreateCompatibleBitmap(hDesktopDC, g_ScreenWidth, g_ScreenHeight)
        SelectObject(g_MemDC, acast g_MemBitmap)

        g_RStartGrabX = 0
        g_RStartGrabY = 0
        g_REndGrabX = 0
        g_REndGrabY = 0

        main.getApp().runFrame()
        let hWnd = createNative(g_ScreenWidth, g_ScreenHeight)
        SetCapture(hWnd)

        var msg: MSG
        while GetMessageW(&msg, null, 0, 0) != 0
        {
            TranslateMessage(&msg)
            DispatchMessageW(&msg)
        }

        DeleteDC(g_MemDC)
        DeleteObject(acast g_MemBitmap)

        catch DestroyWindow(hWnd)
        if !g_GrabDone
            return result
    }

    let (x1, y1, x2, y2) = getGrabRect()
    let wGrab = Math.max(x2 - x1, 8)
    let hGrab = Math.max(y2 - y1, 8)

    // Copy the portion of the screen we want
    let hCaptureDC = CreateCompatibleDC(hDesktopDC)
    let hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, wGrab, hGrab)
    SelectObject(hCaptureDC, acast hCaptureBitmap)
    BitBlt(hCaptureDC, 0, 0, wGrab, hGrab, g_MemDesktopDC, x1, y1, SRCCOPY | CAPTUREBLT)

    // Transform that to an image
    var bmp: BITMAP
    GetObjectA(acast hCaptureBitmap, cast(s32) @sizeof(BITMAP), &bmp)
    result = Image.create(bmp.bmWidth, bmp.bmHeight, .BGRA8)
    GetBitmapBits(hCaptureBitmap, bmp.bmWidth * bmp.bmHeight * 4, result.pixels.buffer)
    result.flip()

    ReleaseDC(hDesktopWnd, hDesktopDC)
    DeleteDC(hCaptureDC)
    DeleteObject(acast hCaptureBitmap)
    DeleteDC(g_MemDesktopDC)
    DeleteObject(acast g_MemDesktopBitmap)

    return result
}
