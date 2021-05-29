#global public
using Core, Ogl, Core.Math

#[Swag.strict]
alias TextureHandle = uint
const InvalidTextureHandle: TextureHandle = acast 0

private const HatchDot: [64] u8 = @[
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
]

public struct RenderOgl
{
    vertexbuffer:       GLuint
    programID:          GLuint
    width, height:      s32
    worldTransform:     Transform2
    modelMatrix:        Matrix4x4
    whiteTexture:       GLuint
    hatchTextures:      [@countof(BrushTextureStyle)] GLuint
}

impl RenderOgl
{
    private func generateTexture(datas: const [..] u8, w, h: s32)->GLuint
    {
        Debug.assert(@countof(datas) <= 8*8)

        var arr: [64] u32
        visit p, i: datas arr[i] = p ? 0xFFFFFFFF : 0

        var result: GLuint
        glGenTextures(1, &result)
        glBindTexture(GL_TEXTURE_2D, result)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &arr[0])
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
        return result
    }

    protected func init(using self, w, h: s32)
    {
        width = w
        height = h

        glInitExtensions()
        createShaders()

        glGenBuffers(1, &vertexbuffer)
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)

        // Vertex buffer layout
        var offset: const *void = null
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector2)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector2)
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, @sizeof(VertexLayout), offset)
        offset += @sizeof(u32)
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
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)

        // Generate white texture
        whiteTexture = generateTexture(@[1], 1, 1)
        hatchTextures[cast(u32) BrushTextureStyle.Dot] = generateTexture(HatchDot, 8, 8)
    }

    func resize(using self, width, height: s32)
    {
        self.width = #scopefct.width
        self.height = #scopefct.height
    }

    private func ortho(b, t, l, r, n, f: f32, m: [4,4] f32)
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

    private func buidModelMatrix(using self, modelTransform: Transform2)
    {
        tr := worldTransform
        tr.multiply(modelTransform)

        // Received matrix is in row major order, so transpose it

        // x
        modelMatrix.m[0, 0] = tr.m11
        modelMatrix.m[0, 1] = tr.m12

        // y
        modelMatrix.m[1, 0] = tr.m21
        modelMatrix.m[1, 1] = tr.m22

        // translation
        modelMatrix.m[3, 0] = tr.m31
        modelMatrix.m[3, 1] = tr.m32
    }

    // Register an image for rendering
    func addImage(using self, image: Image)->Texture
    {
        var fmt: GLenum = 0
        internal := GL_RGB

        #[Swag.complete]
        switch image.pf
        {
        case BGR8:  fmt = GL_BGR
        case BGRA8: fmt = GL_BGRA; internal = GL_RGBA
        case RGB8:  fmt = GL_RGB
        case RGBA8: fmt = GL_RGBA; internal = GL_RGBA
        }

        var textureID: GLuint
        glGenTextures(1, &textureID)
        glBindTexture(GL_TEXTURE_2D, textureID)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
        glTexImage2D(GL_TEXTURE_2D, 0, internal, cast(s32) image.width, cast(s32) image.height, 0, fmt, GL_UNSIGNED_BYTE, image.pixels)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)

        var result: retval
        result.handle = cast(TextureHandle) textureID
        result.width = image.width
        result.height = image.height
        result.pf = image.pf
        return result
    }


    // To be called before rendering
    func begin(using self)
    {
        worldTransform.setIdentity()
    }

    // Translate the world transform
    func translateTransform(using self, x, y: f32)
    {
        tr := Transform2.createTranslation(x, y)
        worldTransform.multiply(tr)
    }

    // Rotate the world transform
    func rotateTransform(using self, angle: f32)
    {
        rad := Math.toRadians(angle)
        tr := Transform2.createRotation(rad, 0, 0)
        worldTransform.multiply(tr)
    }

    // Scale the world transform
    func scaleTransform(using self, x, y: f32)
    {
        tr := Transform2.createScale(x, y, 0, 0)
        worldTransform.multiply(tr)
    }

    // Reset the world transform
    func resetTransform(using self)
    {
        worldTransform.setIdentity()
        modelMatrix.setIdentity()
    }

    // Draw the given painter
    func draw(using self, dc: *Painter)
    {
        if height == 0 or width == 0
            return

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
        glBufferData(GL_ARRAY_BUFFER, dc.vertexBuffer.count * @sizeof(VertexLayout), dc.vertexBuffer.buffer, GL_STATIC_DRAW);

        glUseProgram(programID)

        // View, ortho projection
        var mvp: GLint = glGetUniformLocation(programID, @dataof("mvp"))
        var orthoMatrix: Matrix4x4
        ortho(cast(f32) height, 0, 0, cast(f32) width, 0, 1.0, orthoMatrix.m)
        glUniformMatrix4fv(mvp, 1, GL_FALSE, &orthoMatrix.m[0, 0])
	    glViewport(0, 0, width, height)

        // Draw matrix
        var mdl: GLint = glGetUniformLocation(programID, @dataof("mdl"))
        buidModelMatrix(Transform2.Identity)
        glUniformMatrix4fv(mdl, 1, GL_FALSE, &modelMatrix.m[0, 0])

        // State
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        loop i: 10 glEnableVertexAttribArray(cast(GLuint) i)
        glBindTexture(GL_TEXTURE_2D, cast(GLuint) whiteTexture)

        visit cmd: dc.commandBuffer
        {
            switch cmd.id
            {
            case Clear:
                params := &cmd.params.clear
                (r, g, b) := params.color.toRgbf()
                glClearColor(r, g, b, 0)
                glClear(params.flags)

            case Transform:
                params := &cmd.params.transform
                buidModelMatrix(params.tr)
                glUniformMatrix4fv(mdl, 1, GL_FALSE, &modelMatrix.m[0, 0])

            case DrawTriangles:
                params := &cmd.params.drawTriangles
                glDrawArrays(GL_TRIANGLES, cast(GLint) params.start, cast(GLsizei) params.count)

            case BindTexture:
                params := &cmd.params.texture
                switch params.style
                {
                case None:
                    glBindTexture(GL_TEXTURE_2D, cast(GLuint) whiteTexture)
                case Custom:
                    glBindTexture(GL_TEXTURE_2D, cast(GLuint) params.handle)
                case Dot:
                    glBindTexture(GL_TEXTURE_2D, cast(GLuint) hatchTextures[cast(u32) params.style])
                }
            }
        }

        loop i: 10 glDisableVertexAttribArray(cast(GLuint) i)
    }
}