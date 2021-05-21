#global public
using Core.Drawing, Core.Math

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

    func drawEmptyRect(using self, left, top, right, bottom: f32, pen: Pen)
    {
        drawingPath.clear()
        drawingPath.moveTo(@{left, top})
        drawingPath.lineTo(@{right, top})
        drawingPath.lineTo(@{right, bottom})
        drawingPath.lineTo(@{left, bottom})
        drawingPath.lineTo(@{left, top})
        drawingPath.close()
        drawPath(drawingPath, pen)
    }
}