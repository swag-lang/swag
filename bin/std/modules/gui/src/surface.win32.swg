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
        case WM_PAINT:
            evt := PaintEvent{kind: EventKind.Paint}
            evt.target = &surface.wnd
            surface.app.sendEvent(evt)
        case WM_SIZE:
            surface.rc.width = LOWORD(lParam)
            surface.rc.height = HIWORD(lParam)
            evt := ResizeEvent{kind: EventKind.Resize}
            evt.target = &surface.wnd
            evt.rect.width = surface.rc.width
            evt.rect.height = surface.rc.height
            surface.app.sendEvent(evt)
        case WM_DESTROY:
            PostQuitMessage(0)
        }
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam)
}

impl NativeSurface
{
    // Force the surface to paint
    mtd invalidate()
    {
        InvalidateRect(hWnd, null, FALSE)
    }

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
    protected func create(app: *Application, title: string, x, y, width, height: s32, flags: SurfaceFlags, onEvent: func(*Wnd, *Event))->*Surface throw
    {
        result := Memory.new'Surface()
        result.app = app
        result.wnd.surface = result
        result.wnd.hookOnEvent = onEvent

        // Create class
        var wc: WNDCLASSW
        wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS
        wc.lpfnWndProc   = acast @mkcallback(&winProc)
        wc.hInstance     = GetModuleHandleW(null)
        wc.hCursor       = LoadCursorA(null, MAKEINTRESOURCEA(IDC_ARROW))
        wc.lpszClassName = @dataof(utf16.toZeroTerminated("Swag.Gui.Surface"))
        try RegisterClassW(wc)

        // Win ex flags
        var winExFlags: DWORD
        if flags & .PaletteWindow winExFlags |= WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST

        // Win flags
        var winFlags: DWORD
        if flags & .Caption winFlags |= WS_CAPTION
        if flags & .SysMenu winFlags |= WS_SYSMENU
        if flags & .MinimizeBox winFlags |= WS_MINIMIZEBOX
        if flags & .MaximizeBox winFlags |= WS_MAXIMIZEBOX
        if flags & .DlgFrame winFlags |= WS_DLGFRAME
        if flags & .SizeFrame winFlags |= WS_THICKFRAME
        if flags & .OverlappedWindow winFlags |= WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME
        if flags & .PopupWindow winFlags |= WS_POPUP | WS_BORDER | WS_SYSMENU

        // Create window
        result.hWnd = cast(const *void) CreateWindowExW(
            winExFlags,
            wc.lpszClassName,
            @dataof(utf16.toZeroTerminated(title)),
            winFlags | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            x,
            y,
            width,
            height,
            null,
            null,
            wc.hInstance,
            result
        )

        result.rc = try app.renderer.createContext(result.hWnd)
        result.rc.width = width
        result.rc.height = height
        app.renderer.init()

        return result
    }

    // Returns 'false' when need to quit
    func messageLoop()->bool
    {
        var msg: MSG
        while PeekMessageW(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            if msg.message == WM_QUIT
                return false
            TranslateMessage(&msg)
            DispatchMessageW(&msg)
        }

        return true
    }
}