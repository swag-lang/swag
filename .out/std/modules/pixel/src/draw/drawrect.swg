#global public
using Core.Drawing

impl Painter
{
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
}