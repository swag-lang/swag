using Core, Math

public enum DrawPathListMode
{
    Separate
    Merge
}

struct Context
{
    posBuffer:          Array'Vector2
    penSize:            f32
    penSize2:           f32
    beginLine:          bool
    firstLine:          bool
    lastLineInvalid:    bool
    normalPrev:         Vector2
    normalFirst:        Vector2
    normal:             Vector2
    prim1, prim2:       uint
    prev:               Vector2
    path:               const *LinePath
    pen:                const *Pen
    offsetPath:         f32

    remainingPatternLen: f32
    patternTotalLen:     f32
    patternIndex:        s32
    error:               f32
    divError:            f32
    pattern:             const ^f32
    patternCount:        s32
}

impl Painter
{
    mtd drawCapSquare(p, d: Vector2, sens: f32, pen: Pen)
    {
        n  := Vector2{d.y * sens, -d.x * sens}
        v0 := Vector2{p.x + d.x, p.y + d.y}
        v1 := Vector2{p.x - d.x, p.y - d.y}
        v2 := Vector2{v1.x + n.x, v1.y + n.y}
        v3 := Vector2{v0.x + n.x, v0.y + n.y}

        prim1 := pushTriangle(v0, v1, v3, pen.brush.color)
        prim2 := pushTriangle(v1, v3, v2, pen.brush.color)

        if curState.paintParams & .Antialiased
        {
            addEdgeAA(prim1, v0, v3)
            addEdgeAA(prim1, v1, v2)
            addEdgeAA(prim2, v3, v2)
            addEdgeAA(prim2, v1, v2)
            addEdgeAA(prim2, v0, v3)
        }
    }

    mtd drawCapTriangle(p, d: Vector2, sens: f32, pen: Pen)
    {
        n  := Vector2{d.y * sens, -d.x * sens}
        v0 := Vector2{p.x + d.x, p.y + d.y}
        v1 := Vector2{p.x - d.x, p.y - d.y}
        v2 := Vector2{p.x + n.x, p.y + n.y}
        prim1 := pushTriangle(v0, v1, v2, pen.brush.color)

        if curState.paintParams & .Antialiased
        {
            addEdgeAA(prim1, v1, v2)
            addEdgeAA(prim1, v0, v2)
        }
    }

    mtd drawCapSquareAnchor(p1, d1: Vector2, sens: f32, pen: Pen, start: bool)->Vector2
    {
        sx := start ? pen.capStartScaleX : pen.capEndScaleX
        sy := start ? pen.capStartScaleY : pen.capEndScaleY

        p := p1
        d := d1 * sx
        n := Vector2{d.y * sens, -d.x * sens}
        n.setLengthSafe(d.length() * sy)
        p -= n * 0.5

        v0 := Vector2{p.x + d.x, p.y + d.y}
        v1 := Vector2{p.x - d.x, p.y - d.y}
        v2 := Vector2{v1.x + n.x, v1.y + n.y}
        v3 := Vector2{v0.x + n.x, v0.y + n.y}

        prim1 := pushTriangle(v0, v1, v3, pen.brush.color)
        prim2 := pushTriangle(v1, v3, v2, pen.brush.color)

        if curState.paintParams & .Antialiased
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

        return Vector2.Zero
    }

    mtd drawCapArrowAnchor(p1, d1: Vector2, sens: f32, pen: Pen, start: bool)->Vector2
    {
        sx := start ? pen.capStartScaleX : pen.capEndScaleX
        sy := start ? pen.capStartScaleY : pen.capEndScaleY

        p := p1
        d := d1 * sx
        n := Vector2{d.y * sens * sy, -d.x * sens * sy}
        p -= n

        v0 := Vector2{p.x + d.x, p.y + d.y}
        v1 := Vector2{p.x - d.x, p.y - d.y}
        v2 := Vector2{p.x + n.x, p.y + n.y}
        prim1 := pushTriangle(v0, v1, v2, pen.brush.color)

        if curState.paintParams & .Antialiased
        {
            addEdgeAA(prim1, v1, v2)
            addEdgeAA(prim1, v0, v2)
            addEdgeAA(prim1, v0, v1)
        }

        return n - n.toNormalizeSafe()
    }

