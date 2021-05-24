#global public
using Core, Core.Math, Core.Drawing

#[Swag.enumflags]
private enum Flags
{
    Zero
    CollectPos
}

private struct Context
{
    posBuffer:      Array'Vector2
    size2:          f32
    flags:          Flags = Zero
    beginLine:      bool
    firstLine:      bool
    adapt:          bool
    normalPrev:     Vector2
    normalFirst:    Vector2
    normal:         Vector2
    prim1, prim2:   *VertexLayout
    prev:           Vector2
    path:           const *LinePath
    pen:            const *Pen
    bvMin:          Vector2
    bvMax:          Vector2
    totalLen:       f32

    remainingPatternLen: f32
    patternTotalLen:     f32
    patternIndex:        s32
    error:               f32
    divError:            f32
    calcError:           f32
    negError:            bool
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

    private func drawBezier(using self, cxt: *Context, pos, p1: Vector2)
    {
        using cxt

        v := pos - prev
        fstep := v.length()

        step := 0'u32
        switch paintQuality
        {
        case Draft:  step = cast(s32) (fstep / 8)
        case Normal: step = cast(s32) (fstep / 4)
        case Good:   step = cast(s32) (fstep / 2)
        case High:   step = cast(s32) fstep
        }

        if step < 3 step = 3

        startPt := prev
        loop i: 1..step
        {
            t  := i / cast(f32) step
            it := 1.0 - t
            a1 := it * it
            a2 := 2.0 * t * it
            a3 := t * t

            var point: Vector2 = ?
            point.x = a1 * startPt.x + a2 * p1.x + a3 * pos.x
            point.y = a1 * startPt.y + a2 * p1.y + a3 * pos.y
            drawLine(cxt, point)
        }
    }

    private func drawBezier(using self, cxt: *Context, pos, p1, p2: Vector2)
    {
        using cxt

        v := pos - prev
        fstep := v.length()

        step := 0'u32
        switch paintQuality
        {
        case Draft:  step = cast(s32) (fstep / 8)
        case Normal: step = cast(s32) (fstep / 4)
        case Good:   step = cast(s32) (fstep / 2)
        case High:   step = cast(s32) fstep
        }

        if step < 3 step = 3

        startPt := prev
        loop i: 1..step
        {
            t  := i / cast(f32) step
            it := 1.0 - t
            a1 := it * it * it
            a2 := 3.0 * t * it * it
            a3 := 3.0 * t * t * it
            a4 := t * t * t

            var point: Vector2 = ?
            point.x = a1 * startPt.x + a2 * p1.x + a3 * p2.x + a4 * pos.x
            point.y = a1 * startPt.y + a2 * p1.y + a3 * p2.y + a4 * pos.y
            drawLine(cxt, point)
        }
    }

    private func drawArc(using self, cxt: *Context, pos, p1, p2: Vector2)
    {
        fstep := Math.abs(p1.y - p1.x) * Math.max(p2.x, p2.y)
        step := 0'u32
        switch paintQuality
        {
        case Draft:  step = cast(s32) (fstep / 8)
        case Normal: step = cast(s32) (fstep / 4)
        case Good:   step = cast(s32) (fstep / 2)
        case High:   step = cast(s32) fstep
        }

        if step < 3 step = 3
        loop i: 1..step
        {
            t := p1.x + ((p1.y - p1.x) * i) / step
            v := pos
            v.x += (Math.sin(t) * p2.x)
            v.y += -(Math.cos(t) * p2.y)
            drawLine(cxt, v)
        }
    }

    private func drawEndCap(using self, cxt: *Context, posCap, normalCap: Vector2, capStyle: CapStyle)
    {
        using cxt
        if flags & .CollectPos return
        if !prim1 or !prim2 return

        // Cap is defined
        if capStyle != CapStyle.None
            drawCap(capStyle, posCap, normalCap, -1.0, dref pen)

        // AA end edge if no cap
        else if paintParams & .Antialiased
        {
            prim2t := prim2 + 2
            addEdgeAA(prim1, prim2.pos, prim2t.pos)
            addEdgeAA(prim2, prim2.pos, prim2t.pos)
        }
    }

    private func moveCursor(using self, cxt: *Context, pos: Vector2)
    {
        using cxt
        prev = pos
        beginLine = true
    }

