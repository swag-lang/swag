using Core, Core.Math

enum ElementType
{
    Line
    Arc
    Bezier1
    Bezier2
}

struct Element
{
    type:       ElementType = ?
    position:   Vector2
    p1, p2:     Vector2
}

#[Swag.opaque]
public struct LinePath
{
    elements:       Array'Element
    startPoint:     Vector2
    endPoint:       Vector2
    isOpened:       bool
    isClosed:       bool
    isDirty:        bool

    // Flatten infos
    flattenQuality: PaintQuality = ?
    bvMin:          Vector2
    bvMax:          Vector2
    totalLen:       f32
    posBuffer:      Array'Vector2
    isConvex:       bool
    lastDot:        f32
}

public impl LinePath
{
    // Reset the path
    func start(using self, startX, startY: f32)
    {
        elements.clear()
        isOpened, isClosed = false
        isDirty = true
        startPoint = Vector2{startX, startY}
        endPoint = startPoint
    }

    // Add a line from the previous point to 'pt'
    #[Swag.inline]
    func lineTo(using self, pt: Vector2)
    {
        lineTo(pt.x, pt.y)
    }

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
    #[Swag.inline]
    func bezierTo(using self, pt, ct: Vector2)
    {
        bezierTo(pt.x, pt.y, ct.x, ct.y)
    }

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
    #[Swag.inline]
    func bezierTo(using self, pt, ct1, ct2: Vector2)
    {
        bezierTo(pt.x, pt.y, ct1.x, ct1.y, ct2.x, ct2.y)
    }

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
    #[Swag.inline]
    func arcTo(using self, center: Vector2, radiusX, radiusY: f32, initAngle, maxAngle: f32)
    {
        arcTo(center.x, center.y, radiusX, radiusY, initAngle, maxAngle)
    }

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
            lineTo(startPoint)

        isClosed = true
    }

    protected func getFlattenDistance(quality: PaintQuality, len: f32)->f32
    {
        minDist := 0'f32
        switch quality
        {
        case Draft:  minDist = Math.max(8, len / 4)
        case Good:   minDist = 3
        case High:   minDist = 1
        }

        return minDist
    }

    private func flattenBezier(using self, pos, p1: Vector2, quality: PaintQuality)
    {
        v := pos - endPoint
        minDist := getFlattenDistance(quality, v.length())
        startPt := endPoint

        var tmp: Array'{factor: f32, pos: Vector2}
        tmp.add(@{0, endPoint})
        tmp.add(@{1, pos})
        for i := 0; i < tmp.count - 1; i += 1
        {
            if tmp[i].pos.distance(tmp[i + 1].pos) < minDist
                continue

            t  := (tmp[i].factor + tmp[i + 1].factor) * 0.5
            it := 1.0 - t
            a1 := it * it
            a2 := 2.0 * t * it
            a3 := t * t

            var point: Vector2 = ?
            point.x = a1 * startPt.x + a2 * p1.x + a3 * pos.x
            point.y = a1 * startPt.y + a2 * p1.y + a3 * pos.y
            tmp.insertAt(cast(uint) i + 1, @{t, point})
            i -= 1
        }

        loop i: 1..tmp.count-1
            flattenLine(tmp[i].pos)
    }

    private func flattenBezier(using self, pos, p1, p2: Vector2, quality: PaintQuality)
    {
        /*v := pos - endPoint
        minDist := getFlattenDistance(quality, v.length())
        startPt := endPoint

        var tmp: Array'{factor: f32, pos: Vector2}
        tmp.add(@{0, endPoint})
        tmp.add(@{1, pos})
        for i := 0; i < tmp.count - 1; i += 1
        {
            if tmp[i].pos.distance(tmp[i + 1].pos) < minDist
                continue

            t  := (tmp[i].factor + tmp[i + 1].factor) * 0.5
            it := 1.0 - t
            a1 := it * it * it
            a2 := 3.0 * t * it * it
            a3 := 3.0 * t * t * it
            a4 := t * t * t

            var point: Vector2 = ?
            point.x = a1 * startPt.x + a2 * p1.x + a3 * p2.x + a4 * pos.x
            point.y = a1 * startPt.y + a2 * p1.y + a3 * p2.y + a4 * pos.y
            tmp.insertAt(cast(uint) i + 1, @{t, point})
            i -= 1
        }

        loop i: 1..tmp.count-1
            flattenLine(tmp[i].pos)*/
    }

    private func flattenArc(using self, pos, p1, p2: Vector2, quality: PaintQuality)
    {
        #[Swag.mixin]
        func calc(angle: f32)->Vector2
        {
            var result: retval = pos
            result.x += (Math.sin(angle) * p2.x)
            result.y += -(Math.cos(angle) * p2.y)
            return result
        }

        // Estimate the number of steps to reach a given quality
        minDist := getFlattenDistance(quality, endPoint.distance(pos))
        step := 1'u32
        angleDiv := 0.5
        while true
        {
            v := calc(p1.x + (p1.y - p1.x) * angleDiv)
            if endPoint.distance(v) <= minDist
                break
            angleDiv *= 0.5
            step += 1
        }

        // Approximate curve
        step *= 2
        loop i: 1..step
        {
            v := calc(p1.x + ((p1.y - p1.x) * i) / step)
            flattenLine(v)
        }
    }

    private func flattenLine(using self, pos: Vector2)
    {
        totalLen += endPoint.distance(pos)
        bvMin.x = Math.min(bvMin.x, pos.x)
        bvMin.y = Math.min(bvMin.y, pos.y)
        bvMax.x = Math.max(bvMax.x, pos.x)
        bvMax.y = Math.max(bvMax.y, pos.y)

        // Compute if shape is convex
        if isConvex and posBuffer.count > 2
        {
            v   := pos - endPoint
            v1  := endPoint - posBuffer.buffer[posBuffer.count - 2]
            dot := v.dot(v1)
            if lastDot == 0
                lastDot = dot
            else if Math.sign(dot) != Math.sign(lastDot)
                isConvex = false
        }

        posBuffer.add(pos)
        endPoint = pos
    }

    func flatten(using self, quality: PaintQuality)
    {
        // Already done ?
        if !isDirty and flattenQuality == quality and posBuffer.count
            return

        isConvex = true
        lastDot = 0

        isDirty = false
        flattenQuality = quality

        bvMin.set(Swag.F32.Max, Swag.F32.Max)
        bvMax.set(Swag.F32.Min, Swag.F32.Min)
        totalLen = 0
        posBuffer.clear()

        endPoint = startPoint
        visit elem: elements
        {
            switch elem.type
            {
            case Line:
                flattenLine(elem.position)
            case Bezier1:
                flattenBezier(elem.position, elem.p1, quality)
            case Bezier2:
                flattenBezier(elem.position, elem.p1, elem.p2, quality)
            case Arc:
                flattenArc(elem.position, elem.p1, elem.p2, quality)
            }
        }
    }
}