    mtd drawCapRound(p, d: Vector2, sens: f32, pen: Pen)
    {
        radius := d.length()
        if radius < 2.5
        {
            drawCapTriangle(p, d, sens, pen)
            return
        }

        angle0 := Math.atan2(-d.y, -d.x)
        angle1 := angle0 + Math.ConstF32.Pi
        radius *= sens
        drawRound(p, radius, angle0, angle1, pen)
    }

    mtd drawLineCap(capStyle: LineCapStyle, p, d: Vector2, sens: f32, pen: Pen, start: bool)->Vector2
    {
        #[Swag.Complete]
        switch capStyle
        {
        case None:
            return Vector2.Zero

        case Triangle:
            drawCapTriangle(p, d, sens, pen)
            return Vector2.Zero

        case Round:
            drawCapRound(p, d, sens, pen)
            return Vector2.Zero

        case Square:
            drawCapSquare(p, d, sens, pen)
            return Vector2.Zero

        case SquareAnchor:
            return drawCapSquareAnchor(p, d, sens, pen, start)

        case ArrowAnchor:
            return drawCapArrowAnchor(p, d, sens, pen, start)
        }

        return Vector2.Zero
    }

    mtd drawDashCap(using cxt: *Context, capStyle: DashCapStyle, p, d: Vector2, sens: f32)
    {
        if penSize < 3
            return

        #[Swag.Complete]
        switch capStyle
        {
        case None:          break
        case Triangle:      drawCapTriangle(p, d, sens, dref pen)
        case Round:         drawCapRound(p, d, sens, dref pen)
        case Square:        drawCapSquare(p, d, sens, dref pen)
        }
    }

    mtd drawJoinBevel(p, d1, d2: Vector2, pen: Pen)
    {
        v0 := p

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

        prim1 := pushTriangle(v0, v1, v2, pen.brush.color)
        if curState.paintParams & .Antialiased
            addEdgeAA(prim1, v1, v2)
    }

    mtd drawRound(center: Vector2, radius: f32, angle0, angle1: f32, pen: Pen)
    {
        var startPos, endPos: Vector2 = ?
        startPos.x = center.x + (Math.cos(angle0) * radius)
        startPos.y = center.y + (Math.sin(angle0) * radius)
        endPos.x = center.x + (Math.cos(angle1) * radius)
        endPos.y = center.y + (Math.sin(angle1) * radius)

        minDist := LinePath.getFlattenDistance(curState.paintQuality)

        // We divide a segment by half until the computed point on the curve is close to the corresponding segment
        var tmp: Array'{factor: f32, pos: Vector2}
        tmp.add(@{angle0, startPos})
        tmp.add(@{angle1, endPos})

        for i := 0; i < tmp.count - 1; i += 1
        {
            angle := (tmp[i].factor + tmp[i + 1].factor) * 0.5
            point := center
            point.x += (Math.cos(angle) * radius)
            point.y += (Math.sin(angle) * radius)

            dist := point.distanceToLine(tmp[i].pos, tmp[i + 1].pos)
            if dist < minDist
                continue

            tmp.insertAt(cast(uint) i + 1, @{angle, point})
            i -= 1
        }

        v1 := startPos
        loop i: 1..tmp.count-1
        {
            v2 := tmp[i].pos
            prim1 := pushTriangle(center, v1, v2, pen.brush.color)
            if curState.paintParams & .Antialiased
                addEdgeAA(prim1, v1, v2)
            v1 = v2
        }
    }

