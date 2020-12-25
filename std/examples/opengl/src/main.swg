#import "core"
#import "ogl"
using core, win32, ogl, gdi32

#[swag.callback]
func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    switch msg
    {
    case WM_PAINT:
        glClear(GL_COLOR_BUFFER_BIT)

        var ps: PAINTSTRUCT
        BeginPaint(hWnd, &ps)
        EndPaint(hWnd, &ps)
        return 0

    case WM_SIZE:
	    glViewport(0, 0, LOWORD(lParam), HIWORD(lParam))
	    PostMessageA(hWnd, WM_PAINT, 0, 0)
	    return 0

    case WM_DESTROY:
        PostQuitMessage(0)
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam)
}

func createWindow()->HWND
{
    var wc: WNDCLASSA
    wc.style         = CS_OWNDC
    wc.lpfnWndProc   = autocast @mkcallback(&winProc)
    wc.hInstance     = GetModuleHandleA(null)
    wc.lpszClassName = @dataof("CLASS_NAME")
    RegisterClassA(wc)

    hWnd := CreateWindowExA(
        0,
        @dataof("CLASS_NAME"),
        @dataof("Example"),
        WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        null,
        null,
        wc.hInstance,
        null)

    hDC := GetDC(hWnd)

    var pfd: PIXELFORMATDESCRIPTOR
    pfd.nSize        = @sizeof(pfd)
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
    pfd.iPixelType   = PFD_TYPE_RGBA
    pfd.cColorBits   = 32

    pf := ChoosePixelFormat(hDC, &pfd)
    SetPixelFormat(hDC, pf, &pfd)

    ReleaseDC(hDC, hWnd)
    return hWnd
}

func messageLoop()
{
    var msg: MSG
    while GetMessageA(&msg, null, 0, 0) != 0
    {
        if msg.message == WM_QUIT
            break
        TranslateMessage(&msg)
        DispatchMessageA(&msg)
    }
}

#main
{
    hWnd := createWindow()

    hDC := GetDC(hWnd)
    hRC := wglCreateContext(hDC)
    wglMakeCurrent(hDC, hRC)

    ShowWindow(hWnd, SW_SHOW)
    messageLoop()

    wglMakeCurrent(null, null)
    ReleaseDC(hWnd, hDC)
    wglDeleteContext(hRC)
    DestroyWindow(hWnd)
}