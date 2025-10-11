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
        .clearCache()
    }

    mtd opPostCopy()
    {
        .triangles.clear()
    }

    // Clear internal cache
    mtd clearCache()
    {
        .triangles.release()
        .polygon.clear()
    }

    // Reset the path
    mtd start(startX, startY: f32)
    {
        .elements.clear()
        .isClosed   = false
        .isDirty    = true
        .isPolyOnly = false
        .startPoint = Vector2{startX, startY}
        .endPoint   = .startPoint
    }

    // Add a line from the previous point to 'pt'
    mtd lineTo(x, y: f32)
    {
        let newEndPoint = Vector2{x, y}
        if newEndPoint.isEqualEpsilon(.endPoint) do
            return

        var elem: Element = undefined
        elem.type = .Line
        elem.position.set(x, y)
        .elements.add(elem)
        .endPoint = newEndPoint
        .isDirty  = true
    }

    // Add a curve from the previous point to 'pt'
    mtd curveTo(x, y: f32)
    {
        let newEndPoint = Vector2{x, y}
        if newEndPoint.isEqualEpsilon(.endPoint) do
            return

        var elem: Element = undefined
        elem.type = .Curve
        elem.position.set(x, y)
        .elements.add(elem)
        .endPoint = newEndPoint
        .isDirty  = true
    }

    // Add a curve from the previous point to 'pt'
    #[Swag.Overload]
    mtd bezierTo(x, y: f32, ctx, cty: f32)
    {
        let newEndPoint = Vector2{x, y}
        if newEndPoint.isEqualEpsilon(.endPoint) do
            return

        var elem: Element = undefined
        elem.type = .Bezier1
        elem.position.set(x, y)
        elem.p1.set(ctx, cty)
        .elements.add(elem)
        .endPoint = newEndPoint
        .isDirty  = true
    }

    // Add a curve from the previous point to 'pt'
    #[Swag.Overload]
    mtd bezierTo(x, y: f32, ct1x, ct1y: f32, ct2x, ct2y: f32)
    {
        let newEndPoint = Vector2{x, y}
        if newEndPoint.isEqualEpsilon(.endPoint) do
            return

        var elem: Element = undefined
        elem.type = .Bezier2
        elem.position.set(x, y)
        elem.p1.set(ct1x, ct1y)
        elem.p2.set(ct2x, ct2y)
        .elements.add(elem)
        .endPoint = newEndPoint
        .isDirty  = true
    }

    // Add an arc from the previous point
    mtd arcTo(centerX, centerY: f32, radiusX, radiusY: f32, initAngle, maxAngle: Math.Angle)
    {
        var ia = initAngle.rad
        while ia < 0 do
            ia += Math.ConstF32.TwoPi
        var ma = maxAngle.rad
        while ma < 0 do
            ma += Math.ConstF32.TwoPi
        while ma < ia do
            ma += Math.ConstF32.TwoPi

        let newEndPoint = Vector2{centerX + (Math.sin(ma) * radiusX), centerY + (-Math.cos(ma) * radiusY)}
        if newEndPoint.isEqualEpsilon(.endPoint) do
            return

        var elem: Element = undefined
        elem.type = .Arc
        elem.position.set(centerX, centerY)

        elem.p1.x = ia
        elem.p1.y = ma

        elem.p2.x = radiusX
        elem.p2.y = radiusY
        .elements.add(elem)
        .isDirty = true

        .endPoint = newEndPoint
    }

    // Close the figure
    // Will add a line or a curve to the first point if necessary
    mtd close()
    {
        if .isClosed or !.elements.count do
            return

        let v = .endPoint - .startPoint
        if v.lengthSquared() >= 0.5 * 0.5
        {
            if .elements.backPtr().type == .Curve do
                .curveTo(.startPoint.x, .startPoint.y)
            else do
                .lineTo(.startPoint.x, .startPoint.y)
        }

        .isClosed = true
    }

    internal mtd setRectRaw(l, t, w, h: f32)
    {
        let r = l + w
        let b = t + h

        .start(l, t)
        .lineTo(r, t)
        .lineTo(r, b)
        .lineTo(l, b)
        .lineTo(l, t)
        .close()
    }

    // Initialize the path with a rectangle
    mtd setRect(left, top, width, height: f32)
    {
        let l = Math.round(left)
        let t = Math.round(top)
        let w = Math.round(width)
        let h = Math.round(height)

        .setRectRaw(l, t, w, h)
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

        var rX = Math.min(radiusX, (w * 0.5))
        var rY = Math.min(radiusY, (h * 0.5))
        rX = Math.round(rX)
        rY = Math.round(rY)

        .start(l, t + rY)
        .arcTo(l + rX, t + rY, rX, rY, 270'deg, 360'deg)
        .lineTo(r - rX, t)
        .arcTo(r - rX, t + rY, rX, rY, 0'deg, 90'deg)
        .lineTo(r, b - rY)
        .arcTo(r - rX, b - rY, rX, rY, 90'deg, 180'deg)
        .lineTo(l + rX, b)
        .arcTo(l + rX, b - rY, rX, rY, 180'deg, 270'deg)
        .close()
    }

    internal mtd setEllipseRaw(cX, cY: f32, rX, rY: f32)
    {
        .start(cX, cY - rY)
        .arcTo(cX, cY, rX, rY, 0'deg, 90'deg)
        .arcTo(cX, cY, rX, rY, 90'deg, 180'deg)
        .arcTo(cX, cY, rX, rY, 180'deg, 270'deg)
        .arcTo(cX, cY, rX, rY, 270'deg, 360'deg)
        .close()
    }

    // Initialize the path with an ellipse
    mtd setEllipse(centerX, centerY: f32, radiusX, radiusY: f32)
    {
        let cX = Math.round(centerX)
        let cY = Math.round(centerY)
        let rX = 0.5 * Math.round(radiusX * 2)
        let rY = 0.5 * Math.round(radiusY * 2)

        .setEllipseRaw(cX, cY, rX, rY)
    }

    // Initialize the path with a circle
    mtd setCircle(centerX, centerY: f32, radius: f32)
    {
        let cX = Math.round(centerX)
        let cY = Math.round(centerY)
        let r  = 0.5 * Math.round(radius * 2)

        .start(cX, cY - r)
        .arcTo(cX, cY, r, r, 0'deg, 90'deg)
        .arcTo(cX, cY, r, r, 90'deg, 180'deg)
        .arcTo(cX, cY, r, r, 180'deg, 270'deg)
        .arcTo(cX, cY, r, r, 270'deg, 360'deg)
        .close()
    }

    // Initialize the path with an arc
    mtd setArc(centerX, centerY: f32, radius: f32, startAngle, endAngle: Math.Angle)
    {
        if isZeroEpsilon(endAngle.rad - startAngle.rad) do
            return

        let cX = Math.round(centerX)
        let cY = Math.round(centerY)
        let r  = Math.round(radius)

        var startPos = Math.Vector2{0, -r}
        startPos.rotateBy(startAngle)
        .start(cX + startPos.x, cY + startPos.y)

        var s = startAngle.rad
        var m = 0'f32
        if s < Math.ConstF32.PiBy2
        {
            m = Math.min(endAngle.rad, Math.ConstF32.PiBy2)
            .arcTo(cX, cY, r, r, s, m)
            s = m
        }

        if s < Math.ConstF32.Pi
        {
            m = Math.min(endAngle.rad, Math.ConstF32.Pi)
            .arcTo(cX, cY, r, r, s, m)
            s = m
        }

        if s < Math.ConstF32.Pi + Math.ConstF32.PiBy2
        {
            m = Math.min(endAngle.rad, Math.ConstF32.Pi + Math.ConstF32.PiBy2)
            .arcTo(cX, cY, r, r, s, m)
            s = m
        }

        if s < Math.ConstF32.TwoPi
        {
            m = Math.min(endAngle.rad, Math.ConstF32.TwoPi)
            .arcTo(cX, cY, r, r, s, m)
        }
    }

    // Convert the path to a list of points
    mtd flatten(quality = PaintQuality.Normal)
    {
        // Already done ?
        if .isPolyOnly do
            return
        if !.isDirty and .flattenQuality == quality and .isFlatten do
            return

        .isDirty        = false
        .isFlatten      = true
        .flattenQuality = quality

        .bvMin, .bvMax = .startPoint
        .clearCache()

        if !.elements.count do
            return

        .endPosBuffer = .startPoint
        .polygon.points.add(.endPosBuffer)

        foreach elem, i in .elements
        {
            let pos = elem.position

            #[Swag.Complete]
            switch elem.type
            {
            case Line:
                .flattenLine(pos)
            case Bezier1:
                .flattenBezier(pos, elem.p1, quality)
            case Bezier2:
                .flattenBezier(pos, elem.p1, elem.p2, quality)
            case Curve:
                .flattenCurve(pos, i, quality)
            case Arc:
                .flattenArc(pos, elem.p1, elem.p2, quality)
            }
        }

        .polygon.compute()
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
            minDist = 1
        case Normal:
            minDist = 0.2
        }

        return minDist
    }

    mtd flattenLine(pos: Vector2)
    {
        .bvMin = Vector2.min(.bvMin, pos)
        .bvMax = Vector2.max(.bvMax, pos)
        .polygon.add(pos)
        .endPosBuffer = pos
    }

    #[Swag.Overload]
    mtd flattenCurve(pos: Math.Vector2, i: u64, quality: PaintQuality)
    {
        var p0, p1, p2, p3: Vector2 = undefined
        let lastElem = .elements.count - 1

        if lastElem <= 1
        {
            .flattenLine(pos)
            return
        }

        if i == 0
        {
            p0 = .isClosed ? .elements[lastElem - 1].position : (.startPoint - pos) + .startPoint
            p1 = .startPoint
            p2 = pos
            p3 = .elements[i + 1].position
        }
        elif i < lastElem
        {
            p0 = i == 1 ? .startPoint : .elements[i - 2].position
            p1 = .elements[i - 1].position
            p2 = pos
            p3 = .elements[i + 1].position
        }
        else
        {
            p0 = .elements[i - 2].position
            p1 = .elements[i - 1].position
            p2 = pos
            p3 = .isClosed ? .elements[1].position : pos + (pos - p1)
        }

        const Alpha   = 0.5
        const Tension = 0.0

        let t01 = Math.pow(Vector2.distanceTo(p0, p1), Alpha)
        let t12 = Math.pow(Vector2.distanceTo(p1, p2), Alpha)
        let t23 = Math.pow(Vector2.distanceTo(p2, p3), Alpha)

        let m1 = (1.0 - Tension) * (p2 - p1 + t12 * ((p1 - p0) / t01 - (p2 - p0) / (t01 + t12)))
        let m2 = (1.0 - Tension) * (p2 - p1 + t12 * ((p3 - p2) / t23 - (p3 - p1) / (t12 + t23)))

        let sa = 2.0 * (p1 - p2) + m1 + m2
        let sb = -3.0 * (p1 - p2) - m1 - m1 - m2
        let sc = m1
        let sd = p1
        .flattenCurve(sa, sb, sc, sd, quality)
    }

    #[Swag.Overload]
    mtd flattenCurve(p0, p1, p2, p3: Vector2, quality: PaintQuality)
    {
        let minDist = getFlattenDistance(quality)
        let startPt = .endPosBuffer
        let endPt   = p0 + p1 + p2 + p3               // Point at t=1

        // Calculate optimal number of segments analytically
        let numSegments = calculateCubicPolynomialSegments(p0, p1, p2, p3, startPt, endPt, minDist)

        if numSegments <= 1
        {
            // Curve is flat enough, just draw direct line
            .flattenLine(endPt)
            return
        }

        let stepSize = 1.0 / numSegments

        // Generate all points in one pass using optimized polynomial evaluation
        for i in 1 to numSegments
        {
            let t     = stepSize * i
            let point = Math.Curve.evaluatePolynomial(p0, p1, p2, p3, t)
            .flattenLine(point)
        }
    }

    // Calculate optimal number of segments for cubic polynomial
    func calculateCubicPolynomialSegments(p0, p1, p2, p3: Vector2, startPt, endPt: Vector2, tolerance: f32)->u32
    {
        // Calculate chord length
        let chordLength = startPt.distanceTo(endPt)

        if chordLength < tolerance do
            return 1 // Very short curve

        // For cubic polynomials, we need to estimate maximum curvature
        // Sample a few points to get an approximation of maximum deviation
        var maxDeviation = 0.0

        // Sample at critical points: t = 0.25, 0.5, 0.75
        let samplePoints = [0.25, 0.5, 0.75]

        foreach t in samplePoints
        {
            let point     = Curve.evaluatePolynomial(p0, p1, p2, p3, t)
            let deviation = Geometry.distanceToLine(point, startPt, endPt)
            maxDeviation = Math.max(maxDeviation, deviation)
        }

        if maxDeviation < tolerance
        {
            return 1 // Curve is already flat enough
        }

        // Estimate segments needed based on maximum deviation
        // For cubic curves, use a conservative estimate
        let segments = Math.ceil(Math.sqrt(maxDeviation / tolerance) * 3.0)

        return cast(u32) Math.max(1, Math.min(segments, 1000)) // Reasonable bounds
    }

    ///////////////////////////////

    #[Swag.Overload]
    mtd flattenBezier(pos, p1: Vector2, quality: PaintQuality)
    {
        let startPt = .endPosBuffer
        let minDist = getFlattenDistance(quality)

        // Calculate optimal number of segments analytically
        let numSegments = calculateQuadraticBezierSegments(startPt, p1, pos, minDist)

        if numSegments <= 1
        {
            // Curve is flat enough, just draw direct line
            .flattenLine(pos)
            return
        }

        let stepSize = 1.0 / numSegments

        // Generate all points in one pass using optimized quadratic Bézier evaluation
        for i in 1 to numSegments
        {
            let t     = stepSize * i
            let point = Math.Curve.evaluateBezier(startPt, p1, pos, t)
            .flattenLine(point)
        }
    }

    // Calculate optimal number of segments for quadratic Bézier
    func calculateQuadraticBezierSegments(p0, p1, p2: Vector2, tolerance: f32)->u32
    {
        // For quadratic Bézier, the maximum deviation occurs at the control point
        // Calculate distance from control point to chord line
        let deviation = Geometry.distanceToLine(p1, p0, p2)

        if deviation < tolerance do
            return 1 // Curve is already flat enough

        // For quadratic curves, the relationship is more direct
        // Maximum error ≈ deviation / 8 per segment
        // So segments needed ≈ sqrt(8 * deviation / tolerance)
        let segments = Math.ceil(Math.sqrt(8.0 * deviation / tolerance))

        return cast(u32) Math.max(1, Math.min(segments, 1000)) // Reasonable bounds
    }

    ///////////////////////////////

    #[Swag.Overload]
    mtd flattenBezier(pos, p1, p2: Vector2, quality: PaintQuality)
    {
        let startPt = .endPosBuffer
        let minDist = getFlattenDistance(quality)

        // Calculate optimal number of segments analytically
        let numSegments = calculateBezierSegments(startPt, p1, p2, pos, minDist)

        if numSegments <= 1
        {
            // Curve is flat enough, just draw direct line
            .flattenLine(pos)
            return
        }

        let stepSize = 1.0 / numSegments

        // Generate all points in one pass using optimized Bézier evaluation
        for i in 1 to numSegments
        {
            let t     = stepSize * i
            let point = Math.Curve.evaluateBezier(startPt, p1, p2, pos, t)
            .flattenLine(point)
        }
    }

    // Calculate optimal number of segments using curvature analysis
    func calculateBezierSegments(p0, p1, p2, p3: Vector2, tolerance: f32)->u32
    {
        // Calculate the maximum curvature using the control polygon method
        // This is much faster than iterative subdivision

        // Vector from start to end
        let chord       = Vector2{x: p3.x - p0.x, y: p3.y - p0.y}
        let chordLength = chord.length()

        if chordLength < tolerance do
            return 1 // Very short curve

        // Calculate maximum deviation of control points from chord
        var maxDeviation = 0.0

        // Distance from p1 to chord line
        let dev1 = Geometry.distanceToLine(p1, p0, p3)
        maxDeviation = Math.max(maxDeviation, dev1)

        // Distance from p2 to chord line
        let dev2 = Geometry.distanceToLine(p2, p0, p3)
        maxDeviation = Math.max(maxDeviation, dev2)

        if maxDeviation < tolerance
        {
            return 1 // Curve is already flat enough
        }

        // Use the flatness criterion: segments ≈ sqrt(maxDeviation / tolerance)
        let segments = Math.ceil(Math.sqrt(maxDeviation / tolerance) * 2.0)

        return cast(u32) Math.max(1, Math.min(segments, 1000)) // Reasonable bounds
    }

    ///////////////////////////////

    mtd flattenArc(pos, p1, p2: Vector2, quality: PaintQuality)
    {
        let angleSpan = p1.y - p1.x
        let radius    = Math.max(p2.x, p2.y)     // Use the larger radius component

        // Calculate required number of segments analytically
        let numSegments = calculateFlattenArcSegments(angleSpan, radius, quality)

        if numSegments <= 1
        {
            // Arc is small enough, just draw direct line to end
            let endPos = Vector2{x: pos.x + (Math.sin(p1.y) * p2.x), y: pos.y - (Math.cos(p1.y) * p2.y)}
            .flattenLine(endPos)
            return
        }

        let angleStep = angleSpan / numSegments

        // Generate all points in one pass
        for i in 1 to numSegments
        {
            let angle = p1.x + (angleStep * i)
            let point = Vector2{x: pos.x + (Math.sin(angle) * p2.x), y: pos.y - (Math.cos(angle) * p2.y)}
            .flattenLine(point)
        }
    }

    // Calculate optimal number of segments based on arc properties
    func calculateFlattenArcSegments(angleSpan: f32, radius: f32, quality: PaintQuality)->u32
    {
        if radius < 0.1 do
            return 1

        let minDist = getFlattenDistance(quality)

        // Use the sagitta formula: segments = ceil(angle / (2 * acos(1 - tolerance/radius)))
        // This gives us the minimum segments needed to stay within tolerance
        let tolerance          = minDist
        var maxAnglePerSegment = 2.0 * Math.acos(1.0 - tolerance / radius)

        // Clamp to reasonable bounds
        maxAnglePerSegment = Math.max(maxAnglePerSegment, 0.01) // Minimum angle
        maxAnglePerSegment = Math.min(maxAnglePerSegment, ConstF32.Pi / 4) // Maximum angle (45°)

        let numSegments = Math.ceil(Math.abs(angleSpan) / maxAnglePerSegment)

        return cast(u32) Math.max(1, Math.min(numSegments, 1000)) // Reasonable bounds
    }
}
