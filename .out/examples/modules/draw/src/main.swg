// http://www.schaik.com/pngsuite/

#global if #os == "windows"
using Core, Win32, Ogl, Gdi32, Pixel
using Core.Drawing, Core.Math

const W = 1024
const H = 1024

var hDC:        HDC
var cdc:        Painter
var renderer:   RenderOglWin32
var image0:     Texture
var image1:     Texture
var typeface:   const *TypeFace
var fontSmall:  *Font
var font40:     *Font
var font80:     *Font
var fontFam:    FontFamily

var g_modeAA:       bool = true
var g_paintQuality: PaintQuality = Normal
var g_pattern:      bool = true
var g_page:         s32 = 3
const MaxPages = 5

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
    brush.color = Argb.Red
    cdc.fillPath(&path, brush)
    path.polygon.offset(-10)
    brush.color = Argb.Blue
    cdc.fillPath(&path, brush)
    cdc.end()

    renderer.resetTransform()
    renderer.translateTransform(625, 50)
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

    //////////////////////////////////////
    cdc.begin()
    cdc.paintParams |= g_modeAA ? .Antialiased : .Default
    cdc.paintQuality = g_paintQuality
    var lpl: LinePathList
    cdc.translateTransform(750, 550)
    p0 := lpl.newPath()
    p0.setRect(0, 0, 100, 100)
    p1 := lpl.newPath()
    p1.setCircle(50, 160, 50)
    cdc.fillPath(&lpl, Argb.Red)
    cdc.end()

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

    cdc.drawTexture(200, 0, image1)

    cdc.setInterpolationMode(InterpolationMode.Pixel)
    cdc.drawTexture(50, 0, 96, 96, image0)
    cdc.setInterpolationMode(InterpolationMode.Linear)
    cdc.drawTexture(50, 110, 96, 96, image0)

    cdc.rotateTransform(45, 100, 300)
    cdc.drawTexture(50, 250, 96, 96, image0)

    cdc.resetTransform()
    pen := Pen.createHatch(HatchStyle.SolidDiamond, Argb.Black, 20)
    cdc.drawRoundRect(400, 700, 200, 200, 10, 10, pen)

    cdc.drawTexture(50, 500, 150, 150, @{228, 103, 206, 167}, image1)
    cdc.drawTexture(50, 700, @{228, 103, 206, 167}, image1)

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

    {
        cdc.drawCircle(500, 500, 400, Argb.Red, 1)
    }

    {
        var lp1: LinePath
        lp1.start(800, 800)
        lp1.bezierTo(150, 150, 250, 50)

        cdc.startTransparency()
        cdc.drawPath(&lp1, Pen.createSolid(Color.fromArgb(0x51, Argb.Red), 50))
        cdc.stopTransparency()
    }

    {
        var lp: LinePath
        lp.start(50, 800)
        lp.bezierTo(800, 50, 250, 50)

        cdc.startTransparency()
        cdc.drawPath(&lp, Pen.createSolid(Color.fromArgb(0x51, Argb.Black), 50))
        cdc.stopTransparency()
    }

    cdc.end()
    renderer.resetTransform()
    renderer.draw(&cdc)
}

