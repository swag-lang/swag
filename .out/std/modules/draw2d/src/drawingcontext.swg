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
        var v: VertexLayout
        vertexBuffer.clear()
        v.pos.x = -1; v.pos.y = -1; v.pos.z = 0; v.color.x = 0; v.color.y = 0; v.color.z = 1; vertexBuffer.add(v)
        v.pos.x = 1; v.pos.y = -1; v.pos.z = 0;  v.color.x = 0; v.color.y = 1; v.color.z = 0; vertexBuffer.add(v)
        v.pos.x = 0; v.pos.y = 1; v.pos.z = 0;   v.color.x = 1; v.color.y = 0; v.color.z = 0; vertexBuffer.add(v)
    }
}