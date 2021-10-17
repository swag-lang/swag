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
}

impl Polygon
{
    // Get the given point, with an optional offset
    mtd getPoint(i: uint)->Vector2
    {
        return points.buffer[i]
    }

    // Get the given point, with an optional offset
    mtd getPoint(i: uint, offset: f32)->Vector2
    {
        if Math.isZeroEpsilon(offset)
            return points.buffer[i]

        count := points.count - 1

        v0 := points.buffer[(i + count - 1) % count]
        v1 := points.buffer[i]
        v2 := points.buffer[(i + 1) % count]

        tmp0 := Vector2.toNormalizeSafe(v1 - v0)
        n0 := tmp0.cross(1)
        tmp1 := Vector2.toNormalizeSafe(v2 - v1)
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
            n := Vector2.toNormalizeSafe(n0 + n1)
            result += (n * offset)
        }

        return result
    }

    // Clear all points
    mtd clear()
    {
        area = 0
        isClockwise = false
        isConvex = true
        points.clear()
    }

    // Add a new point
    mtd add(v: Vector2)
    {
        points.add(v)
    }

    // Fast expand polygon by a given amount
    mtd offset(value: f32)
    {
        if Math.isZeroEpsilon(value) return
        if !points.count return

        var copy: Array'Vector2
        copy.reserve(points.count)

        loop i: points.count
            copy.add(getPoint(i, value))

        points =,move copy
    }

    private mtd storeClipperResult(res: Clipper.ClipperPaths)
    {
        clear()
        if !res.count return

        visit *pt: dref res[0]
        {
            vec := Math.Vector2{cast(f32) (pt.x / ClipperScaleCoords), cast(f32) (pt.y / ClipperScaleCoords)}
            add(vec)
        }

        // Closed path needs last point to be the same as the first one
        v0 := getPoint(0)
        add(v0)

        compute()
    }

    // Slow but quality offset with a joinStyle
    mtd offset(value: f32, joinStyle: JoinStyle)
    {
        if Math.isZeroEpsilon(value) return
        if !points.count return

        var joinType:   Clipper.JoinType = JoinType.Square

        switch joinStyle
        {
        case Round: joinType = Clipper.JoinType.Round
        case Bevel: joinType = Clipper.JoinType.Square
        case Miter: joinType = Clipper.JoinType.Miter
        }

        var res: Clipper.ClipperPaths
        Memory.pushTempAllocator()
        {
            var off: Clipper.Offset

            // Register path
            var path: Clipper.ClipperPath
            visit pt: points
                path.add(Clipper.IntPoint{cast(s64) (pt.x * ClipperScaleCoords), cast(s64) (pt.y * ClipperScaleCoords)})
            off.addPath(path, joinType)

            // Compute
            off.execute(&res, cast(f64) (value * ClipperScaleCoords))
        }

        // Get result
        storeClipperResult(res)
    }

    // Clean polygon by removing bad/unecessary points
    mtd clean()
    {
        if !points.count return

        var res: Clipper.ClipperPaths
        Memory.pushTempAllocator()
        {
            var trf: Clipper.Transform
            trf.strictSimple = true

            // Register path
            var path: Clipper.ClipperPath
            visit pt: points
                path.add(Clipper.IntPoint{cast(s64) (pt.x * ClipperScaleCoords), cast(s64) (pt.y * ClipperScaleCoords)})
            trf.addPath(&path, Clipper.PolyType.Subject)

            // Compute
            trf.execute(Clipper.ClipType.Union, &res, Clipper.PolyFillType.NonZero, Clipper.PolyFillType.NonZero)
        }

        // Get result
        storeClipperResult(res)
    }

    // Compute internal values, like convex/clockwise
    // Call it once the polygon contains all its points
    mtd compute()
    {
        area = 0
        isClockwise = false
        isConvex = true

        if !points.count
            return

        n := points.count
        q := 0
        p := n - 1

        first := true
        lastC := 0'f32
        while q < n
        {
            // Convex
            v  := points.buffer[(q + 2) % points.count] - points.buffer[(q + 1) % points.count]
            v1 := points.buffer[(q + 1) % points.count] - points.buffer[(q + 0) % points.count]
            c  := v.cross(v1)
            if first
            {
                lastC = c
                first = false
            }
            else if !Math.isZeroEpsilon(c) and !Math.isZeroEpsilon(lastC) and Math.sign(c) != Math.sign(lastC)
            {
                isConvex = false
            }

            // Area
            area += points[p].x * points[q].y - points[q].x * points[p].y
            p = q
            q += 1
        }

        isClockwise = area >= 0
    }
}