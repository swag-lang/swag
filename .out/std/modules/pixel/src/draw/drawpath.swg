#global public
using Core, Core.Math, Core.Drawing

impl Painter
{
    private func pushTriangle(using self, p1, p2, p3: Vector2, color: Color)
    {
        cmd := newCommand(CommandId.DrawTriangles)
        cmd.params.drawTriangles.start = cast(u32) vertexBuffer.count
        cmd.params.drawTriangles.count = 3

        ptr := vertexBuffer.emplaceAddress(3)
        ptr.pos = p1; ptr.color = color
        ptr += 1
        ptr.pos = p2; ptr.color = color
        ptr += 1
        ptr.pos = p3; ptr.color = color
    }

    private func drawCapTriangle(using self, p, d: Vector2, sens: f32, pen: Pen)
    {
        n  := Vector2{d.y * sens, -d.x * sens}
        v0 := Vector2{p.x + d.x, p.y + d.y}
        v1 := Vector2{p.x - d.x, p.y - d.y}
        v2 := Vector2{p.x + n.x, p.y + n.y}
        pushTriangle(v0, v1, v2, pen.color)
    }

    private func drawCap(using self, capStyle: CapStyle, p, d: Vector2, sens: f32, pen: Pen)
    {
        switch capStyle
        {
        case Triangle: drawCapTriangle(p, d, sens, pen)
        }
    }

    func drawPath(using self, pth: LinePath, pen: Pen)
    {
        if pen.size == 0
            return
        size2 := pen.size * 0.5

        var vPrevN1:    Vector2
        var vN1:        Vector2

        vPrev, vPrevNc       := pth.startPoint
        beginLine, firstLine := true

        visit elem: pth.elements
        {
            if elem.type == .MoveTo
            {
                vPrev, vPrevNc = elem.position
                beginLine = true
                continue
            }

            // Compute line (half width on each side, except if edge)
            vNew, vNewNc := elem.position
            n := vNewNc - vPrevNc
            n.normalize()
            vN1 = n.cross(1)
            n2 := vN1
            vN1 *= size2

            // Begin line cap
            flatBegin := false
            if !pth.isClosed and firstLine
            {
                if (pen.beginCapStyle == CapStyle.None)
                    flatBegin = true
                else
                {
                    drawCap(pen.beginCapStyle, vPrev, vN1, 1.0, pen)
                    flatBegin = false
                }
            }

            // Draw quad
            v0 := vPrev - vN1
            v1 := vNew - vN1
            v3 := vPrev + vN1
            v2 := vNew + vN1
            pushTriangle(v0, v1, v3, pen.color)
            pushTriangle(v1, v3, v2, pen.color)

            vPrevN1 = vN1
            vPrev = vNew
            vPrevNc = vNewNc
            beginLine, firstLine = false
        }

        // End line cap
        flatBegin := false
        if !pth.isClosed and !firstLine and !beginLine
        {
            if pen.endCapStyle == CapStyle.None
                flatBegin = true
            else
            {
                vPrev = pth.endPoint
                drawCap(pen.endCapStyle, vPrev, vN1, -1.0, pen)
                flatBegin = false
            }
        }
    }
}