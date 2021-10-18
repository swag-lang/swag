#global if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32

public struct NativeRenderOgl
{
}

public struct RenderingContext
{
    hDC:    HDC
    hRC:    HGLRC
    width:  s32
    height: s32
}

public impl NativeRenderOgl
{
    func createContext(using self, hWnd: HWND)->RenderingContext throw
    {
        hdc := GetDC(hWnd)
        defer ReleaseDC(hWnd, hdc)
        return try createContext(hdc)
    }

    func createContext(using self, hdc: HDC)->RenderingContext throw
    {
        var result: retval
        result.hDC = hdc

        var pfd: PIXELFORMATDESCRIPTOR
        pfd.nSize        = @sizeof(pfd)
        pfd.nVersion     = 1;
        pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
        pfd.iPixelType   = PFD_TYPE_RGBA
        pfd.cStencilBits = 8
        pfd.cColorBits   = 24
        pfd.cAlphaBits   = 8
        pf := try ChoosePixelFormat(hdc, &pfd)
        try SetPixelFormat(hdc, pf, &pfd)

        result.hRC = try wglCreateContext(hdc)
        return result
    }

    mtd dropContext(rc: RenderingContext)
    {
        catch wglMakeCurrent(null, null)
        catch wglDeleteContext(rc.hRC)
    }

    func setCurrentContext(using self, rc: RenderingContext)
    {
        catch wglMakeCurrent(rc.hDC, rc.hRC)
    }

    func swagBuffers(using self, rc: RenderingContext)
    {
        assume SwapBuffers(rc.hDC)
    }
}