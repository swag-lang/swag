#global public
using Core, Core.Math, Core.Drawing

private struct Context
{
    posBuffer:      Array'Vector2
    size2:          f32
    beginLine:      bool
    firstLine:      bool
    adapt:          bool
    normalPrev:     Vector2
    normalFirst:    Vector2
    normal:         Vector2
    prim1, prim2:   uint
    prev:           Vector2
    path:           const *LinePath
    pen:            const *Pen

    remainingPatternLen: f32
    patternTotalLen:     f32
    patternIndex:        s32
    error:               f32
    divError:            f32
    patternCount:        s32
}

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
        case Draft:     step = cast(s32) (fstep / 8)
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
        case Draft:     step = cast(s32) (fstep / 8)
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

    private func drawJointMiter(using self, p, d1, d2: Vector2, justMiter: bool, pen: Pen)
    {
        a := Vector2{d1.x + d2.x, d1.y + d2.y}
        w := d1.length()
        l := a.length()
        if Math.isZeroEpsilon(l) return

        m2 := (2 * w) / l

        if m2 > pen.jointMiterLimit
        {
            if justMiter
                m2 = pen.jointMiterLimit
            else
            {
                drawJointBevel(p, d1, d2, pen)
                return
            }
        }

        a.x = a.x / l * m2 * w
        a.y = a.y / l * m2 * w

        v0 := p

        var v1, v2, v3: Vector2 = ?
        if d1.x * d2.y - d1.y * d2.x < 0
        {
            v0 += d1 * 0.25
            v0 += d2 * 0.25
            v1 = Vector2{p.x - d1.x, p.y - d1.y}
            v2 = Vector2{p.x - a.x, p.y - a.y}
            v3 = Vector2{p.x - d2.x, p.y - d2.y}
        }
        else
        {
            v0 -= d1 * 0.25
            v0 -= d2 * 0.25
            v1 = Vector2{p.x + d1.x, p.y + d1.y}
            v2 = Vector2{p.x + a.x, p.y + a.y}
            v3 = Vector2{p.x + d2.x, p.y + d2.y}
        }

        prim1 := pushTriangle(v0, v1, v3, pen.color)
        prim2 := pushTriangle(v1, v3, v2, pen.color)
        if paintParams & .Antialiased
        {
            addEdgeAA(prim1, v3, v2)
            addEdgeAA(prim1, v1, v2)
            addEdgeAA(prim2, v3, v2)
            addEdgeAA(prim2, v1, v2)
        }
    }

    private func drawJoint(using self, jointStyle: JointStyle, p, d1, d2: Vector2, pen: Pen)
    {
        #[Swag.complete]
        switch jointStyle
        {
        case None:          break
        case Bevel:         drawJointBevel(p, d1, d2, pen)
        case Round:         drawJointRound(p, d1, d2, pen)
        case Miter:         drawJointMiter(p, d1, d2, true, pen)
        case MiterBevel:    drawJointMiter(p, d1, d2, false, pen)
        }
    }

    private func drawEndCap(using self, cxt: *Context, posCap, normalCap: Vector2, capStyle: CapStyle)
    {
        using cxt
        if !prim1 or !prim2 return
        if beginLine return

        // Cap is defined
        if capStyle != CapStyle.None
            drawCap(capStyle, posCap, normalCap, -1.0, dref pen)

        // AA end edge if no cap
        else if paintParams & .Antialiased
        {
            prim2p := vertexBuffer.buffer + prim2
            prim2t := prim2p + 2
            addEdgeAA(prim1, prim2p.pos, prim2t.pos)
            addEdgeAA(prim2, prim2p.pos, prim2t.pos)
        }
    }

    private func drawMoveCursor(using self, cxt: *Context, pos: Vector2)
    {
        using cxt

        if pos.distance(path.endPoint) < 1
        {
            prev = path.endPoint
            if path.isClosed
                beginLine = true
        }
        else
        {
            prev = pos
            beginLine = true
        }
    }

    private func drawLineInternal(using self, cxt: *Context, pos: Vector2, capStyle: CapStyle, drawCap: bool)
    {
        using cxt

        // Compute line (half width on each side)
        newPos := pos
        n  := newPos - prev
        n2 := normal

        // If previous and and next point are too close, we take
        // the last normal
        if !n.isZeroEpsilon()
        {
            n.normalize()
            normal = n.cross(1)
            normal *= size2
            n2 = normal
        }

        // If this is still invalid, there's nothing we can do
        if normal.isZeroEpsilon()
            return

        // Correction when size is not divisible by 2
        if adapt
        {
            if path.isClosed
                n2 *= 0.5
            else
                n2 = Vector2{-0.5, -0.5}
            newPos -= n2
            if firstLine prev -= n2
        }

        // First line cap
        if capStyle != CapStyle.None and beginLine and drawCap
            drawCap(capStyle, prev, normal, 1.0, dref pen)

        // Joints
        if !beginLine
            drawJoint(pen.jointStyle, prev, normalPrev, normal, dref pen)

        // Draw quad
        v0 := prev - normal
        v1 := newPos - normal
        v3 := prev + normal
        v2 := newPos + normal
        prim1 = pushTriangle(v0, v1, v3, pen.color)
        prim2 = pushTriangle(v1, v3, v2, pen.color)

        // Generate AA edges
        if paintParams & .Antialiased
        {
            addEdgeAA(prim1, v0, v1)
            addEdgeAA(prim1, v3, v2)
            addEdgeAA(prim2, v0, v1)
            addEdgeAA(prim2, v3, v2)

            if capStyle == CapStyle.None and beginLine and drawCap
            {
                addEdgeAA(prim1, v0, v3)
                addEdgeAA(prim2, v0, v3)
            }
        }

        // To draw end joint
        if firstLine normalFirst = normal

        normalPrev = normal
        prev = newPos
        beginLine, firstLine = false
    }

    private func preparePattern(cxt: *Context)
    {
        using cxt

        patternTotalLen = 0
        visit p: pen.pattern
            patternTotalLen += p
        patternTotalLen *= pen.size
        patternCount = cast(s32) pen.pattern.count

        repeatCount := cast(s32) (path.totalLen / patternTotalLen)
        error = path.totalLen - (repeatCount * patternTotalLen)

        // First pattern. We start at half the size of the first pattern, in order to not
        // finish on a blank, but with half the size of the first pattern also.
        patternIndex = 0
        remainingPatternLen = pen.pattern.buffer[patternIndex]
        remainingPatternLen *= pen.size
        if !path.isClosed
            remainingPatternLen *= 0.5

        // We must dispatch error at each blank in the pattern. So "divError" is the
        // number of blanks to add at each 'move', between solid patterns
        numBlanks := repeatCount * patternCount / 2
        divError = error / numBlanks
    }

    private func drawLine(using self, cxt: *Context, pos: Vector2)
    {
        using cxt

        // solid
        if pen.penStyle == .Solid
        {
            drawLineInternal(cxt, pos, pen.beginCapStyle, !path.isClosed)
            return
        }

        v := pos - prev
        remainingSegLen := v.length()
        v.normalize()

        // True when we are "drawing" blanks
        blankPatternIdx := patternIndex & 1 ? true : false

        while true
        {
            lengthInPattern := remainingPatternLen

            // If we are drawing a blank, take care of error adjustement
            addBlank := 0'f32
            if divError > 0 and blankPatternIdx
            {
                addBlank = Math.min(error, divError)
                lengthInPattern += addBlank
            }

            // The current remaining length in the current segment is greater
            // than what's left in the current pattern. We draw to cover the
            // current pattern, and change the pattern to be the next
            if remainingSegLen > lengthInPattern
            {
                nextp := prev + (v * lengthInPattern)
                if blankPatternIdx
                {
                    drawEndCap(cxt, prev, normal, pen.patternEndCapStyle)
                    drawMoveCursor(cxt, nextp)
                    error -= addBlank
                }
                else
                {
                    capStyle := firstLine and !path.isClosed ? pen.beginCapStyle : pen.patternBeginCapStyle
                    drawLineInternal(cxt, nextp, capStyle, true)
                }

                // Start a new fresh pattern index
                remainingSegLen -= lengthInPattern
                patternIndex = (patternIndex + 1) % patternCount
                blankPatternIdx = blankPatternIdx ? false : true
                remainingPatternLen = pen.pattern.buffer[patternIndex] * pen.size
                continue
            }

            // Here the segment does not have enough remaining length to cover the current
            // pattern. So draw what remains, and exit. The next line will try to cover the rest.
            remainingPatternLen -= remainingSegLen
            if blankPatternIdx
            {
                drawEndCap(cxt, prev, normal, pen.patternEndCapStyle)
                drawMoveCursor(cxt, pos)
            }
            else
            {
                capStyle := firstLine and !path.isClosed ? pen.beginCapStyle : pen.patternBeginCapStyle
                drawLineInternal(cxt, pos, capStyle, true)
            }
            break
        }
    }

    private func drawPath(using self, cxt: *Context)
    {
        using cxt
        if pen.size == 0 or path.posBuffer.count == 0
            return

        // AA will reduce visually the size. Compensate
        size := pen.size
        if paintParams & .Antialiased
            size += 0.5

        // Correction
        adapt = false
        if cast(u32) size & 1
            adapt = true

        size2       = size * 0.5
        prev        = path.startPoint
        beginLine   = true
        firstLine   = true

        // Draw all
        visit p: path.posBuffer
            drawLine(cxt, dref p)

        // End line cap
        if !path.isClosed
            drawEndCap(cxt, prev, normal, pen.endCapStyle)

        // Closing join between the end and the beginning in case of a closed path
        if path.isClosed and !firstLine and !beginLine
            drawJoint(pen.jointStyle, prev, normal, normalFirst, dref pen)
    }

    // Draw the line path with the given 'pen'
    func drawPath(using self, path: *LinePath, pen: Pen)
    {
        path.flatten(paintQuality)

        var cxt: Context
        cxt.path = path
        cxt.pen =  &pen
        if pen.penStyle == .Pattern preparePattern(&cxt)
        drawPath(&cxt)
    }

    // Draw the line path with the given 'pen'
    func fillPath(using self, path: *LinePath, brush: Brush)
    {
        path.flatten(paintQuality)

        using path
        center := Vector2{bvMin.x + (bvMax.x - bvMin.x) * 0.5, bvMin.y + (bvMax.y - bvMin.y) * 0.5}
        visit pos, i: posBuffer
        {
            v0 := dref pos
            v1 := posBuffer.buffer[(i + 1) % posBuffer.count]
            prim := pushTriangle(v0, v1, center, brush.color)
            if paintParams & .Antialiased
            {
                vp := posBuffer.buffer[(i + 2) % posBuffer.count]
                v2 := posBuffer.buffer[(i + posBuffer.count - 1) % posBuffer.count]
                addEdgeAA(prim, v0, v1)
                addEdgeAA(prim, v1, v2)
                addEdgeAA(prim, vp, v0)
            }
        }
    }
}