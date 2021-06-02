// http://www.schaik.com/pngsuite/

#global if #os == "windows"
using Core, Win32, Ogl, Gdi32, Pixel
using Core.Drawing, Core.Math

var hDC: HDC
const W = 1024
const H = 1024
var cdc: Painter
var renderer: RenderOglWin32

var image0, image1: Texture

var g_modeAA:       bool = true
var g_paintQuality: PaintQuality = Normal
var g_pattern:      bool = true
var g_page:         s32 = 3

func dataPath()->String
{
    dataPath := Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    dataPath = Path.getDirectoryName(dataPath)
    dataPath = Path.getDirectoryName(dataPath)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "std/tests/pixel/datas")
}

func imagePath(name: string)->String
{
    return Path.combine(dataPath(), name)
}

func displayPage0()
{
    pen := Pen.createSolid(Argb.Black, 20)

    if g_pattern
    {
        pen.dashStyle = .Dot
        //pen.pattern.add(1.0)
        //pen.pattern.add(1.0)
        //pen.pattern.add(2.0)
        //pen.pattern.add(1.0)
    }

    //////////////////////////////////////
    cdc.begin()
    cdc.paintParams |= g_modeAA ? .Antialiased : .Default
    cdc.paintQuality = g_paintQuality

    cdc.resetTransform()
    cdc.translateTransform(50, 50)
    pen.brush.color = Argb.Black
    pen.beginCapStyle = .None
    pen.endCapStyle = .None
    cdc.drawLine(0, 0, 0, 150, pen)

    pen.beginCapStyle = .Square
    pen.endCapStyle = .Square
    cdc.drawLine(100, 0, 150, 150, pen)

    pen.beginCapStyle = .Round
    pen.endCapStyle = .Round
    cdc.drawLine(200, 0, 250, 150, pen)

    pen.beginCapStyle = .ArrowAnchor
    pen.endCapStyle = .ArrowAnchor
    pen.dashBeginCapStyle = .Round
    pen.dashEndCapStyle = .Round
    cdc.drawLine(300, 0, 350, 150, pen)

    pen.beginCapStyle = .SquareAnchor
    pen.endCapStyle = .SquareAnchor
    pen.dashBeginCapStyle = .Triangle
    pen.dashEndCapStyle = .Triangle
    cdc.drawLine(400, 0, 450, 150, pen)

    pen.dashBeginCapStyle = .Round
    pen.dashEndCapStyle = .Round
    cdc.drawLine(500, 0, 550, 150, Argb.Brown, 2)

    cdc.resetTransform()
    cdc.translateTransform(50, 250)
    pen.brush.color = Argb.Blue
    pen.joinStyle = .None
    cdc.drawRect(0, 0, 100, 100, pen)
    pen.joinStyle = .Bevel
    cdc.drawRect(150, 0, 100, 100, pen)
    pen.joinStyle = .Round
    cdc.drawRect(300, 0, 100, 100, pen)
    pen.joinStyle = .Miter
    cdc.drawRect(450, 0, 100, 100, pen)

    pen.brush.color = Argb.Red
    var path: LinePath
    path.lineTo(100, 0)
    path.lineTo(100, 50)
    path.lineTo(200, 50)
    pen.joinStyle = .Miter

    cdc.resetTransform()
    cdc.translateTransform(50, 400)
    cdc.drawPath(&path, pen)
    pen.joinStyle = .Bevel

    cdc.resetTransform()
    cdc.translateTransform(250, 400)
    cdc.drawPath(&path, pen)
    pen.joinStyle = .Round

    cdc.resetTransform()
    cdc.translateTransform(450, 400)
    cdc.drawPath(&path, pen)

    pen.brush.color = Argb.Green
    path.start(0, 0)
    path.lineTo(100, 0)
    path.lineTo(100, 50)
    path.lineTo(0, 50)
    pen.beginCapStyle = .Round
    pen.endCapStyle = .Round

    cdc.resetTransform()
    cdc.translateTransform(50, 500)
    cdc.drawPath(&path, pen)

    path.start(0, 0)
    path.bezierTo(100, 0, 50, -50)

    cdc.resetTransform()
    cdc.translateTransform(250, 500)
    cdc.drawPath(&path, pen)

    path.start(0, 0)
    path.lineTo(50, 0)
    path.bezierTo(100, 50, 100, 0, 50, 50)
    path.lineTo(200, 50)

    cdc.resetTransform()
    cdc.translateTransform(400, 500)
    cdc.drawPath(&path, pen)

    path.start(0, 0)
    pen.brush.color = Argb.Purple
    path.arcTo(0, 50, 50, 50, 0, 90)
    path.lineTo(100, 50)
    path.arcTo(150, 50, 50, 50, -90, 0)

    cdc.resetTransform()
    cdc.translateTransform(50, 600)
    cdc.drawPath(&path, pen)

    cdc.resetTransform()
    cdc.translateTransform(250, 600)
    pen = Pen.createSolid(Argb.Blue, 10)
    cdc.drawRoundRect(0, 0, 100, 100, 20, 20, pen)

    cdc.resetTransform()
    cdc.translateTransform(400, 600)
    pen.size = 4
    cdc.drawCircle(50, 50, 50, pen)

    cdc.resetTransform()
    cdc.translateTransform(500, 600)
    cdc.drawEllipse(50, 50, 25, 50, pen)

    brush := Brush.createSolid(Argb.Black)
    cdc.resetTransform()
    cdc.translateTransform(600, 600)
    cdc.fillCircle(50, 50, 50, brush)
    cdc.end()
    renderer.draw(&cdc)

    //////////////////////////////////////
    cdc.begin()
    cdc.paintParams |= g_modeAA ? .Antialiased : .Default
    cdc.paintQuality = g_paintQuality
    path.start(0, 50)
    path.lineTo(50, 50)
    path.lineTo(50, 20)
    path.lineTo(100, 75)
    path.lineTo(50, 130)
    path.lineTo(50, 100)
    path.lineTo(0, 100)
    path.close()
    cdc.fillPath(&path, brush)
    cdc.end()

    renderer.resetTransform()
    renderer.rotateTransform(45)
    renderer.scaleTransform(0.5, 0.5)
    renderer.translateTransform(700, 50)
    renderer.draw(&cdc)

    //////////////////////////////////////
    cdc.begin()
    cdc.paintParams |= g_modeAA ? .Antialiased : .Default
    cdc.paintQuality = g_paintQuality
    brush = Brush.createTiled(image0)
    cdc.fillRect(750, 50, 100, 150, brush)
    brush = Brush.createTexture(image1)
    cdc.fillRoundRect(750, 250, 150, 150, 20, 20, brush)
    cdc.fillCircle(850, 450, 50, brush)
    cdc.end()

    renderer.resetTransform()
    renderer.draw(&cdc)
}

