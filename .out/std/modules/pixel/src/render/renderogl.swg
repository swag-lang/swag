using Core, Ogl, Core.Math

public struct RenderOgl
{
    vertexbuffer:       GLuint
    programID:          GLuint
    orthoMatrix:        Matrix4x4
    width, height:      s32
}

impl RenderOgl
{
    func init(using self, w, h: s32)
    {
        width = w
        height = h

        glInitExtensions()
        createShaders()

        glGenBuffers(1, &vertexbuffer)
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)

        var offset: const *void = null
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector2)
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, @sizeof(VertexLayout), offset)
        offset += @sizeof(u32)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
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

    public func draw(using self, dc: *Painter)
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

    const VertexShader = @"
        #version 330 core
        layout(location = 0) in vec2 vertexPosition;
        layout(location = 1) in vec4 vertexColor;
        layout(location = 2) in vec4 aa0;
        layout(location = 3) in vec4 aa1;
        layout(location = 4) in vec4 aa2;
        layout(location = 5) in vec4 aa3;
        layout(location = 6) in vec4 aa4;
        layout(location = 7) in vec4 aa5;

        uniform mat4    mvp;

        out vec4        vcolor;
        out vec2        vpixelPos;
        out vec4        vaa0;
        out vec4        vaa1;
        out vec4        vaa2;
        out vec4        vaa3;
        out vec4        vaa4;
        out vec4        vaa5;

        void main()
        {
            gl_Position = mvp * vec4(vertexPosition, 0, 1);
            vpixelPos = vertexPosition.xy;
            vcolor = vertexColor;
            vaa0 = aa0;
            vaa1 = aa1;
            vaa2 = aa2;
            vaa3 = aa3;
            vaa4 = aa4;
            vaa5 = aa5;
        }
    "@

    const PixelShader = @"
        #version 330 core
        in vec4     vcolor;
        in vec2     vpixelPos;
        in vec4     vaa0;
        in vec4     vaa1;
        in vec4     vaa2;
        in vec4     vaa3;
        in vec4     vaa4;
        in vec4     vaa5;

        out vec4    color;

        float computeAlphaAA(vec4 segment)
        {
            float norm;
            vec2 es = segment.zw - segment.xy;
            vec2 es1 = es;
            if (es.x != 0 || es.y != 0)
            {
                norm = sqrt(es.x * es.x + es.y * es.y);
                es1.xy /= norm;
                vec2  ptmp  = vpixelPos - segment.xy;
                float dot   = (es1.x * ptmp.x) + (es1.y * ptmp.y);
                float norm2 = dot < 0 ? 1 : 0;
                dot /= norm;
                norm2 = dot > 1 ? 1 : norm2;
                float norm1 = abs((es.x * (segment.y - vpixelPos.y)) - (es.y * (segment.x - vpixelPos.x))) / norm;
                norm  = clamp(norm1 + norm2, 0, 1);
            }
            else
                norm = 1;
            return norm;
        }

        float computeAlphaEdgesAA()
        {
            float norm;
            norm = min(1,    computeAlphaAA(vaa0));
            norm = min(norm, computeAlphaAA(vaa1));
            norm = min(norm, computeAlphaAA(vaa2));
            norm = min(norm, computeAlphaAA(vaa3));
            norm = min(norm, computeAlphaAA(vaa4));
            norm = min(norm, computeAlphaAA(vaa5));
            return norm;
        }

        void main()
        {
            color = vcolor;
            color.w *= computeAlphaEdgesAA();
        }
    "@

    func createShaders(using self)
    {
        var result: GLint

        vertexShaderID := glCreateShader(GL_VERTEX_SHADER)
        vertexSourcePointer := @dataof(VertexShader)
        glShaderSource(vertexShaderID, 1, &vertexSourcePointer, null)
        glCompileShader(vertexShaderID)

        fragmentShaderID := glCreateShader(GL_FRAGMENT_SHADER)
        pixelSourcePointer := @dataof(PixelShader)
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