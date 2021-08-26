#global public
#global if #os == "windows"
using Core, Win32

#[Swag.NoCopy]
struct Surface
{
    hWnd: HWND
    proc: func(Surface, SurfaceMessage)
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
    if surface and surface.proc
    {
        message := SurfaceMessage{}
        switch msg
        {
        case WM_PAINT:
            message.kind = .Paint
        case WM_SIZE:
            message.kind = .Resize
            message.param0 = LOWORD(lParam)
            message.param1 = HIWORD(lParam)
        case WM_DESTROY:
            PostQuitMessage(0)
        }

        surface.proc(dref surface, message)
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam)
}

impl Surface
{
    // Show the surface
    method show()
    {
        ShowWindow(hWnd, SW_SHOW)
    }

    // Hide the surface
    method showMaximize()
    {
        ShowWindow(hWnd, SW_MAXIMIZE)
    }

    // Hide the surface
    method showUnMaximize()
    {
        ShowWindow(hWnd, SW_RESTORE)
    }

    // Hide the surface
    method hide()
    {
        ShowWindow(hWnd, SW_HIDE)
    }

    // Transform a point in screen space relative to the surface
    method screenToSurface(pt: *Math.Point)
    {
        wpt := POINT{cast(s32) pt.x, cast(s32) pt.y}
        ScreenToClient(hWnd, &wpt)
        pt.x = wpt.x
        pt.y = wpt.y
    }

    // Transform a point relative to the surface in screen space
    method surfaceToScreen(pt: *Math.Point)
    {
        wpt := POINT{cast(s32) pt.x, cast(s32) pt.y}
        ClientToScreen(hWnd, &wpt)
        pt.x = wpt.x
        pt.y = wpt.y
    }

    // Creates a new (invisible) surface
    func create(title: string, width, height: s32, flags: SurfaceFlags, cb: func(Surface, SurfaceMessage))->Surface throw
    {
        var result: retval

        result.proc = cb

        // Create class
        var wc: WNDCLASSW
        wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS
        wc.lpfnWndProc   = acast @mkcallback(&winProc)
        wc.hInstance     = GetModuleHandleW(null)
        wc.hCursor       = LoadCursorA(null, MAKEINTRESOURCEA(IDC_ARROW))
        wc.lpszClassName = @dataof(utf16.toZeroTerminated("Swag.Gui.Surface"))
        try RegisterClassW(wc)

        // Win ex flags
        var winExFlags: DWORD// = WS_EX_TOOLWINDOW
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
        result.hWnd = CreateWindowExW(
            winExFlags,
            wc.lpszClassName,
            @dataof(utf16.toZeroTerminated(title)),
            winFlags | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            100,
            100,
            width,
            height,
            null,
            null,
            wc.hInstance,
            &result
        )

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