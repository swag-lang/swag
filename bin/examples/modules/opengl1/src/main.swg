#global #if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32

var g_HDC:   HDC
var g_Frame: Time.FrameTiming

func display()
{
    const SPEED = 100
    glClearColor(0, 0, 0, 0)
    glRotatef(SPEED * g_Frame.dt, 0.0, 0.0, 1)
    glClear(GL_COLOR_BUFFER_BIT)
    glBegin(GL_TRIANGLES)
    glColor3f(1.0, 0.0, 0.0)
    glVertex2i(0, 1)
    glColor3f(0.0, 1.0, 0.0)
    glVertex2i(-1, -1)
    glColor3f(0.0, 0.0, 1.0)
    glVertex2i(1, -1)
    glEnd()
    glFlush()
    assume SwapBuffers(g_HDC)
}

func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    switch msg
    {
    case WM_PAINT:
        display()
        var ps: PAINTSTRUCT
        BeginPaint(hWnd, &ps)
        EndPaint(hWnd, &ps)
        return 0

    case WM_SIZE:
        glViewport(0, 0, LOWORD(lParam), HIWORD(lParam))
        assume PostMessageA(hWnd, WM_PAINT, 0, 0)
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
    wc.lpfnWndProc   = cast() @mkcallback(&winProc)
    wc.hInstance     = GetModuleHandleA(null)
    wc.lpszClassName = @dataof("CLASS_NAME")
    assume RegisterClassA(wc)

    let hWnd = CreateWindowExA(0, @dataof("CLASS_NAME"), @dataof("Example"), WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 512, 512, null, null, wc.hInstance, null)

    g_HDC = GetDC(hWnd)

    var pfd: PIXELFORMATDESCRIPTOR
    pfd.nSize      = #sizeof(pfd)
    pfd.nVersion   = 1
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
    pfd.iPixelType = PFD_TYPE_RGBA
    pfd.cColorBits = 32

    let pf = assume ChoosePixelFormat(g_HDC, &pfd)
    assume SetPixelFormat(g_HDC, pf, &pfd)
    assume DescribePixelFormat(g_HDC, pf, cast(UINT) #sizeof(PIXELFORMATDESCRIPTOR), &pfd)

    ReleaseDC(hWnd, g_HDC)
    return hWnd
}

func messageLoop()
{
    var msg: MSG
    var cpt  = 0
    while cpt < 100
    {
        g_Frame.update()

        g_Frame.pause()
        while PeekMessageA(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            if msg.message == WM_QUIT do
                return
            TranslateMessage(&msg)
            DispatchMessageA(&msg)
        }
        g_Frame.unpause()

        display()

        if Env.hasArg("swag.test") do
            cpt += 1
    }
}

#main
{
    let hWnd = createWindow()

    g_HDC = GetDC(hWnd)
    let hRC = try wglCreateContext(g_HDC)
    try wglMakeCurrent(g_HDC, hRC)

    display()
    ShowWindow(hWnd, SW_SHOW)
    try UpdateWindow(hWnd)

    messageLoop()

    try wglMakeCurrent(null, null)
    try wglDeleteContext(hRC)
    ReleaseDC(hWnd, g_HDC)
}
