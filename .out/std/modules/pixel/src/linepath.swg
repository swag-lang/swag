using Core, Core.Math

public enum ElementType
{
    Line
    Arc
    Bezier1
    Bezier2
}

public struct Element
{
    type:       ElementType = ?
    position:   Vector2
    p1, p2:     Vector2
}

public struct LinePath
{
    elements:       Array'Element
    startPoint:     Vector2
    endPoint:       Vector2
    isClosed:       bool
    isDirty:        bool
    serial:         s32

    // Flatten infos
    flattenQuality: PaintQuality = ?
    bvMin:          Vector2
    bvMax:          Vector2
    totalLen:       f32
    endPosBuffer:   Vector2
    posBuffer:      Array'Vector2
    clockwise:      bool
    isConvex:       bool
    doneOffsetAA:   bool
    lastDot:        f32
}

public impl LinePath
{
    // Reset the path
    func start(using self, startX, startY: f32)
    {
        elements.clear()
        isClosed = false
        isDirty = true
        startPoint = Vector2{startX, startY}
        endPoint = startPoint
    }

    // Add a line from the previous point to 'pt'
    func lineTo(using self, x, y: f32)
    {
        var elem: Element = ?
        elem.type = .Line
        elem.position.set(x, y)
        elements.add(elem)
        endPoint.set(x, y)
        isDirty = true
    }

    // Add a curve from the previous point to 'pt'
    func bezierTo(using self, x, y: f32, ctx, cty: f32)
    {
        var elem: Element = ?
        elem.type = .Bezier1
        elem.position.set(x, y)
        elem.p1.set(ctx, cty)
        elements.add(elem)
        endPoint.set(x, y)
        isDirty = true
    }

    // Add a curve from the previous point to 'pt'
    func bezierTo(using self, x, y: f32, ct1x, ct1y: f32, ct2x, ct2y: f32)
    {
        var elem: Element = ?
        elem.type = .Bezier2
        elem.position.set(x, y)
        elem.p1.set(ct1x, ct1y)
        elem.p2.set(ct2x, ct2y)
        elements.add(elem)
        endPoint.set(x, y)
        isDirty = true
    }

    // Add an arc from the previous point
    func arcTo(using self, centerX, centerY: f32, radiusX, radiusY: f32, initAngle, maxAngle: f32)
    {
        initAngleRad := Math.toRadians(initAngle)
        maxAngleRad := Math.toRadians(maxAngle)

        var elem: Element = ?
        elem.type = .Arc
        elem.position.set(centerX, centerY)
        elem.p1.x = initAngleRad
        elem.p1.y = maxAngleRad
        elem.p2.x = radiusX
        elem.p2.y = radiusY
        elements.add(elem)
        isDirty = true

        endPoint.x = centerX + (Math.sin(maxAngleRad) * radiusX)
        endPoint.y = centerY + (-Math.cos(maxAngleRad) * radiusY)
    }

    // Close the figure
    // Will add a line to the first point if necessary
    func close(using self)
    {
        if isClosed return

        v := endPoint - startPoint
        if v.lengthSquared() >= 0.5 * 0.5
            lineTo(startPoint.x, startPoint.y)

        isClosed = true
    }

    // Initialize the path with a rectangle
    func setRect(using self, left, top, width, height: f32)
    {
        right := left + width
        bottom := top + height
        start(left, top)
        lineTo(right, top)
        lineTo(right, bottom)
        lineTo(left, bottom)
        lineTo(left, top)
        close()
    }

    // Initialize the path with a round rectangle
    func setRoundRect(using self, left, top, width, height: f32, radiusX, radiusY: f32)
    {
        right := left + width
        bottom := top + height
        start(left, top + radiusY)
        arcTo(left + radiusX, top + radiusY, radiusX, radiusY, -90, 0)
        lineTo(right - radiusX, top)
        arcTo(right - radiusX, top + radiusY, radiusX, radiusY, 0, 90)
        lineTo(right, bottom - radiusY)
        arcTo(right - radiusX, bottom - radiusY, radiusX, radiusY, 90, 180)
        lineTo(left + radiusX, bottom)
        arcTo(left + radiusX, bottom - radiusY, radiusX, radiusY, 180, 270)
        close()
    }

    // Initialize the path with an ellipse
    func setEllipse(using self, centerX, centerY: f32, radiusX, radiusY: f32)
    {
        start(centerX, centerY - radiusY)
        arcTo(centerX, centerY, radiusX, radiusY, 0, 90)
        arcTo(centerX, centerY, radiusX, radiusY, 90, 180)
        arcTo(centerX, centerY, radiusX, radiusY, 180, 270)
        arcTo(centerX, centerY, radiusX, radiusY, 270, 360)
        close()
    }

