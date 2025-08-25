#global public
using Core, Ogl, Core.Math

enum DefaultShaderId
{
    Default
    Blur
}

const MaxAAEdge = 12

#[Swag.Pack(1)]
struct VertexLayout
{
    pos:       Vector2
    uv:        Vector2
    color:     u32
    aa:        [MaxAAEdge] Vector4
    aanum:     f32
}

enum CommandId
{
    Clear
    ColorMask
    Transform
    DrawTriangles
    BindTexture0
    ResetTexture0
    StartFont
    EndFont
    BindTextureFont
    ResetTextureFont
    SetClippingRect
    ResetClippingRect
    StartClippingRegion
    EndClippingRegion
    ResetClippingRegion
    SetClippingRegionMode
    UpdateFont
    StartNoOverlap
    StopNoOverlap
    SetBlendingMode
    BindRenderTarget
    UnbindRenderTarget
    ReleaseRenderTarget
    SetShader
    SetShaderParam
}

#[Swag.EnumFlags]
enum PaintParams
{
    Zero
    Antialiased
    Default     = Zero
}

enum PaintQuality
{
    Draft
    Normal
}

enum ClippingMode
{
    Set
    Clear
}

enum BlendingMode
{
    Copy
    Alpha
    Add
    Sub
    SubDst
    Min
    Max
}

struct PaintState
{
    paintParams:           PaintParams = Default
    paintQuality:          PaintQuality = Normal
    transform:             Transform2
    blendingMode:          BlendingMode = Alpha
    interpolationMode:     InterpolationMode = Pixel
    clippingRect:          Rectangle
    stackClipRect:         Array'Rectangle
    clippingRectOn:        bool
    colorMask:             ColorMask
}

struct ColorMask { r, g, b, a: bool }

struct Command
{
    id:  CommandId = undefined

    using params: union
    {
        clear:              struct{ color: Color }
        drawTriangles:      struct{ start: u32, count: u32 }
        transform:          struct{ tr: Transform2 }
        clippingRect:       struct{ rect: Rectangle }
        clippingRegion:     struct{ mode: ClippingMode = undefined }
        font:               struct{ fontRef: *Font }
        blendingMode:       struct{ mode: BlendingMode = undefined }
        textureFont:        struct{ font: *Font, atlasIndex: s32 }
        colorMask:          ColorMask
        renderTgt:          struct{ tgt: *RenderTarget, paintAlpha: bool }
        shader:             struct{ shader: *ShaderBase }

        shaderParam: struct
        {
            param:     ShaderParamHandle
            type:      typeinfo
            _f32:      f32
            _s32:      s32
        }

        texture0: struct
        {
            boundRect:             Vector4
            textureRect:           Vector4
            type:                  BrushType = undefined
            hatch:                 HatchStyle = undefined
            uvMode:                UVMode = undefined
            interpolationMode:     InterpolationMode = undefined
            texture:               Texture
        }
    }
}

struct Painter
{
    commandBuffer:          Array'Command
    vertexBuffer:           Array'VertexLayout
    triangulateIdx:         Array's32
    fake:                   bool

    sharedSolidPen:         Pen
    sharedSolidBrush:       Brush
    sharedLinePath:         LinePath
    sharedLinePathList:     LinePathList
    sharedRoundTmp:         Array'{ factor: f32, pos: Vector2 }

    curState:               PaintState
    stackState:             Array'PaintState
    stackTransform:         Array'Transform2
    lastTexture0Type:       BrushType = SolidColor
    tess:                   Poly2Tri.Tessellate

    stackRT:                Array'*RenderTarget
    curRT:                  *RenderTarget

    lastTexture0Handle:     TextureHandle
    hasTextureFont:         bool

    drawingBegin:           bool
    overlapMode:            bool
    canSetTexture0:         bool = true
}

impl Painter
{
    internal func getTextureSize(brush: Brush)->{ w: s32, h: s32 }
    {
        var result: retval

        // For a hatch brush, texture size is always 8x8
        if brush.type == .Hatch
        {
            result.w, result.h = 8
        }
        else
        {
            result.w = brush.texture.width
            result.h = brush.texture.height
        }

        return result
    }

    internal mtd resetTexture0()
    {
        if me.lastTexture0Handle or me.lastTexture0Type != .SolidColor
        {
            me.lastTexture0Handle = cast(TextureHandle) 0
            me.lastTexture0Type   = .SolidColor
            discard me.newCommand(.ResetTexture0, true)
        }
    }