    private func drawLineInternal(using self, cxt: *Context, pos: Vector2, capStyle: CapStyle, drawCap: bool)
    {
        using cxt

        // Compute line (half width on each side)
        newPos := pos
        n := newPos - prev
        n.normalizeSafe()
        normal = n.cross(1)
        n2 := normal
        normal *= size2

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
            patternTotalLen += cast(f32) p
        patternTotalLen *= pen.size

        error    = (totalLen / patternTotalLen)
        divError = Math.floor(error)
        error -= divError
        error *= patternTotalLen

        // Increase spaces between lines, or increase it ?
        negError = false
        if error > patternTotalLen / 2
        {
            error -= patternTotalLen
            negError = true
        }

        // We must dispatch error at each blank in the pattern. So "diverror" must be
        // "the number of blanks in the total line"
        patternCount = cast(s32) pen.pattern.count
        divError = Math.floor(totalLen / patternTotalLen)
        divError *= (patternCount / 2.0)
        calcError = error / divError

        // First pattern. We start at half the size of the first pattern, in order to not
        // finish on a blank, but with half the size of the first pattern also.
        patternIndex = 0
        remainingPatternLen = pen.pattern.buffer[patternIndex]
        remainingPatternLen *= pen.size * 0.5
    }

    private func drawLine(using self, cxt: *Context, pos: Vector2)
    {
        using cxt

        // Collect all segments in 'posBuffer'
        if flags & .CollectPos
        {
            totalLen += prev.distance(pos)
            bvMin.x = Math.min(bvMin.x, pos.x)
            bvMin.y = Math.min(bvMin.y, pos.y)
            bvMax.x = Math.max(bvMax.x, pos.x)
            bvMax.y = Math.max(bvMax.y, pos.y)
            posBuffer.add(pos)
            prev = pos
            return
        }

        // solid
        if pen.penStyle == .Solid
        {
            drawLineInternal(cxt, pos, pen.beginCapStyle, !path.isClosed)
            return
        }

        v := pos - prev
        restLen := v.length()
        v.normalize()
        while restLen > 0.1
        {
            len    := remainingPatternLen
            addLen := 0'f32
            if divError > 0 and patternIndex & 1
            {
                if negError
                    addLen = Math.max(error, calcError)
                else
                    addLen = Math.min(error, calcError)
                len += addLen
            }

            if len <= restLen
            {
                nextp := prev + (v * len)
                if !(patternIndex & 1)
                {
                    capStyle := firstLine and !path.isClosed ? pen.beginCapStyle : pen.patternBeginCapStyle
                    drawLineInternal(cxt, nextp, capStyle, true)
                }
                else
                {
                    drawEndCap(cxt, prev, normal, pen.patternEndCapStyle)
                    moveCursor(cxt, nextp)
                    error -= addLen
                }

                restLen -= len
                patternIndex = (patternIndex + 1) % patternCount
                remainingPatternLen   = cast(f32) (pen.pattern.buffer[patternIndex] * pen.size)
            }
            else
            {
                remainingPatternLen -= restLen
                if !(patternIndex & 1)
                {
                    capStyle := firstLine and !path.isClosed ? pen.beginCapStyle : pen.patternBeginCapStyle
                    drawLineInternal(cxt, pos, capStyle, true)
                }
                else
                {
                    drawEndCap(cxt, prev, normal, pen.patternEndCapStyle)
                    moveCursor(cxt, pos)
                }

                break
            }
        }
    }

    private func drawPathInternal(using self, cxt: *Context)
    {
        using cxt
        if pen.size == 0 or path.elements.count == 0
            return

        size := pen.size

        // AA will reduce visually the size. Compensate
        if paintParams & .Antialiased
            size += 0.5

        // Correction
        adapt = false
        if cast(u32) size & 1
            adapt = true

        bvMin.set(Swag.F32.Max, Swag.F32.Max)
        bvMax.set(Swag.F32.Min, Swag.F32.Min)
        posBuffer.clear()
        size2       = size * 0.5
        prev        = path.startPoint
        beginLine   = true
        firstLine   = true

        visit elem: path.elements
        {
            switch elem.type
            {
            case Line:
                drawLine(cxt, elem.position)
            case Bezier1:
                drawBezier(cxt, elem.position, elem.p1)
            case Bezier2:
                drawBezier(cxt, elem.position, elem.p1, elem.p2)
            case Arc:
                drawArc(cxt, elem.position, elem.p1, elem.p2)
            }
        }

        // End line cap
        if !path.isClosed
            drawEndCap(cxt, prev, normal, pen.endCapStyle)

        // Closing join between the end and the beginning in case of a closed path
        if path.isClosed and !firstLine and !beginLine and !(flags & .CollectPos)
            drawJoint(pen.jointStyle, prev, normal, normalFirst, dref pen)
    }

    // Draw the line path with the given 'pen'
    func drawPath(using self, path: LinePath, pen: Pen)
    {
        var cxt: Context
        cxt.path = &path
        cxt.pen = &pen
        if pen.penStyle == .Pattern
        {
            cxt.flags = .CollectPos
            drawPathInternal(&cxt)
            preparePattern(&cxt)
        }

        cxt.flags = .Zero
        drawPathInternal(&cxt)
    }

    // Draw the line path with the given 'pen'
    func fillPath(using self, path: LinePath, brush: Brush)
    {
        var cxt: Context
        cxt.path = &path
        cxt.pen = &sharedSolidPen
        cxt.flags = Flags.CollectPos
        drawPathInternal(&cxt)

        using cxt
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