    // Initialize the path with a circle
    func setCircle(using self, centerX, centerY: f32, radius: f32)
    {
        start(centerX, centerY - radius)
        arcTo(centerX, centerY, radius, radius, 0, 90)
        arcTo(centerX, centerY, radius, radius, 90, 180)
        arcTo(centerX, centerY, radius, radius, 180, 270)
        arcTo(centerX, centerY, radius, radius, 270, 360)
        close()
    }

    protected func getFlattenDistance(quality: PaintQuality)->f32
    {
        minDist := 0'f32
        switch quality
        {
        case Draft:  minDist = 2
        case Normal: minDist = 0.2
        }

        return minDist
    }

    private func flattenBezier(using self, pos, p1: Vector2, quality: PaintQuality)
    {
        minDist := getFlattenDistance(quality)
        startPt := endPosBuffer

        // We divide a segment by half until the computed point on the curve is close to the corresponding segment
        var tmp: Array'{factor: f32, pos: Vector2}
        tmp.add(@{0, endPosBuffer})
        tmp.add(@{1, pos})

        for i := 0; i < tmp.count - 1; i += 1
        {
            t  := (tmp[i].factor + tmp[i + 1].factor) * 0.5
            it := 1.0 - t
            a1 := it * it
            a2 := 2.0 * t * it
            a3 := t * t

            var point: Vector2 = ?
            point.x = a1 * startPt.x + a2 * p1.x + a3 * pos.x
            point.y = a1 * startPt.y + a2 * p1.y + a3 * pos.y

            dist := point.distanceToLine(tmp[i].pos, tmp[i + 1].pos)
            if dist < minDist continue

            tmp.insertAt(cast(uint) i + 1, @{t, point})
            i -= 1
        }

        loop i: 1..tmp.count-1
            flattenLine(tmp[i].pos)
    }

    private func flattenBezier(using self, pos, p1, p2: Vector2, quality: PaintQuality)
    {
        v := pos - endPosBuffer
        minDist := getFlattenDistance(quality)
        startPt := endPosBuffer

        // We divide a segment by half until the computed point on the curve is close to the corresponding segment
        var tmp: Array'{factor: f32, pos: Vector2}
        tmp.add(@{0, endPosBuffer})
        tmp.add(@{1, pos})

        for i := 0; i < tmp.count - 1; i += 1
        {
            t  := (tmp[i].factor + tmp[i + 1].factor) * 0.5
            it := 1.0 - t
            a1 := it * it * it
            a2 := 3.0 * t * it * it
            a3 := 3.0 * t * t * it
            a4 := t * t * t

            var point: Vector2 = ?
            point.x = a1 * startPt.x + a2 * p1.x + a3 * p2.x + a4 * pos.x
            point.y = a1 * startPt.y + a2 * p1.y + a3 * p2.y + a4 * pos.y

            dist := point.distanceToLine(tmp[i].pos, tmp[i + 1].pos)
            if dist < minDist continue

            tmp.insertAt(cast(uint) i + 1, @{t, point})
            i -= 1
        }

        loop i: 1..tmp.count-1
            flattenLine(tmp[i].pos)
    }

    private func flattenArc(using self, pos, p1, p2: Vector2, quality: PaintQuality)
    {
        var endPos: Vector2 = ?
        endPos.x = pos.x + (Math.sin(p1.y) * p2.x)
        endPos.y = pos.y + (-Math.cos(p1.y) * p2.y)

        minDist := getFlattenDistance(quality)

        // We divide a segment by half until the computed point on the curve is close to the corresponding segment
        var tmp: Array'{angle: f32, pos: Vector2}
        tmp.add(@{p1.x, endPosBuffer})
        tmp.add(@{p1.y, endPos})

        for i := 0; i < tmp.count - 1; i += 1
        {
            angle := (tmp[i].angle + tmp[i + 1].angle) * 0.5
            point := pos
            point.x += (Math.sin(angle) * p2.x)
            point.y += -(Math.cos(angle) * p2.y)

            dist := point.distanceToLine(tmp[i].pos, tmp[i + 1].pos)
            if dist < minDist continue

            tmp.insertAt(cast(uint) i + 1, @{angle, point})
            i -= 1
        }

        loop i: 1..tmp.count-1
            flattenLine(tmp[i].pos)
    }