    internal mtd setTexture0(bvMin, bvMax: Vector2, brush: Brush)
    {
        if !me.canSetTexture0:
            return
        if brush.type == .SolidColor
        {
            me.resetTexture0()
            return
        }

        let cmd    = me.newCommand(.BindTexture0, true)
        let params = &cmd.texture0

        params.boundRect.x = bvMin.x
        params.boundRect.y = bvMin.y
        params.boundRect.z = bvMax.x
        params.boundRect.w = bvMax.y

        let (wt, ht) = getTextureSize(brush)
        if brush.uvMode == .FillSubRect
        {
            params.textureRect.x = brush.uvRect.x / wt
            params.textureRect.y = brush.uvRect.y / ht
            params.textureRect.z = brush.uvRect.right() / wt
            params.textureRect.w = brush.uvRect.bottom() / ht
            params.textureRect.x += 0.5 / wt
            params.textureRect.y += 0.5 / ht
            params.textureRect.z -= 0.5 / wt
            params.textureRect.w -= 0.5 / ht
        }
        else
        {
            params.textureRect.x = 0
            params.textureRect.y = 0
            params.textureRect.z = wt
            params.textureRect.w = ht
        }

        params.type    = brush.type
        params.hatch   = brush.hatch
        params.texture = brush.texture
        params.uvMode  = brush.uvMode
        if brush.type == .Hatch:
            params.interpolationMode = .Pixel
        else:
            params.interpolationMode = me.curState.interpolationMode

        me.lastTexture0Handle = brush.texture.handle
        me.lastTexture0Type   = brush.type
    }

    internal mtd startFont()
    {
        discard me.newCommand(.StartFont, true)
    }

    internal mtd endFont()
    {
        discard me.newCommand(.EndFont, true)
    }

    internal mtd resetTextureFont()
    {
        if !me.hasTextureFont:
            return
        me.hasTextureFont = false
        discard me.newCommand(.ResetTextureFont, true)
    }

    internal mtd setTextureFont(font: *Font, atlasIndex: s32)
    {
        let cmd    = me.newCommand(.BindTextureFont, true)
        let params = &cmd.textureFont
        params.font       = font
        params.atlasIndex = atlasIndex
        me.hasTextureFont = true
    }

    internal mtd pushTriangleCmd()->*Command
    {
        var cmd: *Command
        if me.commandBuffer.count
        {
            let last = me.commandBuffer.buffer + me.commandBuffer.count - 1
            if last.id == CommandId.DrawTriangles:
                cmd = last
        }

        if !cmd
        {
            cmd                     = me.newCommand(.DrawTriangles)
            cmd.drawTriangles.start = cast(u32) me.vertexBuffer.count
            cmd.drawTriangles.count = 0
        }

        return cmd
    }

    #[Swag.Overload]
    internal mtd pushTriangle(p1, p2, p3: Vector2, uv1, uv2, uv3: Vector2, color: Color)->u64
    {
        let cmd = me.pushTriangleCmd()
        cmd.drawTriangles.count += 3

        var ptr = me.vertexBuffer.emplaceAddress(3)
        ptr.pos   = p1
        ptr.color = color.argb
        ptr.aanum = 0
        ptr.uv    = uv1
        ptr += 1
        ptr.pos   = p2
        ptr.color = color.argb
        ptr.aanum = 0
        ptr.uv    = uv2
        ptr += 1
        ptr.pos   = p3
        ptr.color = color.argb
        ptr.aanum = 0
        ptr.uv    = uv3

        return me.vertexBuffer.count - 3
    }

    #[Swag.Overload]
    internal mtd pushTriangle(p1, p2, p3: Vector2, color: Color)->u64
    {
        let cmd = me.pushTriangleCmd()
        cmd.drawTriangles.count += 3

        var ptr = me.vertexBuffer.emplaceAddress(3)
        ptr.pos   = p1
        ptr.color = color.argb
        ptr.aanum = 0
        ptr += 1
        ptr.pos   = p2
        ptr.color = color.argb
        ptr.aanum = 0
        ptr += 1
        ptr.pos   = p3
        ptr.color = color.argb
        ptr.aanum = 0

        return me.vertexBuffer.count - 3
    }

