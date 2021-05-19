#global public
using core, mthx, ogl, core.drawing

struct VertexLayout
{
    pos:    Vector2
    color:  Color
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
        clear:          struct{color: Color; flags: u32;}
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
    #[swag.inline]
    private func newCommand(using self, cmd: CommandId)->*Command
    {
        debug.assert(drawingBegin)
        ptr := commandBuffer.emplaceAddress(1)
        ptr.id = cmd
        return ptr
    }

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

    func clear(using self, color: Color, flags: u32 = GL_COLOR_BUFFER_BIT)
    {
        cmd := newCommand(CommandId.Clear)
        cmd.params.clear.color = color
        cmd.params.clear.flags = flags
    }

    func drawSolidRect(using self, left, top, width, height: f32, color: Color)
    {
        cmd := newCommand(CommandId.DrawTriangles)
        cmd.params.drawTriangles.start = cast(u32) vertexBuffer.count
        cmd.params.drawTriangles.count = 6

        ptr := vertexBuffer.emplaceAddress(6)

        ptr.pos.x = left
        ptr.pos.y = top
        ptr.color = color
        ptr += 1

        ptr.pos.x = left + width
        ptr.pos.y = top
        ptr.color = color
        ptr += 1

        ptr.pos.x = left + width
        ptr.pos.y = top + height
        ptr.color = color
        ptr += 1

        ptr.pos.x = left
        ptr.pos.y = top
        ptr.color = color
        ptr += 1

        ptr.pos.x = left + width
        ptr.pos.y = top + height
        ptr.color = color
        ptr += 1

        ptr.pos.x = left
        ptr.pos.y = top + height
        ptr.color = color
    }

    func drawSolidTriangle(using self, p1, p2, p3: Vector2, c1, c2, c3: Color)
    {
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