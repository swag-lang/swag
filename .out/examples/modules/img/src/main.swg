#global if #os == "windows"
using core, win32, ogl, gdi32, drawing

var hDC: HDC
var imageIdx: u32 = 11
var effectIdx: u32 = 0
var myImage: image.Image
const W = 1024
const H = 1024

func dataPath()->String
{
    dataPath := path.getDirectoryName(#location.fileName)
    dataPath = path.getDirectoryName(dataPath)
    dataPath = path.getDirectoryName(dataPath)
    dataPath = path.getDirectoryName(dataPath)
    dataPath = path.getDirectoryName(dataPath)
    return path.combine(dataPath, "std/tests/image/datas")
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
    assume SwapBuffers(hDC)
}

var textureID: GLuint
//#[swag.printbc]
func loadImg()
{
    using image.Image, Color

    imgName := "rgba.bmp"
    switch imageIdx
    {
    case 0: break
    case 1:     imgName = "rgb.bmp"
    case 2:     imgName = "rgb.tga"
    case 3:     imgName = "rgba.tga"
    case 4:     imgName = "rle24.tga"
    case 5:     imgName = "rle32.tga"
    case 6:     imgName = "h2v1.jpg"
    case 7:     imgName = "h2v2.jpg"
    case 8:     imgName = "h1v1.jpg"
    case 9:     imgName = "gray.jpg"
    case 10:    imgName = "progh1v1.jpg"
    case 11:    imgName = "rgb.png"
    default: imageIdx = 0
    }

    var ti: time.Stopwatch
    ti.start()
    data := imagePath(imgName)
    myImage = assume image.load(data)
    ti.stop()

    var te: time.Stopwatch
    te.start()

    switch effectIdx
    {
    case 1:  myImage.halfSize()
    case 2:  myImage.resize(1024, 1024)
    case 3:  myImage.resize(myImage.width / 2, myImage.height / 2)
    case 4:  myImage.flip()
    case 5:  myImage.mirror()
    case 6:  myImage.grayScale(0.5)
    case 7:  myImage.grayScale()
    case 8:  myImage.fillHsl(FillHslType.HueSaturation, Argb.Green)
    case 9:  myImage.fillGradient2(Argb.Green, Argb.Red)
    case 10: myImage.fillGradient4(Argb.Green, Argb.Red, Argb.Blue, Argb.Yellow)
    case 11: myImage.fill(Argb.Cyan)
    case 12: myImage.resize(1024, 1024, Image.ResizeMethod.Bilinear)
    case 13: myImage.resize(1024, 1024, Image.ResizeMethod.Bicubic)
    case 14: myImage.resize(1024, 1024, Image.ResizeMethod.Gaussian)
    case 15: myImage.resize(1024, 1024, Image.ResizeMethod.Hermite)
    case 16: myImage.resize(1024, 1024, Image.ResizeMethod.Hamming)
    case 17: myImage.resize(1024, 1024, Image.ResizeMethod.Catrom)
    case 18: myImage.resize(1024, 1024, Image.ResizeMethod.Quadratic)
    case 19: myImage.applyKernel(Image.Kernel.EdgeDetect3x3A)
    case 20: myImage.applyKernel(Image.Kernel.EdgeDetect3x3B)
    case 21: myImage.applyKernel(Image.Kernel.EdgeDetect3x3C)
    case 22: myImage.applyKernel(Image.Kernel.EdgeDetect3x3D)
    case 23: myImage.applyKernel(Image.Kernel.EdgeDetect3x3E)
    case 24: myImage.applyKernel(Image.Kernel.EdgeDetect3x3F)
    case 25: myImage.lightness(0.1)
    case 26: myImage.lightness(0.2)
    case 27: myImage.lightness(-0.4)
    case 28: myImage.contrast(0.1)
    case 29: myImage.contrast(0.2)
    case 30: myImage.contrast(-0.4)
    case 31: myImage.invert()
    case 32: myImage.crop(229, 105, 206, 166)
    case 33: myImage.setPixelFormat(PixelFormat.BGR8)
    case 34: myImage.setPixelFormat(PixelFormat.BGRA8)
    case 35: myImage.setChannel(ChannelMode.RGB, ChannelValueMode.MinRGB)
    case 36: myImage.setChannel(ChannelMode.RGB, ChannelValueMode.MaxRGB)
    case 37: myImage.setChannel(ChannelMode.RGB, ChannelValueMode.MeanRGB)
    case 38: myImage.gamma(0.5)
    case 39: myImage.gamma(1.0)
    case 40: myImage.gamma(2.2)
    case 41: myImage.colorize(0.5, 0.2, strength: 0.5)
    case 42: myImage.turn(TurnAngle.T90)
    case 43: myImage.turn(TurnAngle.T180)
    case 44: myImage.turn(TurnAngle.T270)
    default: effectIdx = 0
    }

    te.stop()

    console.printf("%: img: % (% x % %) time: %ms / %ms\n", effectIdx, imgName, myImage.width, myImage.height, myImage.pf, ti.elapsedMilliseconds(), te.elapsedMilliseconds())

    var fmt: GLenum = 0
    #[swag.complete]
    switch myImage.pf
    {
    case BGR8:  fmt = GL_BGR
    case BGRA8: fmt = GL_BGRA
    case RGB8:  fmt = GL_RGB
    case RGBA8: fmt = GL_RGBA
    }

    glGenTextures(1, &textureID)
    glBindTexture(GL_TEXTURE_2D, textureID)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
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
    wc.lpfnWndProc   = acast @mkcallback(&winProc)
    wc.hInstance     = GetModuleHandleA(null)
    wc.lpszClassName = @dataof("CLASS_NAME")
    assume RegisterClassA(wc)

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

    assume
    {
        pf := ChoosePixelFormat(hDC, &pfd)
        SetPixelFormat(hDC, pf, &pfd)
        DescribePixelFormat(hDC, pf, cast(UINT) @sizeof(PIXELFORMATDESCRIPTOR), &pfd)
    }

    ReleaseDC(hDC, hWnd)
    return hWnd
}

func messageLoop()
{
    var msg:  MSG
    var keyb: keyboard.Keyboard

    while true
    {
        @getcontext().tempAllocator.freeAll()
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
            loadImg()
        }
        if(keyb.isKeyJustPressed(keyboard.Key.F2))
        {
            effectIdx += 1
            loadImg()
        }

        display()
    }
}

//#[swag.printbc]
#main
{
    try
    {
        hWnd := createWindow()

        hDC = GetDC(hWnd)
        hRC := wglCreateContext(hDC)
        wglMakeCurrent(hDC, hRC)

        jobs.setNumWorkers()
        loadImg()

        display()
        ShowWindow(hWnd, SW_SHOW)
        UpdateWindow(hWnd)

        messageLoop()

        wglMakeCurrent(null, null)
        wglDeleteContext(hRC)
        ReleaseDC(hWnd, hDC)
    }
}