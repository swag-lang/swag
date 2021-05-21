#global public
using Core.Drawing, Core.Math

impl Painter
{
    func drawSolidTriangle(using self, p1, p2, p3: Vector2, color: Color)
    {
        cmd := newCommand(CommandId.DrawTriangles)
        cmd.params.drawTriangles.start = cast(u32) vertexBuffer.count
        cmd.params.drawTriangles.count = 3

        ptr := vertexBuffer.emplaceAddress(3)
        ptr.pos = p1; ptr.color = color.argb
        ptr += 1
        ptr.pos = p2; ptr.color = color.argb
        ptr += 1
        ptr.pos = p3; ptr.color = color.argb
    }

    func drawSolidTriangle(using self, p1, p2, p3: Vector2, c1, c2, c3: Color)
    {
        cmd := newCommand(CommandId.DrawTriangles)
        cmd.params.drawTriangles.start = cast(u32) vertexBuffer.count
        cmd.params.drawTriangles.count = 3

        ptr := vertexBuffer.emplaceAddress(3)
        ptr.pos = p1; ptr.color = c1.argb
        ptr += 1
        ptr.pos = p2; ptr.color = c2.argb
        ptr += 1
        ptr.pos = p3; ptr.color = c3.argb
    }
}