    #[Swag.Safety("", false), Swag.Overload]
    internal mtd addEdgeAA(tri: u64, startx, starty, endx, endy: f32)
    {
        var ptr = me.vertexBuffer.buffer + tri
        if ptr.aanum == MaxAAEdge:
            return

        let idx = cast(u32) ptr.aanum

        var ptr1 = &ptr.aa[0] + idx
        ptr1.x = startx
        ptr1.y = starty
        ptr1.z = endx
        ptr1.w = endy
        ptr.aanum += 1

        ptr += 1
        ptr1   = &ptr.aa[0] + idx
        ptr1.x = startx
        ptr1.y = starty
        ptr1.z = endx
        ptr1.w = endy
        ptr.aanum += 1

        ptr += 1
        ptr1   = &ptr.aa[0] + idx
        ptr1.x = startx
        ptr1.y = starty
        ptr1.z = endx
        ptr1.w = endy
        ptr.aanum += 1
    }

    #[Swag.Inline, Swag.Overload]
    internal mtd addEdgeAA(tri: u64, start, end: Vector2)
    {
        me.addEdgeAA(tri, start.x, start.y, end.x, end.y)
    }

    #[Swag.Inline]
    internal mtd newCommand(cmd: CommandId, replaceLast = false)->*Command
    {
        Debug.assert(me.drawingBegin, "begin() not called before")

        // Take the previous command if it's the same
        if replaceLast and me.commandBuffer.count and me.commandBuffer.buffer[me.commandBuffer.count - 1].id == cmd:
            return me.commandBuffer.buffer + me.commandBuffer.count - 1

        let ptr = me.commandBuffer.emplaceAddress(1)
        ptr.id = cmd
        return ptr
    }

    internal mtd updateFont(fontRef: *Font)
    {
        if !fontRef.dirty:
            return
        let cmd = me.newCommand(.UpdateFont, false)
        cmd.font.fontRef = fontRef
    }

    internal mtd startOverlapMode(pen: Pen)
    {
        var alpha = false
        if pen.brush.color.argb & 0xFF000000 != 0xFF000000:
            alpha = true
        if pen.brush.type == .Texture and pen.brush.texture.pf.hasAlpha():
            alpha = true
        if me.curRT and me.curRT.paintAlpha and me.curState.paintParams & .Antialiased:
            alpha = true
        if alpha and me.curState.blendingMode == .Alpha
        {
            me.overlapMode = true
            discard me.newCommand(.StartNoOverlap, true)
        }
    }

    internal mtd stopOverlapMode()
    {
        if me.overlapMode
        {
            me.overlapMode = false
            discard me.newCommand(.StopNoOverlap, true)
        }
    }

    // Call this before drawing
    mtd begin()
    {
        Debug.assert(me.drawingBegin == false)

        me.commandBuffer.clear()
        me.vertexBuffer.clear()

        me.drawingBegin   = true
        me.canSetTexture0 = true
        me.overlapMode    = false
        me.curRT          = null
        me.stackRT.clear()

        me.resetState()
    }

    // Call this after drawing
    mtd end()
    {
        Debug.assert(me.drawingBegin)
        me.drawingBegin = false
    }

    // Clear the rendering surface
    mtd clear(color: Color)
    {
        let cmd = me.newCommand(.Clear)
        cmd.clear.color = color
    }

    // Reset the current painting state
    mtd resetState()
    {
        with me.curState
        {
            .paintParams  = .Default
            .paintQuality = .Normal
            .transform.setIdentity()
            .interpolationMode = .Pixel
            .blendingMode      = .Alpha
            .colorMask.r, .colorMask.g, .colorMask.b = true
            .colorMask.a = false
        }

        me.resetClippingRect()
        me.resetTexture0()
        me.resetTextureFont()
        me.setState(me.curState)
    }

    // Set the current painting state
    mtd setState(state: PaintState)
    {
        me.curState = state
        me.setTransform(me.curState.transform)
        me.setBlendingMode(me.curState.blendingMode)
        me.setColorMask(me.curState.colorMask)

        if me.curState.clippingRectOn
        {
            me.curState.clippingRectOn = false
            me.setClippingRect(me.curState.clippingRect)
        }
    }

    // Save the current paint state
    mtd pushState()
    {
        me.stackState.add(me.curState)
    }

    // Restore the last paint state
    mtd popState()
    {
        var state = me.stackState.popBack()
        me.setState(state)
    }

    // Set the paint parameters
    mtd setParams(add: PaintParams, remove: PaintParams = .Zero)
    {
        me.curState.paintParams |= add
        me.curState.paintParams &= ~remove
    }

    mtd addParams(add: PaintParams)
    {
        me.curState.paintParams |= add
    }

    mtd removeParams(remove: PaintParams)
    {
        me.curState.paintParams &= ~remove
    }

    // Set antialiased mode on/off
    mtd setAntialiased(aa = true)
    {
        if aa:
            me.curState.paintParams |= .Antialiased
        else:
            me.curState.paintParams &= ~.Antialiased
    }

