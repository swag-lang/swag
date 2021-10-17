using Core, Core.Math

public enum ElementType
{
    Line
    Arc
    Bezier1
    Bezier2
    Curve
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
    isFlatten:      bool
    bvMin:          Vector2
    bvMax:          Vector2
    endPosBuffer:   Vector2
    polygon:        Polygon
}

public impl LinePath
{
    // Reset the path
    mtd start(startX, startY: f32)
    {
        elements.clear()
        isClosed = false
        isDirty = true
        startPoint = Vector2{startX, startY}
        endPoint = startPoint
    }

    // Add a line from the previous point to 'pt'
    mtd lineTo(x, y: f32)
    {
        var elem: Element = ?
        elem.type = .Line
        elem.position.set(x, y)
        elements.add(elem)
        endPoint.set(x, y)
        isDirty = true
    }

    // Add a curve from the previous point to 'pt'
    mtd curveTo(x, y: f32)
    {
        var elem: Element = ?
        elem.type = .Curve
        elem.position.set(x, y)
        elements.add(elem)
        endPoint.set(x, y)
        isDirty = true
    }

    // Add a curve from the previous point to 'pt'
    mtd bezierTo(x, y: f32, ctx, cty: f32)
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
    mtd bezierTo(x, y: f32, ct1x, ct1y: f32, ct2x, ct2y: f32)
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
    mtd arcTo(centerX, centerY: f32, radiusX, radiusY: f32, initAngle, maxAngle: f32)
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
    // Will add a line or a curve to the first point if necessary
    mtd close()
    {
        if isClosed or !elements.count
            return

        v := endPoint - startPoint
        if v.lengthSquared() >= 0.5 * 0.5
        {
            if elements.backPtr().type == .Curve
                curveTo(startPoint.x, startPoint.y)
            else
                lineTo(startPoint.x, startPoint.y)
        }

        isClosed = true
    }

    // Initialize the path with a rectangle
    mtd setRect(left, top, width, height: f32)
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
    mtd setRoundRect(left, top, width, height: f32, radiusX, radiusY: f32)
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
    mtd setEllipse(centerX, centerY: f32, radiusX, radiusY: f32)
    {
        start(centerX, centerY - radiusY)
        arcTo(centerX, centerY, radiusX, radiusY, 0, 90)
        arcTo(centerX, centerY, radiusX, radiusY, 90, 180)
        arcTo(centerX, centerY, radiusX, radiusY, 180, 270)
        arcTo(centerX, centerY, radiusX, radiusY, 270, 360)
        close()
    }

    // Initialize the path with a circle
    mtd setCircle(centerX, centerY: f32, radius: f32)
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

    private mtd flattenBezier(pos, p1: Vector2, quality: PaintQuality)
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

    private mtd flattenBezier(pos, p1, p2: Vector2, quality: PaintQuality)
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

    private mtd flattenCurve(p0, p1, p2, p3: Vector2, quality: PaintQuality)
    {
        minDist := getFlattenDistance(quality)
        startPt := endPosBuffer

        // We divide a segment by half until the computed point on the curve is close to the corresponding segment
        var tmp: Array'{factor: f32, pos: Vector2}
        tmp.add(@{0, endPosBuffer})
        tmp.add(@{1, p0 + p1 + p2 + p3})

        for i := 0; i < tmp.count - 1; i += 1
        {
            t  := (tmp[i].factor + tmp[i + 1].factor) * 0.5

            var point: Vector2 = ?
            point  = p0 * t * t * t
            point += p1 * t * t
            point += p2 * t
            point += p3

            dist := point.distanceToLine(tmp[i].pos, tmp[i + 1].pos)
            if dist < minDist continue

            tmp.insertAt(cast(uint) i + 1, @{t, point})
            i -= 1
        }

        loop i: 1..tmp.count-1
            flattenLine(tmp[i].pos)
    }

    private mtd flattenArc(pos, p1, p2: Vector2, quality: PaintQuality)
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

    private mtd flattenLine(pos: Vector2)
    {
        bvMin = Vector2.min(bvMin, pos)
        bvMax = Vector2.max(bvMax, pos)
        polygon.add(pos)
        endPosBuffer = pos
    }

    // Convert the path to a list of points
    mtd flatten(quality: PaintQuality)
    {
        // Already done ?
        if !isDirty and flattenQuality == quality and isFlatten
            return

        isDirty        = false
        isFlatten      = true
        flattenQuality = quality

        bvMin, bvMax = startPoint
        polygon.clear()

        endPosBuffer = startPoint
        polygon.points.add(endPosBuffer)

        var pos, p1, p2, lastP: Vector2 = ?
        lastElem := elements.count - 1

        visit elem, i: elements
        {
            pos = elem.position

            #[Swag.Complete]
            switch elem.type
            {
            case Line:
                flattenLine(pos)

            case Bezier1:
                flattenBezier(pos, elem.p1, quality)

            case Bezier2:
                flattenBezier(pos, elem.p1, elem.p2, quality)

            case Curve:
                var p0, p1, p2, p3: Vector2 = ?
                if i == 0
                {
                    p0 = isClosed ? elements[lastElem - 1].position : (startPoint - pos) + startPoint
                    p1 = startPoint
                    p2 = pos
                    p3 = elements[i + 1].position
                }
                else if i < lastElem
                {
                    p0 = i == 1 ? startPoint : elements[i - 2].position
                    p1 = elements[i - 1].position
                    p2 = pos
                    p3 = elements[i + 1].position
                }
                else
                {
                    p0 = elements[i - 2].position
                    p1 = elements[i - 1].position
                    p2 = pos
                    p3 = isClosed ? elements[1].position : pos + (pos - p1)
                }

                const Alpha = 0.5
                const Tension = 0.0

                t01 := Math.pow(Vector2.distance(p0, p1), Alpha)
                t12 := Math.pow(Vector2.distance(p1, p2), Alpha)
                t23 := Math.pow(Vector2.distance(p2, p3), Alpha)

                m1 := (1.0 - Tension) * (p2 - p1 + t12 * ((p1 - p0) / t01 - (p2 - p0) / (t01 + t12)))
                m2 := (1.0 - Tension) * (p2 - p1 + t12 * ((p3 - p2) / t23 - (p3 - p1) / (t12 + t23)))

                sa := 2.0 * (p1 - p2) + m1 + m2
                sb := -3.0 * (p1 - p2) - m1 - m1 - m2
                sc := m1
                sd := p1
                flattenCurve(sa, sb, sc, sd, quality)

            case Arc:
                flattenArc(pos, elem.p1, elem.p2, quality)
            }
        }

        polygon.compute()
    }
}