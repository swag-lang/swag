#global #if #os == Swag.TargetOs.Windows

using Win32, Core

func winProc(hwnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    if msg == WM_DESTROY:
        PostQuitMessage(0)
    return DefWindowProcA(hwnd, msg, wParam, lParam)
}

#test
{
    var wc: WNDCLASSA
    wc.lpfnWndProc   = cast() @mkcallback(&winProc)
    wc.hInstance     = GetModuleHandleA(null)
    wc.lpszClassName = @dataof("CLASS_NAME")
    assume RegisterClassA(wc)

    var hwnd = CreateWindowExA(0, @dataof("CLASS_NAME"), // name of window class
    @dataof("Sample"), // title-bar string
    WS_OVERLAPPEDWINDOW, // top-level window
    CW_USEDEFAULT, // default horizontal position
    CW_USEDEFAULT, // default vertical position
    CW_USEDEFAULT, // default width
    CW_USEDEFAULT, // default height
    null, // no owner window
    null, // use class menu
    wc.hInstance, // handle to application instance
    null) // no window-creation data
    //ShowWindow(hwnd, SW_SHOW)

    var msg: MSG
    while GetMessageA(&msg, null, 0, 0) != 0
    {
        if msg.message == WM_QUIT:
            break
        var tm = TranslateMessage(&msg)
        var dm = DispatchMessageA(&msg)
        break
    }
}
