#global public
using Core, Ogl, Core.Drawing, Core.Math

#[Swag.pack(1)]
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

struct Command
{
    id: CommandId = ?
    params: union
    {
        clear:          struct{color: Color; }
        drawTriangles:  struct{start: u32; count: u32; }
        transform:      struct{tr: Transform2; }
        clippingRect:   struct{rect: Rectangle; }
        clippingRegion: struct{mode: ClippingMode = ?; }
        font:           struct{fontRef: *Font; }

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
        texture1: struct
                {
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
    drawingBegin:       bool
    transform:          Transform2
    interpolationMode:  InterpolationMode = Pixel
    clippingRect:       Rectangle
    clippingRectOn:     bool
    lastTexture0Handle: TextureHandle
    lastTexture0Type:   BrushType = SolidColor
    lastTexture1Handle: TextureHandle
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

    protected func resetTexture0(using self)
    {
        if lastTexture0Handle or lastTexture0Type != .SolidColor
        {
            lastTexture0Handle = cast(TextureHandle) 0
            lastTexture0Type = .SolidColor
            discard newCommand(CommandId.ResetTexture0, true)
        }
    }

    protected func resetTexture1(using self)
    {
        if lastTexture1Handle
        {
            lastTexture1Handle = cast(TextureHandle) 0
            discard newCommand(CommandId.ResetTexture1, true)
        }
    }

    protected func setTexture0(using self, bvMin, bvMax: Vector2, brush: Brush)
    {
        if !canSetTexture0
            return
        if brush.type == .SolidColor
        {
            resetTexture0()
            return
        }

        cmd := newCommand(CommandId.BindTexture0, true)
        params := &cmd.params.texture0

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

    protected func setTexture1(using self, texture: Texture)
    {
        if lastTexture1Handle == texture.handle
            return
        cmd := newCommand(CommandId.BindTexture1, true)
        params := &cmd.params.texture1
        params.texture = texture
        lastTexture1Handle = texture.handle
    }

    protected func pushTriangleCmd(using self)->*Command
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
            cmd.params.drawTriangles.start = cast(u32) vertexBuffer.count
            cmd.params.drawTriangles.count = 0
        }

        return cmd
    }

    protected func pushTriangle(using self, p1, p2, p3: Vector2, uv1, uv2, uv3: Vector2, color: Color)->uint
    {
        cmd := pushTriangleCmd()
        cmd.params.drawTriangles.count += 3

        ptr := vertexBuffer.emplaceAddress(3)
        ptr.pos = p1; ptr.color = color.argb; ptr.aanum = 0; ptr.uv = uv1
        ptr += 1
        ptr.pos = p2; ptr.color = color.argb; ptr.aanum = 0; ptr.uv = uv2
        ptr += 1
        ptr.pos = p3; ptr.color = color.argb; ptr.aanum = 0; ptr.uv = uv3

        return vertexBuffer.count - 3
    }

    protected func pushTriangle(using self, p1, p2, p3: Vector2, color: Color)->uint
    {
        cmd := pushTriangleCmd()
        cmd.params.drawTriangles.count += 3

        ptr := vertexBuffer.emplaceAddress(3)
        ptr.pos = p1; ptr.color = color.argb; ptr.aanum = 0
        ptr += 1
        ptr.pos = p2; ptr.color = color.argb; ptr.aanum = 0
        ptr += 1
        ptr.pos = p3; ptr.color = color.argb; ptr.aanum = 0

        return vertexBuffer.count - 3
    }

    protected func addEdgeAA(using self, tri: uint, start, end: Vector2)
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
    protected func newCommand(using self, cmd: CommandId, replaceLast = false)->*Command
    {
        Debug.assert(drawingBegin, "begin() not called before")

        // Take the previous command if it's the same
        if replaceLast and commandBuffer.count and commandBuffer.buffer[commandBuffer.count - 1].id == cmd
            return commandBuffer.buffer + commandBuffer.count - 1;

        ptr := commandBuffer.emplaceAddress(1)
        ptr.id = cmd
        return ptr
    }

    // Call this before drawing
    func begin(using self)
    {
        Debug.assert(drawingBegin == false)
        drawingBegin = true
        paintParams = .Default
        transform.setIdentity()
        commandBuffer.clear()
        vertexBuffer.clear()
        interpolationMode = .Pixel
        canSetTexture0 = true
    }

    // Call this after drawing
    func end(using self)
    {
        Debug.assert(drawingBegin)
        drawingBegin = false
    }

    // Clear the rendering surface
    func clear(using self, color: Color, flags: u32 = GL_COLOR_BUFFER_BIT)
    {
        cmd := newCommand(CommandId.Clear)
        cmd.params.clear.color = color
    }

    // Set the paint position
    func translateTransform(using self, x, y: f32)
    {
        tr := Transform2.createTranslation(x, y)
        transform.multiply(tr)
        setTransform(transform)
    }

    // Set the paint rotation, in degrees
    func rotateTransform(using self, angle: f32)
    {
        rad := Math.toRadians(angle)
        tr := Transform2.createRotation(rad, 0, 0)
        transform.multiply(tr)
        setTransform(transform)
    }

    // Set the paint rotation, in degrees
    func rotateTransform(using self, angle: f32, centerX, centerY: f32)
    {
        rad := Math.toRadians(angle)
        tr := Transform2.createRotation(rad, centerX, centerY)
        transform.multiply(tr)
        setTransform(transform)
    }

    // Set the paint scale
    func scaleTransform(using self, x, y: f32)
    {
        tr := Transform2.createScale(x, y, 0, 0)
        transform.multiply(tr)
        setTransform(transform)
    }

    // Reset the paint position, rotation, scale
    func resetTransform(using self)
    {
        transform.setIdentity()
        setTransform(transform)
    }

    // Set the paint transformation
    func setTransform(using self, tr: Transform2)
    {
        transform = tr

        cmd := newCommand(CommandId.Transform, true)
        cmd.params.transform.tr = tr
    }

    // Set the texture interpolation mode
    func setInterpolationMode(using self, mode: InterpolationMode)
    {
        interpolationMode = mode
    }

    // Set the current clipping rectangle
    func setClippingRect(using self, x, y, width, height: f32)
    {
        clippingRect = @{x, y, width, height}
        clippingRectOn = true
        cmd := newCommand(CommandId.SetClippingRect, true)
        cmd.params.clippingRect.rect = clippingRect
    }

    // Reset the current clipping rectangle
    func resetClippingRect(using self)
    {
        clippingRectOn = false
        discard newCommand(CommandId.ResetClippingRect, true)
    }

    // Start painting in the clipping buffer
    func startClippingRegion(using self, mode = ClippingMode.Set)
    {
        cmd := newCommand(CommandId.StartClippingRegion, true)
        cmd.params.clippingRegion.mode = mode
    }

    // Start painting in the clipping buffer
    func setClippingRegionMode(using self, mode: ClippingMode)
    {
        cmd := newCommand(CommandId.SetClippingRegionMode, true)
        cmd.params.clippingRegion.mode = mode
    }

    // Stop painting in the clipping buffer. Back to normal.
    func endClippingRegion(using self)
    {
        discard newCommand(CommandId.EndClippingRegion, true)
    }

    // Reset clipping buffer to its default value (no more clipping)
    func resetClippingRegion(using self)
    {
        discard newCommand(CommandId.ResetClippingRegion, true)
    }

    func updateFont(using self, fontRef: *Font)
    {
        if !fontRef.dirty return
        cmd := newCommand(CommandId.UpdateFont, false)
        cmd.params.font.fontRef = fontRef
    }
}