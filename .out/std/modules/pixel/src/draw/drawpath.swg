#global public
using Core, Core.Math, Core.Drawing

impl Painter
{
    private func drawCapSquare(using self, p, d: Vector2, sens: f32, pen: Pen)
    {
        n  := Vector2{d.y * sens, -d.x * sens}
        v0 := Vector2{p.x + d.x, p.y + d.y}
        v1 := Vector2{p.x - d.x, p.y - d.y}
        v2 := Vector2{v1.x + n.x, v1.y + n.y}
        v3 := Vector2{v0.x + n.x, v0.y + n.y}

        prim1 := pushTriangle(v0, v1, v3, pen.color)
        prim2 := pushTriangle(v1, v3, v2, pen.color)

        if paintParams & .Antialiased
        {
            addEdgeAA(prim1, v0, v3)
            addEdgeAA(prim1, v1, v2)
            addEdgeAA(prim2, v3, v2)
            addEdgeAA(prim2, v1, v2)
            addEdgeAA(prim2, v0, v3)
        }
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
            addEdgeAA(prim1, v1, v2)
            addEdgeAA(prim1, v0, v2)
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
        switch paintQuality
        {
        case Draft:     step = cast(s32) (fstep / 6)
        case Normal:    step = cast(s32) (fstep / 4)
        case Good:      step = cast(s32) (fstep / 2)
        case High:      step = cast(s32) fstep
        }

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
                addEdgeAA(prim1, v1, v2)
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
        case Square:    drawCapSquare(p, d, sens, pen)
        }
    }

    private func drawJointBevel(using self, p, d1, d2: Vector2, pen: Pen)
    {
        v0  := p

        var v1, v2: Vector2 = ?
        if d1.x * d2.y - d1.y * d2.x < 0
        {
            v0 += d1 * 0.25
            v0 += d2 * 0.25
            v1 = Vector2{p.x - d1.x, p.y - d1.y}
            v2 = Vector2{p.x - d2.x, p.y - d2.y}
        }
        else
        {
            v0 -= d1 * 0.25
            v0 -= d2 * 0.25
            v1 = Vector2{p.x + d1.x, p.y + d1.y}
            v2 = Vector2{p.x + d2.x, p.y + d2.y}
        }

        prim1 := pushTriangle(v0, v1, v2, pen.color)
        if paintParams & .Antialiased
            addEdgeAA(prim1, v1, v2)
    }

    private func drawJointRound(using self, p, d1, d2: Vector2, pen: Pen)
    {
        v0 := p

        var angle0, angle1: f32 = ?
        var v1, v2, vend: Vector2 = ?
        if d1.x * d2.y - d1.y * d2.x < 0
        {
            v0 += d1 * 0.25
            v0 += d2 * 0.25
            angle0 = Math.atan2(-d1.y, -d1.x)
            angle1 = Math.atan2(-d2.y, -d2.x)
            v1     = Vector2{p.x - d1.x, p.y - d1.y}
            vend   = Vector2{p.x - d2.x, p.y - d2.y}
        }
        else
        {
            v0 -= d1 * 0.25
            v0 -= d2 * 0.25
            angle0 = Math.atan2(d1.y, d1.x)
            angle1 = Math.atan2(d2.y, d2.x)
            v1     = Vector2{p.x + d1.x, p.y + d1.y}
            vend   = Vector2{p.x + d2.x, p.y + d2.y}
        }

        radius := d1.length()

        if angle1 > angle0 + Math.ConstF32.Pi
            angle1 -= Math.ConstF32.TwoPi
        else if angle1 < angle0 - Math.ConstF32.Pi
            angle1 += Math.ConstF32.TwoPi

        step  := 0'u32
        fstep := Math.abs(angle1 - angle0) * radius
        switch paintQuality
        {
        case Draft:     step = cast(s32) (fstep / 6)
        case Normal:    step = cast(s32) (fstep / 4)
        case Good:      step = cast(s32) (fstep / 2)
        case High:      step = cast(s32) fstep
        }

        if step < 3 step = 3
        loop i: step + 1
        {
            t := angle0 + ((angle1 - angle0) * i) / step
            v2.x = p.x + (Math.cos(t) * radius)
            v2.y = p.y + (Math.sin(t) * radius)
            prim1 := pushTriangle(v0, v1, v2, pen.color)
            if paintParams & .Antialiased
                addEdgeAA(prim1, v1, v2)
            v1 = v2
        }

        v2 = vend
        prim1 := pushTriangle(v0, v1, v2, pen.color)
        if paintParams & .Antialiased
            addEdgeAA(prim1, v1, v2)
    }

    private func drawJoint(using self, jointStyle: JointStyle, p, d1, d2: Vector2, pen: Pen)
    {
        #[Swag.complete]
        switch jointStyle
        {
        case None:          break
        case Bevel:         drawJointBevel(p, d1, d2, pen)
        case Round:         drawJointRound(p, d1, d2, pen)
        case Miter:         break
        case MiterBevel:    break
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

        var vPrevN1, vFirstN1, vN1: Vector2
        var prim1, prim2: *VertexLayout

        vPrev, vPrevNc       := path.startPoint
        beginLine, firstLine := true

        idxAA := 0'u32
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
                if pen.beginCapStyle == CapStyle.None
                    flatBegin = true
                else
                {
                    drawCap(pen.beginCapStyle, vPrev, vN1, 1.0, pen)
                    flatBegin = false
                }
            }

            // Joints
            if !beginLine
            {
                drawJoint(pen.jointStyle, vPrev, vPrevN1, vN1, pen)
            }

            // Draw quad
            v0 := vPrev - vN1
            v1 := vNew - vN1
            v3 := vPrev + vN1
            v2 := vNew + vN1
            prim1 = pushTriangle(v0, v1, v3, pen.color)
            prim2 = pushTriangle(v1, v3, v2, pen.color)

            // Generate AA edges
            if paintParams & .Antialiased
            {
                addEdgeAA(prim1, v3, v2)
                addEdgeAA(prim2, v3, v2)
                if !edge
                {
                    addEdgeAA(prim1, v0, v1)
                    addEdgeAA(prim2, v0, v1)
                }

                if flatBegin
                {
                    prim1t := prim1 + 2
                    addEdgeAA(prim1, prim1.pos, prim1t.pos);
                    addEdgeAA(prim2, prim1.pos, prim1t.pos);
                }
            }

            // To draw end joint
            if firstLine vFirstN1 = vN1

            vPrevN1 = vN1
            vPrev = vNew
            vPrevNc = vNewNc
            beginLine, firstLine = false
        }

        // End line cap
        flatEnd := false
        if prim1 and prim2 and !path.isClosed and !firstLine and !beginLine
        {
            if pen.endCapStyle == CapStyle.None
                flatEnd = true
            else
            {
                vPrev = path.endPoint
                drawCap(pen.endCapStyle, vPrev, vN1, -1.0, pen)
                flatEnd = false
            }
        }

        // Closing join between the end and the beginning in case of a closed path
        if path.isClosed and !firstLine and !beginLine
        {
            drawJoint(pen.jointStyle, vPrev, vN1, vFirstN1, pen)
        }

        // AA end edge if no cap
        if flatEnd and paintParams & .Antialiased
        {
            prim2t := prim2 + 2
            addEdgeAA(prim1, prim2.pos, prim2t.pos)
            addEdgeAA(prim2, prim2.pos, prim2t.pos)
        }
    }
}