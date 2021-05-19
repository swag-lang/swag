using core, ogl, mthx

public struct RenderOgl
{
    dd:                 *DrawingDevice
    vertexbuffer:       GLuint
    programID:          GLuint
    orthoMatrix:        Matrix4x4
    width, height:      s32
}

impl RenderOgl
{
    func init(using self, device: *DrawingDevice, w, h: s32)
    {
        dd = device
        width = w
        height = h

        glInitExtensions()
        createShaders()

        glGenBuffers(1, &vertexbuffer)
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), null)
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, @sizeof(VertexLayout), cast(const *void) cast(uint) @sizeof(Vector2))
    }

    public func resize(using self, width, height: s32)
    {
        self.width = #scopefct.width
        self.height = #scopefct.height
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
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
        glBufferData(GL_ARRAY_BUFFER, dc.vertexBuffer.count * @sizeof(VertexLayout), dc.vertexBuffer.buffer, GL_STATIC_DRAW);

        glUseProgram(programID)

        var matrixID: GLint = glGetUniformLocation(programID, @dataof("mvp"))
        ortho(cast(f32) height, 0, 0, cast(f32) width, 0, 1.0, orthoMatrix.m)
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &orthoMatrix.m[0, 0])
	    glViewport(0, 0, width, height)

        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glEnableVertexAttribArray(0)
        glEnableVertexAttribArray(1)

        visit cmd: dc.commandBuffer
        {
            switch cmd.id
            {
            case Clear:
                params := &cmd.params.clear
                (r, g, b) := params.color.toRgbf()
                glClearColor(r, g, b, 0)
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
        layout(location = 0) in vec2 vertexPosition;
        layout(location = 1) in vec4 vertexColor;
        uniform mat4 mvp;
        out vec4 vcolor;
        void main()
        {
            gl_Position = mvp * vec4(vertexPosition, 0, 1);
            vcolor = vertexColor;
        }
    "@

    const g_pixel_shader = @"
        #version 330 core
        in vec4 vcolor;
        out vec4 color;
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