#global public
#global #if #os == Swag.TargetOs.Windows
#foreignlib("dwmapi")
using Core, Win32

#[Swag.NoDoc]
struct MARGINS
{
    cxLeftWidth:        s32
    cxRightWidth:       s32
    cyTopHeight:        s32
    cyBottomHeight:     s32
}

#[Swag.Foreign("dwmapi")]
{
    func DwmExtendFrameIntoClientArea(hWnd: HWND, pMarInset: *MARGINS);
}

#[Swag.NoCopy]
struct NativeSurface
{
    hWnd:             HWND
    lastDeadChar:     WPARAM
    topMost:          bool
}

internal func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    // Associate pointer to surface with 'hWnd'
    if msg == WM_CREATE
    {
        let lpcs = cast(*CREATESTRUCTW) cast(u64) lParam
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, lpcs.lpCreateParams)
    }

    // Get 'Surface' pointer
    let surface = cast(*Surface) GetWindowLongPtrW(hWnd, GWLP_USERDATA)
    if surface
    {
        if msg >= WM_USER
        {
            var evt = SysUserEvent{}
            evt.userMsg = msg
            evt.param0  = wParam
            evt.param1  = lParam
            surface.wnd.view.sendEvent(&evt)
            return TRUE
        }

        switch msg
        {
        case WM_SYSKEYDOWN, WM_SYSKEYUP, WM_SYSCHAR:
            return 0
        case WM_KEYDOWN, WM_KEYUP:
            return 0

        case WM_DEADCHAR:
            if (wParam == 27):
                return 1
            if surface.lastDeadChar != wParam
            {
                surface.lastDeadChar = wParam
                break
            }

            fallthrough

        case WM_CHAR:
            if (wParam == 27):
                return 1

            surface.lastDeadChar = 0

            let scanCode = cast(UINT) (lParam & 0xFF0000) >> 16
            let x        = MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX)
            let y        = Input.Keyboard.virtualKeyToKey(x)

            surface.app.sendRuneEvent(y, cast(rune) wParam)
            return 0

        case WM_MOUSEWHEEL:
            let z = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA
            surface.app.sendMouseWheelEvent(z)
            return 0
        case WM_SETCURSOR:
            return 0

        case WM_ACTIVATEAPP:
            surface.app.setActivated(wParam ? true : false)
            return 0

        case WM_ERASEBKGND:
            return 0
        case WM_NCPAINT:
            return 0
        case WM_NCCALCSIZE:
            return 0

        case WM_PAINT:
            var ps: PAINTSTRUCT
            discard BeginPaint(hWnd, &ps)
            surface.invalidate()
            surface.paint()
            EndPaint(hWnd, &ps)
            return 0

        case WM_WINDOWPOSCHANGING:
            return 0

        case WM_GETMINMAXINFO:
            var pt: POINT
            pt.x = cast(s32) surface.position.x
            pt.y = cast(s32) surface.position.y
            let hm = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST)
            if hm != null
            {
                var info: MONITORINFO
                catch GetMonitorInfoA(hm, &info)
                let sdw = cast(s32) surface.app.theme.metrics.surfaceWnd_ShadowSize

                let mm = cast(*MINMAXINFO) cast(u64) lParam
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
                var evt = SysCommandEvent{kind: .Restore}
                surface.wnd.sendEvent(&evt)
            }
        }
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam)
}

internal impl Surface
{
    mtd setFocus()
    {
        SetFocus(me.hWnd)
    }

    // Creates a new (invisible) surface
    func createNative(app: *Application, result: *Surface, x, y, width, height: s32, flags: SurfaceFlags, mainSurface: *Surface) throw
    {
        // Create class
        #[Swag.Global]
        var doneClass: bool
        let className   = @dataof(Utf16.toZeroTerminated("Swag.Gui.Surface"))
        let surfaceName = @dataof(Utf16.toZeroTerminated(result.surfaceID))
        if !doneClass
        {
            doneClass = true

            var hIcon: HICON
            if app.appIcon.isValid():
                hIcon = app.appIcon.toHICON()

            var wc: WNDCLASSW
            wc.hInstance     = GetModuleHandleW(null)
            wc.cbWndExtra    = #sizeof(*void)
            wc.style         = CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW
            wc.lpfnWndProc   = cast() @mkcallback(&winProc)
            wc.hCursor       = LoadCursorA(null, MAKEINTRESOURCEA(IDC_ARROW))
            wc.hIcon         = hIcon
            wc.lpszClassName = className
            try RegisterClassW(wc)
        }

        var winFlags = 0'u32
        winFlags |= WS_POPUP | WS_CLIPCHILDREN

        var winFlagsEx = 0'u32
        if flags & .Secondary:
            winFlagsEx |= WS_EX_TOOLWINDOW
        if flags & .TopMost
        {
            result.topMost = true
            winFlagsEx |= WS_EX_TOPMOST
        }

        // Create window
        result.hWnd = cast(const *void) CreateWindowExW(winFlagsEx, className, surfaceName, winFlags, x, y, width, height, null, null, GetModuleHandleW(null), result)

        // Extend frame inside client area. This will also activate composition.
        if !flags.has(.NoShadow)
        {
            let vm: s32 = cast(s32) app.theme.metrics.surfaceWnd_ShadowSize
            var m:  MARGINS = {vm, vm, vm, vm}
            DwmExtendFrameIntoClientArea(result.hWnd, &m)
        }

        result.rc = try app.renderer.native.createContext(result.hWnd, width, height, mainSurface ? mainSurface.rc.hRC : null)
    }

