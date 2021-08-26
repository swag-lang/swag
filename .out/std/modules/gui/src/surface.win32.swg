#global if #os == "windows"
using Core, Win32

#[Swag.NoCopy]
public struct Surface
{
    hWnd: HWND
}

#[Swag.Callback]
func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    return DefWindowProcA(hWnd, msg, wParam, lParam)
}

public func createSurface(title: string, width, height: s32)->Surface throw
{
    var result: retval

    // Create class
    var wc: WNDCLASSW
    wc.style         = CS_OWNDC
    wc.lpfnWndProc   = acast @mkcallback(&winProc)
    wc.hInstance     = GetModuleHandleA(null)
    wc.lpszClassName = @dataof(utf16.toZeroTerminated("Swag.Gui.Surface"))
    try RegisterClassW(wc)

    // Create window
    title16 := utf16.toZeroTerminated(title)
    result.hWnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        @dataof(title16),
        WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        100,
        100,
        width,
        height,
        null,
        null,
        wc.hInstance,
        null
    )

    return result
}