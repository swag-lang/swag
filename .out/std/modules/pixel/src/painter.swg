#global public
using Core, Ogl, Core.Drawing, Core.Math

struct VertexLayout
{
    pos:    Vector2
    color:  u32
    aa0:    Vector4
    aa1:    Vector4
    aa2:    Vector4
    aa3:    Vector4
    aa4:    Vector4
    aa5:    Vector4
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
    commandBuffer:  Array'Command
    vertexBuffer:   Array'VertexLayout
    drawingBegin:   bool
    drawingPath:    LinePath
    paintParams:    PaintParams = Default
    paintQuality:   PaintQuality = Good
}

impl Painter
{
    func addEdgeAA(tri: *VertexLayout, idx: u32, start, end: Vector2)
    {
        loop i: 3
        {
            ptr := tri + i
            switch idx
            {
            case 0: ptr.aa0.x = start.x; ptr.aa0.y = start.y; ptr.aa0.z = end.x; ptr.aa0.w = end.y;
            case 1: ptr.aa1.x = start.x; ptr.aa1.y = start.y; ptr.aa1.z = end.x; ptr.aa1.w = end.y;
            case 2: ptr.aa2.x = start.x; ptr.aa2.y = start.y; ptr.aa2.z = end.x; ptr.aa2.w = end.y;
            case 3: ptr.aa3.x = start.x; ptr.aa3.y = start.y; ptr.aa3.z = end.x; ptr.aa3.w = end.y;
            case 4: ptr.aa4.x = start.x; ptr.aa4.y = start.y; ptr.aa4.z = end.x; ptr.aa4.w = end.y;
            case 5: ptr.aa5.x = start.x; ptr.aa5.y = start.y; ptr.aa5.z = end.x; ptr.aa5.w = end.y;
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
}