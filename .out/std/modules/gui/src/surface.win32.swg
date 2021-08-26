#global if #os == "windows"
using Core, Win32

#[Swag.NoCopy]
public struct Surface
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
    public method show()
    {
        ShowWindow(hWnd, SW_SHOW)
    }

    // Hide the surface
    public method hide()
    {
        ShowWindow(hWnd, SW_HIDE)
    }

    // Creates a new (invisible) surface
    public func create(title: string, width, height: s32, flags: SurfaceFlags, cb: func(Surface, SurfaceMessage))->Surface throw
    {
        var result: retval

        result.proc = cb

        // Create class
        var wc: WNDCLASSW
        wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS
        wc.lpfnWndProc   = acast @mkcallback(&winProc)
        wc.hInstance     = GetModuleHandleW(null)
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
        if flags & .PaletteWindow winFlags |= WS_SYSMENU | WS_THICKFRAME

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
    public func messageLoop()->bool
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