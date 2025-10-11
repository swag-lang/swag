/*
    :LICENCE
    This code uses some PNGs from the PngSuite (http://www.schaik.com/pngsuite/)
    See LICENCE.md for the corresponding licence.
*/

#global #if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32, Pixel

var g_HDC:       HDC
var g_ImageIdx:  u32 = 7
var g_EffectIdx: u32 = 49
var g_MyImage:   Pixel.Image
var g_TextureID: GLuint

const MAX_IMAGES  = 31
const MAX_EFFECTS = 50

const W = 1024
const H = 1024

func dataPath()->String
{
    var dataPath = Path.getDirectoryName(#curlocation.fileName)
    dataPath = assume Path.getDirectoryName(dataPath)
    dataPath = assume Path.getDirectoryName(dataPath)
    dataPath = assume Path.getDirectoryName(dataPath)
    dataPath = assume Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "std/tests/pixel/datas")
}

func imagePath(name: string)->String
{
    return Path.combine(dataPath(), name)
}

func display()
{
    glClearColor(1, 0, 1, 1)
    glClear(GL_COLOR_BUFFER_BIT)
    glEnable(GL_TEXTURE_2D)

    glBindTexture(GL_TEXTURE_2D, g_TextureID)
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    glBegin(GL_QUADS)
    let w = 2 * g_MyImage.width / cast(f32) W
    let h = 2 * g_MyImage.height / cast(f32) H
    glTexCoord2f(0.0, 0.0)
    glVertex2f(-1.0, -1.0)
    glTexCoord2f(1.0, 0.0)
    glVertex2f(-1.0 + w, -1.0)
    glTexCoord2f(1.0, 1.0)
    glVertex2f(-1.0 + w, -1.0 + h)
    glTexCoord2f(0.0, 1.0)
    glVertex2f(-1.0, -1.0 + h)
    glEnd()

    glFlush()
    assume SwapBuffers(g_HDC)
}