    #[Swag.Inline]
    mtd getParams() => me.curState.paintParams
    #[Swag.Inline]
    mtd getQuality() => me.curState.paintQuality

    // Set the paint quality
    mtd setQuality(quality: PaintQuality)
    {
        me.curState.paintQuality = quality
    }

    // Set the paint position
    mtd translateTransform(x, y: f32)
    {
        if isZeroEpsilon(x) and isZeroEpsilon(y):
            return
        var tr = Transform2.createTranslation(x, y)
        me.curState.transform.multiply(tr)
        me.setTransform(me.curState.transform)
    }

    // Set the paint rotation
    #[Swag.Overload]
    mtd rotateTransform(angle: Math.Angle)
    {
        if isZeroEpsilon(angle.rad):
            return
        var tr = Transform2.createRotation(angle.rad, 0, 0)
        me.curState.transform.multiply(tr)
        me.setTransform(me.curState.transform)
    }

    // Set the paint rotation
    #[Swag.Overload]
    mtd rotateTransform(angle: Math.Angle, centerX, centerY: f32)
    {
        if isZeroEpsilon(angle.rad):
            return
        var tr = Transform2.createRotation(angle.rad, centerX, centerY)
        me.curState.transform.multiply(tr)
        me.setTransform(me.curState.transform)
    }

    // Set the paint rotation.
    // Will rotate around the current transformation position.
    mtd rotateTransformInPlace(angle: Math.Angle)
    {
        if isZeroEpsilon(angle.rad):
            return
        var tr = Transform2.createRotation(angle.rad, me.curState.transform.tx, me.curState.transform.ty)
        me.curState.transform.multiply(tr)
        me.setTransform(me.curState.transform)
    }

    // Set the paint scale
    mtd scaleTransform(x, y: f32)
    {
        if isZeroEpsilon(x) and isZeroEpsilon(y):
            return
        var tr = Transform2.createScale(x, y, 0, 0)
        me.curState.transform.multiply(tr)
        me.setTransform(me.curState.transform)
    }

    // Push the current transformation
    mtd pushTransform()
    {
        me.stackTransform.add(me.curState.transform)
    }

    // Restore the pushed transformation
    mtd popTransform()
    {
        me.setTransform(me.stackTransform.popBack())
    }

    // Reset the paint position, rotation, scale
    mtd resetTransform()
    {
        me.curState.transform.setIdentity()
        me.setTransform(me.curState.transform)
    }

    // Set the paint transformation
    mtd setTransform(tr: Transform2)
    {
        me.curState.transform = tr
        let cmd = me.newCommand(.Transform, true)
        cmd.transform.tr = tr
    }

    // Returns the current tranformation
    mtd getTransform()->Transform2
    {
        var result: retval = me.curState.transform
        return result
    }

    // Set the texture interpolation mode
    mtd setInterpolationMode(mode: InterpolationMode)
    {
        me.curState.interpolationMode = mode
    }

    // Set the current clipping rectangle
    #[Swag.Overload]
    mtd setClippingRect(x, y, width, height: f32)
    {
        me.setClippingRect({x, y, width, height})
    }

    // Set the current clipping rectangle
    #[Swag.Overload]
    mtd setClippingRect(rect: Rectangle)
    {
        if me.curState.clippingRect == rect and me.curState.clippingRectOn:
            return
        me.curState.clippingRect   = rect
        me.curState.clippingRectOn = true
        let cmd = me.newCommand(.SetClippingRect, true)
        cmd.clippingRect.rect = me.curState.clippingRect
    }

    // Reset the current clipping rectangle
    mtd resetClippingRect()
    {
        if me.curState.clippingRectOn
        {
            me.curState.clippingRectOn = false
            me.curState.stackClipRect.clear()
            discard me.newCommand(.ResetClippingRect, true)
        }
    }

    // Interface the given rectangle with the current clipping rectangle
    mtd pushClippingSurfaceRect(rect: Rectangle)
    {
        me.pushClippingRect(rect, true)
    }

    // Interface the given rectangle with the current clipping rectangle
    mtd pushClippingRect(rect: Rectangle, isSurfacePos = false)
    {
        var rrect = rect
        if !isSurfacePos:
            rrect.offset(me.curState.transform.tx, me.curState.transform.ty)
        rrect.round()

        if me.curState.clippingRectOn
        {
            me.curState.stackClipRect.add(me.curState.clippingRect)
            var newRect = me.curState.clippingRect
            newRect.intersect(rrect)
            me.setClippingRect(newRect)
        }
        else
        {
            me.setClippingRect(rrect)
        }
    }

