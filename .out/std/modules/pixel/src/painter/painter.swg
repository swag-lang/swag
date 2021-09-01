#global public
using Core, Ogl, Core.Math

#[Swag.Pack(1)]
struct VertexLayout
{
    pos:    Vector2
    uv:     Vector2
    color:  u32
    aa:     [6] Vector4
    aanum:  f32
}

enum CommandId
{
    Clear
    Transform
    DrawTriangles
    BindTexture0
    BindTexture1
    ResetTexture0
    ResetTexture1
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
}

#[Swag.EnumFlags]
enum PaintParams
{
    Normal
    Antialiased
    Default = Normal
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

struct Command
{
    id: CommandId = ?
    using params: union
    {
        clear:          struct{color: Color; }
        drawTriangles:  struct{start: u32; count: u32; }
        transform:      struct{tr: Transform2; }
        clippingRect:   struct{rect: Rectangle; }
        clippingRegion: struct{mode: ClippingMode = ?; }
        font:           struct{fontRef: *Font; }
        texture1:       struct{texture: Texture; }
        blendingMode:   struct{mode: BlendingMode = ?; }

        texture0: struct
                {
                    boundRect:          Vector4
                    textureRect:        Vector4
                    type:               BrushType = ?
                    hatch:              HatchStyle = ?
                    uvMode:             UVMode = ?
                    interpolationMode:  InterpolationMode = ?
                    texture:            Texture
                }
    }
}

struct Painter
{
    commandBuffer:      Array'Command
    vertexBuffer:       Array'VertexLayout
    triangulateIdx:     Array's32

    sharedSolidPen:     Pen
    sharedSolidBrush:   Brush
    sharedLinePath:     LinePath

    paintParams:        PaintParams = Default
    paintQuality:       PaintQuality = Normal
    interpolationMode:  InterpolationMode = Pixel
    lastTexture0Type:   BrushType = SolidColor
    blendingMode:       BlendingMode = Alpha

    transform:          Transform2
    clippingRect:       Rectangle
    lastTexture0Handle: TextureHandle
    lastTexture1Handle: TextureHandle

