#global public
#global if #os == Swag.TargetOs.Windows
using Core, Win32

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
        case WM_SYSKEYDOWN, WM_SYSKEYUP:
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
        catch SetWindowPos(hWnd, null, cast(s32) pos.x, cast(s32) pos.y, cast(s32) pos.width, cast(s32) pos.height,  SWP_NOCOPYBITS | SWP_DEFERERASE | SWP_NOREDRAW | SWP_NOSENDCHANGING)
    }

    // Bring the surface at the top of the Z order
    mtd bringToTop()
    {
        BringWindowToTop(hWnd)
    }

    // Show the surface
    mtd show()
    {
        ShowWindow(hWnd, SW_SHOW)
    }

    // Hide the surface
    mtd hide()
    {
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
    protected func createNative(app: *Application, result: *Surface, x, y, width, height: s32, flags: SurfaceFlags, mainSurface: *Surface) throw
    {
        // Create class
        #[Swag.Global]
        var doneClass: bool
        className := @dataof(utf16.toZeroTerminated("Swag.Gui.Surface"))
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

        result.rc = try app.renderer.native.createContext(result.hWnd, width, height, mainSurface ? mainSurface.rc.hRC : null)
    }

    protected mtd destroyNative()
    {
        catch DestroyWindow(hWnd)
    }
}