#global if #os == "windows"
using Core, Win32, Ogl, Gdi32

public struct RenderOglWin32
{
    using base: RenderOgl
    hDC:        HDC
    hRC:        HGLRC
}

public impl RenderOglWin32
{
    // Initialize the renderer
    func init(using self, hWnd: HWND, w, h: s32) throw
    {
        hdc := GetDC(hWnd)
        try init(hdc, w, h)
        ReleaseDC(hWnd, hdc)
    }

    // Initialize the renderer
    func init(using self, hdc: HDC, w, h: s32) throw
    {
        Debug.assert(hDC == null)
        hDC = hdc

        var pfd: PIXELFORMATDESCRIPTOR
        pfd.nSize        = @sizeof(pfd)
        pfd.nVersion     = 1;
        pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
        pfd.iPixelType   = PFD_TYPE_RGBA
        pfd.cStencilBits = 8
        pfd.cColorBits   = 24
        pfd.cAlphaBits   = 8
        pf := try ChoosePixelFormat(hDC, &pfd)
        try SetPixelFormat(hDC, pf, &pfd)

        hRC = try wglCreateContext(hdc)
        try wglMakeCurrent(hDC, hRC)
        base.init(w, h)
    }

    method drop()
    {
        catch wglMakeCurrent(null, null)
        catch wglDeleteContext(hRC)
    }

    // To be called before rendering
    func begin(using self)
    {
        catch wglMakeCurrent(hDC, hRC)
        base.start()
    }

    // To be called after rendering
    func end(using self)
    {
        glFlush()
        glFinish()
        assume SwapBuffers(hDC)
    }
}