func loadImg()
{
    using Pixel, Color

    /*{
        var data = imagePath("h2v1.jpg")

        g_MyImage = assume Image.load(data)
        var md: ImageMetaData
        md.tag = "toto"
        md.value = "titi"
        g_MyImage.metaDatas.clear()
        g_MyImage.metaDatas.add(md)
        catch g_MyImage.save(data)

        var opt: DecodeOptions
        opt.decodeMetaDatas = true
        g_MyImage = assume Image.load(data, opt)
    }*/

    var imgName = "rgba.bmp"
    switch g_ImageIdx
    {
    case 0:
        break
    case 1:
        imgName = "rgb.bmp"
    case 2:
        imgName = "rgb.tga"
    case 3:
        imgName = "rgba.tga"
    case 4:
        imgName = "rle24.tga"
    case 5:
        imgName = "rle32.tga"
    case 6:
        imgName = "h2v1.jpg"
    case 7:
        imgName = "h2v2.jpg"
    case 8:
        imgName = "h1v1.jpg"
    case 9:
        imgName = "gray.jpg"
    case 10:
        imgName = "progh1v1.jpg"
    case 11:
        imgName = "rgb.png"
    case 12:
        imgName = "rgba.png"
    case 13:
        imgName = "grayscale.png"
    case 14:
        imgName = "basn4a08.png" // grayscale 8 alpha
    case 15:
        imgName = "tbrn2c08.png" // bkgnd transparent
    case 16:
        imgName = "basn0g01.png" // grayscale 1 bit
    case 17:
        imgName = "basn0g02.png" // grayscale 2 bit
    case 18:
        imgName = "basn0g04.png" // grayscale 4 bit
    case 19:
        imgName = "tbbn0g04.png" // grayscale 4 bit + transparency
    case 20:
        imgName = "basi2c08.png" // color interlaced
    case 21:
        imgName = "basi0g01.png" // grayscale interlaced 1 bit
    case 22:
        imgName = "basi0g02.png" // grayscale interlaced 2 bits
    case 23:
        imgName = "basi0g04.png" // grayscale interlaced 4 bits
    case 24:
        imgName = "basi0g08.png" // grayscale interlaced 8 bits
    case 25:
        imgName = "basi4a08.png" // grayscale interlaced alpha
    case 26:
        imgName = "basi6a08.png" // color interlaced alpha
    case 27:
        imgName = "testa.gif"
    case 28:
        imgName = "test.gif"
    case 29:
        imgName = "paletted.png"
    case 30:
        imgName = "gameover.png"
    default:
        g_ImageIdx = 0
    }

    // Load image
    var ti: Time.Stopwatch
    ti.start()
    let data = imagePath(imgName)
    g_MyImage = assume Image.load(data)
    ti.stop()
    //Console.printf("%: img: % (% x % %) load: %ms ", g_EffectIdx, imgName, g_MyImage.width, g_MyImage.height, g_MyImage.pf, ti.elapsedMilliseconds())
    g_MyImage.resize(512, 512)

    // Apply a filter
    var te: Time.Stopwatch
    te.start()
    switch g_EffectIdx
    {
    case 1:
        g_MyImage.halfSize()
    case 2:
        g_MyImage.resize(1024, 1024)
    case 3:
        g_MyImage.resize(g_MyImage.width / 2, g_MyImage.height / 2)
    case 4:
        g_MyImage.flip()
    case 5:
        g_MyImage.mirror()
    case 6:
        g_MyImage.grayScale(0.5)
    case 7:
        g_MyImage.grayScale()
    case 8:
        g_MyImage.fillHsl(.SaturationLightness, Argb.Green)
    case 9:
        g_MyImage.fillGradient2(Argb.Green, Argb.Red)
    case 10:
        g_MyImage.fillGradient4(Argb.Green, Argb.Red, Argb.Blue, Argb.Yellow)
    case 11:
        g_MyImage.fill(Argb.Cyan)
    case 12:
        g_MyImage.resize(1024, 1024, .Bilinear)
    case 13:
        g_MyImage.resize(1024, 1024, .Bicubic)
    case 14:
        g_MyImage.resize(1024, 1024, .Gaussian)
    case 15:
        g_MyImage.resize(1024, 1024, .Hermite)
    case 16:
        g_MyImage.resize(1024, 1024, .Hamming)
    case 17:
        g_MyImage.resize(1024, 1024, .Catrom)
    case 18:
        g_MyImage.resize(1024, 1024, .Quadratic)
    case 19:
        g_MyImage.applyKernel(.EdgeDetect3x3A)
    case 20:
        g_MyImage.applyKernel(.EdgeDetect3x3B)
    case 21:
        g_MyImage.applyKernel(.EdgeDetect3x3C)
    case 22:
        g_MyImage.applyKernel(.EdgeDetect3x3D)
    case 23:
        g_MyImage.applyKernel(.EdgeDetect3x3E)
    case 24:
        g_MyImage.applyKernel(.EdgeDetect3x3F)
    case 25:
        g_MyImage.lightness(0.1)
    case 26:
        g_MyImage.lightness(0.2)
    case 27:
        g_MyImage.lightness(-0.4)
    case 28:
        g_MyImage.contrast(0.1)
    case 29:
        g_MyImage.contrast(0.2)
    case 30:
        g_MyImage.contrast(-0.4)
    case 31:
        g_MyImage.invert()
    case 32:
        g_MyImage.crop(229, 105, 206, 166)
    case 33:
        g_MyImage.setPixelFormat(.BGR8)
    case 34:
        g_MyImage.setPixelFormat(.BGRA8)
    case 35:
        g_MyImage.setChannel(.RGB, .MinRGB)
    case 36:
        g_MyImage.setChannel(.RGB, .MaxRGB)
    case 37:
        g_MyImage.setChannel(.RGB, .MeanRGB)
    case 38:
        g_MyImage.gamma(0.5)
    case 39:
        g_MyImage.gamma(1.0)
    case 40:
        g_MyImage.gamma(2.2)
    case 41:
        g_MyImage.colorize(0.5, 0.2, strength: 0.5)
    case 42:
        g_MyImage.turn(.T90)
    case 43:
        g_MyImage.turn(.T180)
    case 44:
        g_MyImage.turn(.T270)
    case 45:
        g_MyImage.setPixelFormat(.RGB8)
    case 46:
        g_MyImage.setPixelFormat(.RGBA8)
    case 47:
        g_MyImage.setPixelFormat(.BGR8)
    case 48:
        g_MyImage.setPixelFormat(.BGRA8)
    case 49:
        g_MyImage.expand(-50, 10, 10, 10)

    case 50:
        var img1 = assume Image.load(imagePath("testa.gif"))
        img1.setPixelFormat(.RGBA8)
        g_MyImage.setPixelFormat(.RGBA8)
        g_MyImage.mix(img1, 0, 0, .AlphaBlend)

    default:
        g_EffectIdx = 0
    }

    te.stop()
    //Console.printf("filter: % %ms\n", g_EffectIdx, te.elapsedMilliseconds())

    var fmt:      GLenum = 0
    var finternal = GL_RGB

    #[Swag.Complete]
    switch g_MyImage.pf
    {
    case BGR8:
        fmt = GL_BGR
    case BGRA8:
        fmt       = GL_BGRA
        finternal = GL_RGBA
    case RGB8:
        fmt = GL_RGB
    case RGBA8:
        fmt       = GL_RGBA
        finternal = GL_RGBA
    }

    glGenTextures(1, &g_TextureID)
    glBindTexture(GL_TEXTURE_2D, g_TextureID)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
    glTexImage2D(GL_TEXTURE_2D, 0, finternal, cast(s32) g_MyImage.width, cast(s32) g_MyImage.height, 0, fmt, GL_UNSIGNED_BYTE, g_MyImage.pixels.buffer)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
}