    mtd drawJoinRound(p, d1, d2: Vector2, pen: Pen)
    {
        var angle0, angle1: f32 = ?
        if d1.x * d2.y - d1.y * d2.x < 0
        {
            angle0 = Math.atan2(-d1.y, -d1.x)
            angle1 = Math.atan2(-d2.y, -d2.x)
        }
        else
        {
            angle0 = Math.atan2(d1.y, d1.x)
            angle1 = Math.atan2(d2.y, d2.x)
        }

        if Math.isEqualEpsilon(angle0, angle1)
            return

        radius := d1.length()

        if angle1 > angle0 + Math.ConstF32.Pi
            angle1 -= Math.ConstF32.TwoPi
        elif angle1 < angle0 - Math.ConstF32.Pi
            angle1 += Math.ConstF32.TwoPi

        drawRound(p, radius, angle0, angle1, pen)
    }

    mtd drawJoinMiter(p, d1, d2: Vector2, justMiter: bool, pen: Pen)
    {
        a := Vector2{d1.x + d2.x, d1.y + d2.y}
        w := d1.length()
        l := a.length()
        if Math.isZeroEpsilon(l)
            return

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

        prim1 := pushTriangle(v0, v1, v3, pen.brush.color)
        prim2 := pushTriangle(v1, v3, v2, pen.brush.color)
        if curState.paintParams & .Antialiased
        {
            addEdgeAA(prim1, v3, v2)
            addEdgeAA(prim1, v1, v2)
            addEdgeAA(prim2, v3, v2)
            addEdgeAA(prim2, v1, v2)
        }
    }

    mtd drawJoin(using cxt: *Context, p, d1, d2: Vector2)
    {
        #[Swag.Complete]
        switch pen.joinStyle
        {
        case None:          break
        case Bevel:         drawJoinBevel(p, d1, d2, dref pen)
        case Round:         drawJoinRound(p, d1, d2, dref pen)
        case Miter:         drawJoinMiter(p, d1, d2, true, dref pen)
        case MiterBevel:    drawJoinMiter(p, d1, d2, false, dref pen)
        }
    }

    mtd drawDashEndCap(using cxt: *Context, posCap, normalCap: Vector2)
    {
        if beginLine or lastLineInvalid
            return

        // Cap is defined
        if pen.dashEndCapStyle != DashCapStyle.None
            drawDashCap(cxt, pen.dashEndCapStyle, posCap, normalCap, -1.0)

        // AA end edge if no cap
        elif curState.paintParams & .Antialiased
        {
            prim2p := vertexBuffer.buffer + prim2
            prim2t := prim2p + 2
            addEdgeAA(prim1, prim2p.pos, prim2t.pos)
            addEdgeAA(prim2, prim2p.pos, prim2t.pos)
        }
    }

    mtd drawMoveCursor(using cxt: *Context, pos: Vector2)
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

