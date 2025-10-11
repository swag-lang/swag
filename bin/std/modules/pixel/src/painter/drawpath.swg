using Core

public enum DrawPathListMode
{
    Separate
    Merge
}

struct Context
{
    posBuffer:               Array'Math.Vector2
    penSize:                 f32
    penSize2:                f32
    beginLine:               bool
    firstLine:               bool
    lastLineInvalid:         bool
    normalPrev:              Math.Vector2
    normalFirst:             Math.Vector2
    normal:                  Math.Vector2
    prim1, prim2:            u64
    prev:                    Math.Vector2
    path:                    const *LinePath
    pen:                     const *Pen
    offsetPath:              f32

    remainingPatternLen:     f32
    patternTotalLen:         f32
    patternIndex:            s32
    error:                   f32
    divError:                f32
    pattern:                 const [*] f32
    patternCount:            s32
}

impl Painter
{
    mtd drawCapSquare(p, d: Math.Vector2, sens: f32, pen: Pen)
    {
        let n  = Math.Vector2{d.y * sens, -d.x * sens}
        let v0 = Math.Vector2{p.x + d.x, p.y + d.y}
        let v1 = Math.Vector2{p.x - d.x, p.y - d.y}
        let v2 = Math.Vector2{v1.x + n.x, v1.y + n.y}
        let v3 = Math.Vector2{v0.x + n.x, v0.y + n.y}

        let prim1 = .pushTriangle(v0, v1, v3, pen.brush.color)
        let prim2 = .pushTriangle(v1, v3, v2, pen.brush.color)

        if .curState.paintParams & .Antialiased
        {
            .addEdgeAA(prim1, v0, v3)
            .addEdgeAA(prim1, v1, v2)
            .addEdgeAA(prim2, v3, v2)
            .addEdgeAA(prim2, v1, v2)
            .addEdgeAA(prim2, v0, v3)
        }
    }

    mtd drawCapTriangle(p, d: Math.Vector2, sens: f32, pen: Pen)
    {
        let n     = Math.Vector2{d.y * sens, -d.x * sens}
        let v0    = Math.Vector2{p.x + d.x, p.y + d.y}
        let v1    = Math.Vector2{p.x - d.x, p.y - d.y}
        let v2    = Math.Vector2{p.x + n.x, p.y + n.y}
        let prim1 = .pushTriangle(v0, v1, v2, pen.brush.color)

        if .curState.paintParams & .Antialiased
        {
            .addEdgeAA(prim1, v1, v2)
            .addEdgeAA(prim1, v0, v2)
        }
    }

    mtd drawCapSquareAnchor(p1, d1: Math.Vector2, sens: f32, pen: Pen, start: bool)
    {
        let sx = start ? pen.capBegScaleX : pen.capEndScaleX
        let sy = start ? pen.capBegScaleY : pen.capEndScaleY

        var p = p1
        let d = d1 * sx
        var n = Math.Vector2{d.y * sens, -d.x * sens}
        n.setLengthSafe(d.length() * sy)
        p -= n * 0.5

        let v0 = Math.Vector2{p.x + d.x, p.y + d.y}
        let v1 = Math.Vector2{p.x - d.x, p.y - d.y}
        let v2 = Math.Vector2{v1.x + n.x, v1.y + n.y}
        let v3 = Math.Vector2{v0.x + n.x, v0.y + n.y}

        let prim1 = .pushTriangle(v0, v1, v3, pen.brush.color)
        let prim2 = .pushTriangle(v1, v3, v2, pen.brush.color)

        if .curState.paintParams & .Antialiased
        {
            .addEdgeAA(prim1, v0, v3)
            .addEdgeAA(prim1, v1, v2)
            .addEdgeAA(prim1, v3, v2)
            .addEdgeAA(prim1, v0, v1)

            .addEdgeAA(prim2, v0, v3)
            .addEdgeAA(prim2, v1, v2)
            .addEdgeAA(prim2, v3, v2)
            .addEdgeAA(prim2, v0, v1)
        }
    }

