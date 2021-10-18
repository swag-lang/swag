#global if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32

public struct RenderOglWin32
{
    using base: RenderOgl
}

public struct RenderingContext
{
    hDC:    HDC
    hRC:    HGLRC
    width:  s32
    height: s32
}

public impl RenderOglWin32
{
    // Initialize the renderer
    func createContext(using self, hWnd: HWND)->RenderingContext throw
    {
        hdc := GetDC(hWnd)
        defer ReleaseDC(hWnd, hdc)
        return try createContext(hdc)
    }

    // Initialize the renderer
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
        try wglMakeCurrent(result.hDC, result.hRC)
        base.init()

        return result
    }

    mtd dropContext(rc: RenderingContext)
    {
        catch wglMakeCurrent(null, null)
        catch wglDeleteContext(rc.hRC)
    }

    mtd drop()
    {
        dropContext(base.mainRC)
    }

    // To be called before rendering
    func begin(using self, rc: RenderingContext)
    {
        base.curRC = rc
        catch wglMakeCurrent(base.curRC.hDC, base.curRC.hRC)
        base.start()
    }

    // To be called after rendering
    func end(using self)
    {
        glFlush()
        glFinish()
        assume SwapBuffers(base.curRC.hDC)
    }
}