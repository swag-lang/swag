#global public
using core, mthx

struct VertexLayout
{
    pos:    Vector3
    color:  Vector3
}

typeset Command
{
    Clear{}
    DrawTriangles{start: u32, count: u32}
}

struct DrawingContext
{
    commandBuffer:  Array'u32
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

    func draw(using self)
    {
        debug.assert(drawingBegin)

        ptr := vertexBuffer.emplaceAddress(3)

        ptr.pos.x = -1; ptr.pos.y = -1; ptr.pos.z = 0; ptr.color.x = 0; ptr.color.y = 0; ptr.color.z = 1
        ptr += 1

        ptr.pos.x = 1; ptr.pos.y = -1; ptr.pos.z = 0;  ptr.color.x = 0; ptr.color.y = 1; ptr.color.z = 0
        ptr += 1

        ptr.pos.x = 0; ptr.pos.y = 1; ptr.pos.z = 0;   ptr.color.x = 1; ptr.color.y = 0; ptr.color.z = 0
    }
}