    mtd drawCapArrowAnchor2(p1, d1: Math.Vector2, sens: f32, pen: Pen, start: bool)
    {
        let sx  = start ? pen.capBegScaleX : pen.capEndScaleX
        let sy  = start ? pen.capBegScaleY : pen.capEndScaleY
        let sx2 = sx * sy * 2

        var p  = p1
        let d  = d1 * sx
        let n  = Math.Vector2{d.y * sens * sy, -d.x * sens * sy}
        let n1 = n.toNormalized()
        p -= n

        {
            let v0    = Math.Vector2{p.x + d.x - n1.x * sx2, p.y + d.y - n1.y * sx2}
            let v1    = Math.Vector2{p.x, p.y}
            let v2    = Math.Vector2{p.x + n.x, p.y + n.y}
            let prim1 = .pushTriangle(v0, v1, v2, pen.brush.color)

            if .curState.paintParams & .Antialiased
            {
                .addEdgeAA(prim1, v0, v2)
                .addEdgeAA(prim1, v0, v1)
            }
        }

        {
            let v0    = Math.Vector2{p.x - d.x - n1.x * sx2, p.y - d.y - n1.y * sx2}
            let v1    = Math.Vector2{p.x, p.y}
            let v2    = Math.Vector2{p.x + n.x, p.y + n.y}
            let prim1 = .pushTriangle(v0, v1, v2, pen.brush.color)

            if .curState.paintParams & .Antialiased
            {
                .addEdgeAA(prim1, v0, v2)
                .addEdgeAA(prim1, v0, v1)
            }
        }
    }

    mtd drawCapArrowAnchor(p1, d1: Math.Vector2, sens: f32, pen: Pen, start: bool)
    {
        let sx = start ? pen.capBegScaleX : pen.capEndScaleX
        let sy = start ? pen.capBegScaleY : pen.capEndScaleY

        var p = p1
        let d = d1 * sx
        let n = Math.Vector2{d.y * sens * sy, -d.x * sens * sy}
        p -= n

        let v0    = Math.Vector2{p.x + d.x, p.y + d.y}
        let v1    = Math.Vector2{p.x - d.x, p.y - d.y}
        let v2    = Math.Vector2{p.x + n.x, p.y + n.y}
        let prim1 = .pushTriangle(v0, v1, v2, pen.brush.color)

        if .curState.paintParams & .Antialiased
        {
            .addEdgeAA(prim1, v1, v2)
            .addEdgeAA(prim1, v0, v2)
            .addEdgeAA(prim1, v0, v1)
        }
    }

    mtd drawCapRoundAnchor(p, d: Math.Vector2, pen: Pen, start: bool)
    {
        let sx = start ? pen.capBegScaleX : pen.capEndScaleX

        let angle0 = Math.atan2(-d.y, -d.x)
        let angle1 = angle0 + Math.ConstF32.TwoPi
        let radius = sx * d.length()
        .drawRound(p, radius, angle0, angle1, pen)
    }

    mtd drawCapRound(p, d: Math.Vector2, sens: f32, pen: Pen)
    {
        var radius = d.length()
        if radius < 2.5
        {
            .drawCapTriangle(p, d, sens, pen)
            return
        }

        let angle0 = Math.atan2(-d.y, -d.x)
        let angle1 = angle0 + Math.ConstF32.Pi
        radius *= sens
        .drawRound(p, radius, angle0, angle1, pen)
    }

    mtd drawLineCap(capStyle: LineCapStyle, p, d: Math.Vector2, sens: f32, pen: Pen, start: bool)
    {
        #[Swag.Complete]
        switch capStyle
        {
        case None:
            return
        case Triangle:
            .drawCapTriangle(p, d, sens, pen)
        case Round:
            .drawCapRound(p, d, sens, pen)
        case Square:
            .drawCapSquare(p, d, sens, pen)
        case SquareAnchor:
            .drawCapSquareAnchor(p, d, sens, pen, start)
        case ArrowAnchor:
            .drawCapArrowAnchor(p, d, sens, pen, start)
        case ArrowAnchor2:
            .drawCapArrowAnchor2(p, d, sens, pen, start)
        case RoundAnchor:
            .drawCapRoundAnchor(p, d, pen, start)
        }
    }