func displayPage1()
{
    var brush: Brush
    brush.color = Argb.Black

    cdc.begin()
    cdc.paintParams |= g_modeAA ? .Antialiased : .Default
    cdc.paintQuality = g_paintQuality
    cdc.translateTransform(50, 50)

    cpt, cptLines := 0
    visit e: HatchStyle
    {
        brush.type   = .Hatch
        brush.hatch  = e
        brush.uvMode = .Fill
        cdc.fillRect(0, 0, 48, 48, brush)
        brush.uvMode = .Tile
        cdc.fillRect(0, 75, 48, 48, brush)

        cdc.translateTransform(75, 0)
        cpt += 1
        if cpt == 12
        {
            cptLines += 1
            cpt = 0
            cdc.resetTransform()
            cdc.translateTransform(50, 50 + 150.0 * cptLines)
        }
    }

    cdc.end()

    renderer.resetTransform()
    renderer.draw(&cdc)
}

func displayPage2()
{
    cdc.begin()
    cdc.paintParams |= g_modeAA ? .Antialiased : .Default
    cdc.paintQuality = g_paintQuality

    cdc.drawTexture(image1, 200, 0)

    cdc.setInterpolationMode(InterpolationMode.Pixel)
    cdc.drawTexture(image0, 50, 0, 96, 96)
    cdc.setInterpolationMode(InterpolationMode.Linear)
    cdc.drawTexture(image0, 50, 110, 96, 96)

    cdc.rotateTransform(45, 100, 300)
    cdc.drawTexture(image0, 50, 250, 96, 96)

    cdc.resetTransform()
    pen := Pen.createHatch(HatchStyle.SolidDiamond, Argb.Black, 20)
    cdc.drawRoundRect(400, 700, 200, 200, 10, 10, pen)

    cdc.drawTexture(image1, 50, 500, 150, 150, @{228, 103, 206, 167})
    cdc.drawTexture(image1, 50, 700, @{228, 103, 206, 167})

    cdc.end()

    renderer.resetTransform()
    renderer.draw(&cdc)
}

