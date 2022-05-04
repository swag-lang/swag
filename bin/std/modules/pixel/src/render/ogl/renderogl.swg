#global public
using Core, Ogl, Math

private
{
    const VertexShaderAA   = #load("shaders/aa.vert")
    const PixelShaderAA    = #load("shaders/aa.frag")
    const VertexShaderBlur = #load("shaders/blur.vert")
    const PixelShaderBlur  = #load("shaders/blur.frag")
}

#[Swag.Strict]
alias TextureHandle = uint
const InvalidTextureHandle: TextureHandle = acast 0

#[Swag.Strict]
alias RenderTargetHandle = uint
const InvalidRenderTargetHandle: RenderTargetHandle = acast 0

#[Swag.Strict]
alias RenderTargetSurfaceHandle = uint
const InvalidRenderTargetSurfaceHandle: RenderTargetSurfaceHandle = acast 0

struct RenderOgl
{
    using native:       NativeRenderOgl
    vertexbuffer:       GLuint
    shaderAA:           ShaderHandle
    shaderBlur:         ShaderHandle
    worldTransform:     Transform2
    orthoMatrix:        Matrix4x4
    modelMatrix:        Matrix4x4
    whiteTexture:       GLuint
    hatchTextures:      [@countof(HatchStyle)] GLuint
    blendingMode:       BlendingMode = .Alpha
    curRC:              RenderingContext

    stackRT:            Array'*RenderTarget
    stackReleasedRT:    Array'*RenderTarget
    curRT:              *RenderTarget
}

private impl RenderOgl
{
    func createShaders(using self)
    {
        {
            shaderAA = createShader(VertexShaderAA, PixelShaderAA)
            text0 := glGetUniformLocation(cast(GLuint) shaderAA, @dataof("inTexture0"));
            text1 := glGetUniformLocation(cast(GLuint) shaderAA, @dataof("inTexture1"));
            glUseProgram(cast(GLuint) shaderAA)
            glUniform1i(text0, 0);
            glUniform1i(text1, 1);
        }

        {
            shaderBlur = createShader(VertexShaderBlur, PixelShaderBlur)
            text0 := glGetUniformLocation(cast(GLuint) shaderBlur, @dataof("inTexture0"));
            glUseProgram(cast(GLuint) shaderBlur)
            glUniform1i(text0, 0);
        }
    }

    func generateTexture(datas: const [..] u8, w, h: s32)->GLuint
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

    mtd buidModelMatrix(modelTransform: Transform2)
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

    mtd setBlendingMode(bm: BlendingMode)
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
}

