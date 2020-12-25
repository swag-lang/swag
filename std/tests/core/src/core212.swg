#if #os == "windows";
#import "core"
using win32, core

#[swag.callback]
func winProc(hwnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)
{
    if msg == WM_DESTROY
        PostQuitMessage(0)
    DefWindowProcA(hwnd, msg, wParam, lParam)
}

#test
{
    var wc: WNDCLASSA
    wc.lpfnWndProc = autocast @mkcallback(&winProc)
    wc.hInstance = GetModuleHandleA(null)
    wc.lpszClassName = @dataof("CLASS_NAME")
    RegisterClassA(wc)

    hwnd := CreateWindowExA(
        0,
        @dataof("CLASS_NAME"),  // name of window class
        @dataof("Sample"),      // title-bar string
        WS_OVERLAPPEDWINDOW,    // top-level window
        CW_USEDEFAULT,          // default horizontal position
        CW_USEDEFAULT,          // default vertical position
        CW_USEDEFAULT,          // default width
        CW_USEDEFAULT,          // default height
        null,                   // no owner window
        null,                   // use class menu
        wc.hInstance,           // handle to application instance
        null)                   // no window-creation data
    ShowWindow(hwnd, SW_SHOW)

    var msg: MSG
    while GetMessageA(&msg, null, 0, 0) != 0
    {
        if msg.message == WM_QUIT break
        TranslateMessage(&msg)
        DispatchMessageA(&msg)
    }
}