    private func flattenLine(using self, pos: Vector2)
    {
        totalLen += endPosBuffer.distance(pos)
        bvMin = Vector2.min(bvMin, pos)
        bvMax = Vector2.max(bvMax, pos)

        // Compute if shape is convex
        if isConvex and posBuffer.count > 2
        {
            v   := pos - endPosBuffer
            v1  := endPosBuffer - posBuffer.buffer[posBuffer.count - 2]
            dot := v.dot(v1)
            if lastDot == 0
                lastDot = dot
            else if Math.sign(dot) != Math.sign(lastDot)
                isConvex = false
        }

        posBuffer.add(pos)
        endPosBuffer = pos
    }

    protected func getFlattenPoint(using const self, i: uint, offset: f32)->Vector2
    {
        if !isClosed or Math.isZeroEpsilon(offset)
            return posBuffer.buffer[i]

        count := posBuffer.count - 1

        v0 := posBuffer.buffer[(i + count - 1) % count]
        v1 := posBuffer.buffer[i]
        v2 := posBuffer.buffer[(i + 1) % count]

        tmp0 := Vector2.toNormalize(v1 - v0)
        n0 := tmp0.cross(1)
        tmp1 := Vector2.toNormalize(v2 - v1)
        n1 := tmp1.cross(1)

        vva1 := v1 + n0
        vva0 := v0 + n0
        vvb1 := v2 + n1
        vvb0 := v1 + n1
        (yes, intPos) := Vector2.lineLineIntersect(vva0, vva1, vvb0, vvb1)

        var result: retval = v1
        if yes
        {
            result += ((intPos - v1) * offset)
        }
        else
        {
            n := Vector2.toNormalize(n0 + n1)
            result += (n * offset)
        }

        return result
    }

    // Expand outline by a given amount
    // Path must be closed and flattened
    func offset(using self, value: f32)
    {
        if Math.isZeroEpsilon(value) return
        if !isClosed or !posBuffer.count return

        var copy: Array'Vector2
        copy.reserve(posBuffer.count)
        loop i: posBuffer.count
            copy.add(getFlattenPoint(i, value))

        posBuffer =,move copy
    }

    // Convert the path to a list of points
    func flatten(using self, quality: PaintQuality)
    {
        #[Swag.inline]
        func roundVal(x: f32) => x//0.4 + Math.round(x)

        // Already done ?
        if !isDirty and flattenQuality == quality and posBuffer.count
            return

        doneOffsetAA = false
        isConvex = true
        lastDot = 0

        isDirty = false
        flattenQuality = quality

        bvMin.set(Swag.F32.Max, Swag.F32.Max)
        bvMax.set(-Swag.F32.Max, -Swag.F32.Max)
        totalLen = 0
        posBuffer.clear()

        endPosBuffer.x = roundVal(startPoint.x)
        endPosBuffer.y = roundVal(startPoint.y)
        posBuffer.add(endPosBuffer)

        var pos, p1, p2: Vector2 = ?
        visit elem: elements
        {
            pos.x = roundVal(elem.position.x)
            pos.y = roundVal(elem.position.y)
            switch elem.type
            {
            case Line:
                flattenLine(pos)
            case Bezier1:
                p1.x = roundVal(elem.p1.x)
                p1.y = roundVal(elem.p1.y)
                flattenBezier(pos, p1, quality)
            case Bezier2:
                p1.x = roundVal(elem.p1.x)
                p1.y = roundVal(elem.p1.y)
                p2.x = roundVal(elem.p2.x)
                p2.y = roundVal(elem.p2.y)
                flattenBezier(pos, p1, p2, quality)
            case Arc:
                flattenArc(pos, elem.p1, elem.p2, quality)
            }
        }

        // Compute area
        // Could be done during flatten ?
        n := @countof(posBuffer)
        q := 0
        p := n - 1
        area := 0'f32
        while q < n
        {
            area += posBuffer[p].x * posBuffer[q].y - posBuffer[q].x * posBuffer[p].y
            p = q
            q += 1
        }

        clockwise = area >= 0

        if isClosed
        {
            using Clipper
            var   clipper: Transform

            catch clipper.addPath(posBuffer, PolyType.Subject)

            var   lpl: LinePathList
            catch clipper.execute(ClipType.Union, &lpl, PolyFillType.NonZero, PolyFillType.NonZero)
            if lpl.paths.count
            {
                posBuffer =,move lpl.paths[0].posBuffer
                v := posBuffer.buffer[0]
                posBuffer.add(v)
            }
        }
    }
}