impl RenderOgl
{
    // To be called before rendering
    func begin(using self, rc: RenderingContext)
    {
        curRC = rc
        native.setCurrentContext(curRC)
        resetTransform()
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

    // Clear current render buffers
    mtd clear()
    {
        glClearStencil(0)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)
        glClearColor(0, 0, 0, 0)
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)
    }

    // Get the actual render target size
    mtd getTargetSize()->{s32, s32}
    {
        if curRT
            return @{curRT.colorBuffer.width, curRT.colorBuffer.height}
        return @{curRC.width, curRC.height}
    }

    // Force the scissor
    mtd setScissorRect(rect: Math.Rectangle)
    {
        glEnable(GL_SCISSOR_TEST)
        (?, h) := getTargetSize()
        glScissor(cast(GLint) rect.x, cast(GLint) (h - rect.y - rect.height), cast(GLint) rect.width, cast(GLint) rect.height)
    }

    // Reset the scissor
    mtd resetScissorRect()
    {
        glDisable(GL_SCISSOR_TEST)
    }

    // First init
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

    // Set the current render target
    mtd bindRenderTarget(rt: *RenderTarget)
    {
        Debug.assert(rt != null)
        if !rt.colorBuffer.valid
            return
        if curRT
            stackRT.add(curRT)
        curRT = rt
        glBindFramebuffer(GL_FRAMEBUFFER, cast(GLuint) curRT.handle)
        glViewport(0, 0, curRT.colorBuffer.width, curRT.colorBuffer.height)
    }

    // Go back to the frame buffer
    mtd unbindRenderTarget()
    {
        if stackRT.count
        {
            curRT = stackRT.popBack()
            glBindFramebuffer(GL_FRAMEBUFFER, cast(GLuint) curRT.handle)
            glViewport(0, 0, curRT.colorBuffer.width, curRT.colorBuffer.height)
        }
        else
        {
            curRT = null
            glBindFramebuffer(GL_FRAMEBUFFER, 0)
            glViewport(0, 0, curRC.width, curRC.height)
        }
    }

    // Create a render target
    mtd createRenderTarget(w, h: s32)->RenderTarget
    {
        var colorID: GLuint
        glGenTextures(1, &colorID)
        glBindTexture(GL_TEXTURE_2D, colorID)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, null)

        var fbo: GLuint
        glGenFramebuffers(1, &fbo)
        glBindFramebuffer(GL_FRAMEBUFFER, fbo)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorID, 0)

        var rbo: GLuint
        glGenRenderbuffers(1, &rbo)
        glBindRenderbuffer(GL_RENDERBUFFER, rbo)
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h)
        glBindRenderbuffer(GL_RENDERBUFFER, 0)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo)

        glBindFramebuffer(GL_FRAMEBUFFER, 0)

        var result: retval
        result.handle = cast(RenderTargetHandle) fbo
        result.colorBuffer.handle = cast(TextureHandle) colorID
        result.colorBuffer.width = w
        result.colorBuffer.height = h
        result.colorBuffer.valid = true
        result.colorBuffer.pf = .RGBA8
        result.depthStencil = acast(rbo)
        return result
    }

    // Delete a render target
    mtd deleteRenderTarget(rt: *RenderTarget)
    {
        if rt.handle == InvalidRenderTargetHandle or !rt.colorBuffer.valid
            return

        var cb: GLuint = acast rt.colorBuffer.handle
        glDeleteTextures(1, &cb)

        var ds: GLuint = acast rt.depthStencil
        glDeleteRenderbuffers(1, &ds)

        var fb: GLuint = acast rt.handle
        glDeleteFramebuffers(1, &fb)

        rt.handle = InvalidRenderTargetHandle
        rt.depthStencil = InvalidRenderTargetSurfaceHandle
        rt.colorBuffer.valid = false
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
        result.valid = true
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
    mtd removeTexture(texture: *Texture)
    {
        if !texture.valid
            return
        glDeleteTextures(1, cast (*GLuint) &texture.handle)
        texture.valid = false
    }

    // Translate the world transform
    mtd translateTransform(x, y: f32)
    {
        tr := Transform2.createTranslation(x, y)
        worldTransform.multiply(tr)
    }

    // Rotate the world transform
    mtd rotateTransform(angle: Math.Angle)
    {
        tr := Transform2.createRotation(angle.rad, 0, 0)
        worldTransform.multiply(tr)
    }

    // Rotate the world transform
    mtd rotateTransform(angle: Math.Angle, centerX, centerY: f32)
    {
        tr := Transform2.createRotation(angle.rad, centerX, centerY)
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

    mtd computeMatrices(mvp, mdl: GLint)
    {
        (width, height) := getTargetSize()

        // View, ortho projection
        ortho(cast(f32) height, 0, 0, cast(f32) width, 0, 1.0, orthoMatrix.m)
        glUniformMatrix4fv(mvp, 1, GL_FALSE, &orthoMatrix.m[0, 0])
	    glViewport(0, 0, width, height)

        // Draw matrix
        buidModelMatrix(Transform2.Identity)
        glUniformMatrix4fv(mdl, 1, GL_FALSE, &modelMatrix.m[0, 0])
    }

    // Draw the given painter
    mtd draw(dc: *Painter)
    {
        (width, height) := getTargetSize()
        if width <= 0 or height <= 0
            return

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
        glBufferData(GL_ARRAY_BUFFER, dc.vertexBuffer.count * @sizeof(VertexLayout), dc.vertexBuffer.buffer, GL_STATIC_DRAW);

        glUseProgram(cast(GLuint) shaderAA)
        mvp         := glGetUniformLocation(cast(GLuint) shaderAA, @dataof("mvp"))
        mdl         := glGetUniformLocation(cast(GLuint) shaderAA, @dataof("mdl"))
        boundRect   := glGetUniformLocation(cast(GLuint) shaderAA, @dataof("boundRect"))
        textureRect := glGetUniformLocation(cast(GLuint) shaderAA, @dataof("textureRect"))
        uvMode      := glGetUniformLocation(cast(GLuint) shaderAA, @dataof("uvMode"))
        copyMode    := glGetUniformLocation(cast(GLuint) shaderAA, @dataof("copyMode"))

        // Matrices
        computeMatrices(mvp, mdl)

        // Default state
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
        setBlendingMode(BlendingMode.Alpha)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE)

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
                (a, r, g, b) := params.color.toArgbf()
                glClearColor(r, g, b, a)
                glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)

            case ColorMask:
                params := &cmd.colorMask
                glColorMask(params.r ? GL_TRUE : GL_FALSE,
                            params.g ? GL_TRUE : GL_FALSE,
                            params.b ? GL_TRUE : GL_FALSE,
                            params.a ? GL_TRUE : GL_FALSE)

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
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE)
                glStencilFunc(GL_EQUAL, 1, 0xFF)
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP)

            case ResetClippingRegion:
                glDisable(GL_STENCIL_TEST)

            case SetClippingRect:
                params := &cmd.clippingRect
                glEnable(GL_SCISSOR_TEST)
                glScissor(cast(GLint) params.rect.x, cast(GLint) (height - params.rect.y - params.rect.height), cast(GLint) params.rect.width, cast(GLint) params.rect.height)

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
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE)
                }

            case UpdateFont:
                font := cmd.font.fontRef
                font.dirty = false
                glActiveTexture(GL_TEXTURE1)
                visit &atlas: font.atlases
                {
                    if !atlas.dirty continue
                    atlas.dirty = false
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
                glUniform1f(uvMode, cast(GLfloat) params.uvMode)
                glUniform1i(copyMode, cast(GLint) (blendingMode == .Copy))
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

            case BindRenderTarget:
                bindRenderTarget(cmd.renderTgt.tgt)
                computeMatrices(mvp, mdl)

            case UnBindRenderTarget:
                unbindRenderTarget()
                computeMatrices(mvp, mdl)

            case ReleaseRenderTarget:
                releaseRenderTarget(cmd.renderTgt.tgt)

            case SetShader:
                params := &cmd.shader
                shader := cast(GLuint) params.handle
                glUseProgram(shader)
                mvp         = glGetUniformLocation(shader, @dataof("mvp"))
                mdl         = glGetUniformLocation(shader, @dataof("mdl"))
                boundRect   = glGetUniformLocation(shader, @dataof("boundRect"))
                textureRect = glGetUniformLocation(shader, @dataof("textureRect"))
                uvMode      = glGetUniformLocation(shader, @dataof("uvMode"))
                copyMode    = glGetUniformLocation(shader, @dataof("copyMode"))
                glUniformMatrix4fv(mvp, 1, GL_FALSE, &orthoMatrix.m[0, 0])
                glUniformMatrix4fv(mdl, 1, GL_FALSE, &modelMatrix.m[0, 0])
            }
        }

        loop i: 10 glDisableVertexAttribArray(cast(GLuint) i)
    }

    // Get a render target of the given size
    // releaseRenderTarget must be called to release it
    mtd getRenderTarget(w, h: s32)->*RenderTarget
    {
        w2 := Math.roundUpToPowerOf2(cast(u32) w)
        h2 := Math.roundUpToPowerOf2(cast(u32) h)

        visit rt, i: stackReleasedRT
        {
            if rt.colorBuffer.width >= w2 and rt.colorBuffer.height >= h2
            {
                stackReleasedRT.removeAt(i)
                return rt
            }
        }

        rt := Memory.new'RenderTarget()
        dref rt = createRenderTarget(cast(s32) w2, cast(s32) h2)
        return rt
    }

    // Release the given render target
    mtd releaseRenderTarget(tgt: *RenderTarget)
    {
        stackReleasedRT.add(tgt)
    }

    // Returns a predefined shader handle
    mtd getShader(id: DefaultShaderId)->ShaderHandle
    {
        switch id
        {
        case Default: return cast(ShaderHandle) shaderAA;
        case Blur:    return cast(ShaderHandle) shaderBlur;
        }

        return InvalidShaderHandle
    }

    // Create the given shader
    func createShader(using self, vertex, pixel: const [..] u8)->ShaderHandle
    {
        var result: GLint
	    var infoLogLength: s32
        var infoLog: String

        vertexShaderID := glCreateShader(GL_VERTEX_SHADER)
        vertexSourcePointer := @dataof(vertex)
        vertexSourceLength := cast(GLint) @countof(vertex)
        glShaderSource(vertexShaderID, 1, &vertexSourcePointer, &vertexSourceLength)
        glCompileShader(vertexShaderID)

        // Get err
        glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result)
        glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength)
        infoLog.reserve(cast(uint) infoLogLength + 1)
        glGetShaderInfoLog(vertexShaderID, infoLogLength, null, @dataof(infoLog))
        if infoLogLength
        {
            infoLog.length = infoLogLength
            Console.print(infoLog)
        }

        fragmentShaderID := glCreateShader(GL_FRAGMENT_SHADER)
        pixelSourcePointer := @dataof(pixel)
        pixelSourceLength := cast(GLint) @countof(pixel)
        glShaderSource(fragmentShaderID, 1, &pixelSourcePointer, &pixelSourceLength)
        glCompileShader(fragmentShaderID)

        // Get err
        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result)
        glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength)
        infoLog.reserve(cast(uint) infoLogLength + 1)
        glGetShaderInfoLog(fragmentShaderID, infoLogLength, null, @dataof(infoLog))
        if infoLogLength
        {
            infoLog.length = infoLogLength
            Console.print(infoLog)
        }

        progID := glCreateProgram()
        glAttachShader(progID, vertexShaderID)
        glAttachShader(progID, fragmentShaderID)
        glLinkProgram(progID)

        glDetachShader(progID, vertexShaderID)
        glDetachShader(progID, fragmentShaderID)
        glDeleteShader(vertexShaderID)
        glDeleteShader(fragmentShaderID)

        return cast(ShaderHandle) progID
    }
}