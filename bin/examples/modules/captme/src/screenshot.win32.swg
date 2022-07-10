#global if #os == Swag.TargetOs.Windows
using Core, Win32, Gdi32, Pixel

using namespace ScreenShotPriv {

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

#[Swag.Callback]
func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    switch msg
    {
    case WM_RBUTTONDOWN:
        PostQuitMessage(0)

    case WM_LBUTTONDOWN:
        g_StartGrabX, g_EndGrabX = GET_X_LPARAM(lParam)
        g_StartGrabY, g_EndGrabY = GET_Y_LPARAM(lParam)
        g_StartGrab = true
        InvalidateRect(hWnd, null, FALSE)

    case WM_LBUTTONUP:
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

        // Mouse cross
        rc = RECT{g_EndGrabX - 1, 0, g_EndGrabX + 1, rc1.bottom}
        FillRect(hDC, rc, cast(HBRUSH) GetStockObject(WHITE_BRUSH))
        rc = RECT{0, g_EndGrabY - 1, rc1.right, g_EndGrabY + 1}
        FillRect(hDC, rc, cast(HBRUSH) GetStockObject(WHITE_BRUSH))

        // Around grab pos, back transparency
        if g_StartGrab
        {
            rc = RECT{0, 0, g_EndGrabX-1, g_StartGrabY}
            FillRect(hDC, rc, g_BlackBrush)
            rc = RECT{g_EndGrabX+1, 0, rc1.right, g_EndGrabY-1}
            FillRect(hDC, rc, g_BlackBrush)
            rc = RECT{0, g_StartGrabY, g_StartGrabX, g_EndGrabY-1}
            FillRect(hDC, rc, g_BlackBrush)
            rc = RECT{0, g_EndGrabY+1, g_EndGrabX-1, rc1.bottom}
            FillRect(hDC, rc, g_BlackBrush)
            rc = RECT{g_EndGrabX+1, g_EndGrabY+1, rc1.right, rc1.bottom}
            FillRect(hDC, rc, g_BlackBrush)
        }
        else
        {
            rc = RECT{0, 0, g_StartGrabX-1, g_StartGrabY-1}
            FillRect(hDC, rc, g_BlackBrush)
            rc = RECT{g_EndGrabX+1, 0, rc1.right, g_StartGrabY-1}
            FillRect(hDC, rc, g_BlackBrush)
            rc = RECT{0, g_StartGrabY+1, g_StartGrabX-1, rc1.bottom}
            FillRect(hDC, rc, g_BlackBrush)
            rc = RECT{g_EndGrabX+1, g_EndGrabY+1, rc1.right, rc1.bottom}
            FillRect(hDC, rc, g_BlackBrush)
        }

        if g_StartGrab
        {
            rc = RECT{g_StartGrabX+1, g_StartGrabY+1, g_EndGrabX-2, g_EndGrabY-2}
            FillRect(hDC, rc, g_RedBrush)

            SetBkMode(hDC, OPAQUE)
            SetBkColor(hDC, RGB(0, 0, 0))
            SelectObject(hDC, acast g_PenRect)
            SelectObject(hDC, GetStockObject(NULL_BRUSH))
            Rectangle(hDC, g_StartGrabX, g_StartGrabY, g_EndGrabX-1, g_EndGrabY-1)
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

    SetLayeredWindowAttributes(hWnd, RGB(255, 0, 0), 128, LWA_ALPHA | LWA_COLORKEY)
    ShowWindow(hWnd, SW_SHOW)
    UpdateWindow(hWnd)
    InvalidateRect(hWnd, null, FALSE)
    return hWnd
}
}

func screenShot()->Image throw
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

    // Grab the portion of the screen
    hDesktopWnd := GetDesktopWindow()
    hDesktopDC  := GetDC(hDesktopWnd)

    wGrab := Math.max(g_EndGrabX - g_StartGrabX, 8)
    hGrab := Math.max(g_EndGrabY - g_StartGrabY, 8)

    hCaptureDC     := CreateCompatibleDC(hDesktopDC)
    hCaptureBitmap := CreateCompatibleBitmap(hDesktopDC, wGrab, hGrab)
    SelectObject(hCaptureDC, acast hCaptureBitmap)
    BitBlt(hCaptureDC, 0, 0, wGrab, hGrab, hDesktopDC, g_StartGrabX, g_StartGrabY, SRCCOPY | CAPTUREBLT)

    var bmp: BITMAP
    GetObjectA(acast hCaptureBitmap, cast(s32) @sizeof(BITMAP), &bmp)

    result = Image.create(bmp.bmWidth, bmp.bmHeight, .BGRA8)
    GetBitmapBits(hCaptureBitmap, bmp.bmWidth * bmp.bmHeight * 4, result.pixels)
    result.flip()

    ReleaseDC(hDesktopWnd, hDesktopDC)
    DeleteDC(hCaptureDC)
    DeleteObject(acast hCaptureBitmap)

    return result
}