func displayPage3()
{
    cdc.begin()
    cdc.paintParams |= g_modeAA ? .Antialiased : .Default
    cdc.paintQuality = g_paintQuality

    brush := Brush.createHatch(HatchStyle.SolidDiamond, Argb.Black)
    cdc.setClippingRect(0, 0, 100, 100)
    cdc.fillCircle(100, 100, 100, brush)
    cdc.drawCircle(100, 100, 100, Argb.Red, 4)
    cdc.resetClippingRect()

    cdc.startClippingRegion(ClippingMode.Set)
    cdc.fillRoundRect(150, 0, 100, 100, 20, 20, Argb.Green)
    cdc.setClippingRegionMode(ClippingMode.Clear)
    cdc.fillCircle(200, 50, 50, Argb.White)
    cdc.endClippingRegion()

    cdc.fillRect(0, 0, 1000, 1000, Argb.Red)
    cdc.resetClippingRegion()

    cdc.drawCircle(500, 500, 400, Argb.Red, 1)
    var lp: LinePath
    lp.start(50, 800)
    lp.bezierTo(800, 50, 250, 50)
    cdc.drawPath(&lp, Pen.createSolid(Argb.Black, 50))

    cdc.end()
    renderer.resetTransform()
    renderer.draw(&cdc)
}

func display()
{
    renderer.begin()

    cdc.begin()
    cdc.clear(Argb.Red)
    cdc.fillRect(0,0,cast(f32) W,cast(f32) H, Argb.White)
    cdc.end()
    renderer.draw(&cdc)

    switch g_page
    {
    case 0: displayPage0()
    case 1: displayPage1()
    case 2: displayPage2()
    case 3: displayPage3()
    }

    renderer.end()
}

#[Swag.callback]
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
        renderer.resize(LOWORD(lParam), HIWORD(lParam))
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
    pfd.cStencilBits = 8
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
    var keyb: Input.Keyboard

    loop
    {
        @getcontext().tempAllocator.freeAll()
        while PeekMessageA(&msg, null, 0, 0, PM_REMOVE) != 0
        {
            if msg.message == WM_QUIT
                return
            TranslateMessage(&msg)
            DispatchMessageA(&msg)
        }

        display()

        keyb.update()
        if(keyb.isKeyJustPressed(Input.Key.F1))
            g_modeAA = g_modeAA ? false : true
        if(keyb.isKeyJustPressed(Input.Key.F2))
            g_paintQuality = cast(PaintQuality) ((cast(u32) g_paintQuality + 1) % 2)
        if(keyb.isKeyJustPressed(Input.Key.F3))
            g_pattern = g_pattern ? false : true
        if(keyb.isKeyJustPressed(Input.Key.F4))
            g_page = (g_page + 1) % 4
    }
}

#main
{
    try
    {
        hWnd := createWindow()

        hDC = GetDC(hWnd)

        renderer.init(hDC, W, H)
        img0 := assume Image.load(imagePath("basn0g01.png"))
        image0 = renderer.addImage(img0)
        img1 := assume Image.load(imagePath("rgba.bmp"))
        image1 = renderer.addImage(img1)

        display()
        ShowWindow(hWnd, SW_SHOW)
        UpdateWindow(hWnd)

        messageLoop()

        renderer.drop()
        ReleaseDC(hWnd, hDC)
    }
}