    mtd drawLineInternal(using cxt: *Context, pos: Vector2)
    {
        // Compute line (half width on each side)
        newPos := pos
        n  := newPos - prev

        // If previous and next point are too close, we will take
        // the last normal
        if !n.isZeroEpsilon()
        {
            n.normalize()
            normal = n.cross(1)
            normal *= penSize2
        }

        // If this is still invalid, there's nothing we can do
        lastLineInvalid = normal.isZeroEpsilon()
        if lastLineInvalid
            return

        // First line cap
        flatBegin := !path.isClosed or pen.dashStyle != .None
        if pen.dashStyle != .None and (!firstLine or path.isClosed) and beginLine and pen.dashBeginCapStyle != .None
        {
            flatBegin = false
            drawDashCap(cxt, pen.dashBeginCapStyle, prev, normal, 1.0)
        }

        // Joints
        if !beginLine
            drawJoin(cxt, prev, normalPrev, normal)

        // Draw quad
        v0 := prev - normal
        v1 := newPos - normal
        v3 := prev + normal
        v2 := newPos + normal
        prim1 = pushTriangle(v0, v1, v3, pen.brush.color)
        prim2 = pushTriangle(v1, v3, v2, pen.brush.color)

        // Generate AA edges
        if curState.paintParams & .Antialiased
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

    func preparePattern(using cxt: *Context)
    {
        #[Swag.Complete]
        switch pen.dashStyle
        {
        case None:
            return
        case Pattern:
            pattern = pen.pattern.buffer
            patternCount = cast(s32) pen.pattern.count
            if patternCount == 0
                fallthrough
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

        repeatCount := cast(s32) (path.polygon.totalLen / patternTotalLen)
        error = path.polygon.totalLen - (repeatCount * patternTotalLen)

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

    mtd drawLine(using cxt: *Context, pos: Vector2)
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

    mtd drawLineBeginCap(using cxt: *Context)->Vector2
    {
        if pen.beginCapStyle == .None or penSize < 3
            return Vector2.Zero

        v1 := path.polygon.getPoint(0, cxt.offsetPath)
        v2 := path.polygon.getPoint(1, cxt.offsetPath)
        n := v2 - v1
        n.normalizeSafe()
        c := n.cross(1)
        c *= penSize2
        return drawLineCap(pen.beginCapStyle, v1, c, 1.0, dref pen, true)
    }

    mtd drawLineEndCap(using cxt: *Context)->Vector2
    {
        if pen.endCapStyle == .None or penSize < 3
            return Vector2.Zero

        v1 := path.polygon.getPoint(path.polygon.points.count - 1, cxt.offsetPath)
        v2 := path.polygon.getPoint(path.polygon.points.count - 2, cxt.offsetPath)
        n := v1 - v2
        n.normalizeSafe()
        c := n.cross(1)
        c *= penSize2
        return drawLineCap(pen.endCapStyle, v1, c, -1.0, dref pen, false)
    }

    mtd drawPath(using cxt: *Context)
    {
        if pen.size < 1 or path.polygon.points.count == 0
            return

        penSize = pen.size
        cxt.offsetPath = pen.plotOffset

        penSize2    = penSize * 0.5
        prev        = path.polygon.getPoint(0, cxt.offsetPath)
        beginLine   = true
        firstLine   = true

        var offset0, offsetn: Vector2

        // Line caps. Draw them before the line, so that the line will overwrite them a
        // little if necessary
        if !path.isClosed
        {
            offset0 = drawLineBeginCap(cxt)
            offsetn = drawLineEndCap(cxt)
            prev -= offset0
        }

        // Draw all except last point, because of potential offset for the last point
        // (because of cap styles)
        if path.polygon.points.count > 2
        {
            loop i: 1..path.polygon.points.count-2
            {
                p := path.polygon.getPoint(i, cxt.offsetPath)
                drawLine(cxt, p)
            }
        }

        // Last point
        p := path.polygon.getPoint(path.polygon.points.count-1, cxt.offsetPath)
        p -= offsetn
        drawLine(cxt, p)

        // AA end edge if no end cap
        if !lastLineInvalid and !path.isClosed and (curState.paintParams & .Antialiased) and (pen.endCapStyle == .None)
        {
            prim2p := vertexBuffer.buffer + prim2
            prim2t := prim2p + 2
            addEdgeAA(prim1, prim2p.pos, prim2t.pos)
            addEdgeAA(prim2, prim2p.pos, prim2t.pos)
        }

        // Closing join between the end and the beginning in case of a closed path
        elif path.isClosed and !firstLine and !beginLine
        {
            drawJoin(cxt, prev, normal, normalFirst)
        }
    }

    func triangulateSnip(border: const [..] Vector2, u, v, w, n: s32, pv: ^s32)->bool
    {
        a := border[pv[u]]
        b := border[pv[v]]
        c := border[pv[w]]

        // Cross product to detect if the triangle is in the wrong direction
        val := ((b.x - a.x) * (c.y - a.y)) - ((b.y - a.y) * (c.x - a.x))
        if val < Math.ConstF32.Epsilon
            return false

        // Detect if a point of the graph is inside that triangle
        for p := 0; p < n; p += 1
        {
            if (p == u) or (p == v) or (p == w)
                continue
            pt := border[pv[p]]
            if pt.isInTriangle(a, b, c)
                return false
        }

        return true
    }

    mtd triangulate(path: *LinePath)->Array's32
    {
        var result: retval

        slice := @mkslice(path.polygon.points.buffer + 1, path.polygon.points.count - 1)
        n := cast(s32) @countof(slice)

        // We want a counter-clockwise polygon in V
        triangulateIdx.resize(cast(uint) n)
        idxPtr := triangulateIdx.buffer
        if path.polygon.isClockwise
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

            if triangulateSnip(slice, u, v, w, nv, idxPtr)
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

    mtd triangulate(t: *Poly2Tri.Tesselate, p: *LinePath, bvMin, bvMax: Vector2, brush: Brush)
    {
        setTexture0(bvMin, bvMax, brush)
        resetTextureFont()

        dirtyAA := false
        if p.triangles.count == 0
        {
            t.triangulate()

            // "Eat" points and triangles
            // t.triangles only contains 'interior' triangles, so we need to delete all other
            // allocated triangles to avoid a leak

            p.points    = t.points
            p.triangles = t.triangles

            visit tri: t.map
            {
                if !tri.interior
                    Memory.delete(tri)
            }

            t.points.clear()
            t.map.clear()
            dirtyAA = true
        }

        t.clear()

        var toProcess:  Array'*Poly2Tri.Triangle
        var done:       HashSet'*Poly2Tri.Triangle
        toProcess.reserve(p.triangles.count)

        // Compute the border of the polygon
        var border: Array'{Math.Vector2, Math.Vector2}
        if (curState.paintParams & .Antialiased) and dirtyAA
        {
            visit tri: p.triangles
            {
                loop e: 3
                {
                    v0 := Vector2{cast(f32) tri.points[0].x, cast(f32) tri.points[0].y}
                    v1 := Vector2{cast(f32) tri.points[1].x, cast(f32) tri.points[1].y}
                    v2 := Vector2{cast(f32) tri.points[2].x, cast(f32) tri.points[2].y}
                    if tri.constrainedEdge[e]
                    {
                        switch e
                        {
                        case 0:
                            border.add(@{v1, v2})
                        case 1:
                            border.add(@{v0, v2})
                        case 2:
                            border.add(@{v0, v1})
                        }
                    }
                }
            }
        }

        visit tri, ii: p.triangles
        {
            Debug.assert(tri.interior)
            v0 := Vector2{cast(f32) tri.points[0].x, cast(f32) tri.points[0].y}
            v1 := Vector2{cast(f32) tri.points[1].x, cast(f32) tri.points[1].y}
            v2 := Vector2{cast(f32) tri.points[2].x, cast(f32) tri.points[2].y}
            prim := pushTriangle(v0, v1, v2, brush.color)

            if curState.paintParams & .Antialiased
            {
                // Just use stored edges
                if !dirtyAA
                {
                    visit e: tri.aaEdges
                        addEdgeAA(prim, e.item0, e.item1)
                }

                // Compute aa edges
                else
                {
                    primPtr := vertexBuffer.buffer + prim

                    const D = 4.000
                    visit e1: border
                    {
                        if e1.item0.distanceSquared(v0) <= D or e1.item0.distanceSquared(v1) <= D or e1.item0.distanceSquared(v2) <= D or
                           e1.item1.distanceSquared(v0) <= D or e1.item1.distanceSquared(v1) <= D or e1.item1.distanceSquared(v2) <= D
                        {
                            if addEdgeAA(prim, e1.item0, e1.item1)
                                tri.aaEdges.add(@{e1.item0, e1.item1})
                            if primPtr.aanum == MaxAAEdge
                                break
                        }
                    }

                    done.clear()
                    toProcess.clear()

                    toProcess.add(tri)
                    done.add(tri)

                    for i := 0; i < toProcess.count and primPtr.aanum < MaxAAEdge; i += 1
                    {
                        tri0 := toProcess[i]

                        v0 = Vector2{cast(f32) tri0.points[0].x, cast(f32) tri0.points[0].y}
                        v1 = Vector2{cast(f32) tri0.points[1].x, cast(f32) tri0.points[1].y}
                        v2 = Vector2{cast(f32) tri0.points[2].x, cast(f32) tri0.points[2].y}

                        loop e: 3
                        {
                            neighbor := tri0.neighbors[e]
                            if neighbor
                            {
                                if !done.contains(neighbor)
                                {
                                    toProcess.add(neighbor)
                                    done.add(neighbor)
                                }
                            }

                            if tri0.constrainedEdge[e]
                            {
                                switch e
                                {
                                case 0:
                                    if addEdgeAA(prim, v1, v2)
                                        tri.aaEdges.add(@{v1, v2})
                                case 1:
                                    if addEdgeAA(prim, v0, v2)
                                        tri.aaEdges.add(@{v0, v2})
                                case 2:
                                    if addEdgeAA(prim, v0, v1)
                                        tri.aaEdges.add(@{v0, v1})
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

public impl Painter
{
    // Draw the line path
    mtd drawPath(path: *LinePath, color: Color, penSize: f32)
    {
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawPath(path, sharedSolidPen)
    }

    // Draw the line path with the given 'pen'
    mtd drawPath(path: *LinePath, pen: Pen)
    {
        path.flatten(curState.paintQuality)
        if Math.isZeroEpsilon(path.polygon.totalLen)
            return

        var cxt: Context
        cxt.path = path
        cxt.pen =  &pen
        if pen.dashStyle != .None
            preparePattern(&cxt)

        bvMin := path.bvMin
        bvMax := path.bvMax
        bvMin -= pen.size / 2
        bvMax += pen.size / 2
        setTexture0(bvMin, bvMax, pen.brush)
        resetTextureFont()

        startOverlapMode(pen)
        drawPath(&cxt)
        stopOverlapMode()
    }

    // Draw the line path
    mtd drawPathList(pathList: *LinePathList, pen: Pen, mode: DrawPathListMode = .Separate)
    {
        if mode == .Separate
        {
            visit p: pathList.paths
                drawPath(p, pen)
            return
        }

        pathList.flatten(curState.paintQuality)

        bvMin := pathList.bvMin
        bvMax := pathList.bvMax
        bvMin -= pen.size / 2
        bvMax += pen.size / 2
        setTexture0(bvMin, bvMax, pen.brush)
        resetTextureFont()

        startOverlapMode(pen)

        var cxt: Context
        visit p: pathList.paths
        {
            cxt.path = p
            cxt.pen =  &pen
            if pen.dashStyle != .None
                preparePattern(&cxt)
            drawPath(&cxt)
        }

        stopOverlapMode()
    }

    // Draw the line path
    mtd drawPathList(pathList: *LinePathList, color: Color, penSize: f32, mode: DrawPathListMode = .Separate)
    {
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawPathList(pathList, sharedSolidPen, mode)
    }

    // Fill the line path
    mtd fillPath(pathList: *LinePathList, color: Color)
    {
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        fillPath(pathList, sharedSolidBrush)
    }

    // Fill the line path with the given 'brush'
    mtd fillPath(path: *LinePath, brush: Brush)
    {
        path.flatten(curState.paintQuality)
        if Math.isZeroEpsilon(path.polygon.totalLen)
            return

        // Setup texture
        setTexture0(path.bvMin, path.bvMax, brush)
        resetTextureFont()

        // Convex shape: just add a triangle from the center of the shape to each pair of
        // vertices
        polygon := &path.polygon
        if polygon.isConvex
        {
            count := path.polygon.points.count - 1
            center := Vector2{path.bvMin.x + (path.bvMax.x - path.bvMin.x) * 0.5, path.bvMin.y + (path.bvMax.y - path.bvMin.y) * 0.5}
            loop i: count
            {
                v0 := polygon.getPoint(i)
                v1 := polygon.getPoint((i + 1) % count)
                prim := pushTriangle(center, v0, v1, brush.color)

                if curState.paintParams & .Antialiased
                {
                    v2 := polygon.getPoint((i + 2) % count)
                    vp := polygon.getPoint((i + count - 1) % count)
                    addEdgeAA(prim, v0, v1)
                    addEdgeAA(prim, v1, v2)
                    addEdgeAA(prim, vp, v0)
                }
            }
        }

        // Non convex shape: triangulate
        else
        {
            mtd storeResult(polygon: *Polygon, result: Array's32, brush: Brush)
            {
                pidx := result.buffer
                firstPrim := vertexBuffer.count
                loop result.count / 3
                {
                    v0 := polygon.getPoint(cast(uint) pidx[0] + 1)
                    v1 := polygon.getPoint(cast(uint) pidx[1] + 1)
                    v2 := polygon.getPoint(cast(uint) pidx[2] + 1)
                    prim := pushTriangle(v0, v1, v2, brush.color)
                    pidx += 3
                }

                // Generate all AA edges : for each vertex of the triangle, add the previous and
                // next edge (even if not in the triangle)
                if curState.paintParams & .Antialiased
                {
                    cpt := cast(s32) polygon.points.count - 1
                    psrc := result.buffer
                    for i := 0; i < result.count; i += 3
                    {
                        index  := psrc[0]
                        indexn := (index + 1) % cpt
                        indexp := index ? index - 1 : cpt - 1
                        v  := polygon.getPoint(cast(uint) index + 1)
                        vp := polygon.getPoint(cast(uint) indexp + 1)
                        vn := polygon.getPoint(cast(uint) indexn + 1)
                        addEdgeAA(firstPrim, v, vp)
                        addEdgeAA(firstPrim, v, vn)

                        index  = psrc[1]
                        indexn = (index + 1) % cpt
                        indexp = index ? index - 1 : cpt - 1
                        v  = polygon.getPoint(cast(uint) index + 1)
                        vp = polygon.getPoint(cast(uint) indexp + 1)
                        vn = polygon.getPoint(cast(uint) indexn + 1)
                        addEdgeAA(firstPrim, v, vp)
                        addEdgeAA(firstPrim, v, vn)

                        index  = psrc[2]
                        indexn = (index + 1) % cpt
                        indexp = index ? index - 1 : cpt - 1
                        v  = polygon.getPoint(cast(uint) index + 1)
                        vp = polygon.getPoint(cast(uint) indexp + 1)
                        vn = polygon.getPoint(cast(uint) indexn + 1)
                        addEdgeAA(firstPrim, v, vp)
                        addEdgeAA(firstPrim, v, vn)

                        psrc += 3
                        firstPrim += 3
                    }
                }
            }

            result := triangulate(path)
            if result.isEmpty()
            {
                res := path.polygon.getCleanedPaths()
                if !res.count
                    return
                savePoly := path.polygon
                visit &r: res
                {
                    path.polygon.storeClipperResult(dref r, false)
                    result = triangulate(path)
                    self.storeResult(&path.polygon, result, brush)
                }
                path.polygon = savePoly
            }
            else
                self.storeResult(&path.polygon, result, brush)
        }
    }

    // Fill the line path
    mtd fillPath(path: *LinePath, color: Color)
    {
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        fillPath(path, sharedSolidBrush)
    }

    // Fill the list of paths
    // clockwise = fill, anti-clockwise = hole
    mtd fillPath(pathList: *LinePathList, brush: Brush)
    {
        // Flatten all paths
        pathList.flatten(curState.paintQuality)

        var t:      Poly2Tri.Tesselate
        var flush:  *LinePath

        serial := 0
        visit p: pathList.paths
        {
            if (p.polygon.isClockwise or serial != p.serial) and flush
            {
                serial = p.serial
                triangulate(&t, flush, pathList.bvMin, pathList.bvMax, brush)
                flush = null
            }

            if p.polygon.points.count >= 3
            {
                flush = p
                t.startPolyLine()
                loop i: 0..p.polygon.points.count-2
                {
                    pt := p.polygon.getPoint(i)
                    t.addPoint(cast(f64) pt.x, cast(f64) pt.y)
                }

                t.endPolyLine()
            }
        }

        if flush
            triangulate(&t, flush, pathList.bvMin, pathList.bvMax, brush)
    }
}