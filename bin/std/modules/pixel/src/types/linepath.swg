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
    type:         ElementType = undefined
    position:     Vector2
    p1, p2:       Vector2
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
    flattenQuality:     PaintQuality = undefined
    isPolyOnly:         bool
    isFlatten:          bool
    bvMin:              Vector2
    bvMax:              Vector2
    endPosBuffer:       Vector2
    polygon:            Polygon
    triangles:          Poly2Tri.Result
}

public impl LinePath
{
    mtd opDrop()
    {
        clearCache()
    }

    mtd opPostCopy()
    {
        triangles.clear()
    }

    // Clear internal cache
    mtd clearCache()
    {
        triangles.release()
        polygon.clear()
    }

    // Reset the path
    mtd start(startX, startY: f32)
    {
        elements.clear()
        isClosed   = false
        isDirty    = true
        isPolyOnly = false
        startPoint = Vector2{startX, startY}
        endPoint   = startPoint
    }

    // Add a line from the previous point to 'pt'
    mtd lineTo(x, y: f32)
    {
        var newEndPoint = Vector2{x, y}
        if newEndPoint.isEqualEpsilon(endPoint):
            return

        var elem: Element = undefined
        elem.type = .Line
        elem.position.set(x, y)
        elements.add(elem)
        endPoint = newEndPoint
        isDirty  = true
    }

    // Add a curve from the previous point to 'pt'
    mtd curveTo(x, y: f32)
    {
        var newEndPoint = Vector2{x, y}
        if newEndPoint.isEqualEpsilon(endPoint):
            return

        var elem: Element = undefined
        elem.type = .Curve
        elem.position.set(x, y)
        elements.add(elem)
        endPoint = newEndPoint
        isDirty  = true
    }

    // Add a curve from the previous point to 'pt'
    #[Swag.Overload]
    mtd bezierTo(x, y: f32, ctx, cty: f32)
    {
        var newEndPoint = Vector2{x, y}
        if newEndPoint.isEqualEpsilon(endPoint):
            return

        var elem: Element = undefined
        elem.type = .Bezier1
        elem.position.set(x, y)
        elem.p1.set(ctx, cty)
        elements.add(elem)
        endPoint = newEndPoint
        isDirty  = true
    }

    // Add a curve from the previous point to 'pt'
    #[Swag.Overload]
    mtd bezierTo(x, y: f32, ct1x, ct1y: f32, ct2x, ct2y: f32)
    {
        var newEndPoint = Vector2{x, y}
        if newEndPoint.isEqualEpsilon(endPoint):
            return

        var elem: Element = undefined
        elem.type = .Bezier2
        elem.position.set(x, y)
        elem.p1.set(ct1x, ct1y)
        elem.p2.set(ct2x, ct2y)
        elements.add(elem)
        endPoint = newEndPoint
        isDirty  = true
    }

    // Add an arc from the previous point
    mtd arcTo(centerX, centerY: f32, radiusX, radiusY: f32, initAngle, maxAngle: Math.Angle)
    {
        var ia = initAngle.rad
        while ia < 0:
            ia += Math.ConstF32.TwoPi
        var ma = maxAngle.rad
        while ma < 0:
            ma += Math.ConstF32.TwoPi
        while ma < ia:
            ma += Math.ConstF32.TwoPi

        var newEndPoint = Vector2{centerX + (Math.sin(ma) * radiusX), centerY + (-Math.cos(ma) * radiusY)}
        if newEndPoint.isEqualEpsilon(endPoint):
            return

        var elem: Element = undefined
        elem.type = .Arc
        elem.position.set(centerX, centerY)

        elem.p1.x = ia
        elem.p1.y = ma

        elem.p2.x = radiusX
        elem.p2.y = radiusY
        elements.add(elem)
        isDirty = true

        endPoint = newEndPoint
    }

