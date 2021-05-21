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

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
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
        offset += @sizeof(Vector4)

        glVertexAttribPointer(8, 1, GL_UNSIGNED_INT, GL_FALSE, @sizeof(VertexLayout), offset)
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
        glEnableVertexAttribArray(2)
        glEnableVertexAttribArray(3)
        glEnableVertexAttribArray(4)
        glEnableVertexAttribArray(5)
        glEnableVertexAttribArray(6)
        glEnableVertexAttribArray(7)
        glEnableVertexAttribArray(8)

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
        glDisableVertexAttribArray(2)
        glDisableVertexAttribArray(3)
        glDisableVertexAttribArray(4)
        glDisableVertexAttribArray(5)
        glDisableVertexAttribArray(6)
        glDisableVertexAttribArray(7)
        glDisableVertexAttribArray(8)
    }
}