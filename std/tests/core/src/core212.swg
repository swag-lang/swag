#if #os == "windows";
#import "core"
using win32

#[swag.callback]
func winProc(hwnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)
{
    @print("procccc\n")
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
}