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
    func init(using self, hdc: HDC, w, h: s32) throw
    {
        Debug.assert(hDC == null)
        hDC = hdc
        hRC = try wglCreateContext(hdc)
        try wglMakeCurrent(hDC, hRC)
        base.init(w, h)
    }

    func drop(using self)
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