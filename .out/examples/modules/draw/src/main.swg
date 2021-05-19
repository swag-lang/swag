// http://www.schaik.com/pngsuite/

#global if #os == "windows"
using core, win32, ogl, gdi32, draw2d, mthx
using core.drawing

var hDC: HDC
const W = 1024
const H = 1024
var dev: DrawingDevice
var cdc: DrawingContext
var renderer: RenderOglWin32

var ff: f32 = 0
func display()
{
    cdc.begin()
    cdc.clear(Argb.White)
    cdc.drawSolidRect(0,0,cast(f32) W,cast(f32) H, Argb.White)
    cdc.drawSolidRect(0,0,50,50, Argb.Black)
    cdc.drawSolidTriangle(@{ff+10,10},@{ff+100,10},@{ff+100,100},Argb.Red,Argb.Green,Argb.Blue)
    cdc.end()

    //ff += 0.1

    renderer.draw(&cdc)
    renderer.flush()
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
    var keyb: keyboard.Keyboard

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
    }
}

//#[swag.printbc]
#main
{
    try
    {
        hWnd := createWindow()

        hDC = GetDC(hWnd)
        renderer.init(&dev, hDC, W, H)

        display()
        ShowWindow(hWnd, SW_SHOW)
        UpdateWindow(hWnd)

        messageLoop()

        renderer.drop()
        ReleaseDC(hWnd, hDC)
    }
}