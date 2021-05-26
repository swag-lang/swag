#global public
using Core, Ogl, Core.Drawing, Core.Math

struct VertexLayout
{
    pos:    Vector2
    color:  u32
    aa:     [6] Vector4
    aanum:  f32
}

enum CommandId
{
    Clear
    Position
    Rotation
    ResetTransform
    DrawTriangles
}

struct Command
{
    id: CommandId = ?
    params: union
    {
        clear:          struct{color: Color; flags: u32;}
        drawTriangles:  struct{start: u32; count: u32;}
        transform:      struct{pos: Vector2; angle: f32;}
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
    sharedLinePath:     LinePath
    paintParams:        PaintParams = Default
    paintQuality:       PaintQuality = High
    drawingBegin:       bool
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
    func newCommand(using self, cmd: CommandId)->*Command
    {
        Debug.assert(drawingBegin)
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
    func position(using self, pos: Vector2)
    {
        cmd := newCommand(CommandId.Position)
        cmd.params.transform.pos = pos
    }

    // Set the paint position
    func rotation(using self, angle: f32)
    {
        cmd := newCommand(CommandId.Rotation)
        cmd.params.transform.angle = Math.toRadians(angle)
    }

    // Reset the paint position, rotation, scale
    func resetTransform(using self)
    {
        cmd := newCommand(CommandId.ResetTransform)
    }
}