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
        }

        surface.proc(dref surface, message)
    }

    switch msg
    {
    case WM_DESTROY:
        PostQuitMessage(0)
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
    public func create(title: string, width, height: s32, cb: func(Surface, SurfaceMessage))->Surface throw
    {
        var result: retval

        result.proc = cb

        // Create class
        var wc: WNDCLASSW
        wc.style         = CS_OWNDC
        wc.lpfnWndProc   = acast @mkcallback(&winProc)
        wc.hInstance     = GetModuleHandleW(null)
        wc.lpszClassName = @dataof(utf16.toZeroTerminated("Swag.Gui.Surface"))
        try RegisterClassW(wc)

        // Create window
        result.hWnd = CreateWindowExW(
            0,
            wc.lpszClassName,
            @dataof(utf16.toZeroTerminated(title)),
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
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