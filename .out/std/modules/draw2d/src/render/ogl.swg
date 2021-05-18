#global namespace render
using core, win32, ogl, gdi32, mthx

#[swag.opaque]
public struct Ogl
{
    dd:  *DrawingDevice
    hDC: HDC
    hRC: HGLRC

    vertexbuffer: GLuint
    programID:    GLuint
}

struct VertexLayout
{
    pos:    Vector3
    color:  Vector3
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

        createShaders()
        glGenBuffers(1, &vertexbuffer)
    }

    public func drop(using self)
    {
        catch wglMakeCurrent(null, null)
        catch wglDeleteContext(hRC)
    }

    public func flush(using self, dc: *DrawingContext)
    {
        glClearColor(0, 0, 0, 0)
        glClear(GL_COLOR_BUFFER_BIT)

        var arr:    Array'VertexLayout
        var v:      VertexLayout
        v.pos.x = -1; v.pos.y = -1; v.pos.z = 0; v.color.x = 0; v.color.y = 0; v.color.z = 1; arr.add(v)
        v.pos.x = 1; v.pos.y = -1; v.pos.z = 0;  v.color.x = 0; v.color.y = 1; v.color.z = 0; arr.add(v)
        v.pos.x = 0; v.pos.y = 1; v.pos.z = 0;   v.color.x = 1; v.color.y = 0; v.color.z = 0; arr.add(v)

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
        glBufferData(GL_ARRAY_BUFFER, arr.count * @sizeof(VertexLayout), arr.buffer, GL_DYNAMIC_DRAW);

        glUseProgram(programID)

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), null)
        glEnableVertexAttribArray(0)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), cast(const *void) cast(uint) @sizeof(Vector3))
        glEnableVertexAttribArray(1)

        glDrawArrays(GL_TRIANGLES, 0, 3) // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0)



        glFlush()
        assume gdi32.SwapBuffers(hDC)
    }

    const g_vertex_shader = @"
        #version 330 core
        layout(location = 0) in vec3 vertexPosition;
        layout(location = 1) in vec3 vertexColor;
        out vec3 vcolor;
        void main()
        {
            gl_Position.xyz = vertexPosition;
            gl_Position.w = 1.0;
            vcolor = vertexColor;
        }
    "@

    const g_pixel_shader = @"
        #version 330 core
        in vec3 vcolor;
        out vec3 color;
        void main()
        {
            color = vcolor;//vec3(1,0,0);
        }
    "@

    func createShaders(using self)
    {
        var result: GLint

        vertexShaderID := glCreateShader(GL_VERTEX_SHADER)
        vertexSourcePointer := @dataof(g_vertex_shader)
        glShaderSource(vertexShaderID, 1, &vertexSourcePointer, null)
        glCompileShader(vertexShaderID)

        fragmentShaderID := glCreateShader(GL_FRAGMENT_SHADER)
        pixelSourcePointer := @dataof(g_pixel_shader)
        glShaderSource(fragmentShaderID, 1, &pixelSourcePointer, null)
        glCompileShader(fragmentShaderID)

        programID = glCreateProgram()
        glAttachShader(programID, vertexShaderID)
        glAttachShader(programID, fragmentShaderID)
        glLinkProgram(programID)

        glDetachShader(programID, vertexShaderID)
        glDetachShader(programID, fragmentShaderID)
        glDeleteShader(vertexShaderID)
        glDeleteShader(fragmentShaderID)
    }
}