    // Close the figure
    // Will add a line or a curve to the first point if necessary
    mtd close()
    {
        if isClosed or !elements.count:
            return

        var v = endPoint - startPoint
        if v.lengthSquared() >= 0.5 * 0.5
        {
            if elements.backPtr().type == .Curve:
                curveTo(startPoint.x, startPoint.y)
            else:
                lineTo(startPoint.x, startPoint.y)
        }

        isClosed = true
    }

    internal mtd setRectRaw(l, t, w, h: f32)
    {
        let r = l + w
        let b = t + h

        start(l, t)
        lineTo(r, t)
        lineTo(r, b)
        lineTo(l, b)
        lineTo(l, t)
        close()
    }

    // Initialize the path with a rectangle
    mtd setRect(left, top, width, height: f32)
    {
        let l = Math.round(left)
        let t = Math.round(top)
        let w = Math.round(width)
        let h = Math.round(height)

        setRectRaw(l, t, w, h)
    }

    // Initialize the path with a round rectangle
    mtd setRoundRect(left, top, width, height: f32, radiusX, radiusY: f32)
    {
        let l = Math.round(left)
        let t = Math.round(top)
        let w = Math.round(width)
        let h = Math.round(height)

        let r = l + w
        let b = t + h

        var rX = Math.min(radiusX, (w * 0.5) - 2)
        var rY = Math.min(radiusY, (h * 0.5) - 2)
        rX = Math.round(rX)
        rY = Math.round(rY)

        start(l, t + rY)
        arcTo(l + rX, t + rY, rX, rY, 270'deg, 360'deg)
        lineTo(r - rX, t)
        arcTo(r - rX, t + rY, rX, rY, 0'deg, 90'deg)
        lineTo(r, b - rY)
        arcTo(r - rX, b - rY, rX, rY, 90'deg, 180'deg)
        lineTo(l + rX, b)
        arcTo(l + rX, b - rY, rX, rY, 180'deg, 270'deg)
        close()
    }

    internal mtd setEllipseRaw(cX, cY: f32, rX, rY: f32)
    {
        start(cX, cY - rY)
        arcTo(cX, cY, rX, rY, 0'deg, 90'deg)
        arcTo(cX, cY, rX, rY, 90'deg, 180'deg)
        arcTo(cX, cY, rX, rY, 180'deg, 270'deg)
        arcTo(cX, cY, rX, rY, 270'deg, 360'deg)
        close()
    }

    // Initialize the path with an ellipse
    mtd setEllipse(centerX, centerY: f32, radiusX, radiusY: f32)
    {
        let cX = Math.round(centerX)
        let cY = Math.round(centerY)
        let rX = 0.5 * Math.round(radiusX * 2)
        let rY = 0.5 * Math.round(radiusY * 2)

        setEllipseRaw(cX, cY, rX, rY)
    }

    // Initialize the path with a circle
    mtd setCircle(centerX, centerY: f32, radius: f32)
    {
        let cX = Math.round(centerX)
        let cY = Math.round(centerY)
        let r  = 0.5 * Math.round(radius * 2)

        start(cX, cY - r)
        arcTo(cX, cY, r, r, 0'deg, 90'deg)
        arcTo(cX, cY, r, r, 90'deg, 180'deg)
        arcTo(cX, cY, r, r, 180'deg, 270'deg)
        arcTo(cX, cY, r, r, 270'deg, 360'deg)
        close()
    }

