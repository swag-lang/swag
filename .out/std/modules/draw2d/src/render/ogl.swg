#global namespace render
using core, win32, ogl, gdi32

#[swag.opaque]
public struct Ogl
{
    dd:  *DrawingDevice
    hDC: HDC
    hRC: HGLRC

    vertexbuffer: GLuint
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
        glInitExtensions()

        glGenBuffers(1, &vertexbuffer)
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

        const vb: [] GLfloat = @[
            -1.0, -1.0, 0.0,
            1.0, -1.0, 0.0,
            0.0,  1.0, 0.0,
        ]
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
        glBufferData(GL_ARRAY_BUFFER, @sizeof(vb), @dataof(vb), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0)
        //glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, null)
        glDrawArrays(GL_TRIANGLES, 0, 3) // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0)



        glFlush()
        assume gdi32.SwapBuffers(hDC)
    }
}