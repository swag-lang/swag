#global if #os == "windows"
#import "core"
#import "image"
#import "ogl"
using core, win32, ogl, gdi32

var hDC: HDC

func dataPath()->String
{
    dataPath := path.getDirectoryName(#location.fileName)
    dataPath = path.getDirectoryName(dataPath)
    return path.combine(dataPath, "datas")
}

func imagePath(name: string)->String
{
    return path.combine(dataPath(), name)
}

func display()
{
    glClearColor(0, 1, 0, 0)
    glClear(GL_COLOR_BUFFER_BIT)
    glEnable(GL_TEXTURE_2D)

    glBindTexture(GL_TEXTURE_2D, textureID)
    glBegin(GL_QUADS)
    glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0, 0.0); glVertex2f( 1.0, -1.0);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0, 1.0); glVertex2f( 1.0,  1.0);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0, 1.0); glVertex2f(-1.0,  1.0);  // Top Left Of The Texture and Quad
    glEnd()

    glFlush()
    SwapBuffers(hDC)
}

var textureID: GLuint
#[swag.printbc]
func loadImg()
{
    data := imagePath("image3.bmp")
    bytes := try file.readAllBytes(data)
    var dec: image.bmp.Decoder
    img := try dec.decode(bytes.toSlice())

    glGenTextures(1, &textureID)
    glBindTexture(GL_TEXTURE_2D, textureID)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cast(s32) img.width, cast(s32) img.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.pixels)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
}

#[swag.callback]
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
        100,
        100,
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
    DescribePixelFormat(hDC, pf, cast(UINT) @sizeof(PIXELFORMATDESCRIPTOR), &pfd)

    ReleaseDC(hDC, hWnd)
    return hWnd
}

func messageLoop()
{
    var msg: MSG
    while true
    {
        while PeekMessageA(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            if msg.message == WM_QUIT
                return
            TranslateMessage(&msg)
            DispatchMessageA(&msg)
        }

        display()
    }
}

#main
{
    hWnd := createWindow()

    hDC = GetDC(hWnd)
    hRC := wglCreateContext(hDC)
    wglMakeCurrent(hDC, hRC)

    assume loadImg()

    display()
    ShowWindow(hWnd, SW_SHOW)
    UpdateWindow(hWnd)

    messageLoop()

    wglMakeCurrent(null, null)
    wglDeleteContext(hRC)
    ReleaseDC(hWnd, hDC)
    DestroyWindow(hWnd)
}