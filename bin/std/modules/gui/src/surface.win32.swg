#global public
#global if #os == Swag.TargetOs.Windows
using Core, Win32

#[Swag.NoCopy]
struct NativeSurface
{
    hWnd: HWND
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
        case WM_PAINT:
            surface.invalidate()
            surface.paint()
        case WM_MOVE:
            surface.position.x = LOWORD(lParam)
            surface.position.y = HIWORD(lParam)
        case WM_SIZE:
            surface.sendResizeEvent(LOWORD(lParam), HIWORD(lParam))
        case WM_DESTROY:
            surface.sendCloseEvent()
        }
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam)
}

impl NativeSurface
{
    // Show the surface
    mtd show()
    {
        ShowWindow(hWnd, SW_SHOW)
    }

    // Hide the surface
    mtd showMaximize()
    {
        ShowWindow(hWnd, SW_MAXIMIZE)
    }

    // Hide the surface
    mtd showUnMaximize()
    {
        ShowWindow(hWnd, SW_RESTORE)
    }

    // Hide the surface
    mtd hide()
    {
        ShowWindow(hWnd, SW_HIDE)
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
    protected func create(app: *Application, title: string, x, y, width, height: s32, flags: SurfaceFlags, mainSurface: *Surface)->*Surface throw
    {
        result := Memory.new'Surface()
        result.app = app
        result.position = @{x, y, width, height}
        result.wnd.position = @{0, 0, width, height}
        result.wnd.itf = cast(IWnd) result.wnd
        result.wnd.drawBackground = true
        result.wnd.surface = result

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

        // Create window
        result.hWnd = cast(const *void) CreateWindowExW(
            flags & .SecondaryWindow ? WS_EX_TOOLWINDOW : 0,
            className,
            null,
            WS_POPUP | WS_CLIPCHILDREN,
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
        if !mainSurface
        {
            app.renderer.setCurrentContext(result.rc)
            app.renderer.init()
        }

        return result
    }
}