    mtd drawDashCap(cxt: *Context, capStyle: DashCapStyle, p, d: Math.Vector2, sens: f32)
    {
        if cxt.penSize < 3 do
            return

        #[Swag.Complete]
        switch capStyle
        {
        case None:
            break
        case Triangle:
            .drawCapTriangle(p, d, sens, dref cxt.pen)
        case Round:
            .drawCapRound(p, d, sens, dref cxt.pen)
        case Square:
            .drawCapSquare(p, d, sens, dref cxt.pen)
        }
    }

    mtd drawJoinBevel(p, d1, d2: Math.Vector2, pen: Pen)
    {
        var v0 = p

        var v1, v2: Math.Vector2 = undefined
        if d1.x * d2.y - d1.y * d2.x < 0
        {
            v0 += d1 * 0.25
            v0 += d2 * 0.25
            v1 = Math.Vector2{p.x - d1.x, p.y - d1.y}
            v2 = Math.Vector2{p.x - d2.x, p.y - d2.y}
        }
        else
        {
            v0 -= d1 * 0.25
            v0 -= d2 * 0.25
            v1 = Math.Vector2{p.x + d1.x, p.y + d1.y}
            v2 = Math.Vector2{p.x + d2.x, p.y + d2.y}
        }

        let prim1 = .pushTriangle(v0, v1, v2, pen.brush.color)
        if .curState.paintParams & .Antialiased do
            .addEdgeAA(prim1, v1, v2)
    }

    mtd drawRound(center: Math.Vector2, radius: f32, angle0, angle1: f32, pen: Pen)
    {
        var startPos, endPos: Math.Vector2 = undefined
        startPos.x = center.x + (Math.cos(angle0) * radius)
        startPos.y = center.y + (Math.sin(angle0) * radius)
        endPos.x   = center.x + (Math.cos(angle1) * radius)
        endPos.y   = center.y + (Math.sin(angle1) * radius)

        let minDist = LinePath.getFlattenDistance(.curState.paintQuality)

        // We divide a segment by half until the computed point on the curve is
        // close to the corresponding segment
        .sharedRoundTmp.clear()
        .sharedRoundTmp.add({angle0, startPos})
        .sharedRoundTmp.add({angle1, endPos})

        var point: Math.Vector2 = undefined
        for var i = 0; i < .sharedRoundTmp.count - 1; i += 1
        {
            let angle = (.sharedRoundTmp[i].factor + .sharedRoundTmp[i + 1].factor) * 0.5
            point.x = center.x + (Math.cos(angle) * radius)
            point.y = center.y + (Math.sin(angle) * radius)
            let dist = Math.Geometry.distanceToLine(point, .sharedRoundTmp[i].pos, .sharedRoundTmp[i + 1].pos)
            if dist < minDist do
                continue

            .sharedRoundTmp.insertAt(cast(u64) i + 1, {angle, point})
            i -= 1
        }

        var v1 = startPos
        for i in 1 until .sharedRoundTmp.count
        {
            let v2    = .sharedRoundTmp[i].pos
            let prim1 = .pushTriangle(center, v1, v2, pen.brush.color)
            if .curState.paintParams & .Antialiased do
                .addEdgeAA(prim1, v1, v2)
            v1 = v2
        }
    }

    mtd drawJoinRound(p, d1, d2: Math.Vector2, pen: Pen)
    {
        var angle0, angle1: f32 = undefined
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

        if Math.isEqualEpsilon(angle0, angle1) do
            return

        let radius = d1.length()

        if angle1 > angle0 + Math.ConstF32.Pi do
            angle1 -= Math.ConstF32.TwoPi
        elif angle1 < angle0 - Math.ConstF32.Pi do
            angle1 += Math.ConstF32.TwoPi

        .drawRound(p, radius, angle0, angle1, pen)
    }

