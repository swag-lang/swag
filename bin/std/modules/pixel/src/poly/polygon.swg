#global public
using Core.Math

const ClipperScaleCoords = 1000.0'f64

struct Polygon
{
    points:         Array'Vector2

    // Fill with a call to compute()
    isClockwise:    bool
    isConvex:       bool
    area:           f32
    totalLen:       f32
    bvMin:          Math.Vector2
    bvMax:          Math.Vector2
}

impl Polygon
{
    // Get the given point
    #[Swag.Inline, Swag.Overload]
    mtd getPoint(i: u64)->Vector2
    {
        return points.buffer[i]
    }

    // Get the given point, with an optional offset
    #[Swag.Overload]
    mtd getPoint(i: u64, offset: f32)->Vector2
    {
        if Math.isZeroEpsilon(offset)
            return points.buffer[i]

        let count = points.count - 1

        let v0 = points.buffer[(i + count - 1) % count]
        let v1 = points.buffer[i]
        let v2 = points.buffer[(i + 1) % count]

        let tmp0 = Vector2.toNormalizeSafe(v1 - v0)
        let n0 = tmp0.cross(1)
        let tmp1 = Vector2.toNormalizeSafe(v2 - v1)
        let n1 = tmp1.cross(1)

        let vva1 = v1 + n0
        let vva0 = v0 + n0
        let vvb1 = v2 + n1
        let vvb0 = v1 + n1
        let (yes, intPos) = Vector2.lineLineIntersect(vva0, vva1, vvb0, vvb1)

        var result: retval = v1
        if yes
        {
            result += ((intPos - v1) * offset)
        }
        else
        {
            let n = Vector2.toNormalizeSafe(n0 + n1)
            result += (n * offset)
        }

        return result
    }

    // Divide by adding points if two points are too far away
    mtd normalizeDist(dist: f32, closed: bool = true)
    {
        var i = 0'u64
        while i < (closed ? points.count : points.count - 1)
        {
            defer i += 1
            var v = points[(i + 1) % points.count] - points[i]
            let d = v.length()
            var divide = cast(s32) (d / dist)
            if divide > 0
            {
                v /= cast(f32) (divide + 1)
                while divide > 0
                {
                    var np = points[i] + v
                    points.insertAt(i + 1, np)
                    divide -= 1
                    i += 1
                }
            }
        }
    }

    // Clear all points
    mtd clear()
    {
        area = 0
        isClockwise = false
        isConvex = true
        totalLen = 0
        points.clear()
    }

    // Add a new point
    mtd add(v: Vector2)
    {
        points.add(v)
    }

    // Fast expand polygon by a given amount
    #[Swag.Overload]
    mtd offset(value: f32)
    {
        if Math.isZeroEpsilon(value)
            return
        if !points.count
            return

        var copy: Array'Vector2
        copy.reserve(points.count)

        loop i: points.count
            copy.add(getPoint(i, value))

        points =,move copy
    }

    internal mtd storeClipperResult(res: Clipper.ClipperPath)
    {
        clear()
        if !res.count
            return

        visit pt: res
        {
            var vec = Math.Vector2{cast(f32) (pt.x / ClipperScaleCoords), cast(f32) (pt.y / ClipperScaleCoords)}
            add(vec)
        }

        // Closed path needs last point to be the same as the first one
        var v0 = getPoint(0)
        add(v0)

        compute()
    }

    #[Swag.Overload]
    internal mtd offset(res: *Clipper.ClipperPaths, value: f32, joinStyle: JoinStyle, toler: f32)
    {
        if Math.isZeroEpsilon(value)
            return
        if !points.count
            return

        var joinType = Clipper.JoinType.Square
        switch joinStyle
        {
        case Round: joinType = .Round
        case Bevel: joinType = .Square
        case Miter: joinType = .Miter
        }

        var off: Clipper.Offset
        off.arcTolerance = toler * ClipperScaleCoords

        // Register path
        var path: Clipper.ClipperPath
        visit pt: points
            path.add(Clipper.IntPoint{cast(s64) (pt.x * ClipperScaleCoords), cast(s64) (pt.y * ClipperScaleCoords)})
        off.addPath(path, joinType)

        // Compute
        off.execute(res, cast(f64) (value * ClipperScaleCoords))
    }

