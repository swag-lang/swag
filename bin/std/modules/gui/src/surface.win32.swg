#global public
#global if #os == Swag.TargetOs.Windows
using Core, Win32

#[Swag.NoCopy]
struct NativeSurface
{
    hWnd:           HWND
    movingSurface:  bool
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

        case WM_ACTIVATEAPP:
            surface.app.setActivated(wParam ? true : false)
            return 0

        case WM_ERASEBKGND:
            surface.invalidate()
            surface.paint()
            return TRUE

        case WM_PAINT:
            var ps: PAINTSTRUCT
            hdc := BeginPaint(hWnd, &ps)
            surface.invalidate()
            surface.paint()
            EndPaint(hWnd, &ps)
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

    protected func onMouseEvent(wnd: *Wnd, evt: *MouseEvent)->bool
    {
        surface := wnd.surface
        switch evt.kind
        {
        case MousePressed:
            if evt.button == .Left
            {
                wnd.captureMouse()
                BringWindowToTop(surface.native.hWnd)
                if !surface.isMaximized
                    surface.movingSurface = true
                return true
            }

        case MouseReleased:
            if evt.button == .Left
            {
                wnd.releaseMouse()
                surface.movingSurface = false
                return true
            }

        case MouseMove:
            if surface.movingSurface
            {
                surface.position.offset(evt.move.x, evt.move.y);
                catch MoveWindow(surface.hWnd, cast(s32) surface.position.x, cast(s32) surface.position.y, cast(s32) surface.position.width, cast(s32) surface.position.height, FALSE)
                return true
            }
        }

        return false
    }

    // Creates a new (invisible) surface
    protected func createNative(app: *Application, result: *Surface, x, y, width, height: s32, flags: SurfaceFlags, mainSurface: *Surface) throw
    {
        result.wnd.hookOnMouseEvent = &onMouseEvent

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
            wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS
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