#global public
using Core, Ogl, Core.Math

#[Swag.Strict]
alias TextureHandle = uint
const InvalidTextureHandle: TextureHandle = acast 0

public struct RenderOgl
{
    using native:       NativeRenderOgl
    vertexbuffer:       GLuint
    shaderImage:        GLuint
    worldTransform:     Transform2
    modelMatrix:        Matrix4x4
    whiteTexture:       GLuint
    hatchTextures:      [@countof(HatchStyle)] GLuint
    blendingMode:       BlendingMode = .Alpha
    curRC:              RenderingContext
}

impl RenderOgl
{
    // To be called before rendering
    func begin(using self, rc: RenderingContext)
    {
        curRC = rc
        native.setCurrentContext(curRC)
        resetTransform()
        glClearStencil(0)
        glClearColor(0, 0, 0, 0)
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)
    }

    // To be called after rendering
    func end(using self)
    {
        glFlush()
        glFinish()
        native.swagBuffers(curRC)
    }

    // Set the current rendering context
    func setCurrentContext(using self, rc: RenderingContext)
    {
        native.setCurrentContext(rc)
    }

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
        return result
    }

    mtd init()
    {
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

        // Generate predefined textures
        whiteTexture = generateTexture(@[1], 1, 1)
        using HatchStyle
        hatchTextures[Dot] = generateTexture(Hatch.Dot, 8, 8)
        hatchTextures[Horizontal] = generateTexture(Hatch.Horizontal, 8, 8)
        hatchTextures[HorizontalLight] = generateTexture(Hatch.HorizontalLight, 8, 8)
        hatchTextures[HorizontalNarrow] = generateTexture(Hatch.HorizontalNarrow, 8, 8)
        hatchTextures[Vertical] = generateTexture(Hatch.Vertical, 8, 8)
        hatchTextures[VerticalLight] = generateTexture(Hatch.VerticalLight, 8, 8)
        hatchTextures[VerticalNarrow] = generateTexture(Hatch.VerticalNarrow, 8, 8)
        hatchTextures[SolidDiamond] = generateTexture(Hatch.SolidDiamond, 8, 8)
        hatchTextures[OutlinedDiamond] = generateTexture(Hatch.OutlinedDiamond, 8, 8)
        hatchTextures[HorizontalDark] = generateTexture(Hatch.HorizontalDark, 8, 8)
        hatchTextures[VerticalDark] = generateTexture(Hatch.VerticalDark, 8, 8)
        hatchTextures[Grid] = generateTexture(Hatch.Grid, 8, 8)
        hatchTextures[GridLarge] = generateTexture(Hatch.GridLarge, 8, 8)
        hatchTextures[GridDotted] = generateTexture(Hatch.GridDotted, 8, 8)
        hatchTextures[Cross] = generateTexture(Hatch.Cross, 8, 8)
        hatchTextures[CrossLarge] = generateTexture(Hatch.CrossLarge, 8, 8)
        hatchTextures[Percent5] = generateTexture(Hatch.Percent5, 8, 8)
        hatchTextures[Percent10] = generateTexture(Hatch.Percent10, 8, 8)
        hatchTextures[Percent20] = generateTexture(Hatch.Percent20, 8, 8)
        hatchTextures[Percent25] = generateTexture(Hatch.Percent25, 8, 8)
        hatchTextures[Percent30] = generateTexture(Hatch.Percent30, 8, 8)
        hatchTextures[Percent40] = generateTexture(Hatch.Percent40, 8, 8)
        hatchTextures[Percent50] = generateTexture(Hatch.Percent50, 8, 8)
        hatchTextures[Percent60] = generateTexture(Hatch.Percent60, 8, 8)
        hatchTextures[Percent70] = generateTexture(Hatch.Percent70, 8, 8)
        hatchTextures[Percent75] = generateTexture(Hatch.Percent75, 8, 8)
        hatchTextures[Percent80] = generateTexture(Hatch.Percent80, 8, 8)
        hatchTextures[Percent90] = generateTexture(Hatch.Percent90, 8, 8)
        hatchTextures[CheckBoard] = generateTexture(Hatch.CheckBoard, 8, 8)
        hatchTextures[CheckBoardLarge] = generateTexture(Hatch.CheckBoardLarge, 8, 8)
        hatchTextures[Weave] = generateTexture(Hatch.Weave, 8, 8)
        hatchTextures[DiagonalForward] = generateTexture(Hatch.DiagonalForward, 8, 8)
        hatchTextures[DiagonalForwardLight] = generateTexture(Hatch.DiagonalForwardLight, 8, 8)
        hatchTextures[DiagonalBackward] = generateTexture(Hatch.DiagonalBackward, 8, 8)
        hatchTextures[DiagonalBackwardLight] = generateTexture(Hatch.DiagonalBackwardLight, 8, 8)
        hatchTextures[DiagonalCross] = generateTexture(Hatch.DiagonalCross, 8, 8)
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

    private mtd buidModelMatrix(modelTransform: Transform2)
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

        // z
        modelMatrix.m[2, 2] = 1

        // translation
        modelMatrix.m[3, 0] = tr.m31
        modelMatrix.m[3, 1] = tr.m32
    }

    // Register an image for rendering
    mtd addImage(image: Image)->Texture
    {
        var fmt: GLenum = 0
        internal := GL_RGB

        #[Swag.Complete]
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

        var result: retval
        result.handle = cast(TextureHandle) textureID
        result.width = image.width
        result.height = image.height
        result.pf = image.pf
        return result
    }

    // Update content of texture
    mtd updateTexture(texture: Texture, pixels: const*u8)
    {
        var fmt: GLenum = 0

        #[Swag.Complete]
        switch texture.pf
        {
        case BGR8:  fmt = GL_BGR
        case BGRA8: fmt = GL_BGRA
        case RGB8:  fmt = GL_RGB
        case RGBA8: fmt = GL_RGBA
        }

        textureID := cast(GLuint) texture.handle
        glBindTexture(GL_TEXTURE_2D, textureID)
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cast(s32) texture.width, cast(s32) texture.height, fmt, GL_UNSIGNED_BYTE, pixels)
    }

    // Unregister a texture created with 'addImage'
    mtd removeTexture(texture: Texture)
    {
        glDeleteTextures(1, cast (*GLuint) &texture.handle)
    }

    // Translate the world transform
    mtd translateTransform(x, y: f32)
    {
        tr := Transform2.createTranslation(x, y)
        worldTransform.multiply(tr)
    }

    // Rotate the world transform
    mtd rotateTransform(angle: f32)
    {
        rad := Math.toRadians(angle)
        tr := Transform2.createRotation(rad, 0, 0)
        worldTransform.multiply(tr)
    }

    // Rotate the world transform
    mtd rotateTransform(angle: f32, centerX, centerY: f32)
    {
        rad := Math.toRadians(angle)
        tr := Transform2.createRotation(rad, centerX, centerY)
        worldTransform.multiply(tr)
    }

    // Scale the world transform
    mtd scaleTransform(x, y: f32)
    {
        tr := Transform2.createScale(x, y, 0, 0)
        worldTransform.multiply(tr)
    }

    // Reset the world transform
    mtd resetTransform()
    {
        worldTransform.setIdentity()
        modelMatrix.setIdentity()
    }

    private mtd setBlendingMode(bm: BlendingMode)
    {
        blendingMode = bm

        #[Swag.Complete]
        switch bm
        {
        case Copy:
            glDisable(GL_BLEND)
        case Alpha:
            glEnable(GL_BLEND)
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
            glBlendEquation(GL_FUNC_ADD)
        case Add:
            glEnable(GL_BLEND)
            glBlendFunc(GL_ONE, GL_ONE)
            glBlendEquation(GL_FUNC_ADD)
        case Sub:
            glEnable(GL_BLEND)
            glBlendFunc(GL_ONE, GL_ONE)
            glBlendEquation(GL_FUNC_SUBTRACT)
        case SubDst:
            glEnable(GL_BLEND)
            glBlendFunc(GL_ONE, GL_ONE)
            glBlendEquation(GL_FUNC_REVERSE_SUBTRACT)
        case Min:
            glEnable(GL_BLEND)
            glBlendFunc(GL_ONE, GL_ONE)
            glBlendEquation(GL_MIN)
        case Max:
            glEnable(GL_BLEND)
            glBlendFunc(GL_ONE, GL_ONE)
            glBlendEquation(GL_MAX)
        }
    }

    // Draw the given painter
    mtd draw(dc: *Painter)
    {
        if curRC.height == 0 or curRC.width == 0
            return

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
        glBufferData(GL_ARRAY_BUFFER, dc.vertexBuffer.count * @sizeof(VertexLayout), dc.vertexBuffer.buffer, GL_STATIC_DRAW);

        glUseProgram(shaderImage)

        // View, ortho projection
        mvp := glGetUniformLocation(shaderImage, @dataof("mvp"))
        var orthoMatrix: Matrix4x4
        ortho(cast(f32) curRC.height, 0, 0, cast(f32) curRC.width, 0, 1.0, orthoMatrix.m)
        glUniformMatrix4fv(mvp, 1, GL_FALSE, &orthoMatrix.m[0, 0])
	    glViewport(0, 0, curRC.width, curRC.height)

        // Draw matrix
        mdl := glGetUniformLocation(shaderImage, @dataof("mdl"))
        buidModelMatrix(Transform2.Identity)
        glUniformMatrix4fv(mdl, 1, GL_FALSE, &modelMatrix.m[0, 0])

        // UV parameters
        uvmode      := glGetUniformLocation(shaderImage, @dataof("uvmode"))
        boundRect   := glGetUniformLocation(shaderImage, @dataof("boundRect"))
        textureRect := glGetUniformLocation(shaderImage, @dataof("textureRect"))

        // Default state
        setBlendingMode(BlendingMode.Alpha)
        glBindTexture(GL_TEXTURE_2D, whiteTexture)
        glDisable(GL_SCISSOR_TEST)
        glDisable(GL_STENCIL_TEST)
        glDisable(GL_DEPTH_TEST)
        glClearStencil(0)
        glStencilMask(0xFF)
        glDepthMask(GL_FALSE)

        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, whiteTexture)
        glActiveTexture(GL_TEXTURE1)
        glBindTexture(GL_TEXTURE_2D, whiteTexture)

        loop i: 10 glEnableVertexAttribArray(cast(GLuint) i)

        overlapMode := false
        visit cmd: dc.commandBuffer
        {
            switch cmd.id
            {
            case SetBlendingMode:
                params := &cmd.blendingMode
                setBlendingMode(params.mode)

            case Clear:
                params := &cmd.clear
                (r, g, b) := params.color.toRgbf()
                glClearColor(r, g, b, 0)
                glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)

            case Transform:
                params := &cmd.transform
                buidModelMatrix(params.tr)
                glUniformMatrix4fv(mdl, 1, GL_FALSE, &modelMatrix.m[0, 0])

            case StartClippingRegion:
                glEnable(GL_STENCIL_TEST)
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE)
                glStencilFunc(GL_ALWAYS, 1, 0xFF)
                fallthrough

            case SetClippingRegionMode:
                params := &cmd.clippingRegion
                mode := params.mode == .Set ? GL_REPLACE : cast(GLenum) GL_ZERO
                glStencilOp(mode, mode, mode)

            case EndClippingRegion:
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)
                glStencilFunc(GL_EQUAL, 1, 0xFF)
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP)

            case ResetClippingRegion:
                glDisable(GL_STENCIL_TEST)

            case SetClippingRect:
                params := &cmd.clippingRect
                glEnable(GL_SCISSOR_TEST)
                glScissor(cast(GLint) params.rect.x, cast(GLint) (curRC.height - params.rect.y - params.rect.height), cast(GLint) params.rect.width, cast(GLint) params.rect.height)

            case ResetClippingRect:
                glDisable(GL_SCISSOR_TEST)

            case StartNoOverlap:
                overlapMode = true

            case StopNoOverlap:
                overlapMode = false
                setBlendingMode(blendingMode)

            case DrawTriangles:
                params := &cmd.drawTriangles
                if !overlapMode
                {
                    glDrawArrays(GL_TRIANGLES, cast(GLint) params.start, cast(GLsizei) params.count)
                }
                else
                {
                    // First we mark the alpha buffer with the 'maximum' alpha to paint.
                    // If a pixel is drawn twice because of overlapping, we will have the 'most opaque'
                    glEnable(GL_BLEND)
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE)
                    glBlendFunc(GL_SRC_ALPHA, GL_ZERO)
                    glBlendEquation(GL_MAX)
                    glDrawArrays(GL_TRIANGLES, cast(GLint) params.start, cast(GLsizei) params.count)

                    // Then we use the stored alpha as the blend factor.
                    // When a pixel is drawn once, the alpha value is set to 0, so this means that
                    // the second pixel will not be written.
                    glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ZERO, GL_ZERO)
                    glBlendEquation(GL_FUNC_ADD)
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)
                    glDrawArrays(GL_TRIANGLES, cast(GLint) params.start, cast(GLsizei) params.count)
                }

            case UpdateFont:
                font := cmd.font.fontRef
                font.dirty = false
                visit *atlas: font.atlases
                {
                    if !atlas.dirty continue
                    atlas.dirty = false
                    glActiveTexture(GL_TEXTURE1)
                    if cast(u32) atlas.texture.handle == 0
                        glGenTextures(1, cast(*GLuint) &atlas.texture.handle)
                    atlas.texture.width = atlas.width / atlas.bpp
                    atlas.texture.height = atlas.height
                    atlas.texture.pf = atlas.bpp == 1 ? .RGBA8 : .RGB8
                    glBindTexture(GL_TEXTURE_2D, cast(GLuint) atlas.texture.handle)
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
                    fmt := atlas.bpp == 1 ? GL_RED : GL_RGB
                    glTexImage2D(GL_TEXTURE_2D, 0, fmt, cast(GLsizei) atlas.texture.width, cast(GLsizei) atlas.texture.height, 0, fmt, GL_UNSIGNED_BYTE, atlas.datas.buffer)
                }

            case ResetTexture0:
                glActiveTexture(GL_TEXTURE0)
                glBindTexture(GL_TEXTURE_2D, whiteTexture)

            case BindTexture0:
                params := &cmd.texture0
                glUniform4fv(boundRect, 1, &params.boundRect.x)
                glUniform4fv(textureRect, 1, &params.textureRect.x)
                glUniform1f(uvmode, cast(GLfloat) params.uvMode)
                glActiveTexture(GL_TEXTURE0)

                #[Swag.Complete]
                switch params.type
                {
                case SolidColor:
                    glBindTexture(GL_TEXTURE_2D, whiteTexture)
                case Texture:
                    glBindTexture(GL_TEXTURE_2D, cast(GLuint) params.texture.handle)
                case Hatch:
                    glBindTexture(GL_TEXTURE_2D, hatchTextures[params.hatch])
                }

                #[Swag.Complete]
                switch params.uvMode
                {
                case Fill, FillSubRect:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
                case Tile:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
                }

                #[Swag.Complete]
                switch params.interpolationMode
                {
                case Pixel:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
                case Linear:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
                }

            case BindTextureFont:
                params := &cmd.textureFont
                glActiveTexture(GL_TEXTURE1)
                texture := params.font.atlases[params.atlasIndex].texture.handle
                glBindTexture(GL_TEXTURE_2D, cast(GLuint) texture)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)

            case ResetTextureFont:
                glActiveTexture(GL_TEXTURE1)
                glBindTexture(GL_TEXTURE_2D, whiteTexture)
            }
        }

        loop i: 10 glDisableVertexAttribArray(cast(GLuint) i)
    }
}