    drawingBegin:       bool
    clippingRectOn:     bool
    overlapMode:        bool
    canSetTexture0:     bool = true
}

impl Painter
{
    protected func getTextureSize(brush: Brush)->{w: s32, h: s32}
    {
        var result: retval
        var wt, ht: s32 = ?

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

    protected mtd resetTexture0()
    {
        if lastTexture0Handle or lastTexture0Type != .SolidColor
        {
            lastTexture0Handle = cast(TextureHandle) 0
            lastTexture0Type = .SolidColor
            discard newCommand(CommandId.ResetTexture0, true)
        }
    }

    protected mtd resetTexture1()
    {
        if lastTexture1Handle
        {
            lastTexture1Handle = cast(TextureHandle) 0
            discard newCommand(CommandId.ResetTexture1, true)
        }
    }

    protected mtd setTexture0(bvMin, bvMax: Vector2, brush: Brush)
    {
        if !canSetTexture0
            return
        if brush.type == .SolidColor
        {
            resetTexture0()
            return
        }

        cmd := newCommand(CommandId.BindTexture0, true)
        params := &cmd.texture0

        params.boundRect.x = bvMin.x
        params.boundRect.y = bvMin.y
        params.boundRect.z = bvMax.x
        params.boundRect.w = bvMax.y

        (wt, ht) := getTextureSize(brush)
        if brush.uvMode == .FillSubRect
        {
            params.textureRect.x = brush.uvRect.x / wt
            params.textureRect.y = brush.uvRect.y / ht
            params.textureRect.z = brush.uvRect.right() / wt
            params.textureRect.w = brush.uvRect.bottom() / ht
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
        params.interpolationMode = interpolationMode

        lastTexture0Handle = brush.texture.handle
        lastTexture0Type   = brush.type
    }

    protected mtd setTexture1(texture: Texture)
    {
        if lastTexture1Handle == texture.handle
            return
        cmd := newCommand(CommandId.BindTexture1, true)
        params := &cmd.texture1
        params.texture = texture
        lastTexture1Handle = texture.handle
    }

    protected mtd pushTriangleCmd()->*Command
    {
        var cmd: *Command
        if commandBuffer.count
        {
            last := commandBuffer.buffer + commandBuffer.count - 1
            if last.id == CommandId.DrawTriangles
                cmd = last
        }

        if !cmd
        {
            cmd = newCommand(CommandId.DrawTriangles)
            cmd.drawTriangles.start = cast(u32) vertexBuffer.count
            cmd.drawTriangles.count = 0
        }

        return cmd
    }

    protected mtd pushTriangle(p1, p2, p3: Vector2, uv1, uv2, uv3: Vector2, color: Color)->uint
    {
        cmd := pushTriangleCmd()
        cmd.drawTriangles.count += 3

        ptr := vertexBuffer.emplaceAddress(3)
        ptr.pos = p1; ptr.color = color.argb; ptr.aanum = 0; ptr.uv = uv1
        ptr += 1
        ptr.pos = p2; ptr.color = color.argb; ptr.aanum = 0; ptr.uv = uv2
        ptr += 1
        ptr.pos = p3; ptr.color = color.argb; ptr.aanum = 0; ptr.uv = uv3

        return vertexBuffer.count - 3
    }

    protected mtd pushTriangle(p1, p2, p3: Vector2, color: Color)->uint
    {
        cmd := pushTriangleCmd()
        cmd.drawTriangles.count += 3

        ptr := vertexBuffer.emplaceAddress(3)
        ptr.pos = p1; ptr.color = color.argb; ptr.aanum = 0
        ptr += 1
        ptr.pos = p2; ptr.color = color.argb; ptr.aanum = 0
        ptr += 1
        ptr.pos = p3; ptr.color = color.argb; ptr.aanum = 0

        return vertexBuffer.count - 3
    }

    protected mtd addEdgeAA(tri: uint, start, end: Vector2)
    {
        loop i: 3
        {
            ptr := vertexBuffer.buffer + tri + i
            idx := cast(s32) ptr.aanum
            if idx != 6
            {
                ptr.aa[idx].x = start.x
                ptr.aa[idx].y = start.y
                ptr.aa[idx].z = end.x
                ptr.aa[idx].w = end.y
                ptr.aanum += 1
            }
        }
    }

    #[Swag.Inline]
    protected mtd newCommand(cmd: CommandId, replaceLast = false)->*Command
    {
        Debug.assert(drawingBegin, "begin() not called before")

        // Take the previous command if it's the same
        if replaceLast and commandBuffer.count and commandBuffer.buffer[commandBuffer.count - 1].id == cmd
            return commandBuffer.buffer + commandBuffer.count - 1;

        ptr := commandBuffer.emplaceAddress(1)
        ptr.id = cmd
        return ptr
    }

    protected mtd updateFont(fontRef: *Font)
    {
        if !fontRef.dirty return
        cmd := newCommand(CommandId.UpdateFont, false)
        cmd.font.fontRef = fontRef
    }

    protected mtd startOverlapMode(pen: Pen)
    {
        alpha := false
        if pen.brush.color.argb & 0xFF000000 != 0xFF000000
            alpha = true
        if pen.brush.type == .Texture and pen.brush.texture.pf.hasAlpha()
            alpha = true
        if alpha and blendingMode == .Alpha
        {
            overlapMode = true
            discard newCommand(CommandId.StartNoOverlap, true)
        }
    }

    protected mtd stopOverlapMode()
    {
        if overlapMode
        {
            overlapMode = false
            discard newCommand(CommandId.StopNoOverlap, true)
        }
    }

    // Call this before drawing
    func begin(using self)
    {
        Debug.assert(drawingBegin == false)

        transform.setIdentity()
        commandBuffer.clear()
        vertexBuffer.clear()

        interpolationMode = .Pixel
        blendingMode      = .Alpha

        drawingBegin   = true
        canSetTexture0 = true
        overlapMode    = false

        paintParams  = .Default
        paintQuality = .Normal
    }

    // Call this after drawing
    mtd end()
    {
        Debug.assert(drawingBegin)
        drawingBegin = false
    }

    // Clear the rendering surface
    mtd clear(color: Color, flags: u32 = GL_COLOR_BUFFER_BIT)
    {
        cmd := newCommand(CommandId.Clear)
        cmd.clear.color = color
    }

    // Set the paint position
    mtd translateTransform(x, y: f32)
    {
        tr := Transform2.createTranslation(x, y)
        transform.multiply(tr)
        setTransform(transform)
    }

    // Set the paint rotation, in degrees
    mtd rotateTransform(angle: f32)
    {
        rad := Math.toRadians(angle)
        tr := Transform2.createRotation(rad, 0, 0)
        transform.multiply(tr)
        setTransform(transform)
    }

    // Set the paint rotation, in degrees
    mtd rotateTransform(angle: f32, centerX, centerY: f32)
    {
        rad := Math.toRadians(angle)
        tr := Transform2.createRotation(rad, centerX, centerY)
        transform.multiply(tr)
        setTransform(transform)
    }

    // Set the paint scale
    mtd scaleTransform(x, y: f32)
    {
        tr := Transform2.createScale(x, y, 0, 0)
        transform.multiply(tr)
        setTransform(transform)
    }

    // Reset the paint position, rotation, scale
    mtd resetTransform()
    {
        transform.setIdentity()
        setTransform(transform)
    }

    // Set the paint transformation
    mtd setTransform(tr: Transform2)
    {
        transform = tr

        cmd := newCommand(CommandId.Transform, true)
        cmd.transform.tr = tr
    }

    // Set the texture interpolation mode
    mtd setInterpolationMode(mode: InterpolationMode)
    {
        interpolationMode = mode
    }

    // Set the current clipping rectangle
    mtd setClippingRect(x, y, width, height: f32)
    {
        clippingRect = @{x, y, width, height}
        clippingRectOn = true
        cmd := newCommand(CommandId.SetClippingRect, true)
        cmd.clippingRect.rect = clippingRect
    }

    // Reset the current clipping rectangle
    mtd resetClippingRect()
    {
        clippingRectOn = false
        discard newCommand(CommandId.ResetClippingRect, true)
    }

    // Start painting in the clipping buffer
    mtd startClippingRegion(mode = ClippingMode.Set)
    {
        cmd := newCommand(CommandId.StartClippingRegion, true)
        cmd.clippingRegion.mode = mode
    }

    // Start painting in the clipping buffer
    mtd setClippingRegionMode(mode: ClippingMode)
    {
        cmd := newCommand(CommandId.SetClippingRegionMode, true)
        cmd.clippingRegion.mode = mode
    }

    // Stop painting in the clipping buffer. Back to normal.
    mtd endClippingRegion()
    {
        discard newCommand(CommandId.EndClippingRegion, true)
    }

    // Reset clipping buffer to its default value (no more clipping)
    mtd resetClippingRegion()
    {
        discard newCommand(CommandId.ResetClippingRegion, true)
    }

    // Set the current blending mode
    mtd setBlendingMode(mode: BlendingMode)
    {
        cmd := newCommand(CommandId.SetBlendingMode, true)
        cmd.blendingMode.mode = mode
        blendingMode = mode
    }
}