#global public
using core, mthx

struct VertexLayout
{
    pos:    Vector3
    color:  Vector3
}

enum CommandId
{
    Clear
    DrawTriangles
}

struct Command
{
    id: CommandId = ?
    params: union
    {
        clear:          struct{color: mthx.Vector3; flags: u32;}
        drawTriangles:  struct{start: u32; count: u32;}
    }
}

struct DrawingContext
{
    commandBuffer:  Array'Command
    vertexBuffer:   Array'VertexLayout
    drawingBegin:   bool
}

impl DrawingContext
{
    // Call this before drawing
    func begin(using self)
    {
        debug.assert(drawingBegin == false)
        drawingBegin = true
        commandBuffer.clear()
        vertexBuffer.clear()
    }

    // Call this after drawing
    func end(using self)
    {
        debug.assert(drawingBegin)
        drawingBegin = false
    }

    func newCommand(using self, cmd: CommandId)->*Command
    {
        ptr := commandBuffer.emplaceAddress(1)
        ptr.id = cmd
        return ptr
    }

    func clear(using self, color: Vector3, flags: u32 = ogl.GL_COLOR_BUFFER_BIT)
    {
        cmd := newCommand(CommandId.Clear)
        cmd.params.clear.color = color
        cmd.params.clear.flags = flags
    }

    func drawSolidTriangle(using self, p1, p2, p3: Vector3, c1, c2, c3: Vector3)
    {
        debug.assert(drawingBegin)

        cmd := newCommand(CommandId.DrawTriangles)
        cmd.params.drawTriangles.start = cast(u32) vertexBuffer.count
        cmd.params.drawTriangles.count = 3

        ptr := vertexBuffer.emplaceAddress(3)
        ptr.pos = p1; ptr.color = c1
        ptr += 1
        ptr.pos = p2; ptr.color = c2
        ptr += 1
        ptr.pos = p3; ptr.color = c3
    }
}