func displayPage4()
{
    cdc.begin()
    cdc.paintParams |= g_modeAA ? .Antialiased : .Default
    cdc.paintQuality = g_paintQuality

    {
        var format: StringFormat
        var rect:   Rectangle{350, 50, 100, 100}
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.lineGap = 1
        cdc.fillRect(rect.x, rect.y, rect.width, rect.height, Color.fromArgb(0x1F, Argb.Blue))
        cdc.drawString(rect, "on multiple lines\nthis is\na text\non multiple lines", fontSmall, Argb.Black, format)
    }

    {
        var format: StringFormat
        var rect:   Rectangle{500, 50, 100, 100}
        format.flags |= .WordWrap
        format.underline = .Underline
        format.lineGap = 1
        cdc.fillRect(rect.x, rect.y, rect.width, rect.height, Color.fromArgb(0x1F, Argb.Blue))
        cdc.drawString(rect, "on multiple lines this is a text on multiple lines", fontSmall, Argb.Black, format)
    }

    {
        cdc.drawString(100, 200, "on multiple lines this is a text on multiple lines", fontSmall, Argb.Black)
    }

    {
        fnt := fontFam.getFont(FontFamilyStyle.Regular)
        cdc.fillCircle(100, 100, 5, Argb.Red)
        cdc.drawString(100, 100, "abcdefghijkl", fnt, Argb.Black)
    }

    const str = "abcdefghijklmnopqrstuvwxyz"

    {
        //brush := Brush.createHatch(HatchStyle.SolidDiamond, Argb.Red)
        brush := Brush.createTexture(image1)
        rect := cdc.measureString(100, 300, str, font40)
        cdc.fillRect(rect.left, rect.top, rect.width, rect.height, Color.fromArgb(0x1F, Argb.Blue))
        cdc.fillRect(rect.left, rect.top, rect.width, rect.baseLine, Color.fromArgb(0x1F, Argb.Red))
        cdc.drawString(100, 300, str, font40, brush, UnderlineStyle.Strikeout)
        cdc.drawString(100, 350, str, font40, Argb.Black)
    }

    {
        var pathl: LinePathList
        cdc.translateTransform(50, 470)

        pen := Pen.createSolid(Argb.Red, 5)
        pen.joinStyle = .Round
        pen.plotOffset = 2

        assume typeface.getStringOutline(&pathl, str, 70)
        cdc.drawPath(&pathl, pen)
        cdc.fillPath(&pathl, Argb.Black)

        cdc.resetTransform()
    }

    {
        var p: LinePathList
        cdc.translateTransform(50, 570)
        assume typeface.getStringOutline(&p, "A B C D", 70)
        p.flatten(PaintQuality.Normal)
        p.offset(4)
        p.clean()
        cdc.drawPath(&p, Argb.Red, 2)

        cdc.resetTransform()
    }

    {
        var p: LinePathList
        cdc.translateTransform(50, 670)
        assume typeface.getStringOutline(&p, "A B C D", 70)
        p.flatten(PaintQuality.Normal)
        p.offset(5, JoinStyle.Round)
        cdc.fillPath(&p, Argb.Red)
        cdc.drawPath(&p, Argb.Black, 2)

        cdc.resetTransform()
    }

    {
        var p: LinePath
        cdc.translateTransform(500, 570)
        p.start(0, 0)
        p.lineTo(100, 0)
        p.lineTo(100, 100)
        p.lineTo(0, 100)
        p.close()
        p.flatten(PaintQuality.Normal)
        cdc.drawPath(&p, Argb.Red, 2)
        p.polygon.offset(10, JoinStyle.Miter)
        cdc.drawPath(&p, Argb.Green, 2)
        p.polygon.offset(10, JoinStyle.Bevel)
        cdc.drawPath(&p, Argb.Green, 2)
        p.polygon.offset(10, JoinStyle.Round)
        cdc.drawPath(&p, Argb.Green, 4)

        cdc.resetTransform()
    }

    // Draw font page
    //cdc.drawTexture(100, 400, font80.atlases[0].texture)

    cdc.end()
    renderer.resetTransform()
    renderer.draw(&cdc)
}

func display()
{
    renderer.begin()

    cdc.begin()
    cdc.clear(Color.fromArgb(0xFFEEEEEE))
    cdc.end()
    renderer.draw(&cdc)

    switch g_page
    {
    case 0: displayPage0()
    case 1: displayPage1()
    case 2: displayPage2()
    case 3: displayPage3()
    case 4: displayPage4()
    }

    renderer.end()
}

#[Swag.Callback]
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
    pfd.cDepthBits   = 32

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
        //Console.printf("max used: %\n", @getcontext().tempAllocator.maxUsed)
        Memory.freeTemp()

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
            g_page = (g_page + 1) % MaxPages
    }
}

#main
{
    try
    {
        hWnd := createWindow()

        hDC = GetDC(hWnd)

        renderer.init(hDC, W, H)

        img0 := Image.load(imagePath("basn0g01.png"))
        image0 = renderer.addImage(img0)
        img1 := Image.load(imagePath("rgba.bmp"))
        image1 = renderer.addImage(img1)

        typeface = TypeFace.create(imagePath("arial.ttf"))
        fontSmall = Font.create(typeface, cast(u32) ((12 * 96) / 72))
        font40 = Font.create(typeface, 40)
        font80 = Font.create(typeface, 80)

        fontFam.regular = Font.create(imagePath("arial.ttf"), 30)
        fontFam.bold = Font.create(imagePath("arialbd.ttf"), 30)
        fontFam.italic = Font.create(imagePath("ariali.ttf"), 30)
        fontFam.boldItalic = Font.create(imagePath("arialbi.ttf"), 30)

        display()
        ShowWindow(hWnd, SW_SHOW)
        UpdateWindow(hWnd)

        messageLoop()

        renderer.drop()
        ReleaseDC(hWnd, hDC)
    }
}