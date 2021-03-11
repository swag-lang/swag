#global if #os == "windows"
using core, win32, ogl, gdi32, drawing

var hDC: HDC
var imageIdx: u32
var effectIdx: u32
var myImage: image.Image
const W = 1024
const H = 1024

func dataPath()->String
{
    dataPath := path.getDirectoryName(#location.fileName)
    dataPath = path.getDirectoryName(dataPath)
    dataPath = path.getDirectoryName(dataPath)
    dataPath = path.getDirectoryName(dataPath)
    return path.combine(dataPath, "tests/image/datas")
}

func imagePath(name: string)->String
{
    return path.combine(dataPath(), name)
}

func display()
{
    glClearColor(0, 0, 0, 0)
    glClear(GL_COLOR_BUFFER_BIT)
    glEnable(GL_TEXTURE_2D)

    glBindTexture(GL_TEXTURE_2D, textureID)
    glBegin(GL_QUADS)

    w := 2 * myImage.width / cast(f32) W
    h := 2 * myImage.height / cast(f32) H
    glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0, 0.0); glVertex2f(-1.0 + w, -1.0);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0, 1.0); glVertex2f(-1.0 + w, -1.0 + h);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, -1.0 + h);  // Top Left Of The Texture and Quad
    glEnd()

    glFlush()
    SwapBuffers(hDC)
}

var textureID: GLuint
func loadImg() throw
{
    using image.Image, Color, Argb

    imgName := "bmp_24.bmp"
    switch imageIdx
    {
    case 0: break
    case 1: imgName = "bmp_32.bmp"
    default: imageIdx = 0
    }

    data := imagePath(imgName)
    myImage = try image.load(data)

    var t: time.Stopwatch
    t.start()

    switch effectIdx
    {
    case 0: break
    case 4: myImage.resize(128, 128)
    case 2: myImage.resize(1024, 1024)
    case 3: myImage.flip()
    case 1: myImage.mirror()
    case 5: myImage.grayScale(0.5)
    case 6: myImage.grayScale()
    case 7: myImage.fillHsl(FillHslType.HueSaturation, fromArgb(Green))
    case 8: myImage.fillGradient2(fromArgb(Green), fromArgb(Red))
    case 9: myImage.fillGradient4(fromArgb(Green), fromArgb(Red), fromArgb(Blue), fromArgb(Yellow))
    case 10: myImage.fill(fromArgb(Cyan))
    default: effectIdx = 0
    }

    t.stop()
    elapsed := t.elapsedMilliseconds()
    console.println("img: % (% x % %) time: %ms", imgName, myImage.width, myImage.height, myImage.pf, elapsed)

    fmt := GL_BGR
    if myImage.bpp == 32 fmt = GL_BGRA

    glGenTextures(1, &textureID)
    glBindTexture(GL_TEXTURE_2D, textureID)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cast(s32) myImage.width, cast(s32) myImage.height, 0, fmt, GL_UNSIGNED_BYTE, myImage.pixels)
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
        W,
        H,
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
    var msg:  MSG
    var keyb: keyboard.Keyboard

    while true
    {
        while PeekMessageA(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            if msg.message == WM_QUIT
                return
            TranslateMessage(&msg)
            DispatchMessageA(&msg)
        }

        keyb.update()
        if(keyb.isKeyJustPressed(keyboard.Key.F1))
        {
            imageIdx += 1
            assume loadImg()
        }
        if(keyb.isKeyJustPressed(keyboard.Key.F2))
        {
            effectIdx += 1
            assume loadImg()
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

    try jobs.setNumWorkers(8)
    try loadImg()

    display()
    ShowWindow(hWnd, SW_SHOW)
    UpdateWindow(hWnd)

    messageLoop()

    wglMakeCurrent(null, null)
    wglDeleteContext(hRC)
    ReleaseDC(hWnd, hDC)
    DestroyWindow(hWnd)
}