    mtd drawJoinMiter(p, d1, d2: Math.Vector2, justMiter: bool, pen: Pen)
    {
        var a = Math.Vector2{d1.x + d2.x, d1.y + d2.y}
        let w = d1.length()
        let l = a.length()
        if Math.isZeroEpsilon(l) do
            return

        var m2 = (2 * w) / l
        if m2 > pen.joinMiterLimit
        {
            if justMiter do
                m2 = pen.joinMiterLimit
            else do
                return .drawJoinBevel(p, d1, d2, pen)
        }

        a.x = a.x / l * m2 * w
        a.y = a.y / l * m2 * w

        var v0 = p

        var v1, v2, v3: Math.Vector2 = undefined
        if d1.x * d2.y - d1.y * d2.x < 0
        {
            v0 += d1 * 0.25
            v0 += d2 * 0.25
            v1 = Math.Vector2{p.x - d1.x, p.y - d1.y}
            v2 = Math.Vector2{p.x - a.x, p.y - a.y}
            v3 = Math.Vector2{p.x - d2.x, p.y - d2.y}
        }
        else
        {
            v0 -= d1 * 0.25
            v0 -= d2 * 0.25
            v1 = Math.Vector2{p.x + d1.x, p.y + d1.y}
            v2 = Math.Vector2{p.x + a.x, p.y + a.y}
            v3 = Math.Vector2{p.x + d2.x, p.y + d2.y}
        }

        let prim1 = .pushTriangle(v0, v1, v3, pen.brush.color)
        let prim2 = .pushTriangle(v1, v3, v2, pen.brush.color)
        if .curState.paintParams & .Antialiased
        {
            .addEdgeAA(prim1, v3, v2)
            .addEdgeAA(prim1, v1, v2)
            .addEdgeAA(prim2, v3, v2)
            .addEdgeAA(prim2, v1, v2)
        }
    }

    mtd drawJoin(cxt: *Context, p, d1, d2: Math.Vector2)
    {
        #[Swag.Complete]
        switch cxt.pen.joinStyle
        {
        case None:
            break
        case Bevel:
            .drawJoinBevel(p, d1, d2, dref cxt.pen)
        case Round:
            .drawJoinRound(p, d1, d2, dref cxt.pen)
        case Miter:
            .drawJoinMiter(p, d1, d2, true, dref cxt.pen)
        case MiterBevel:
            .drawJoinMiter(p, d1, d2, false, dref cxt.pen)
        }
    }

    mtd drawDashEndCap(cxt: *Context, posCap, normalCap: Math.Vector2)
    {
        if cxt.beginLine or cxt.lastLineInvalid do
            return

        // Cap is defined
        if cxt.pen.dashEndCapStyle != DashCapStyle.None do
            .drawDashCap(cxt, cxt.pen.dashEndCapStyle, posCap, normalCap, -1.0)

        // AA end edge if no cap
        elif .curState.paintParams & .Antialiased
        {
            let prim2p = .vertexBuffer.buffer + cxt.prim2
            let prim2t = prim2p + 2
            .addEdgeAA(cxt.prim1, prim2p.pos, prim2t.pos)
            .addEdgeAA(cxt.prim2, prim2p.pos, prim2t.pos)
        }
    }

    func drawMoveCursor(cxt: *Context, pos: Math.Vector2)
    {
        if pos.distanceTo(cxt.path.endPoint) < 1
        {
            cxt.prev = cxt.path.endPoint
            if cxt.path.isClosed do
                cxt.beginLine = true
        }
        else
        {
            cxt.prev      = pos
            cxt.beginLine = true
        }
    }

    mtd drawLineInternal(cxt: *Context, pos: Math.Vector2)
    {
        // Compute line (half width on each side)
        let newPos = pos
        var n      = newPos - cxt.prev

        // If previous and next point are too close, we will take
        // the last normal
        if !n.isZeroEpsilon()
        {
            n.normalize()
            cxt.normal = n.cross(1)
            cxt.normal *= cxt.penSize2
        }

        // If this is still invalid, there's nothing we can:
        cxt.lastLineInvalid = cxt.normal.isZeroEpsilon()
        if cxt.lastLineInvalid do
            return

        // First line cap
        var flatBegin = !cxt.path.isClosed or cxt.pen.dashStyle != .None
        if cxt.pen.dashStyle != .None and (!cxt.firstLine or cxt.path.isClosed) and cxt.beginLine and cxt.pen.dashBegCapStyle != .None
        {
            flatBegin = false
            .drawDashCap(cxt, cxt.pen.dashBegCapStyle, cxt.prev, cxt.normal, 1.0)
        }

        // Joints
        if !cxt.beginLine do
            .drawJoin(cxt, cxt.prev, cxt.normalPrev, cxt.normal)

        // Draw quad
        let v0 = cxt.prev - cxt.normal
        let v1 = newPos - cxt.normal
        let v3 = cxt.prev + cxt.normal
        let v2 = newPos + cxt.normal
        cxt.prim1 = .pushTriangle(v0, v1, v3, cxt.pen.brush.color)
        cxt.prim2 = .pushTriangle(v1, v3, v2, cxt.pen.brush.color)

        // Generate AA edges
        if .curState.paintParams & .Antialiased
        {
            .addEdgeAA(cxt.prim1, v0, v1)
            .addEdgeAA(cxt.prim1, v3, v2)
            .addEdgeAA(cxt.prim2, v0, v1)
            .addEdgeAA(cxt.prim2, v3, v2)

            // If we are starting a line, and the line does not have a cap before, then
            // we must AA the corresponding edge
            if flatBegin and cxt.beginLine and (!cxt.firstLine or cxt.pen.begCapStyle == .None or cxt.path.isClosed)
            {
                .addEdgeAA(cxt.prim1, v0, v3)
                .addEdgeAA(cxt.prim2, v0, v3)
            }
        }

        // To draw end joint
        if cxt.firstLine do
            cxt.normalFirst = cxt.normal

        cxt.normalPrev = cxt.normal
        cxt.prev       = newPos
        cxt.beginLine, cxt.firstLine = false
    }

