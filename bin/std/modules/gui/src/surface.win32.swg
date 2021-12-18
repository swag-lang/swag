#global public
#global if #os == Swag.TargetOs.Windows
#foreignlib "dwmapi"
using Core, Win32

struct MARGINS
{
    cxLeftWidth:    s32
    cxRightWidth:   s32
    cyTopHeight:    s32
    cyBottomHeight: s32
}

#[Swag.Foreign("dwmapi.lib")]
{
    func DwmExtendFrameIntoClientArea(hWnd: HWND, pMarInset: *MARGINS);
}

#[Swag.NoCopy]
struct NativeSurface
{
    hWnd:           HWND
    isMaximized:    bool
}

#[Swag.Callback]
private func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    // Associate pointer to surface with 'hWnd'
    if msg == WM_CREATE
    {
        lpcs := cast(*CREATESTRUCTW) cast(uint) lParam
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, lpcs.lpCreateParams)
    }

    // Get 'Surface' pointer
    surface := cast(*Surface) GetWindowLongPtrW(hWnd, GWLP_USERDATA)
    if surface
    {
        switch msg
        {
        case WM_SYSKEYDOWN, WM_SYSKEYUP, WM_SYSCHAR:
            return 0
        case WM_KEYDOWN, WM_KEYUP:
            return 0
        case WM_CHAR:
            surface.app.sendCharEvent(cast(rune) wParam)
            return 0
        case WM_MOUSEWHEEL:
            z := GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA
            surface.app.sendMouseWheelEvent(z)
            return 0
        case WM_SETCURSOR:
            return 0

        case WM_ACTIVATEAPP:
            surface.app.setActivated(wParam ? true : false)
            return 0

        case WM_ERASEBKGND: return 0
        case WM_NCPAINT:    return 0
        case WM_NCCALCSIZE: return 0

        case WM_PAINT:
            var ps: PAINTSTRUCT
            hdc := BeginPaint(hWnd, &ps)
            surface.invalidate()
            surface.paint()
            EndPaint(hWnd, &ps)
            return 0

        case WM_WINDOWPOSCHANGING:
            ppos := cast(*WINDOWPOS) cast(uint) lParam
            ppos.flags |= SWP_NOCOPYBITS | SWP_NOREDRAW
            if !(ppos.flags & SWP_NOMOVE)
            {
                surface.position.x = cast(f32) ppos.x
                surface.position.y = cast(f32) ppos.y
            }
            if !(ppos.flags & SWP_NOSIZE)
            {
                surface.position.width = cast(f32) ppos.cx
                surface.position.height = cast(f32) ppos.cy
                surface.sendResizeEvent(cast(f32) ppos.cx, cast(f32) ppos.cy)
            }
            return 0

        case WM_GETMINMAXINFO:
            var pt: POINT
            pt.x = cast(s32) surface.position.x
            pt.y = cast(s32) surface.position.y
            hm := MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST)
            if hm != null
            {
                var info: MONITORINFO
                catch GetMonitorInfoA(hm, &info)
                sdw := cast(s32) surface.app.theme.metrics.surfaceWnd_ShadowSize

                mm := cast(*MINMAXINFO) cast(uint) lParam
                mm.ptMaxPosition.x = (info.rcWork.left - info.rcMonitor.left) - sdw
                mm.ptMaxPosition.y = (info.rcWork.top - info.rcMonitor.top) - sdw
                mm.ptMaxSize.x     = (info.rcWork.right - info.rcWork.left) + 2 * sdw
                mm.ptMaxSize.y     = (info.rcWork.bottom - info.rcWork.top) + 2 * sdw
            }

            return 0

        case WM_MOVE:
            surface.position.x = LOWORD(lParam)
            surface.position.y = HIWORD(lParam)
        case WM_SIZE:
            surface.sendResizeEvent(cast(f32) LOWORD(lParam), cast(f32) HIWORD(lParam))

        case WM_SYSCOMMAND:
            switch wParam
            {
            case SC_RESTORE:
                evt := SysCommandEvent{}
                evt.sysKind = .Restore
                surface.wnd.sendEvent(&evt)
            }
        }
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam)
}

