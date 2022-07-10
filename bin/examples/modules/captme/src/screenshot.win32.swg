#global if #os == Swag.TargetOs.Windows
using Core, Win32, Gdi32, Pixel

using namespace ScreenShotPriv {

var g_StartGrabX: s32
var g_StartGrabY: s32
var g_EndGrabX:   s32
var g_EndGrabY:   s32
var g_StartGrab:  bool
var g_GrabDone:   bool

#[Swag.Callback]
func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    switch msg
    {
    case WM_LBUTTONDOWN:
        g_StartGrabX, g_EndGrabX = GET_X_LPARAM(lParam)
        g_StartGrabY, g_EndGrabY = GET_Y_LPARAM(lParam)
        g_StartGrab = true
        InvalidateRect(hWnd, null, TRUE)

    case WM_LBUTTONUP:
        g_StartGrab = false
        g_GrabDone = true
        InvalidateRect(hWnd, null, TRUE)
        PostQuitMessage(0)
        //catch DestroyWindow(hWnd)

    case WM_MOUSEMOVE:
        if g_StartGrab
        {
            g_EndGrabX = GET_X_LPARAM(lParam)
            g_EndGrabY = GET_Y_LPARAM(lParam)
            InvalidateRect(hWnd, null, TRUE)
        }

    case WM_PAINT:
        var ps: PAINTSTRUCT
        hDC := BeginPaint(hWnd, &ps)

        var rc1: RECT
        GetClientRect(hWnd, &rc1)
        FillRect(hDC, rc1, GetStockObject(BLACK_BRUSH))

        if g_StartGrab
        {
            var rc: RECT = @{g_StartGrabX, g_StartGrabY, g_EndGrabX, g_EndGrabY}
            FillRect(hDC, rc, GetStockObject(WHITE_BRUSH))
        }

        EndPaint( hWnd, &ps )
        return 0
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam)
}

// Creates a new (invisible) surface
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

    // Create window
    hWnd := cast(const *void) CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED,
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

    SetLayeredWindowAttributes(hWnd, 0, 128, LWA_ALPHA)// | LWA_COLORKEY)
    ShowWindow(hWnd, SW_SHOW)
    UpdateWindow(hWnd)
    InvalidateRect(hWnd, null, TRUE)
    return hWnd
}
}

func screenShot()->Image throw
{
    var result: retval

    nScreenWidth  := GetSystemMetrics(SM_CXMAXTRACK)
    nScreenHeight := GetSystemMetrics(SM_CYMAXTRACK)
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

    hDesktopWnd := GetDesktopWindow()
    hDesktopDC  := GetDC(hDesktopWnd)

    wGrab := Math.max(g_EndGrabX - g_StartGrabX, 8)
    hGrab := Math.max(g_EndGrabY - g_StartGrabY, 8)

    try
    {
        hCaptureDC     := CreateCompatibleDC(hDesktopDC)
        hCaptureBitmap := CreateCompatibleBitmap(hDesktopDC, wGrab, hGrab)
        SelectObject(hCaptureDC, hCaptureBitmap)
        BitBlt(hCaptureDC, 0, 0, wGrab, hGrab, hDesktopDC, g_StartGrabX, g_StartGrabY, SRCCOPY | CAPTUREBLT)

        var bmp: BITMAP
        GetObjectA(hCaptureBitmap, cast(s32) @sizeof(BITMAP), &bmp)

        result = Image.create(bmp.bmWidth, bmp.bmHeight, .BGRA8)
        GetBitmapBits(hCaptureBitmap, bmp.bmWidth * bmp.bmHeight * 4, result.pixels)
        result.flip()

        ReleaseDC(hDesktopWnd, hDesktopDC)
        DeleteDC(hCaptureDC)
        DeleteObject(hCaptureBitmap)
    }

    return result
}
