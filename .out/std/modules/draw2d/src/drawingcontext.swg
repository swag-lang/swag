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
}

struct DrawingContext
{
    vertexBuffer:   Array'VertexLayout
}

impl DrawingContext
{
    func draw(using self)
    {
        vertexBuffer.clear()
        ptr := vertexBuffer.emplaceAddress(3)

        ptr.pos.x = -1; ptr.pos.y = -1; ptr.pos.z = 0; ptr.color.x = 0; ptr.color.y = 0; ptr.color.z = 1
        ptr += 1

        ptr.pos.x = 1; ptr.pos.y = -1; ptr.pos.z = 0;  ptr.color.x = 0; ptr.color.y = 1; ptr.color.z = 0
        ptr += 1

        ptr.pos.x = 0; ptr.pos.y = 1; ptr.pos.z = 0;   ptr.color.x = 1; ptr.color.y = 0; ptr.color.z = 0
    }
}