    func preparePattern(cxt: *Context)
    {
        #[Swag.Complete]
        switch cxt.pen.dashStyle
        {
        case None:
            return
        case Pattern:
            cxt.pattern      = cxt.pen.pattern.buffer
            cxt.patternCount = cast(s32) cxt.pen.pattern.count
            if cxt.patternCount == 0 do
                fallthrough
        case Dot:
            const P = [1.0, 1.0]
            cxt.pattern      = @dataof(P)
            cxt.patternCount = 2
        case Dash:
            const P = [2.0, 1.0]
            cxt.pattern      = @dataof(P)
            cxt.patternCount = 2
        case DashDot:
            const P = [2.0, 1.0, 1.0, 1.0]
            cxt.pattern      = @dataof(P)
            cxt.patternCount = 4
        }

        cxt.patternTotalLen = 0
        for i in cxt.patternCount do
            cxt.patternTotalLen += Math.max(cxt.pattern[i], 0.01)
        cxt.patternTotalLen *= cxt.pen.size

        let repeatCount = cast(s32) (cxt.path.polygon.totalLen / cxt.patternTotalLen)
        cxt.error = cxt.path.polygon.totalLen - (repeatCount * cxt.patternTotalLen)

        // We must dispatch error at each blank in the pattern. So "divError" is the
        // number of blanks to add at each 'move', between solid patterns
        let numBlanks = repeatCount * cxt.patternCount / 2
        cxt.divError = numBlanks ? cxt.error / numBlanks : 0

        // First pattern. We start at half the size of the first pattern, in order to not
        // finish on a blank, but with half the size of the first pattern also.
        cxt.patternIndex        = 0
        cxt.remainingPatternLen = Math.max(cxt.pattern[cxt.patternIndex], 0.01)
        cxt.remainingPatternLen *= cxt.pen.size
        if !cxt.path.isClosed do
            cxt.remainingPatternLen *= 0.5
    }