    // Slow but quality offset with a joinStyle
    #[Swag.Overload]
    mtd offset(value: f32, joinStyle: JoinStyle, toler: f32 = 0.5)
    {
        var res: Clipper.ClipperPaths

        offset(&res, value, joinStyle, toler)
        clear()
        if res.count
            storeClipperResult(res[0])
    }

    // Slow but quality offset with a joinStyle
    mtd getOffset(value: f32, joinStyle: JoinStyle, toler: f32 = 0.5)->LinePathList
    {
        var result: retval
        result.bvMin = bvMin
        result.bvMax = bvMax

        var res: Clipper.ClipperPaths
        offset(&res, value, joinStyle, toler)

        visit p: res
        {
            let np = result.newPath()
            np.isPolyOnly = true
            np.polygon.storeClipperResult(p)
            np.bvMin = np.polygon.bvMin
            np.bvMax = np.polygon.bvMax
        }

        return result
    }

    #[Swag.Overload]
    internal mtd clean(res: *Clipper.ClipperPaths)
    {
        if !points.count
            return

        var trf: Clipper.Transform
        trf.strictSimple = true
        trf.reverseOutput = !isClockwise

        // Register path
        var path: Clipper.ClipperPath
        visit pt: points
            path.add(Clipper.IntPoint{cast(s64) (pt.x * ClipperScaleCoords), cast(s64) (pt.y * ClipperScaleCoords)})
        trf.addPath(&path, .Subject)

        // Compute
        trf.execute(.Union, res, .NonZero, .NonZero)
    }

    // Clean polygon by removing bad/unecessary points
    #[Swag.Overload]
    mtd clean()
    {
        var res: Clipper.ClipperPaths
        clean(&res)
        clear()
        if res.count
            storeClipperResult(res[0])
    }

    // Clean polygon by removing bad/unecessary points
    mtd getClean()->LinePathList
    {
        var result: retval
        result.bvMin = bvMin
        result.bvMax = bvMax

        var res: Clipper.ClipperPaths
        clean(&res)

        visit p: res
        {
            let np = result.newPath()
            np.isPolyOnly = true
            np.polygon.storeClipperResult(p)
            np.bvMin = np.polygon.bvMin
            np.bvMax = np.polygon.bvMax
            if !isClockwise break
        }

        return result
    }

    // Compute internal values, like convex/clockwise
    // Call it once the polygon contains all its points
    mtd compute()
    {
        area = 0
        isClockwise = false
        isConvex = true
        totalLen = 0

        if !points.count
            return

        let n = points.count - 1
        let isClosed = points[0].distanceSquared(points.back()) < 0.5*0.5
        if isClosed
            points.count -= 1

        var q = 0
        var p = n - 1

        bvMin = points[0]
        bvMax = points[0]

        var first = true
        var lastC = 0'f32
        while q < n
        {
            // Convex
            let v = points.buffer[(q + 2) % points.count] - points.buffer[(q + 1) % points.count]
            let v1 = points.buffer[(q + 1) % points.count] - points.buffer[(q + 0) % points.count]
            let c = v.cross(v1)
            if first
            {
                lastC = c
                first = false
            }
            elif !Math.isZeroEpsilon(c) and !Math.isZeroEpsilon(lastC) and Math.sign(c) != Math.sign(lastC)
            {
                isConvex = false
            }

            bvMin = Vector2.min(bvMin, points[p])
            bvMax = Vector2.max(bvMax, points[p])

            // Area
            area += points[p].x * points[q].y - points[q].x * points[p].y
            totalLen += v1.length()
            p = q
            q += 1
        }

        if isClosed
            points.count += 1
        isClockwise = area >= 0
    }
}