#global public
using Core, mthx, Ogl, Core.Drawing

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
    drawingPath:    Path
}

impl DrawingContext
{
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
        commandBuffer.clear()
        vertexBuffer.clear()
    }

    // Call this after drawing
    func end(using self)
    {
        Debug.assert(drawingBegin)
        drawingBegin = false
    }

    func clear(using self, color: Color, flags: u32 = GL_COLOR_BUFFER_BIT)
    {
        cmd := newCommand(CommandId.Clear)
        cmd.params.clear.color = color
        cmd.params.clear.flags = flags
    }
}