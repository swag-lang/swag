#global #if #os == Swag.TargetOs.Windows
using Core, Win32, Gdi32

private
{
    var g_HitTest:   POINT
    var g_Hit:       HWND
    var g_Ignore:    HWND
    var g_Awareness: DPI_AWARENESS_CONTEXT

    func enumProc(hWnd: HWND, _lParam: LPARAM)->BOOL
    {
        if !IsWindowVisible(hWnd) do
            return TRUE

        if hWnd == g_Ignore do
            return TRUE

        var r: RECT
        GetWindowRect(hWnd, &r)
        if g_HitTest.x >= r.left and
           g_HitTest.x < r.right and
           g_HitTest.y >= r.top and
           g_HitTest.y < r.bottom
        {
            var className: [256] CHAR
            let n          = GetClassNameA(hWnd, &className[0], 256)
            let str        = String.from(@mkstring(&className[0], n))

            let systemClasses = [
                "Windows.UI.Core.CoreWindow", // UWP system windows
                "ApplicationFrameWindow"] // UWP app frames

            foreach s in systemClasses
            {
                if str == s do
                    return TRUE
            }

            g_Hit = hWnd
            return FALSE
        }

        return TRUE
    }
}

func getWndRectAtPos(srf: *Surface, x, y: f32)->Math.Rectangle
{
    g_HitTest.x = cast() x
    g_HitTest.y = cast() y

    g_Hit    = null
    g_Ignore = srf.hWnd
    EnumWindows(cast() &enumProc, 0)
    if !g_Hit do
        return {}

    for
    {
        var pt1: POINT
        pt1.x = g_HitTest.x
        pt1.y = g_HitTest.y
        ScreenToClient(g_Hit, &pt1)
        let child = ChildWindowFromPointEx(g_Hit, pt1, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT)
        if !child or child == g_Hit do
            break
        g_Hit = child
    }

    if !g_Hit do
        return {}

    var r: RECT
    GetWindowRect(g_Hit, &r)
    return Math.Rectangle{r.left, r.top, r.right - r.left, r.bottom - r.top}
}

func getBackgroundWndRect()->Math.Rectangle
{
    let hit = GetForegroundWindow()
    if !hit do
        return {}

    let awareness = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)
    defer discard SetThreadDpiAwarenessContext(awareness)

    var r: RECT
    GetWindowRect(hit, &r)
    return Math.Rectangle{r.left, r.top, r.right - r.left, r.bottom - r.top}
}

func getDesktopImage()->Image throw
{
    let awareness = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)
    defer discard SetThreadDpiAwarenessContext(awareness)

    let hDesktopWnd = GetDesktopWindow()
    let hDesktopDC  = GetDC(hDesktopWnd)

    let w = Env.getMetric(.DesktopWidth)
    let h = Env.getMetric(.DesktopHeight)

    let memDC = CreateCompatibleDC(hDesktopDC)
    defer catch DeleteDC(memDC)
    let memBMP = CreateCompatibleBitmap(hDesktopDC, w, h)
    defer catch DeleteObject(cast() memBMP)

    SelectObject(memDC, cast() memBMP)
    BitBlt(memDC, 0, 0, w, h, hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT)
    return Image.from(memBMP)
}