    #[Swag.Overload]
    mtd drawLine(cxt: *Context, pos: Math.Vector2)
    {
        // solid
        if cxt.pen.dashStyle == .None
        {
            .drawLineInternal(cxt, pos)
            return
        }

        var v               = pos - cxt.prev
        var remainingSegLen = v.length()
        v.normalizeSafe()

        // True when we are "drawing" blanks
        var blankPatternIdx = cxt.patternIndex & 1 ? true : false

        for
        {
            var lengthInPattern = cxt.remainingPatternLen

            // If we are drawing a blank, take care of error adjustement
            var addBlank = 0'f32
            if cxt.divError > 0 and blankPatternIdx
            {
                addBlank = Math.min(cxt.error, cxt.divError)
                lengthInPattern += addBlank
            }

            if blankPatternIdx do
                lengthInPattern *= cxt.pen.dashBlankScale

            // The current remaining length in the current segment is greater
            // than what's left in the current pattern. We draw to cover the
            // current pattern, and change the pattern to be the next
            if remainingSegLen > lengthInPattern
            {
                let nextp = cxt.prev + (v * lengthInPattern)
                if blankPatternIdx
                {
                    .drawDashEndCap(cxt, cxt.prev, cxt.normal)
                    drawMoveCursor(cxt, nextp)
                    cxt.error -= addBlank
                }
                else
                {
                    .drawLineInternal(cxt, nextp)
                }

                // Start a new fresh pattern index
                remainingSegLen -= lengthInPattern
                cxt.patternIndex        = (cxt.patternIndex + 1) % cxt.patternCount
                blankPatternIdx         = blankPatternIdx ? false : true
                cxt.remainingPatternLen = Math.max(cxt.pattern[cxt.patternIndex], 0.01)
                cxt.remainingPatternLen *= cxt.pen.size
                continue
            }

            // Here the segment does not have enough remaining length to cover the current
            // pattern. So draw what remains, and exit. The next line will try to cover the rest.
            cxt.remainingPatternLen -= remainingSegLen
            if blankPatternIdx
            {
                .drawDashEndCap(cxt, cxt.prev, cxt.normal)
                drawMoveCursor(cxt, pos)
            }
            else
            {
                .drawLineInternal(cxt, pos)
            }
            break
        }
    }

    // Optimized version that returns both distance and point information
    struct AnchorSkipInfo
    {
        distance:          f32
        adjustedPoint:     Math.Vector2
        pointIndex:        u64
        hasAdjustment:     bool
    }

    // Enhanced function that calculates skip distance AND finds the corresponding point
    func getAnchorSkipInfo(cxt: *Context, capStyle: LineCapStyle, start: bool)->AnchorSkipInfo
    {
        var info: AnchorSkipInfo
        info.hasAdjustment = false

        // Calculate skip distance
        switch capStyle
        {
        case ArrowAnchor:
            let scale = start ? cxt.pen.capBegScaleX : cxt.pen.capEndScaleX
            info.distance = cxt.pen.size * 0.5 * scale

        case ArrowAnchor2:
            let scale = start ? cxt.pen.capBegScaleX : cxt.pen.capEndScaleX
            info.distance = cxt.pen.size * 0.7 * scale

        case SquareAnchor:
            let scale = start ? cxt.pen.capBegScaleX : cxt.pen.capEndScaleX
            info.distance = cxt.pen.size * 0.5 * scale
            info.distance -= 1

        case RoundAnchor:
            let scale = start ? cxt.pen.capBegScaleX : cxt.pen.capEndScaleX
            info.distance = cxt.pen.size * 0.5 * scale
            info.distance -= Math.max(1, info.distance * 0.2)

        default:
            info.distance = 0
            return info
        }

        // If no skip distance needed, return early
        if info.distance <= 0 do
            return info

        let pointCount = cxt.path.polygon.points.count
        if pointCount < 2 do
            return info

        var accumulatedDistance = 0'f32

        if start
        {
            let startPoint = cxt.path.polygon.getPoint(0, cxt.offsetPath)
            var lastPoint  = startPoint

            for i in 1 until pointCount
            {
                let currentPoint = cxt.path.polygon.getPoint(i, cxt.offsetPath)
                let segmentVec   = currentPoint - lastPoint
                let segmentLen   = segmentVec.length()

                if segmentLen > 0.01
                {
                    if accumulatedDistance + segmentLen >= info.distance
                    {
                        // Found the target segment - calculate adjusted point
                        let remainingDist = info.distance - accumulatedDistance
                        let ratio         = remainingDist / segmentLen
                        info.adjustedPoint = lastPoint + segmentVec * ratio
                        info.pointIndex    = i
                        info.hasAdjustment = true
                        return info
                    }

                    accumulatedDistance += segmentLen
                    lastPoint = currentPoint
                }
            }
        }
        else
        {
            let endPoint  = cxt.path.polygon.getPoint(pointCount - 1, cxt.offsetPath)
            var lastPoint = endPoint

            for #reverse i in 0 to pointCount - 2
            {
                let currentPoint = cxt.path.polygon.getPoint(i, cxt.offsetPath)
                let segmentVec   = lastPoint - currentPoint
                let segmentLen   = segmentVec.length()

                if segmentLen > 0.01
                {
                    if accumulatedDistance + segmentLen >= info.distance
                    {
                        // Found the target segment - calculate adjusted point
                        let remainingDist = info.distance - accumulatedDistance
                        let ratio         = remainingDist / segmentLen
                        info.adjustedPoint = lastPoint - segmentVec * ratio
                        info.pointIndex    = i
                        info.hasAdjustment = true
                        return info
                    }

                    accumulatedDistance += segmentLen
                    lastPoint = currentPoint
                }
            }
        }

        return info
    }

