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
    pattern:                 const ^f32
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

        let prim1 = me.pushTriangle(v0, v1, v3, pen.brush.color)
        let prim2 = me.pushTriangle(v1, v3, v2, pen.brush.color)

        if me.curState.paintParams & .Antialiased
        {
            me.addEdgeAA(prim1, v0, v3)
            me.addEdgeAA(prim1, v1, v2)
            me.addEdgeAA(prim2, v3, v2)
            me.addEdgeAA(prim2, v1, v2)
            me.addEdgeAA(prim2, v0, v3)
        }
    }

    mtd drawCapTriangle(p, d: Math.Vector2, sens: f32, pen: Pen)
    {
        let n     = Math.Vector2{d.y * sens, -d.x * sens}
        let v0    = Math.Vector2{p.x + d.x, p.y + d.y}
        let v1    = Math.Vector2{p.x - d.x, p.y - d.y}
        let v2    = Math.Vector2{p.x + n.x, p.y + n.y}
        let prim1 = me.pushTriangle(v0, v1, v2, pen.brush.color)

        if me.curState.paintParams & .Antialiased
        {
            me.addEdgeAA(prim1, v1, v2)
            me.addEdgeAA(prim1, v0, v2)
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

        let prim1 = me.pushTriangle(v0, v1, v3, pen.brush.color)
        let prim2 = me.pushTriangle(v1, v3, v2, pen.brush.color)

        if me.curState.paintParams & .Antialiased
        {
            me.addEdgeAA(prim1, v0, v3)
            me.addEdgeAA(prim1, v1, v2)
            me.addEdgeAA(prim1, v3, v2)
            me.addEdgeAA(prim1, v0, v1)

            me.addEdgeAA(prim2, v0, v3)
            me.addEdgeAA(prim2, v1, v2)
            me.addEdgeAA(prim2, v3, v2)
            me.addEdgeAA(prim2, v0, v1)
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
            let prim1 = me.pushTriangle(v0, v1, v2, pen.brush.color)

            if me.curState.paintParams & .Antialiased
            {
                me.addEdgeAA(prim1, v0, v2)
                me.addEdgeAA(prim1, v0, v1)
            }
        }

        {
            let v0    = Math.Vector2{p.x - d.x - n1.x * sx2, p.y - d.y - n1.y * sx2}
            let v1    = Math.Vector2{p.x, p.y}
            let v2    = Math.Vector2{p.x + n.x, p.y + n.y}
            let prim1 = me.pushTriangle(v0, v1, v2, pen.brush.color)

            if me.curState.paintParams & .Antialiased
            {
                me.addEdgeAA(prim1, v0, v2)
                me.addEdgeAA(prim1, v0, v1)
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
        let prim1 = me.pushTriangle(v0, v1, v2, pen.brush.color)

        if me.curState.paintParams & .Antialiased
        {
            me.addEdgeAA(prim1, v1, v2)
            me.addEdgeAA(prim1, v0, v2)
            me.addEdgeAA(prim1, v0, v1)
        }
    }

    mtd drawCapRoundAnchor(p, d: Math.Vector2, pen: Pen, start: bool)
    {
        let sx = start ? pen.capBegScaleX : pen.capEndScaleX

        let angle0 = Math.atan2(-d.y, -d.x)
        let angle1 = angle0 + Math.ConstF32.TwoPi
        let radius = sx * d.length()
        me.drawRound(p, radius, angle0, angle1, pen)
    }

    mtd drawCapRound(p, d: Math.Vector2, sens: f32, pen: Pen)
    {
        var radius = d.length()
        if radius < 2.5
        {
            me.drawCapTriangle(p, d, sens, pen)
            return
        }

        let angle0 = Math.atan2(-d.y, -d.x)
        let angle1 = angle0 + Math.ConstF32.Pi
        radius *= sens
        me.drawRound(p, radius, angle0, angle1, pen)
    }

    mtd drawLineCap(capStyle: LineCapStyle, p, d: Math.Vector2, sens: f32, pen: Pen, start: bool)
    {
        #[Swag.Complete]
        switch capStyle
        {
        case None:
            return
        case Triangle:
            me.drawCapTriangle(p, d, sens, pen)
        case Round:
            me.drawCapRound(p, d, sens, pen)
        case Square:
            me.drawCapSquare(p, d, sens, pen)
        case SquareAnchor:
            me.drawCapSquareAnchor(p, d, sens, pen, start)
        case ArrowAnchor:
            me.drawCapArrowAnchor(p, d, sens, pen, start)
        case ArrowAnchor2:
            me.drawCapArrowAnchor2(p, d, sens, pen, start)
        case RoundAnchor:
            me.drawCapRoundAnchor(p, d, pen, start)
        }
    }

    mtd drawDashCap(using cxt: *Context, capStyle: DashCapStyle, p, d: Math.Vector2, sens: f32)
    {
        if penSize < 3:
            return

        #[Swag.Complete]
        switch capStyle
        {
        case None:
            break
        case Triangle:
            me.drawCapTriangle(p, d, sens, dref pen)
        case Round:
            me.drawCapRound(p, d, sens, dref pen)
        case Square:
            me.drawCapSquare(p, d, sens, dref pen)
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

        let prim1 = me.pushTriangle(v0, v1, v2, pen.brush.color)
        if me.curState.paintParams & .Antialiased:
            me.addEdgeAA(prim1, v1, v2)
    }

    mtd drawRound(center: Math.Vector2, radius: f32, angle0, angle1: f32, pen: Pen)
    {
        var startPos, endPos: Math.Vector2 = undefined
        startPos.x = center.x + (Math.cos(angle0) * radius)
        startPos.y = center.y + (Math.sin(angle0) * radius)
        endPos.x   = center.x + (Math.cos(angle1) * radius)
        endPos.y   = center.y + (Math.sin(angle1) * radius)

        let minDist = LinePath.getFlattenDistance(me.curState.paintQuality)

        // We divide a segment by half until the computed point on the curve is
        // close to the corresponding segment
        me.sharedRoundTmp.clear()
        me.sharedRoundTmp.add({angle0, startPos})
        me.sharedRoundTmp.add({angle1, endPos})

        var point: Math.Vector2 = undefined
        for var i = 0; i < me.sharedRoundTmp.count - 1; i += 1
        {
            let angle = (me.sharedRoundTmp[i].factor + me.sharedRoundTmp[i + 1].factor) * 0.5
            point.x = center.x + (Math.cos(angle) * radius)
            point.y = center.y + (Math.sin(angle) * radius)
            let dist = Math.Geometry.distanceToLine(point, me.sharedRoundTmp[i].pos, me.sharedRoundTmp[i + 1].pos)
            if dist < minDist:
                continue

            me.sharedRoundTmp.insertAt(cast(u64) i + 1, {angle, point})
            i -= 1
        }

        var v1 = startPos
        for i in 1 until me.sharedRoundTmp.count
        {
            let v2    = me.sharedRoundTmp[i].pos
            let prim1 = me.pushTriangle(center, v1, v2, pen.brush.color)
            if me.curState.paintParams & .Antialiased:
                me.addEdgeAA(prim1, v1, v2)
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

        if Math.isEqualEpsilon(angle0, angle1):
            return

        let radius = d1.length()

        if angle1 > angle0 + Math.ConstF32.Pi:
            angle1 -= Math.ConstF32.TwoPi
        elif angle1 < angle0 - Math.ConstF32.Pi:
            angle1 += Math.ConstF32.TwoPi

        me.drawRound(p, radius, angle0, angle1, pen)
    }

    mtd drawJoinMiter(p, d1, d2: Math.Vector2, justMiter: bool, pen: Pen)
    {
        var a = Math.Vector2{d1.x + d2.x, d1.y + d2.y}
        let w = d1.length()
        let l = a.length()
        if Math.isZeroEpsilon(l):
            return

        var m2 = (2 * w) / l
        if m2 > pen.joinMiterLimit
        {
            if justMiter:
                m2 = pen.joinMiterLimit
            else:
                return me.drawJoinBevel(p, d1, d2, pen)
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

        let prim1 = me.pushTriangle(v0, v1, v3, pen.brush.color)
        let prim2 = me.pushTriangle(v1, v3, v2, pen.brush.color)
        if me.curState.paintParams & .Antialiased
        {
            me.addEdgeAA(prim1, v3, v2)
            me.addEdgeAA(prim1, v1, v2)
            me.addEdgeAA(prim2, v3, v2)
            me.addEdgeAA(prim2, v1, v2)
        }
    }

    mtd drawJoin(using cxt: *Context, p, d1, d2: Math.Vector2)
    {
        #[Swag.Complete]
        switch pen.joinStyle
        {
        case None:
            break
        case Bevel:
            me.drawJoinBevel(p, d1, d2, dref pen)
        case Round:
            me.drawJoinRound(p, d1, d2, dref pen)
        case Miter:
            me.drawJoinMiter(p, d1, d2, true, dref pen)
        case MiterBevel:
            me.drawJoinMiter(p, d1, d2, false, dref pen)
        }
    }

    mtd drawDashEndCap(using cxt: *Context, posCap, normalCap: Math.Vector2)
    {
        if beginLine or lastLineInvalid:
            return

        // Cap is defined
        if pen.dashEndCapStyle != DashCapStyle.None:
            me.drawDashCap(cxt, pen.dashEndCapStyle, posCap, normalCap, -1.0)

        // AA end edge if no cap
        elif me.curState.paintParams & .Antialiased
        {
            let prim2p = me.vertexBuffer.buffer + prim2
            let prim2t = prim2p + 2
            me.addEdgeAA(prim1, prim2p.pos, prim2t.pos)
            me.addEdgeAA(prim2, prim2p.pos, prim2t.pos)
        }
    }

    func drawMoveCursor(using cxt: *Context, pos: Math.Vector2)
    {
        if pos.distanceTo(path.endPoint) < 1
        {
            prev = path.endPoint
            if path.isClosed:
                beginLine = true
        }
        else
        {
            prev      = pos
            beginLine = true
        }
    }

    mtd drawLineInternal(using cxt: *Context, pos: Math.Vector2)
    {
        // Compute line (half width on each side)
        var newPos = pos
        var n      = newPos - prev

        // If previous and next point are too close, we will take
        // the last normal
        if !n.isZeroEpsilon()
        {
            n.normalize()
            normal = n.cross(1)
            normal *= penSize2
        }

        // If this is still invalid, there's nothing we can:
        lastLineInvalid = normal.isZeroEpsilon()
        if lastLineInvalid:
            return

        // First line cap
        var flatBegin = !path.isClosed or pen.dashStyle != .None
        if pen.dashStyle != .None and (!firstLine or path.isClosed) and beginLine and pen.dashBegCapStyle != .None
        {
            flatBegin = false
            me.drawDashCap(cxt, pen.dashBegCapStyle, prev, normal, 1.0)
        }

        // Joints
        if !beginLine:
            me.drawJoin(cxt, prev, normalPrev, normal)

        // Draw quad
        var v0 = prev - normal
        var v1 = newPos - normal
        var v3 = prev + normal
        var v2 = newPos + normal
        prim1 = me.pushTriangle(v0, v1, v3, pen.brush.color)
        prim2 = me.pushTriangle(v1, v3, v2, pen.brush.color)

        // Generate AA edges
        if me.curState.paintParams & .Antialiased
        {
            me.addEdgeAA(prim1, v0, v1)
            me.addEdgeAA(prim1, v3, v2)
            me.addEdgeAA(prim2, v0, v1)
            me.addEdgeAA(prim2, v3, v2)

            // If we are starting a line, and the line does not have a cap before, then
            // we must AA the corresponding edge
            if flatBegin and beginLine and (!firstLine or pen.begCapStyle == .None or path.isClosed)
            {
                me.addEdgeAA(prim1, v0, v3)
                me.addEdgeAA(prim2, v0, v3)
            }
        }

        // To draw end joint
        if firstLine:
            normalFirst = normal

        normalPrev = normal
        prev       = newPos
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
            pattern      = pen.pattern.buffer
            patternCount = cast(s32) pen.pattern.count
            if patternCount == 0:
                fallthrough
        case Dot:
            const P = [1.0, 1.0]
            pattern      = @dataof(P)
            patternCount = 2
        case Dash:
            const P = [2.0, 1.0]
            pattern      = @dataof(P)
            patternCount = 2
        case DashDot:
            const P = [2.0, 1.0, 1.0, 1.0]
            pattern      = @dataof(P)
            patternCount = 4
        }

        patternTotalLen = 0
        for i in patternCount:
            patternTotalLen += Math.max(pattern[i], 0.01)
        patternTotalLen *= pen.size

        let repeatCount = cast(s32) (path.polygon.totalLen / patternTotalLen)
        error = path.polygon.totalLen - (repeatCount * patternTotalLen)

        // We must dispatch error at each blank in the pattern. So "divError" is the
        // number of blanks to add at each 'move', between solid patterns
        let numBlanks = repeatCount * patternCount / 2
        divError = numBlanks ? error / numBlanks : 0

        // First pattern. We start at half the size of the first pattern, in order to not
        // finish on a blank, but with half the size of the first pattern also.
        patternIndex        = 0
        remainingPatternLen = Math.max(pattern[patternIndex], 0.01)
        remainingPatternLen *= pen.size
        if !path.isClosed:
            remainingPatternLen *= 0.5
    }

    #[Swag.Overload]
    mtd drawLine(using cxt: *Context, pos: Math.Vector2)
    {
        // solid
        if pen.dashStyle == .None
        {
            me.drawLineInternal(cxt, pos)
            return
        }

        var v               = pos - prev
        var remainingSegLen = v.length()
        v.normalizeSafe()

        // True when we are "drawing" blanks
        var blankPatternIdx = patternIndex & 1 ? true : false

        for
        {
            var lengthInPattern = remainingPatternLen

            // If we are drawing a blank, take care of error adjustement
            var addBlank = 0'f32
            if divError > 0 and blankPatternIdx
            {
                addBlank = Math.min(error, divError)
                lengthInPattern += addBlank
            }

            if blankPatternIdx:
                lengthInPattern *= pen.dashBlankScale

            // The current remaining length in the current segment is greater
            // than what's left in the current pattern. We draw to cover the
            // current pattern, and change the pattern to be the next
            if remainingSegLen > lengthInPattern
            {
                var nextp = prev + (v * lengthInPattern)
                if blankPatternIdx
                {
                    me.drawDashEndCap(cxt, prev, normal)
                    drawMoveCursor(cxt, nextp)
                    error -= addBlank
                }
                else
                {
                    me.drawLineInternal(cxt, nextp)
                }

                // Start a new fresh pattern index
                remainingSegLen -= lengthInPattern
                patternIndex        = (patternIndex + 1) % patternCount
                blankPatternIdx     = blankPatternIdx ? false : true
                remainingPatternLen = Math.max(pattern[patternIndex], 0.01)
                remainingPatternLen *= pen.size
                continue
            }

            // Here the segment does not have enough remaining length to cover the current
            // pattern. So draw what remains, and exit. The next line will try to cover the rest.
            remainingPatternLen -= remainingSegLen
            if blankPatternIdx
            {
                me.drawDashEndCap(cxt, prev, normal)
                drawMoveCursor(cxt, pos)
            }
            else
            {
                me.drawLineInternal(cxt, pos)
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
    func getAnchorSkipInfo(using cxt: *Context, capStyle: LineCapStyle, start: bool)->AnchorSkipInfo
    {
        var info: AnchorSkipInfo
        info.hasAdjustment = false

        // Calculate skip distance
        switch capStyle
        {
        case ArrowAnchor, ArrowAnchor2:
            let scale = start ? pen.capBegScaleX : pen.capEndScaleX
            info.distance = pen.size * 0.5 * scale

        case SquareAnchor:
            let scale = start ? pen.capBegScaleX : pen.capEndScaleX
            info.distance = pen.size * 0.5 * scale

        case RoundAnchor:
            let scale = start ? pen.capBegScaleX : pen.capEndScaleX
            info.distance = pen.size * 0.5 * scale

        default:
            info.distance = 0
            return info
        }

        // If no skip distance needed, return early
        if info.distance <= 0:
            return info

        let pointCount = path.polygon.points.count
        if pointCount < 2:
            return info

        var accumulatedDistance = 0'f32

        if start
        {
            let startPoint = path.polygon.getPoint(0, offsetPath)
            var lastPoint  = startPoint

            for i in 1 until pointCount
            {
                let currentPoint = path.polygon.getPoint(i, offsetPath)
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
            let endPoint  = path.polygon.getPoint(pointCount - 1, offsetPath)
            var lastPoint = endPoint

            for #reverse i in 0 to pointCount - 2
            {
                let currentPoint = path.polygon.getPoint(i, offsetPath)
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
    mtd drawPath(using cxt: *Context)
    {
        if pen.size < 1 or path.polygon.points.count == 0:
            return

        penSize        = pen.size
        cxt.offsetPath = pen.plotOffset
        penSize2       = penSize * 0.5
        beginLine      = true
        firstLine      = true

        // Get skip information for both caps in one pass
        let beginSkipInfo = getAnchorSkipInfo(cxt, pen.begCapStyle, true)
        let endSkipInfo   = getAnchorSkipInfo(cxt, pen.endCapStyle, false)

        var actualStartPoint = path.polygon.getPoint(0, cxt.offsetPath)
        var actualEndPoint   = path.polygon.getPoint(path.polygon.points.count - 1, cxt.offsetPath)
        var startPointIndex  = 0'u64
        var endPointIndex    = path.polygon.points.count - 1

        // Apply adjustments if needed
        if !path.isClosed and beginSkipInfo.hasAdjustment
        {
            actualStartPoint = beginSkipInfo.adjustedPoint
            startPointIndex  = beginSkipInfo.pointIndex
        }

        if !path.isClosed and endSkipInfo.hasAdjustment
        {
            actualEndPoint = endSkipInfo.adjustedPoint
            endPointIndex  = endSkipInfo.pointIndex
        }

        // Set starting position
        prev = actualStartPoint

        // Draw line caps first (if not closed)
        if !path.isClosed
        {
            // Draw begin cap
            if pen.begCapStyle != .None
            {
                var v1 = path.polygon.getPoint(0, cxt.offsetPath)
                var v2 = beginSkipInfo.hasAdjustment ? actualStartPoint : path.polygon.getPoint(1, cxt.offsetPath)
                var n  = v2 - v1
                n.normalizeSafe()
                var c = n.cross(1)
                c *= penSize2
                me.drawLineCap(pen.begCapStyle, v1, c, 1.0, dref pen, true)
            }

            // Draw end cap
            if pen.endCapStyle != .None
            {
                var v1 = path.polygon.getPoint(path.polygon.points.count - 1, cxt.offsetPath)
                var v2 = endSkipInfo.hasAdjustment ? actualEndPoint : path.polygon.getPoint(path.polygon.points.count - 2, cxt.offsetPath)
                var n  = v1 - v2
                n.normalizeSafe()
                var c = n.cross(1)
                c *= penSize2
                me.drawLineCap(pen.endCapStyle, v1, c, -1.0, dref pen, false)
            }
        }

        // Draw the line segments with proper start/end adjustment
        if path.polygon.points.count > 1
        {
            // Draw intermediate segments
            if startPointIndex < endPointIndex
            {
                // Draw from adjusted start to the appropriate next point
                if startPointIndex < path.polygon.points.count
                {
                    let nextPoint = path.polygon.getPoint(startPointIndex, cxt.offsetPath)
                    let startTo   = actualStartPoint - nextPoint
                    if startTo.length() > 0.01:
                        me.drawLine(cxt, nextPoint)
                }

                for i in (startPointIndex + 1) to endPointIndex
                {
                    if i < path.polygon.points.count
                    {
                        var p = path.polygon.getPoint(i, cxt.offsetPath)
                        me.drawLine(cxt, p)
                    }
                }

                // Draw final segment to adjusted end point if needed
                let endTo = actualEndPoint - path.polygon.getPoint(endPointIndex, cxt.offsetPath)
                if endTo.length() > 0.01
                {
                    me.drawLine(cxt, actualEndPoint)
                }
            }
            else
            {
                me.drawLine(cxt, actualEndPoint)
            }
        }

        // Handle AA and closing as before...
        if !lastLineInvalid and !path.isClosed and (me.curState.paintParams & .Antialiased) and (pen.endCapStyle == .None)
        {
            let prim2p = me.vertexBuffer.buffer + prim2
            let prim2t = prim2p + 2
            me.addEdgeAA(prim1, prim2p.pos, prim2t.pos)
            me.addEdgeAA(prim2, prim2p.pos, prim2t.pos)
        }
        elif path.isClosed and !firstLine and !beginLine
        {
            me.drawJoin(cxt, prev, normal, normalFirst)
        }
    }
}

public impl Painter
{
    // Draw a line path.
    #[Swag.Overload]
    mtd drawPath(path: *LinePath, color: Color, penSize: f32)
    {
        me.sharedSolidPen.brush.type  = .SolidColor
        me.sharedSolidPen.brush.color = color
        me.sharedSolidPen.size        = penSize
        me.drawPath(path, me.sharedSolidPen)
    }

    #[Swag.Overload]
    mtd drawPath(path: *LinePath, pen: Pen)
    {
        path.flatten(me.curState.paintQuality)
        if Math.isZeroEpsilon(path.polygon.totalLen):
            return

        var cxt: Context
        cxt.path = path
        cxt.pen  = &pen
        if pen.dashStyle != .None:
            preparePattern(&cxt)

        var bvMin = path.bvMin
        var bvMax = path.bvMax
        bvMin -= pen.size / 2
        bvMax += pen.size / 2
        me.setTexture0(bvMin, bvMax, pen.brush)
        me.resetTextureFont()

        me.startOverlapMode(pen)
        me.drawPath(&cxt)
        me.stopOverlapMode()
    }

    // Draw a list of paths.
    #[Swag.Overload]
    mtd drawPath(pathList: *LinePathList, pen: Pen, mode: DrawPathListMode = .Separate)
    {
        if !pathList.paths.count:
            return
        if mode == .Separate
        {
            foreach p in pathList.paths:
                drawPath(p, pen)
            return
        }

        pathList.flatten(me.curState.paintQuality)

        var bvMin = pathList.bvMin
        var bvMax = pathList.bvMax
        bvMin -= pen.size / 2
        bvMax += pen.size / 2
        me.setTexture0(bvMin, bvMax, pen.brush)
        me.resetTextureFont()

        me.startOverlapMode(pen)

        var cxt: Context
        foreach p in pathList.paths
        {
            cxt.path = p
            cxt.pen  = &pen
            if pen.dashStyle != .None:
                preparePattern(&cxt)
            drawPath(&cxt)
        }

        me.stopOverlapMode()
    }

    #[Swag.Overload]
    mtd drawPath(pathList: *LinePathList, color: Color, penSize: f32, mode: DrawPathListMode = .Separate)
    {
        if !pathList.paths.count:
            return
        me.sharedSolidPen.brush.type  = .SolidColor
        me.sharedSolidPen.brush.color = color
        me.sharedSolidPen.size        = penSize
        me.drawPath(pathList, me.sharedSolidPen, mode)
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
