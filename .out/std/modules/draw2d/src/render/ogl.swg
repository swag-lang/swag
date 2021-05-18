#global namespace render
using core, win32, ogl, gdi32

#[swag.opaque]
public struct Ogl
{
    dd:     *DrawingDevice
    hDC:    HDC
    hRC:    HGLRC
}

impl Ogl
{
    public func init(using self, device: *DrawingDevice, hdc: HDC) throw
    {
        debug.assert(hDC == null)
        dd = device
        hDC = hdc
        hRC = try wglCreateContext(hdc)
        try wglMakeCurrent(hDC, hRC)
    }

    public func drop(using self)
    {
        catch wglMakeCurrent(null, null)
        catch wglDeleteContext(hRC)
    }

    public func flush(using self, dc: *DrawingContext)
    {
        glClearColor(1, 0, 1, 1)
        glClear(GL_COLOR_BUFFER_BIT)
        glFlush()
        assume gdi32.SwapBuffers(hDC)
    }
}