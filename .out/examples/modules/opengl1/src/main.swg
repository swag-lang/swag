#global if #os == "windows"
using core, win32, ogl, gdi32

var hDC:    HDC
var frame:  time.FrameTiming

func display()
{
    const SPEED = 100
    glClearColor(0, 0, 0, 0)
    glRotatef(SPEED * frame.dt, 0.0, 0.0, 1)
    glClear(GL_COLOR_BUFFER_BIT)
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2i(0,  1);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2i(-1, -1);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2i(1, -1);
    glEnd();
    glFlush()
    hw := SwapBuffers(hDC)
}

#[swag.callback]
func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    switch msg
    {
    case WM_PAINT:
        display()
        var ps: PAINTSTRUCT
        hw1 := BeginPaint(hWnd, &ps)
        hw2 := EndPaint(hWnd, &ps)
        return 0

    case WM_SIZE:
	    glViewport(0, 0, LOWORD(lParam), HIWORD(lParam))
	    hw := PostMessageA(hWnd, WM_PAINT, 0, 0)
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
    hw := RegisterClassA(wc)

    hWnd := CreateWindowExA(
        0,
        @dataof("CLASS_NAME"),
        @dataof("Example"),
        WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        512,
        512,
        null,
        null,
        wc.hInstance,
        null)

    hDC = GetDC(hWnd)

    var pfd: PIXELFORMATDESCRIPTOR
    pfd.nSize        = @sizeof(pfd)
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
    pfd.iPixelType   = PFD_TYPE_RGBA
    pfd.cColorBits   = 32

    pf := ChoosePixelFormat(hDC, &pfd)
    @assert(pf != 0)
    res := SetPixelFormat(hDC, pf, &pfd)
    @assert(res != FALSE)
    hw2 := DescribePixelFormat(hDC, pf, cast(UINT) @sizeof(PIXELFORMATDESCRIPTOR), &pfd)

    hw3 := ReleaseDC(hDC, hWnd)
    return hWnd
}

func messageLoop()
{
    var msg: MSG
    while true
    {
        frame.update()

        // Windows messages
        frame.pause()
        while PeekMessageA(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            if msg.message == WM_QUIT
                return
            tm := TranslateMessage(&msg)
            dm := DispatchMessageA(&msg)
        }
        frame.unpause()

        display()
    }
}

#main
{
    hWnd := createWindow()

    hDC = GetDC(hWnd)
    hRC := wglCreateContext(hDC)
    hw1 := wglMakeCurrent(hDC, hRC)

    display()
    hw2 := ShowWindow(hWnd, SW_SHOW)
    hw3 := UpdateWindow(hWnd)

    messageLoop()

    hw4 := wglMakeCurrent(null, null)
    hw5 := wglDeleteContext(hRC)
    hw6 := ReleaseDC(hWnd, hDC)
    hw7 := DestroyWindow(hWnd)
}