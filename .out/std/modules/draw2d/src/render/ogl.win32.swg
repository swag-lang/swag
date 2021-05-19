#global if #os == "windows"
using core, win32, ogl, gdi32

public struct RenderOgl
{
    using base: BaseRenderOgl

    hDC: HDC
    hRC: HGLRC
}

impl RenderOgl
{
    public func init(using self, device: *DrawingDevice, hdc: HDC) throw
    {
        debug.assert(hDC == null)
        hDC = hdc
        hRC = try wglCreateContext(hdc)
        try wglMakeCurrent(hDC, hRC)
        base.init(device)
    }

    public func drop(using self)
    {
        catch wglMakeCurrent(null, null)
        catch wglDeleteContext(hRC)
    }

    public func flush(using self)
    {
        glFlush()
        assume gdi32.SwapBuffers(hDC)
    }
}