    // Simplified drawPath method using the optimized approach
    #[Swag.Overload]
    mtd drawPath(cxt: *Context)
    {
        if cxt.pen.size < 1 or cxt.path.polygon.points.count == 0 do
            return

        cxt.penSize    = cxt.pen.size
        cxt.offsetPath = cxt.pen.plotOffset
        cxt.penSize2   = cxt.penSize * 0.5
        cxt.beginLine  = true
        cxt.firstLine  = true

        // Get skip information for both caps in one pass
        let beginSkipInfo = getAnchorSkipInfo(cxt, cxt.pen.begCapStyle, true)
        let endSkipInfo   = getAnchorSkipInfo(cxt, cxt.pen.endCapStyle, false)

        var actualStartPoint = cxt.path.polygon.getPoint(0, cxt.offsetPath)
        var actualEndPoint   = cxt.path.polygon.getPoint(cxt.path.polygon.points.count - 1, cxt.offsetPath)
        var startPointIndex  = 0'u64
        var endPointIndex    = cxt.path.polygon.points.count - 1

        // Apply adjustments if needed
        if !cxt.path.isClosed and beginSkipInfo.hasAdjustment
        {
            actualStartPoint = beginSkipInfo.adjustedPoint
            startPointIndex  = beginSkipInfo.pointIndex
        }

        if !cxt.path.isClosed and endSkipInfo.hasAdjustment
        {
            actualEndPoint = endSkipInfo.adjustedPoint
            endPointIndex  = endSkipInfo.pointIndex
        }

        // Set starting position
        cxt.prev = actualStartPoint

        // Draw line caps first (if not closed)
        if !cxt.path.isClosed
        {
            // Draw begin cap
            if cxt.pen.begCapStyle != .None
            {
                let v1 = cxt.path.polygon.getPoint(0, cxt.offsetPath)
                let v2 = beginSkipInfo.hasAdjustment ? actualStartPoint : cxt.path.polygon.getPoint(1, cxt.offsetPath)
                var n  = v2 - v1
                n.normalizeSafe()
                var c = n.cross(1)
                c *= cxt.penSize2
                .drawLineCap(cxt.pen.begCapStyle, v1, c, 1.0, dref cxt.pen, true)
            }

            // Draw end cap
            if cxt.pen.endCapStyle != .None
            {
                let v1 = cxt.path.polygon.getPoint(cxt.path.polygon.points.count - 1, cxt.offsetPath)
                let v2 = endSkipInfo.hasAdjustment ? actualEndPoint : cxt.path.polygon.getPoint(cxt.path.polygon.points.count - 2, cxt.offsetPath)
                var n  = v1 - v2
                n.normalizeSafe()
                var c = n.cross(1)
                c *= cxt.penSize2
                .drawLineCap(cxt.pen.endCapStyle, v1, c, -1.0, dref cxt.pen, false)
            }
        }

        // Draw the line segments with proper start/end adjustment
        if cxt.path.polygon.points.count > 1
        {
            // Draw intermediate segments
            if startPointIndex < endPointIndex
            {
                // Draw from adjusted start to the appropriate next point
                if startPointIndex < cxt.path.polygon.points.count
                {
                    let nextPoint = cxt.path.polygon.getPoint(startPointIndex, cxt.offsetPath)
                    let startTo   = actualStartPoint - nextPoint
                    if startTo.length() > 0.01 do
                        .drawLine(cxt, nextPoint)
                }

                for i in (startPointIndex + 1) to endPointIndex
                {
                    if i < cxt.path.polygon.points.count
                    {
                        let p = cxt.path.polygon.getPoint(i, cxt.offsetPath)
                        .drawLine(cxt, p)
                    }
                }

                // Draw final segment to adjusted end point if needed
                let endTo = actualEndPoint - cxt.path.polygon.getPoint(endPointIndex, cxt.offsetPath)
                if endTo.length() > 0.01
                {
                    .drawLine(cxt, actualEndPoint)
                }
            }
            else
            {
                .drawLine(cxt, actualEndPoint)
            }
        }

        // Handle AA and closing as before...
        if !cxt.lastLineInvalid and !cxt.path.isClosed and (.curState.paintParams & .Antialiased) and (cxt.pen.endCapStyle == .None)
        {
            let prim2p = .vertexBuffer.buffer + cxt.prim2
            let prim2t = prim2p + 2
            .addEdgeAA(cxt.prim1, prim2p.pos, prim2t.pos)
            .addEdgeAA(cxt.prim2, prim2p.pos, prim2t.pos)
        }
        elif cxt.path.isClosed and !cxt.firstLine and !cxt.beginLine
        {
            .drawJoin(cxt, cxt.prev, cxt.normal, cxt.normalFirst)
        }
    }
}

