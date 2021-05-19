using core, ogl, mthx

public struct BaseRenderOgl
{
    dd:                 *DrawingDevice
    vertexbuffer:       GLuint
    vertexBufferCount:  uint
    programID:          GLuint
    orthoMatrix:        Matrix4x4
}

impl BaseRenderOgl
{
    public func init(using self, device: *DrawingDevice)
    {
        dd = device

        glInitExtensions()
        createShaders()

        glGenBuffers(1, &vertexbuffer)
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), null)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), cast(const *void) cast(uint) @sizeof(Vector3))
    }

    func ortho(b, t, l, r, n, f: f32, m: [4,4] f32)
    {
        m[0,0] = 2 / (r - l);
        m[0,1] = 0;
        m[0,2] = 0;
        m[0,3] = 0;

        m[1,0] = 0;
        m[1,1] = 2 / (t - b);
        m[1,2] = 0;
        m[1,3] = 0;

        m[2,0] = 0;
        m[2,1] = 0;
        m[2,2] = -2 / (f - n);
        m[2,3] = 0;

        m[3,0] = -(r + l) / (r - l);
        m[3,1] = -(t + b) / (t - b);
        m[3,2] = -(f + n) / (f - n);
        m[3,3] = 1;
    }

    public func draw(using self, dc: *DrawingContext)
    {
        // Rebind vertex buffer if vertex count has increased
        if vertexBufferCount < dc.vertexBuffer.count
        {
            vertexBufferCount = dc.vertexBuffer.count
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
            glBufferData(GL_ARRAY_BUFFER, vertexBufferCount * @sizeof(VertexLayout), dc.vertexBuffer.buffer, GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
        glUseProgram(programID)

        var matrixID: GLint = glGetUniformLocation(programID, @dataof("mvp"))
        ortho(1024, 0, 0, 1024, 0, 1000, orthoMatrix.m)
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &orthoMatrix.m[0, 0])

        glEnableVertexAttribArray(0)
        glEnableVertexAttribArray(1)

        visit cmd: dc.commandBuffer
        {
            switch cmd.id
            {
            case Clear:
                params := &cmd.params.clear
                glClearColor(params.color.x, params.color.y, params.color.z, 0)
                glClear(params.flags)

            case DrawTriangles:
                params := &cmd.params.drawTriangles
                glDrawArrays(GL_TRIANGLES, cast(GLint) params.start, cast(GLsizei) params.count)
            }
        }

        glDisableVertexAttribArray(0)
        glDisableVertexAttribArray(1)
    }

    const g_vertex_shader = @"
        #version 330 core
        layout(location = 0) in vec3 vertexPosition;
        layout(location = 1) in vec3 vertexColor;
        uniform mat4 mvp;
        out vec3 vcolor;
        void main()
        {
            gl_Position = mvp * vec4(vertexPosition, 1);
            vcolor = vertexColor;
        }
    "@

    const g_pixel_shader = @"
        #version 330 core
        in vec3 vcolor;
        out vec3 color;
        void main()
        {
            color = vcolor;
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