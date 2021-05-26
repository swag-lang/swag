#global public
using Core, Core.Math, Core.Drawing

private struct Context
{
    posBuffer:          Array'Vector2
    size2:              f32
    beginLine:          bool
    firstLine:          bool
    adapt:              bool
    lastLineInvalid:    bool
    normalPrev:         Vector2
    normalFirst:        Vector2
    normal:             Vector2
    prim1, prim2:       uint
    prev:               Vector2
    path:               const *LinePath
    pen:                const *Pen

    remainingPatternLen: f32
    patternTotalLen:     f32
    patternIndex:        s32
    error:               f32
    divError:            f32
    pattern:             const *f32
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

    private func drawCapSquareAnchor(using self, p1, d1: Vector2, sens: f32, pen: Pen)
    {
        // Shift a little bit "inside" the line, so that the line will
        // overwrite the bad AA edges
        p := p1
        v := d1.cross(-sens)
        v.normalizeSafe()
        p += v

        d := d1 * pen.squareAnchorScaleX
        n := Vector2{d.y * sens, -d.x * sens}
        n.setLengthSafe(d.length() * pen.squareAnchorScaleY)

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
            addEdgeAA(prim1, v3, v2)
            addEdgeAA(prim1, v0, v1)

            addEdgeAA(prim2, v0, v3)
            addEdgeAA(prim2, v1, v2)
            addEdgeAA(prim2, v3, v2)
            addEdgeAA(prim2, v0, v1)
        }
    }

    private func drawCapArrowAnchor(using self, p1, d1: Vector2, sens: f32, pen: Pen)
    {
        // Shift a little bit "inside" the line, so that the line will
        // overwrite the bad AA edges
        p := p1
        v := d1.cross(-sens)
        v.normalizeSafe()
        p += v

        d := d1 * pen.arrowAnchorScaleX
        n  := Vector2{d.y * sens * pen.arrowAnchorScaleY, -d.x * sens * pen.arrowAnchorScaleY}

        v0 := Vector2{p.x + d.x, p.y + d.y}
        v1 := Vector2{p.x - d.x, p.y - d.y}
        v2 := Vector2{p.x + n.x, p.y + n.y}
        prim1 := pushTriangle(v0, v1, v2, pen.color)

        if paintParams & .Antialiased
        {
            addEdgeAA(prim1, v1, v2)
            addEdgeAA(prim1, v0, v2)
            addEdgeAA(prim1, v0, v1)
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

    private func drawLineCap(using self, capStyle: LineCapStyle, p, d: Vector2, sens: f32, pen: Pen)
    {
        #[Swag.complete]
        switch capStyle
        {
        case None:          break
        case Triangle:      drawCapTriangle(p, d, sens, pen)
        case Round:         drawCapRound(p, d, sens, pen)
        case Square:        drawCapSquare(p, d, sens, pen)
        case ArrowAnchor:   drawCapArrowAnchor(p, d, sens, pen)
        case SquareAnchor:  drawCapSquareAnchor(p, d, sens, pen)
        }
    }

    private func drawDashCap(using self, capStyle: DashCapStyle, p, d: Vector2, sens: f32, pen: Pen)
    {
        #[Swag.complete]
        switch capStyle
        {
        case None:          break
        case Triangle:      drawCapTriangle(p, d, sens, pen)
        case Round:         drawCapRound(p, d, sens, pen)
        case Square:        drawCapSquare(p, d, sens, pen)
        }
    }

    private func drawJoinBevel(using self, p, d1, d2: Vector2, pen: Pen)
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

    private func drawJoinRound(using self, p, d1, d2: Vector2, pen: Pen)
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

    private func drawJoinMiter(using self, p, d1, d2: Vector2, justMiter: bool, pen: Pen)
    {
        a := Vector2{d1.x + d2.x, d1.y + d2.y}
        w := d1.length()
        l := a.length()
        if Math.isZeroEpsilon(l) return

        m2 := (2 * w) / l

        if m2 > pen.joinMiterLimit
        {
            if justMiter
                m2 = pen.joinMiterLimit
            else
            {
                drawJoinBevel(p, d1, d2, pen)
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

    private func drawJoin(using self, joinStyle: JoinStyle, p, d1, d2: Vector2, pen: Pen)
    {
        #[Swag.complete]
        switch joinStyle
        {
        case None:          break
        case Bevel:         drawJoinBevel(p, d1, d2, pen)
        case Round:         drawJoinRound(p, d1, d2, pen)
        case Miter:         drawJoinMiter(p, d1, d2, true, pen)
        case MiterBevel:    drawJoinMiter(p, d1, d2, false, pen)
        }
    }

    private func drawDashEndCap(using self, using cxt: *Context, posCap, normalCap: Vector2)
    {
        if beginLine or lastLineInvalid return

        // Cap is defined
        if pen.dashEndCapStyle != DashCapStyle.None
            drawDashCap(pen.dashEndCapStyle, posCap, normalCap, -1.0, dref pen)

        // AA end edge if no cap
        else if paintParams & .Antialiased
        {
            prim2p := vertexBuffer.buffer + prim2
            prim2t := prim2p + 2
            addEdgeAA(prim1, prim2p.pos, prim2t.pos)
            addEdgeAA(prim2, prim2p.pos, prim2t.pos)
        }
    }

    private func drawMoveCursor(using self, using cxt: *Context, pos: Vector2)
    {
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

    private func drawLineInternal(using self, using cxt: *Context, pos: Vector2)
    {
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
        lastLineInvalid = normal.isZeroEpsilon()
        if lastLineInvalid
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
        flatBegin := !path.isClosed or pen.dashStyle != .None
        if pen.dashStyle != .None and (!firstLine or path.isClosed) and beginLine and pen.dashBeginCapStyle != .None
        {
            flatBegin = false
            drawDashCap(pen.dashBeginCapStyle, prev, normal, 1.0, dref pen)
        }

        // Joints
        if !beginLine
            drawJoin(pen.joinStyle, prev, normalPrev, normal, dref pen)

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

            // If we are starting a line, and the line does not have a cap before, then
            // we must AA the corresponding edge
            if flatBegin and beginLine and (!firstLine or pen.beginCapStyle == .None or path.isClosed)
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

    private func preparePattern(using cxt: *Context)
    {
        #[Swag.complete]
        switch pen.dashStyle
        {
        case None:
            return
        case Pattern:
            pattern = pen.pattern.buffer
            patternCount = cast(s32) pen.pattern.count
            if patternCount == 0 fallthrough
        case Dot:
            const P = @[1.0, 1.0]
            pattern = @dataof(P)
            patternCount = 2
        case Dash:
            const P = @[2.0, 1.0]
            pattern = @dataof(P)
            patternCount = 2
        case DashDot:
            const P = @[2.0, 1.0, 1.0, 1.0]
            pattern = @dataof(P)
            patternCount = 4
        }

        patternTotalLen = 0
        loop i: patternCount
            patternTotalLen += Math.max(pattern[i], 0.01)
        patternTotalLen *= pen.size

        repeatCount := cast(s32) (path.totalLen / patternTotalLen)
        error = path.totalLen - (repeatCount * patternTotalLen)

        // We must dispatch error at each blank in the pattern. So "divError" is the
        // number of blanks to add at each 'move', between solid patterns
        numBlanks := repeatCount * patternCount / 2
        divError = numBlanks ? error / numBlanks : 0

        // First pattern. We start at half the size of the first pattern, in order to not
        // finish on a blank, but with half the size of the first pattern also.
        patternIndex = 0
        remainingPatternLen = Math.max(pattern[patternIndex], 0.01)
        remainingPatternLen *= pen.size
        if !path.isClosed
            remainingPatternLen *= 0.5
    }

    private func drawLine(using self, using cxt: *Context, pos: Vector2)
    {
        // solid
        if pen.dashStyle == .None
        {
            drawLineInternal(cxt, pos)
            return
        }

        v := pos - prev
        remainingSegLen := v.length()
        v.normalizeSafe()

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
                    drawDashEndCap(cxt, prev, normal)
                    drawMoveCursor(cxt, nextp)
                    error -= addBlank
                }
                else
                {
                    drawLineInternal(cxt, nextp)
                }

                // Start a new fresh pattern index
                remainingSegLen -= lengthInPattern
                patternIndex = (patternIndex + 1) % patternCount
                blankPatternIdx = blankPatternIdx ? false : true
                remainingPatternLen = Math.max(pattern[patternIndex], 0.01)
                remainingPatternLen *= pen.size
                continue
            }

            // Here the segment does not have enough remaining length to cover the current
            // pattern. So draw what remains, and exit. The next line will try to cover the rest.
            remainingPatternLen -= remainingSegLen
            if blankPatternIdx
            {
                drawDashEndCap(cxt, prev, normal)
                drawMoveCursor(cxt, pos)
            }
            else
            {
                drawLineInternal(cxt, pos)
            }
            break
        }
    }

    private func drawLineBeginCap(using self, using cxt: *Context)
    {
        if pen.beginCapStyle == .None
            return

        v1 := path.startPoint
        v2 := path.posBuffer.buffer[0]
        n := v2 - v1
        n.normalizeSafe()
        c := n.cross(1)
        c *= size2
        drawLineCap(pen.beginCapStyle, v1, c, 1.0, dref pen)
    }

    private func drawLineEndCap(using self, using cxt: *Context)
    {
        if pen.endCapStyle == .None
            return

        v1 := path.posBuffer.buffer[path.posBuffer.count - 1]
        v2 := path.startPoint
        if path.posBuffer.count > 1
            v2 = path.posBuffer.buffer[path.posBuffer.count - 2]
        n := v1 - v2
        n.normalizeSafe()
        c := n.cross(1)
        c *= size2
        drawLineCap(pen.endCapStyle, v1, c, -1.0, dref pen)
    }

    private func drawPath(using self, using cxt: *Context)
    {
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

        // Line caps. Draw them before the line, so that the line will overwrite them a
        // little if necessary
        if !path.isClosed
        {
            drawLineBeginCap(cxt)
            drawLineEndCap(cxt)
        }

        // Draw all
        visit p: path.posBuffer
            drawLine(cxt, dref p)

        // AA end edge if no end cap
        if !lastLineInvalid and !path.isClosed and (paintParams & .Antialiased) and (pen.endCapStyle == .None)
        {
            prim2p := vertexBuffer.buffer + prim2
            prim2t := prim2p + 2
            addEdgeAA(prim1, prim2p.pos, prim2t.pos)
            addEdgeAA(prim2, prim2p.pos, prim2t.pos)
        }

        // Closing join between the end and the beginning in case of a closed path
        else if path.isClosed and !firstLine and !beginLine
        {
            drawJoin(pen.joinStyle, prev, normal, normalFirst, dref pen)
        }
    }

    // Draw the line path with the given 'pen'
    func drawPath(using self, path: *LinePath, pen: Pen)
    {
        path.flatten(paintQuality)
        if Math.isZeroEpsilon(path.totalLen) return

        var cxt: Context
        cxt.path = path
        cxt.pen =  &pen
        if pen.dashStyle != .None
            preparePattern(&cxt)

        drawPath(&cxt)
    }

    private func triangulateArea(border: const [..] Vector2)->f32
    {
        n := @countof(border)
        q := 0
        p := n - 1
        area := 0'f32
        while q < n
        {
            area += border[p].x * border[q].y - border[q].x * border[p].y
            p = q
            q += 1
        }

        return area * 0.5
    }

    private func triangulateSnip(border: const [..] Vector2, u, v, w, n: s32, pv: *s32)->bool
    {
        a := border[pv[u]]
        b := border[pv[v]]
        c := border[pv[w]]

        // Cross product to detect if the triangle in the wrong direction
        val := ((b.x - a.x) * (c.y - a.y)) - ((b.y - a.y) * (c.x - a.x))
        if val < Math.ConstF32.Epsilon
            return false

        // Detect if a point of the graph is inside that triangle
        for p := 0; p < n; p += 1
        {
            if (p == u) or (p == v) or (p == w)
                continue
            pt := border[pv[p]]
            if Math.isVec2InTriangle(pt, a, b, c)
                return false
        }

        return true
    }

    private func triangulate(using self, path: *LinePath)->Array's32
    {
        var result: retval

        // We want a counter-clockwise polygon in V
        n := cast(s32) path.posBuffer.count
        triangulateIdx.resize(cast(uint) n)
        idxPtr := triangulateIdx.buffer
        if triangulateArea(path.posBuffer.toSlice()) >= 0
        {
            loop i: n
                idxPtr[i] = cast(s32) i
        }
        else
        {
            loop i: n
                idxPtr[i] = cast(s32) ((n - 1) - i)
        }

        // Remove nv-2 Vertices, creating 1 triangle every time
        nv := n
        count := 2 * nv
        m := 0
        v := nv - 1
        while nv > 2
        {
            // If we loop, it is probably a non-simple polygon
            if count <= 0
            {
                result.clear()
                return result
            }

            count -= 1

            // Three consecutive vertices in current polygon, <u,v,w>
            u := v
            if nv <= u u = 0
            v = u + 1
            if nv <= v v = 0
            w := v + 1
            if nv <= w w = 0

            if triangulateSnip(path.posBuffer.toSlice(), u, v, w, nv, idxPtr)
            {
                // Output Triangle
                a := idxPtr[u]
                b := idxPtr[v]
                c := idxPtr[w]
                result.add(a)
                result.add(b)
                result.add(c)

                // Remove v from remaining polygon
                m += 1
                s := v
                t := v + 1
                while t < nv
                {
                    triangulateIdx.buffer[s] = triangulateIdx.buffer[t]
                    s += 1
                    t += 1
                }

                nv -= 1

                // Reset error detection counter
                count = 2 * nv
            }
        }

        return result
    }

    // Draw the line path with the given 'pen'
    func fillPath(using self, path: *LinePath, brush: Brush)
    {
        path.flatten(paintQuality)
        if Math.isZeroEpsilon(path.totalLen) return

        using path

        // Convex shape: just add a triangle from the center of the shape to each pair of
        // vertices
        if isConvex
        {
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

        // Non convex shape: triangulate
        else
        {
            result := triangulate(path)
            if result.isEmpty() return
            pidx := result.buffer
            tpos := path.posBuffer.buffer
            firstPrim := vertexBuffer.count
            loop result.count / 3
            {
                v0 := tpos[pidx[0]]
                v1 := tpos[pidx[1]]
                v2 := tpos[pidx[2]]
                prim := pushTriangle(v0, v1, v2, brush.color)
                pidx += 3
            }

            // Generate all AA edges : for each vertex of the triangle, add the previous and
            // next edge (even if not in the triangle)
            if paintParams & .Antialiased
            {
                cpt := cast(s32) path.posBuffer.count
                psrc := result.buffer
                for i := 0; i < result.count; i += 3
                {
                    index  := psrc[0]
                    indexn := (index + 1) % cpt;
                    indexp := index ? index - 1 : cpt - 1;
                    addEdgeAA(firstPrim, tpos[index], tpos[indexp]);
                    addEdgeAA(firstPrim, tpos[index], tpos[indexn]);

                    index  = psrc[1]
                    indexn = (index + 1) % cpt;
                    indexp = index ? index - 1 : cpt - 1;
                    addEdgeAA(firstPrim, tpos[index], tpos[indexp]);
                    addEdgeAA(firstPrim, tpos[index], tpos[indexn]);

                    index  = psrc[2]
                    indexn = (index + 1) % cpt;
                    indexp = index ? index - 1 : cpt - 1;
                    addEdgeAA(firstPrim, tpos[index], tpos[indexp]);
                    addEdgeAA(firstPrim, tpos[index], tpos[indexn]);

                    psrc += 3
                    firstPrim += 3
                }
            }
        }
    }
}