#global public
using core, mthx

struct VertexLayout
{
    pos:    Vector3
    color:  Vector3
}

enum CommandId
{
    DrawTriangles
}

struct Command
{
    id: CommandId = ?
    //union params
    {
        drawTriangles = @{start: 0'u32, count: 0'u32}
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

    func drawSolidTriangle(using self, p1, p2, p3: Vector3, c1, c2, c3: Vector3)
    {
        debug.assert(drawingBegin)

        cmd := newCommand(CommandId.DrawTriangles)
        cmd.drawTriangles.start = cast(u32) vertexBuffer.count
        cmd.drawTriangles.count = 3

        ptr := vertexBuffer.emplaceAddress(3)
        ptr.pos = p1; ptr.color = c1
        ptr += 1
        ptr.pos = p2; ptr.color = c2
        ptr += 1
        ptr.pos = p3; ptr.color = c3
    }
}