    // Initialize the path with an arc
    mtd setArc(centerX, centerY: f32, radius: f32, startAngle, endAngle: Math.Angle)
    {
        if isZeroEpsilon(endAngle.rad - startAngle.rad):
            return

        let cX = Math.round(centerX)
        let cY = Math.round(centerY)
        let r  = Math.round(radius)

        Debug.assert(endAngle.rad > startAngle.rad)
        var startPos = Math.Vector2{0, -r}
        startPos.rotate(startAngle)
        start(cX + startPos.x, cY + startPos.y)

        var s = startAngle.rad
        var m = 0'f32
        if s < Math.ConstF32.PiBy2
        {
            m = Math.min(endAngle.rad, Math.ConstF32.PiBy2)
            arcTo(cX, cY, r, r, s, m)
            s = m
        }

        if s < Math.ConstF32.Pi
        {
            m = Math.min(endAngle.rad, Math.ConstF32.Pi)
            arcTo(cX, cY, r, r, s, m)
            s = m
        }

        if s < Math.ConstF32.Pi + Math.ConstF32.PiBy2
        {
            m = Math.min(endAngle.rad, Math.ConstF32.Pi + Math.ConstF32.PiBy2)
            arcTo(cX, cY, r, r, s, m)
            s = m
        }

        if s < Math.ConstF32.TwoPi
        {
            m = Math.min(endAngle.rad, Math.ConstF32.TwoPi)
            arcTo(cX, cY, r, r, s, m)
        }
    }

    // Convert the path to a list of points
    mtd flatten(quality = PaintQuality.Normal)
    {
        // Already done ?
        if isPolyOnly:
            return
        if !isDirty and flattenQuality == quality and isFlatten:
            return

        isDirty        = false
        isFlatten      = true
        flattenQuality = quality

        bvMin, bvMax = startPoint
        clearCache()

        if !elements.count:
            return

        endPosBuffer = startPoint
        polygon.points.add(endPosBuffer)

        let lastElem = elements.count - 1
        visit elem, i in elements
        {
            var pos = elem.position

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
                var p0, p1, p2, p3: Vector2 = undefined
                if lastElem <= 1
                {
                    flattenLine(pos)
                    break
                }

                if i == 0
                {
                    p0 = isClosed ? elements[lastElem - 1].position : (startPoint - pos) + startPoint
                    p1 = startPoint
                    p2 = pos
                    p3 = elements[i + 1].position
                }
                elif i < lastElem
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

                const Alpha   = 0.5
                const Tension = 0.0

                let t01 = Math.pow(Vector2.distance(p0, p1), Alpha)
                let t12 = Math.pow(Vector2.distance(p1, p2), Alpha)
                let t23 = Math.pow(Vector2.distance(p2, p3), Alpha)

                let m1 = (1.0 - Tension) * (p2 - p1 + t12 * ((p1 - p0) / t01 - (p2 - p0) / (t01 + t12)))
                let m2 = (1.0 - Tension) * (p2 - p1 + t12 * ((p3 - p2) / t23 - (p3 - p1) / (t12 + t23)))

                let sa = 2.0 * (p1 - p2) + m1 + m2
                let sb = -3.0 * (p1 - p2) - m1 - m1 - m2
                let sc = m1
                let sd = p1
                flattenCurve(sa, sb, sc, sd, quality)

            case Arc:
                flattenArc(pos, elem.p1, elem.p2, quality)
            }
        }

        polygon.compute()
    }
}

impl LinePath
{
    func getFlattenDistance(quality: PaintQuality)->f32
    {
        var minDist = 0'f32
        switch quality
        {
        case Draft:
            minDist = 2
        case Normal:
            minDist = 0.2
        }

        return minDist
    }

    #[Swag.Overload]
    mtd flattenBezier(pos, p1: Vector2, quality: PaintQuality)
    {
        let minDist = getFlattenDistance(quality)
        var startPt = endPosBuffer

        // We divide a segment by half until the computed point on the curve is close to the corresponding segment
        var tmp: Array'{ factor: f32, pos: Vector2 }
        tmp.add({0, endPosBuffer})
        tmp.add({1, pos})

        for var i = 0; i < tmp.count - 1; i += 1
        {
            let t  = (tmp[i].factor + tmp[i + 1].factor) * 0.5
            let it = 1.0 - t
            let a1 = it * it
            let a2 = 2.0 * t * it
            let a3 = t * t

            var point: Vector2 = undefined
            point.x = a1 * startPt.x + a2 * p1.x + a3 * pos.x
            point.y = a1 * startPt.y + a2 * p1.y + a3 * pos.y

            let dist = point.distanceToLine(tmp[i].pos, tmp[i + 1].pos)
            if dist < minDist:
                continue

            tmp.insertAt(cast(u64) i + 1, {t, point})
            i -= 1
        }

        loop i in 1 until tmp.count:
            flattenLine(tmp[i].pos)
    }