    // Restore the original clipping rectangle
    mtd popClippingRect()
    {
        if me.curState.stackClipRect.isEmpty()
        {
            me.resetClippingRect()
        }
        else
        {
            var clipRect = me.curState.stackClipRect.popBack()
            me.setClippingRect(clipRect)
        }
    }

    // Get the current clipping rectangle
    mtd getClippingRect()->Rectangle
    {
        if me.curState.clippingRectOn:
            return me.curState.clippingRect
        return {0, 0, Swag.F32.Max, Swag.F32.Max}
    }

    // Returns true if the current clipping rectangle is empty (no drawing !)
    mtd isEmptyClippingRect()->bool
    {
        return me.curState.clippingRectOn and me.curState.clippingRect.isEmpty()
    }

    // Start painting in the clipping buffer
    mtd startClippingRegion(mode = ClippingMode.Set)
    {
        let cmd = me.newCommand(.StartClippingRegion, true)
        cmd.clippingRegion.mode = mode
    }

    // Set the clipping region mode
    mtd setClippingRegionMode(mode: ClippingMode)
    {
        let cmd = me.newCommand(.SetClippingRegionMode, true)
        cmd.clippingRegion.mode = mode
    }

    // Stop painting in the clipping buffer. Back to normal.
    mtd endClippingRegion()
    {
        discard me.newCommand(.EndClippingRegion, true)
    }

    // Reset clipping buffer to its default value (no more clipping)
    mtd resetClippingRegion()
    {
        discard me.newCommand(.ResetClippingRegion, true)
    }

    // Set the current blending mode
    mtd setBlendingMode(mode: BlendingMode)
    {
        let cmd = me.newCommand(.SetBlendingMode, true)
        cmd.blendingMode.mode    = mode
        me.curState.blendingMode = mode
    }

    // Set color mask
    #[Swag.Overload]
    mtd setColorMask(cm: ColorMask)
    {
        let cmd = me.newCommand(.ColorMask, true)
        cmd.colorMask         = cm
        me.curState.colorMask = cm
    }

    // Set color mask
    #[Swag.Overload]
    mtd setColorMask(r, g, b, a: bool)
    {
        let cmd = me.newCommand(.ColorMask, true)
        cmd.colorMask.r       = r
        cmd.colorMask.g       = g
        cmd.colorMask.b       = b
        cmd.colorMask.a       = a
        me.curState.colorMask = cmd.colorMask
    }

    mtd setColorMaskColor() = me.setColorMask(true, true, true, false)
    mtd setColorMaskFull()  = me.setColorMask(true, true, true, true)
    mtd setColorMaskAlpha() = me.setColorMask(false, false, false, true)

    // Bind a render target
    mtd bindRenderTarget(tgt: *RenderTarget, paintAlpha = false)
    {
        me.pushState()
        let cmd = me.newCommand(.BindRenderTarget, true)
        cmd.renderTgt.tgt        = tgt
        cmd.renderTgt.paintAlpha = paintAlpha
        me.resetState()

        if me.curRT:
            me.stackRT.add(me.curRT)
        me.curRT = tgt
    }

    // Bind a render target
    mtd unbindRenderTarget()
    {
        discard me.newCommand(.UnbindRenderTarget, true)
        me.popState()

        if me.stackRT.count:
            me.curRT = me.stackRT.popBack()
        else
        {
            Debug.assert(me.curRT != null)
            me.curRT = null
        }
    }

    // Release the render target
    mtd releaseRenderTarget(tgt: *RenderTarget)
    {
        if !tgt:
            return
        let cmd = me.newCommand(.ReleaseRenderTarget)
        cmd.renderTgt.tgt = tgt
    }

    // Set the current shader
    mtd setShader(shader: *ShaderBase = null)
    {
        let cmd = me.newCommand(.SetShader, true)
        cmd.shader.shader = shader
    }

    // Set a shader parameter
    #[Swag.Overload]
    mtd setShaderParam(param: ShaderParamHandle, val: s32)
    {
        let cmd = me.newCommand(.SetShaderParam)
        cmd.shaderParam.param = param
        cmd.shaderParam.type  = s32
        cmd.shaderParam._s32  = val
    }

    // Set a shader parameter
    #[Swag.Overload]
    mtd setShaderParam(param: ShaderParamHandle, val: f32)
    {
        let cmd = me.newCommand(.SetShaderParam)
        cmd.shaderParam.param = param
        cmd.shaderParam.type  = f32
        cmd.shaderParam._f32  = val
    }
}
