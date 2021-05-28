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
    BindImage
}

struct Texture
{
    handle:             TextureHandle
    pf:                 PixelFormat = ?
    width, height: s32
}

struct Command
{
    id: CommandId = ?
    params: union
    {
        clear:          struct{color: Color; flags: u32;}
        drawTriangles:  struct{start: u32; count: u32;}
        transform:      struct{tr: Transform2;}
        image:          struct{handle: TextureHandle;}
    }
}

#[Swag.enumflags]
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
    Good
    High
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
    paintQuality:       PaintQuality = High
    drawingBegin:       bool
    transform:          Transform2
}

impl Painter
{
    func pushTriangle(using self, p1, p2, p3: Vector2, color: Color)->uint
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

        cmd.params.drawTriangles.count += 3

        ptr := vertexBuffer.emplaceAddress(3)

        ptr.pos = p1; ptr.color = color.argb; ptr.aanum = 0
        ptr += 1
        ptr.pos = p2; ptr.color = color.argb; ptr.aanum = 0
        ptr += 1
        ptr.pos = p3; ptr.color = color.argb; ptr.aanum = 0

        return vertexBuffer.count - 3
    }

    func addEdgeAA(using self, tri: uint, start, end: Vector2)
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

    #[Swag.inline]
    func newCommand(using self, cmd: CommandId, replaceLast = false)->*Command
    {
        Debug.assert(drawingBegin)

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
        cmd.params.clear.flags = flags
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
}