    #[Swag.Overload]
    mtd flattenBezier(pos, p1, p2: Vector2, quality: PaintQuality)
    {
        let minDist = getFlattenDistance(quality)
        var startPt = endPosBuffer

        // We divide a segment by half until the computed point on the curve is close to the corresponding segment
        var tmp: Array'{ factor: f32, pos: Vector2 }
        tmp.add({0, endPosBuffer})
        tmp.add({1, pos})

        for var i = 0; i < tmp.count - 1; i += 1
        {
            let t  = (tmp[i].factor + tmp[i + 1].factor) * 0.5
            let it = 1.0 - t
            let a1 = it * it * it
            let a2 = 3.0 * t * it * it
            let a3 = 3.0 * t * t * it
            let a4 = t * t * t

            var point: Vector2 = undefined
            point.x = a1 * startPt.x + a2 * p1.x + a3 * p2.x + a4 * pos.x
            point.y = a1 * startPt.y + a2 * p1.y + a3 * p2.y + a4 * pos.y

            let dist = point.distanceToLine(tmp[i].pos, tmp[i + 1].pos)
            if dist < minDist:
                continue

            tmp.insertAt(cast(u64) i + 1, {t, point})
            i -= 1
        }

        loop i in 1 until tmp.count:
            flattenLine(tmp[i].pos)
    }

    mtd flattenCurve(p0, p1, p2, p3: Vector2, quality: PaintQuality)
    {
        let minDist = getFlattenDistance(quality)

        // We divide a segment by half until the computed point on the curve is close to the corresponding segment
        var tmp: Array'{ factor: f32, pos: Vector2 }
        tmp.add({0, endPosBuffer})
        tmp.add({1, p0 + p1 + p2 + p3})

        for var i = 0; i < tmp.count - 1; i += 1
        {
            let t = (tmp[i].factor + tmp[i + 1].factor) * 0.5

            var point: Vector2 = undefined
            point = p0 * t * t * t
            point += p1 * t * t
            point += p2 * t
            point += p3

            let dist = point.distanceToLine(tmp[i].pos, tmp[i + 1].pos)
            if dist < minDist:
                continue

            tmp.insertAt(cast(u64) i + 1, {t, point})
            i -= 1
        }

        loop i in 1 until tmp.count:
            flattenLine(tmp[i].pos)
    }

    mtd flattenArc(pos, p1, p2: Vector2, quality: PaintQuality)
    {
        var endPos: Vector2 = undefined
        endPos.x = pos.x + (Math.sin(p1.y) * p2.x)
        endPos.y = pos.y + (-Math.cos(p1.y) * p2.y)

        let minDist = getFlattenDistance(quality)

        // We divide a segment by half until the computed point on the curve is close to the corresponding segment
        var tmp: Array'{ angle: f32, pos: Vector2 }
        tmp.add({p1.x, endPosBuffer})
        tmp.add({p1.y, endPos})

        for var i = 0; i < tmp.count - 1; i += 1
        {
            let angle = (tmp[i].angle + tmp[i + 1].angle) * 0.5
            var point = pos
            point.x += (Math.sin(angle) * p2.x)
            point.y += -(Math.cos(angle) * p2.y)

            let dist = point.distanceToLine(tmp[i].pos, tmp[i + 1].pos)
            if dist < minDist:
                continue

            tmp.insertAt(cast(u64) i + 1, {angle, point})
            i -= 1
        }

        loop i in 1 until tmp.count:
            flattenLine(tmp[i].pos)
    }

    mtd flattenLine(pos: Vector2)
    {
        bvMin = Vector2.min(bvMin, pos)
        bvMax = Vector2.max(bvMax, pos)
        polygon.add(pos)
        endPosBuffer = pos
    }
}
