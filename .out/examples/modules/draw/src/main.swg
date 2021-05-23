// http://www.schaik.com/pngsuite/

#global if #os == "windows"
using Core, Win32, Ogl, Gdi32, Pixel
using Core.Drawing, Core.Math

var hDC: HDC
const W = 1024
const H = 1024
var cdc: Painter
var renderer: RenderOglWin32

var g_modeAA: bool
var g_paintQuality: PaintQuality = Normal
var g_pattern: bool

var ff: f32 = 0
func display()
{
    var pen: Pen

    if g_pattern
    {
        pen.penStyle = .Pattern
        pen.pattern.add(0.5)
        pen.pattern.add(1)
        pen.pattern.add(0.5)
    }

    pen.size = 20
    pen.color = 0xFF000000

    cdc.begin()
    cdc.clear(Argb.Red)
    cdc.paintParams |= g_modeAA ? .Antialiased : .Default
    cdc.paintQuality = g_paintQuality
    cdc.fillRect(0,0,cast(f32) W,cast(f32) H, Argb.White)

    cdc.position(@{50, 50})
    pen.color = Argb.Black
    pen.beginCapStyle = .None
    pen.endCapStyle = .None
    cdc.drawLine(@{0,0},@{50,150}, pen)
    pen.beginCapStyle = .Square
    pen.endCapStyle = .Square
    cdc.drawLine(@{100,0},@{150,150}, pen)
    pen.beginCapStyle = .Round
    pen.endCapStyle = .Round
    cdc.drawLine(@{200,0},@{250,150}, pen)
    pen.beginCapStyle = .Triangle
    pen.endCapStyle = .Triangle
    cdc.drawLine(@{300,0},@{350,150}, pen)
    cdc.drawLine(400, 0, 450, 150, Argb.Brown, 2)

    cdc.position(@{50, 250})
    pen.color = Argb.Blue
    pen.jointStyle = .None
    cdc.drawRect(0, 0, 100, 100, pen)
    pen.jointStyle = .Bevel
    cdc.drawRect(150, 0, 100, 100, pen)
    pen.jointStyle = .Round
    cdc.drawRect(300, 0, 100, 100, pen)
    pen.jointStyle = .Miter
    cdc.drawRect(450, 0, 100, 100, pen)

    pen.color = Argb.Red
    var path: LinePath
    path.lineTo(@{100, 0})
    path.lineTo(@{100, 50})
    path.lineTo(@{200, 50})
    pen.jointStyle = .Miter
    cdc.position(@{50, 400})
    cdc.drawPath(path, pen)
    pen.jointStyle = .Bevel
    cdc.position(@{250, 400})
    cdc.drawPath(path, pen)
    pen.jointStyle = .Round
    cdc.position(@{450, 400})
    cdc.drawPath(path, pen)

    pen.color = Argb.Green
    path.clear()
    path.lineTo(@{100, 0})
    path.moveTo(@{0, 50})
    path.lineTo(@{100, 50})
    pen.beginCapStyle = .Round
    pen.endCapStyle = .Round
    cdc.position(@{50, 500})
    cdc.drawPath(path, pen)

    path.clear()
    path.bezierTo(@{100, 0}, @{50, -50})
    cdc.position(@{250, 500})
    cdc.drawPath(path, pen)

    path.clear()
    path.lineTo(@{50, 0})
    path.bezierTo(@{100, 50}, @{100, 0}, @{50, 50})
    path.lineTo(@{200, 50})
    cdc.position(@{400, 500})
    cdc.drawPath(path, pen)

    path.clear()
    pen.color = Argb.Purple
    path.arcTo(@{0, 50}, 50, 50, 0, 90)
    path.moveTo(@{100, 50})
    path.arcTo(@{150, 50}, 50, 50, -90, 0)
    cdc.position(@{50, 600})
    cdc.drawPath(path, pen)

    cdc.position(@{250, 600})
    pen.size = 4
    pen.color = Argb.Purple
    cdc.drawRoundRect(0, 0, 100, 100, 20, 20, pen)
    cdc.position(@{400, 600})
    cdc.drawCircle(50, 50, 50, pen)
    cdc.position(@{500, 600})
    cdc.drawEllipse(50, 50, 25, 50, pen)

    var brush: Brush
    brush.color = Argb.Black
    cdc.position(@{600, 600})
    cdc.fillCircle(50, 50, 50, brush)
    cdc.drawCircle(50, 50, 50, pen)

    cdc.end()

    //ff += 0.1

    renderer.draw(&cdc)
    renderer.flush()
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
            g_paintQuality = cast(PaintQuality) ((cast(u32) g_paintQuality + 1) % 4)
        if(keyb.isKeyJustPressed(Input.Key.F3))
            g_pattern = g_pattern ? false : true
    }
}

//#[Swag.printbc]
#main
{
    try
    {
        hWnd := createWindow()

        hDC = GetDC(hWnd)
        renderer.init(hDC, W, H)

        display()
        ShowWindow(hWnd, SW_SHOW)
        UpdateWindow(hWnd)

        messageLoop()

        renderer.drop()
        ReleaseDC(hWnd, hDC)
    }
}