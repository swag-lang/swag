#global if #os == Swag.TargetOs.Windows
using Core, Win32, Gdi32, Pixel

enum CaptureMode
{
    None
    Area
    LastArea
    FullScreen
    Screen0
    Screen1
    Screen2
    Screen3
}

scopefile {

var g_StartGrabX: s32
var g_StartGrabY: s32
var g_EndGrabX:   s32
var g_EndGrabY:   s32
var g_StartGrab:  bool
var g_GrabDone:   bool
var g_InitDone:   bool

var g_BlackBrush:   HBRUSH
var g_RedBrush:     HBRUSH
var g_PenRect:      HPEN

func getGrabRect()->{x1: s32, y1: s32, x2: s32, y2: s32}
{
    var result: retval
    result.x1 = Math.min(g_StartGrabX, g_EndGrabX)
    result.y1 = Math.min(g_StartGrabY, g_EndGrabY)
    result.x2 = Math.max(g_StartGrabX, g_EndGrabX)
    result.y2 = Math.max(g_StartGrabY, g_EndGrabY)
    return result
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
        g_StartGrabX, g_EndGrabX = GET_X_LPARAM(lParam)
        g_StartGrabY, g_EndGrabY = GET_Y_LPARAM(lParam)
        g_StartGrab = true
        InvalidateRect(hWnd, null, FALSE)

    case WM_LBUTTONUP:
        if !g_StartGrab
            break
        g_StartGrab = false
        g_GrabDone = true
        InvalidateRect(hWnd, null, FALSE)
        PostQuitMessage(0)

    case WM_MOUSEMOVE:
        g_EndGrabX = GET_X_LPARAM(lParam)
        g_EndGrabY = GET_Y_LPARAM(lParam)
        if !g_StartGrab
        {
            g_StartGrabX = g_EndGrabX
            g_StartGrabY = g_EndGrabY
        }

        InvalidateRect(hWnd, null, FALSE)

    case WM_ERASEBKGND:
        return TRUE

    case WM_PAINT:
        var ps: PAINTSTRUCT
        hDC := BeginPaint(hWnd, &ps)

        var rc, rc1: RECT
        GetClientRect(hWnd, &rc1)

        (x1, y1, x2, y2) := getGrabRect()

        // Around grab pos, back transparency
        if g_StartGrab
        {
            rc = RECT{0, 0, rc1.right, y1}
            FillRect(hDC, rc, g_BlackBrush)
            rc = RECT{0, y2, rc1.right, rc1.bottom}
            FillRect(hDC, rc, g_BlackBrush)
            rc = RECT{0, y1, x1, y2}
            FillRect(hDC, rc, g_BlackBrush)
            rc = RECT{x2, y1, rc1.right, y2}
            FillRect(hDC, rc, g_BlackBrush)
        }
        else
        {
            FillRect(hDC, rc1, g_BlackBrush)
        }

        if g_StartGrab
        {
            rc = RECT{x1, y1, x2, y2}
            FillRect(hDC, rc, g_RedBrush)

            SetBkMode(hDC, OPAQUE)
            SetBkColor(hDC, RGB(0, 0, 0))
            SelectObject(hDC, acast g_PenRect)
            SelectObject(hDC, GetStockObject(NULL_BRUSH))
            Rectangle(hDC, x1, y1, x2-2, y2-2)
        }

        EndPaint( hWnd, &ps )
        return 0
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam)
}

func createNative(w, h: s32)->HWND throw
{
    className := @dataof(Utf16.toZeroTerminated("CaptMe.CaptureSurface"))

    var wc: WNDCLASSW
    wc.hInstance     = GetModuleHandleW(null)
    wc.cbWndExtra    = @sizeof(*void);
    wc.style         = CS_HREDRAW | CS_VREDRAW
    wc.lpfnWndProc   = acast @mkcallback(&winProc)
    wc.hCursor       = LoadCursorA(null, MAKEINTRESOURCEA(IDC_CROSS))
    wc.lpszClassName = className
    catch RegisterClassW(wc)

    hWnd := cast(const *void) CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
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

    SetLayeredWindowAttributes(hWnd, RGB(255, 0, 0), 128, LWA_ALPHA | LWA_COLORKEY)
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
        g_InitDone = true
        g_BlackBrush = cast(HBRUSH) GetStockObject(BLACK_BRUSH)
        g_RedBrush   = cast(HBRUSH) catch CreateSolidBrush(RGB(255, 0, 0))
        g_PenRect    = CreatePen(PS_DASH, 1, RGB(255,255,0))
    }

    g_StartGrab  = false
    g_GrabDone   = false

    // Capture with mouse
    nScreenWidth  := GetSystemMetrics(SM_CXMAXTRACK)
    nScreenHeight := GetSystemMetrics(SM_CYMAXTRACK)

    if captureMode == .FullScreen
    {
        g_StartGrabX, g_StartGrabY = 0
        g_EndGrabX = nScreenWidth
        g_EndGrabY = nScreenHeight
    }
    elif captureMode >= .Screen0 and captureMode <= .Screen3
    {
        idx := cast(s32) captureMode - cast(s32) CaptureMode.Screen0
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
        main.getApp().runFrame()
        hWnd := createNative(nScreenWidth, nScreenHeight)

        var msg: MSG
        while GetMessageW(&msg, null, 0, 0) != 0
        {
            TranslateMessage(&msg)
            DispatchMessageW(&msg)
        }

        catch DestroyWindow(hWnd)
        if !g_GrabDone
            return result
    }

    // Grab the portion of the screen
    hDesktopWnd := GetDesktopWindow()
    hDesktopDC  := GetDC(hDesktopWnd)

    (x1, y1, x2, y2) := getGrabRect()
    wGrab := Math.max(x2 - x1, 8)
    hGrab := Math.max(y2 - y1, 8)

    hCaptureDC     := CreateCompatibleDC(hDesktopDC)
    hCaptureBitmap := CreateCompatibleBitmap(hDesktopDC, wGrab, hGrab)
    SelectObject(hCaptureDC, acast hCaptureBitmap)
    BitBlt(hCaptureDC, 0, 0, wGrab, hGrab, hDesktopDC, x1, y1, SRCCOPY | CAPTUREBLT)

    var bmp: BITMAP
    GetObjectA(acast hCaptureBitmap, cast(s32) @sizeof(BITMAP), &bmp)

    result = Image.create(bmp.bmWidth, bmp.bmHeight, .BGRA8)
    GetBitmapBits(hCaptureBitmap, bmp.bmWidth * bmp.bmHeight * 4, result.pixels.buffer)
    result.flip()

    ReleaseDC(hDesktopWnd, hDesktopDC)
    DeleteDC(hCaptureDC)
    DeleteObject(acast hCaptureBitmap)

    return result
}