    mtd destroyNative()
    {
        catch DestroyWindow(me.hWnd)
    }
}

impl Surface
{
    mtd applyNativeSurfaceFlags()
    {
        var exStyle = cast(u64) GetWindowLongPtrA(me.hWnd, GWL_EXSTYLE)
        if me.flags & .Secondary:
            exStyle |= WS_EX_TOOLWINDOW
        else:
            exStyle &= ~WS_EX_TOOLWINDOW
        if me.flags & .TopMost:
            exStyle |= WS_EX_TOPMOST
        else:
            exStyle &= ~WS_EX_TOPMOST
        SetWindowLongPtrA(me.hWnd, GWL_EXSTYLE, cast(LONG_PTR) exStyle)
    }

    // Send a custom message to the surface application with name 'surfaceID'
    func sendCustomMessage(surfaceID: string, id: u32, param: u64)->bool
    {
        let hwnd = Win32.FindWindowA(@dataof("Swag.Gui.Surface"), @dataof(surfaceID))
        if !hwnd:
            return false
        discard SendMessageA(hwnd, WM_USER + id, 0, param)
        return true
    }

    // Set the surface position
    mtd setPosition(pos: Math.Rectangle)
    {
        var clampPos = pos

        if me.minSize.x > 0:
            clampPos.width = Math.max(clampPos.width, me.minSize.x)
        if me.minSize.y > 0:
            clampPos.height = Math.max(clampPos.height, me.minSize.y)
        if me.maxSize.x > 0:
            clampPos.width = Math.min(clampPos.width, me.maxSize.x)
        if me.maxSize.y > 0:
            clampPos.height = Math.min(clampPos.height, me.maxSize.y)

        me.position = clampPos
        catch SetWindowPos(me.hWnd, null, cast(s32) clampPos.x, cast(s32) clampPos.y, cast(s32) clampPos.width, cast(s32) clampPos.height, SWP_NOCOPYBITS | SWP_DEFERERASE | SWP_NOREDRAW | SWP_NOSENDCHANGING)
    }

    // Set the window as a topmost window (above all non topmost)
    mtd setTopMost()
    {
        catch SetWindowPos(me.hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE)
        me.topMost = true
    }

    mtd isTopMost()->bool
    {
        return me.topMost
    }

    // Bring the surface at the top of the Z order
    mtd bringToTop()
    {
        BringWindowToTop(me.hWnd)
    }

    // Enable the surface
    mtd enable()
    {
        if !me.isDisabled:
            return
        me.isDisabled = false
        EnableWindow(me.hWnd, TRUE)
    }

    // Enable the surface
    mtd disable()
    {
        if me.isDisabled:
            return
        me.isDisabled = true
        EnableWindow(me.hWnd, FALSE)
    }

    // Show the surface
    mtd show()
    {
        if !me.isHidden:
            return

        me.isHidden = false
        me.invalidate()

        me.wnd.applyLayout({me.position.width, me.position.height})
        ShowWindow(me.hWnd, SW_SHOW)

        let curFocus = me.wnd.view.getFocus()
        if !curFocus or !me.wnd.isParentOf(curFocus):
            me.wnd.view.setFocus()
        SetActiveWindow(me.hWnd)
        SetFocus(me.hWnd)

        me.wnd.view.sendStateEvent(.Show)
    }

    // Hide the surface
    mtd hide()
    {
        if me.isHidden:
            return
        me.isHidden = true

        ShowWindow(me.hWnd, SW_HIDE)
        me.wnd.view.sendStateEvent(.Hide)
    }

    // Show the surface as maximized
    mtd showMaximized()
    {
        if me.isMaximized:
            return
        me.isMaximized          = true
        me.isMinimized          = false
        me.positionNotMaximized = me.position
        ShowWindow(me.hWnd, SW_SHOWMAXIMIZED)
    }

    // Show the surface in its default state
    mtd showNormal()
    {
        if !me.isMaximized and !me.isMinimized:
            return
        me.isMaximized = false
        me.isMinimized = false
        ShowWindow(me.hWnd, SW_RESTORE)
    }

    // Show the surface as minimized
    mtd showMinimized()
    {
        if me.isMinimized:
            return
        me.isMinimized = true
        ShowWindow(me.hWnd, SW_SHOWMINIMIZED)
    }

    // Transform a point in screen space relative to the surface
    mtd screenToSurface(pt: *Math.Point)
    {
        var wpt = POINT{cast(s32) pt.x, cast(s32) pt.y}
        ScreenToClient(me.hWnd, &wpt)
        pt.x = wpt.x
        pt.y = wpt.y
    }

    // Transform a point relative to the surface in screen space
    mtd surfaceToScreen(pt: *Math.Point)
    {
        var wpt = POINT{cast(s32) pt.x, cast(s32) pt.y}
        ClientToScreen(me.hWnd, &wpt)
        pt.x = wpt.x
        pt.y = wpt.y
    }
}