impl Surface
{
    // Set the surface position
    mtd setPosition(pos: Math.Rectangle)
    {
        clampPos := pos
        wnd.clampSize(&clampPos)
        catch SetWindowPos(hWnd, null, cast(s32) clampPos.x, cast(s32) clampPos.y, cast(s32) clampPos.width, cast(s32) clampPos.height,  SWP_NOCOPYBITS | SWP_DEFERERASE | SWP_NOREDRAW | SWP_NOSENDCHANGING)
    }

    // Bring the surface at the top of the Z order
    mtd bringToTop()
    {
        BringWindowToTop(hWnd)
    }

    // Enable the surface
    mtd enable()
    {
        if !isDisabled
            return
        isDisabled = false
        EnableWindow(hWnd, TRUE)
    }

    // Enable the surface
    mtd disable()
    {
        if isDisabled
            return
        isDisabled = true
        EnableWindow(hWnd, FALSE)
    }

    // Show the surface
    mtd show()
    {
        if !isHidden
            return
        isHidden = false
        invalidate()
        wnd.applyLayout()
        ShowWindow(hWnd, SW_SHOW)
        wnd.view.setFocus()
    }

    // Hide the surface
    mtd hide()
    {
        if isHidden
            return
        isHidden = true
        ShowWindow(hWnd, SW_HIDE)
    }

    // Show the surface as maximized
    mtd showMaximized()
    {
        if isMaximized
            return
        isMaximized = true
        ShowWindow(hWnd, SW_SHOWMAXIMIZED)
    }

    // Show the surface in its default state
    mtd showNormal()
    {
        if !isMaximized
            return
        isMaximized = false
        ShowWindow(hWnd, SW_RESTORE)
    }

    // Show the surface as minimized
    mtd showMinimized()
    {
        ShowWindow(hWnd, SW_SHOWMINIMIZED)
    }

    // Transform a point in screen space relative to the surface
    mtd screenToSurface(pt: *Math.Point)
    {
        wpt := POINT{cast(s32) pt.x, cast(s32) pt.y}
        ScreenToClient(hWnd, &wpt)
        pt.x = wpt.x
        pt.y = wpt.y
    }

    // Transform a point relative to the surface in screen space
    mtd surfaceToScreen(pt: *Math.Point)
    {
        wpt := POINT{cast(s32) pt.x, cast(s32) pt.y}
        ClientToScreen(hWnd, &wpt)
        pt.x = wpt.x
        pt.y = wpt.y
    }

    // Creates a new (invisible) surface
    private func createNative(app: *Application, result: *Surface, x, y, width, height: s32, flags: SurfaceFlags, mainSurface: *Surface) throw
    {
        // Create class
        #[Swag.Global]
        var doneClass: bool
        className := @dataof(Utf16.toZeroTerminated("Swag.Gui.Surface"))
        if !doneClass
        {
            doneClass = true

            var wc: WNDCLASSW
            wc.hInstance     = GetModuleHandleW(null)
            wc.cbWndExtra    = @sizeof(*void);
            wc.style         = CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW
            wc.lpfnWndProc   = acast @mkcallback(&winProc)
            wc.hCursor       = LoadCursorA(null, MAKEINTRESOURCEA(IDC_ARROW))
            wc.lpszClassName = className
            try RegisterClassW(wc)
        }

        winFlags := 0'u32
        winFlags |= WS_POPUP | WS_CLIPCHILDREN

        winFlagsEx := 0'u32
        if flags & .Secondary
            winFlagsEx |= WS_EX_TOOLWINDOW

        // Create window
        result.hWnd = cast(const *void) CreateWindowExW(
            winFlagsEx,
            className,
            null,
            winFlags,
            x,
            y,
            width,
            height,
            null,
            null,
            GetModuleHandleW(null),
            result
        )

        // Extend frame inside client area. This will also activate composition.
        var vm: s32 = cast(s32) app.theme.metrics.surfaceWnd_ShadowSize
        var m:  MARGINS = @{vm,vm,vm,vm}
        DwmExtendFrameIntoClientArea(result.hWnd, &m)

        result.rc = try app.renderer.native.createContext(result.hWnd, width, height, mainSurface ? mainSurface.rc.hRC : null)
    }

    private mtd destroyNative()
    {
        catch DestroyWindow(hWnd)
    }
}