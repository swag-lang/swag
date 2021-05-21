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

var ff: f32 = 0
func display()
{
    var pen: Pen
    pen.color = 0xFF000000
    pen.size = 50

    cdc.begin()
    cdc.clear(Argb.Red)
    cdc.paintParams |= g_modeAA ? .Antialiased : .Default
    cdc.paintQuality = g_paintQuality
    cdc.drawSolidRect(0,0,cast(f32) W,cast(f32) H, Argb.White)
    cdc.drawSolidRect(0,0,50,50, Argb.Red)
    //cdc.drawSolidTriangle(@{ff+10,10},@{ff+500,10},@{ff+1000,500},Argb.Red,Argb.Green,Argb.Blue)
    cdc.drawSolidLine(@{50,50},@{950,500}, pen)
    cdc.drawEmptyRect(50, 50, 500, 500, pen)
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