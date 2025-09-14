#global public
using Core.Math

const ClipperScaleCoords = 1000.0'f64

struct Polygon
{
    points: Array'Vector2

    // Fill with a call to compute()
    isClockwise:     bool
    isConvex:        bool
    area:            f32
    totalLen:        f32
    bvMin:           Math.Vector2
    bvMax:           Math.Vector2
}

impl Polygon
{
    // Get the given point
    #[Swag.Inline, Swag.Overload]
    mtd getPoint(i: u64)->&Vector2
    {
        return me.points.buffer + i
    }

    #[Swag.Inline, Swag.Overload]
    mtd const getPoint(i: u64)->const &Vector2
    {
        return me.points.buffer + i
    }

    // Get the given point, with an optional offset
    #[Swag.Overload]
    mtd getPoint(i: u64, offset: f32)->Vector2
    {
        if Math.isZeroEpsilon(offset):
            return me.points.buffer[i]

        let count = me.points.count - 1

        let v0 = me.points.buffer[(i + count - 1) % count]
        let v1 = me.points.buffer[i]
        let v2 = me.points.buffer[(i + 1) % count]

        let tmp0 = Vector2.toNormalizedSafe(v1 - v0)
        let n0   = tmp0.cross(1)
        let tmp1 = Vector2.toNormalizedSafe(v2 - v1)
        let n1   = tmp1.cross(1)

        let vva1 = v1 + n0
        let vva0 = v0 + n0
        let vvb1 = v2 + n1
        let vvb0 = v1 + n1
        let (yes, intPos) = Geometry.lineLineIntersect(vva0, vva1, vvb0, vvb1)

        var result: retval = v1
        if yes
        {
            result += ((intPos - v1) * offset)
        }
        else
        {
            let n = Vector2.toNormalizedSafe(n0 + n1)
            result += (n * offset)
        }

        return result
    }

    // Divide by adding points if two points are too far away
    mtd normalizeDist(dist: f32, closed: bool = true)
    {
        var i = 0'u64
        while i < (closed ? me.points.count : me.points.count - 1)
        {
            defer i += 1
            var v      = me.points[(i + 1) % me.points.count] - me.points[i]
            let d      = v.length()
            var divide = cast(s32) (d / dist)
            if divide > 0
            {
                v /= cast(f32) (divide + 1)
                while divide > 0
                {
                    let np = me.points[i] + v
                    me.points.insertAt(i + 1, np)
                    divide -= 1
                    i += 1
                }
            }
        }
    }

    // Clear all points
    mtd clear()
    {
        me.area        = 0
        me.isClockwise = false
        me.isConvex    = true
        me.totalLen    = 0
        me.points.clear()
    }

    // Add a new point
    mtd add(v: Vector2)
    {
        me.points.add(v)
    }

    // Fast expand polygon by a given amount
    #[Swag.Overload]
    mtd offset(value: f32)
    {
        if Math.isZeroEpsilon(value):
            return
        if !me.points.count:
            return

        var copy: Array'Vector2
        copy.reserve(me.points.count)

        for i in me.points.count:
            copy.add(me.getPoint(i, value))

        me.points = #move copy
    }

    internal mtd storeClipperResult(res: Clipper.ClipperPath)
    {
        me.clear()
        if !res.count:
            return

        foreach pt in res
        {
            let vec = Math.Vector2{cast(f32) (pt.x / ClipperScaleCoords), cast(f32) (pt.y / ClipperScaleCoords)}
            me.add(vec)
        }

        // Closed path needs last point to be the same as the first one
        let v0 = me.getPoint(0)
        me.add(v0)

        me.compute()
    }

    #[Swag.Overload]
    internal mtd offset(res: *Clipper.ClipperPaths, value: f32, joinStyle: JoinStyle, toler: f32)
    {
        if Math.isZeroEpsilon(value):
            return
        if !me.points.count:
            return

        var joinType = Clipper.JoinType.Square
        switch joinStyle
        {
        case Round:
            joinType = .Round
        case Bevel:
            joinType = .Square
        case Miter:
            joinType = .Miter
        }

        var off: Clipper.Offset
        off.arcTolerance = toler * ClipperScaleCoords

        // Register path
        var path: Clipper.ClipperPath
        foreach pt in me.points:
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

        me.offset(&res, value, joinStyle, toler)
        me.clear()
        if res.count:
            me.storeClipperResult(res[0])
    }

    // Slow but quality offset with a joinStyle
    mtd getOffset(value: f32, joinStyle: JoinStyle, toler: f32 = 0.5)->LinePathList
    {
        var result: retval
        result.bvMin = me.bvMin
        result.bvMax = me.bvMax

        var res: Clipper.ClipperPaths
        me.offset(&res, value, joinStyle, toler)

        foreach p in res
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
        if !me.points.count:
            return

        var trf: Clipper.Transform
        trf.strictSimple  = true
        trf.reverseOutput = !me.isClockwise

        // Register path
        var path: Clipper.ClipperPath
        foreach pt in me.points:
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
        me.clean(&res)
        me.clear()
        if res.count:
            me.storeClipperResult(res[0])
    }

    // Clean polygon by removing bad/unecessary points
    mtd getClean()->LinePathList
    {
        var result: retval
        result.bvMin = me.bvMin
        result.bvMax = me.bvMax

        var res: Clipper.ClipperPaths
        me.clean(&res)

        foreach p in res
        {
            let np = result.newPath()
            np.isPolyOnly = true
            np.polygon.storeClipperResult(p)
            np.bvMin = np.polygon.bvMin
            np.bvMax = np.polygon.bvMax
            if !me.isClockwise:
                break
        }

        return result
    }

    // Compute internal values, like convex/clockwise
    // Call it once the polygon contains all its points
    mtd compute()
    {
        me.area        = 0
        me.isClockwise = false
        me.isConvex    = true
        me.totalLen    = 0

        if !me.points.count:
            return

        let n        = me.points.count - 1
        let isClosed = me.points[0].distanceToSquared(me.points.back()) < 0.5 * 0.5
        if isClosed:
            me.points.count -= 1

        var q = 0
        var p = n - 1

        me.bvMin = me.points[0]
        me.bvMax = me.points[0]

        var first = true
        var lastC = 0'f32
        while q < n
        {
            // Convex
            let v  = me.points.buffer[(q + 2) % me.points.count] - me.points.buffer[(q + 1) % me.points.count]
            let v1 = me.points.buffer[(q + 1) % me.points.count] - me.points.buffer[(q + 0) % me.points.count]
            let c  = v.cross(v1)
            if first
            {
                lastC = c
                first = false
            }
            elif !Math.isZeroEpsilon(c) and !Math.isZeroEpsilon(lastC) and Math.sign(c) != Math.sign(lastC)
            {
                me.isConvex = false
            }

            me.bvMin = Vector2.min(me.bvMin, me.points[p])
            me.bvMax = Vector2.max(me.bvMax, me.points[p])

            // Area
            me.area += me.points[p].x * me.points[q].y - me.points[q].x * me.points[p].y
            me.totalLen += v1.length()
            p = q
            q += 1
        }

        if isClosed:
            me.points.count += 1
        me.isClockwise = me.area >= 0
    }
}
