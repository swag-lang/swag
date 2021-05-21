#global public
using Core, Core.Math, Core.Drawing

impl Painter
{
    private func pushTriangle(using self, p1, p2, p3: Vector2, color: Color)->*VertexLayout
    {
        cmd := newCommand(CommandId.DrawTriangles)
        cmd.params.drawTriangles.start = cast(u32) vertexBuffer.count
        cmd.params.drawTriangles.count = 3

        ptr := vertexBuffer.emplaceAddress(3)
        var result: retval = ptr

        Memory.clear(ptr, 3'uint * @sizeof(VertexLayout))
        ptr.pos = p1; ptr.color = color
        ptr += 1
        ptr.pos = p2; ptr.color = color
        ptr += 1
        ptr.pos = p3; ptr.color = color

        return result
    }

    private func drawCapTriangle(using self, p, d: Vector2, sens: f32, pen: Pen)
    {
        n  := Vector2{d.y * sens, -d.x * sens}
        v0 := Vector2{p.x + d.x, p.y + d.y}
        v1 := Vector2{p.x - d.x, p.y - d.y}
        v2 := Vector2{p.x + n.x, p.y + n.y}
        prim1 := pushTriangle(v0, v1, v2, pen.color)

        if paintParams & .Antialiased
        {
            addEdgeAA(prim1, 0, v1, v2)
            addEdgeAA(prim1, 1, v0, v2)
        }
    }

    private func drawCapRound(using self, p, d: Vector2, sens: f32, pen: Pen)
    {
        radius := d.length()
        if radius < 2.5
        {
            drawCapTriangle(p, d, sens, pen)
            return
        }

        v0 := p
        v1 := Vector2{p.x + d.x, p.y + d.y}

        angle0 := Math.atan2(-d.y, -d.x)
        angle1 := angle0 + Math.ConstF32.Pi
        fstep  := Math.ConstF32.Pi * radius

        step := 0
        step = cast(s32) fstep
        if step < 3 step = 3
        radius *= sens

        var v2: Vector2 = ?
        loop i: step + 1
        {
            t := angle0 + ((angle1 - angle0) * i) / step
            v2.x = p.x + (Math.cos(t) * radius)
            v2.y = p.y + (Math.sin(t) * radius)
            prim1 := pushTriangle(v0, v1, v2, pen.color)
            if paintParams & .Antialiased
                addEdgeAA(prim1, 0, v1, v2)
            v1 = v2
        }
    }

    private func drawCap(using self, capStyle: CapStyle, p, d: Vector2, sens: f32, pen: Pen)
    {
        #[Swag.complete]
        switch capStyle
        {
        case None:      break
        case Triangle:  drawCapTriangle(p, d, sens, pen)
        case Round:     drawCapRound(p, d, sens, pen)
        }
    }

    // Draw the line path with the given 'pen'
    func drawPath(using self, path: LinePath, pen: Pen, edge = false)
    {
        if pen.size == 0 or path.elements.count == 0
            return

        size := pen.size

        // AA will reduce visually the size. Compensate
        if paintParams & .Antialiased
        {
            if edge
                size += 1.5
            else
                size += 0.5
        }

        size2 := size * 0.5

        var vPrevN1, vN1: Vector2
        var prim1, prim2: *VertexLayout

        vPrev, vPrevNc       := path.startPoint
        beginLine, firstLine := true

        visit elem: path.elements
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
            if !path.isClosed and firstLine
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
            prim1 = pushTriangle(v0, v1, v3, pen.color)
            prim2 = pushTriangle(v1, v3, v2, pen.color)

            if paintParams & .Antialiased
            {
                addEdgeAA(prim1, 0, v3, v2)
                addEdgeAA(prim2, 0, v3, v2)
                if !edge
                {
                    addEdgeAA(prim1, 1, v0, v1)
                    addEdgeAA(prim2, 1, v0, v1)
                }
            }

            vPrevN1 = vN1
            vPrev = vNew
            vPrevNc = vNewNc
            beginLine, firstLine = false
        }

        // End line cap
        flatBegin := false
        if prim1 and prim2 and !path.isClosed and !firstLine and !beginLine
        {
            if pen.endCapStyle == CapStyle.None
                flatBegin = true
            else
            {
                vPrev = path.endPoint
                drawCap(pen.endCapStyle, vPrev, vN1, -1.0, pen)
                flatBegin = false
            }
        }
    }
}