public impl Painter
{
    // Draw a line path.
    #[Swag.Overload]
    mtd drawPath(path: *LinePath, color: Color, penSize: f32)
    {
        let pen = Pen.createSolid(color, penSize)
        .drawPath(path, pen)
    }

    #[Swag.Overload]
    mtd drawPath(path: *LinePath, pen: Pen)
    {
        path.flatten(.curState.paintQuality)
        if Math.isZeroEpsilon(path.polygon.totalLen) do
            return

        var cxt: Context
        cxt.path = path
        cxt.pen  = &pen
        if pen.dashStyle != .None do
            preparePattern(&cxt)

        var bvMin = path.bvMin
        var bvMax = path.bvMax
        bvMin -= pen.size / 2
        bvMax += pen.size / 2
        .setTexture0(bvMin, bvMax, pen.brush)
        .resetTextureFont()

        .startOverlapMode(pen)
        .drawPath(&cxt)
        .stopOverlapMode()
    }

    // Draw a list of paths.
    #[Swag.Overload]
    mtd drawPath(pathList: *LinePathList, pen: Pen, mode: DrawPathListMode = .Separate)
    {
        if !pathList.paths.count do
            return
        if mode == .Separate
        {
            foreach p in pathList.paths do
                .drawPath(p, pen)
            return
        }

        pathList.flatten(.curState.paintQuality)

        var bvMin = pathList.bvMin
        var bvMax = pathList.bvMax
        bvMin -= pen.size / 2
        bvMax += pen.size / 2
        .setTexture0(bvMin, bvMax, pen.brush)
        .resetTextureFont()

        .startOverlapMode(pen)

        var cxt: Context
        foreach p in pathList.paths
        {
            cxt.path = p
            cxt.pen  = &pen
            if pen.dashStyle != .None do
                preparePattern(&cxt)
            .drawPath(&cxt)
        }

        .stopOverlapMode()
    }

    #[Swag.Overload]
    mtd drawPath(pathList: *LinePathList, color: Color, penSize: f32, mode: DrawPathListMode = .Separate)
    {
        if !pathList.paths.count do
            return

        let pen = Pen.createSolid(color, penSize)
        .drawPath(pathList, pen, mode)
    }

    // Returns the extent in both directions of a given LineCapStyle.
    // * 'lineSize' is the supposed line drawing size
    // * 'capScale' is the supposed LineCapStyle scale
    // Returns the width and the length.
    func capStyleExtent(capStyle: LineCapStyle, lineSize, capScale: f32)->{ w: f32, l: f32 }
    {
        #[Swag.Complete]
        switch capStyle
        {
        case None:
            return {lineSize * 0.5, 0}
        case Triangle:
            return {lineSize * 0.5, lineSize * 0.5}
        case Round:
            return {lineSize * 0.5, lineSize * 0.5}
        case Square:
            return {lineSize * 0.5, lineSize * 0.5}
        case ArrowAnchor:
            return {lineSize * 0.5 * capScale, 0}
        case ArrowAnchor2:
            return {lineSize * 0.5 * capScale, 0}
        case SquareAnchor:
            return {lineSize * 0.5 * capScale, lineSize * 0.5 * capScale}
        case RoundAnchor:
            return {lineSize * 0.5 * capScale, lineSize * 0.5 * capScale}
        }

        return {0, 0}
    }
}