func winProc(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT
{
    switch msg
    {
    case WM_PAINT:
        display()

    case WM_SIZE:
        glViewport(0, 0, LOWORD(lParam), HIWORD(lParam))
        display()
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

    let hWnd = CreateWindowExA(0, @dataof("CLASS_NAME"), @dataof("Example"), WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 100, 100, W, H, null, null, wc.hInstance, null)

    g_HDC = GetDC(hWnd)

    var pfd: PIXELFORMATDESCRIPTOR
    pfd.nSize      = #sizeof(pfd)
    pfd.nVersion   = 1
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
    pfd.iPixelType = PFD_TYPE_RGBA
    pfd.cColorBits = 32

    assume
    {
        let pf = ChoosePixelFormat(g_HDC, &pfd)
        SetPixelFormat(g_HDC, pf, &pfd)
        DescribePixelFormat(g_HDC, pf, cast(UINT) #sizeof(PIXELFORMATDESCRIPTOR), &pfd)
    }

    ReleaseDC(hWnd, g_HDC)
    return hWnd
}

func bigLoop()
{
    if Env.hasArg("swag.test")
    {
        for i in MAX_IMAGES
        {
            g_ImageIdx = i
            loadImg()
            display()
        }

        for j in MAX_EFFECTS
        {
            g_ImageIdx  = 0
            g_EffectIdx = j
            loadImg()
            display()
        }

        return
    }

    var keyb: Input.Keyboard
    for
    {
        var msg: MSG
        @getcontext().tempAllocator.freeAll()
        while PeekMessageA(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            if msg.message == WM_QUIT do
                return
            TranslateMessage(&msg)
            DispatchMessageA(&msg)
        }

        keyb.update()
        if keyb.isKeyJustPressed(Input.Key.F1)
        {
            g_ImageIdx += 1
            loadImg()
        }
        if keyb.isKeyJustPressed(Input.Key.F2)
        {
            g_EffectIdx += 1
            loadImg()
        }

        display()
    }
}

#main
{
    try
    {
        let hWnd = createWindow()

        g_HDC = GetDC(hWnd)
        let hRC = wglCreateContext(g_HDC)
        wglMakeCurrent(g_HDC, hRC)

        Jobs.setNumWorkers()
        loadImg()

        display()
        ShowWindow(hWnd, SW_SHOW)
        UpdateWindow(hWnd)

        bigLoop()

        wglMakeCurrent(null, null)
        wglDeleteContext(hRC)
        ReleaseDC(hWnd, g_HDC)
    }
}
