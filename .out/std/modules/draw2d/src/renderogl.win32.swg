#global if #os == "windows"
using core, win32, ogl, gdi32

public struct RenderOglWin32
{
    using base: RenderOgl

    hDC: HDC
    hRC: HGLRC
}

public impl RenderOglWin32
{
    func init(using self, device: *DrawingDevice, hdc: HDC, w, h: s32) throw
    {
        debug.assert(hDC == null)
        hDC = hdc
        hRC = try wglCreateContext(hdc)
        try wglMakeCurrent(hDC, hRC)
        base.init(device, w, h)
    }

    func drop(using self)
    {
        catch wglMakeCurrent(null, null)
        catch wglDeleteContext(hRC)
    }

    func flush(using self)
    {
        glFlush()
        assume SwapBuffers(hDC)
    }
}