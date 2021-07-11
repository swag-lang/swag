#global public
using Core.Math

struct Polygon
{
    points:     Array'Vector2
    clockwise:  bool
    isConvex:   bool
    area:       f32
}

impl Polygon
{
    // Get the given point, with an optional offset
    method getPoint(i: uint)->Vector2
    {
        return points.buffer[i]
    }

    // Get the given point, with an optional offset
    method getPoint(i: uint, offset: f32)->Vector2
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
    method clear()
    {
        points.clear()
    }

    // Add a new point
    method add(v: Vector2)
    {
        points.add(v)
    }

    // Fast expand outline by a given amount
    method offset(value: f32)
    {
        if Math.isZeroEpsilon(value) return
        if !points.count return

        var copy: Array'Vector2
        copy.reserve(points.count)

        loop i: points.count
            copy.add(getPoint(i, value))

        points =,move copy
    }

    method offset1(value: f32, joinStyle: JoinStyle)
    {
        var off:  Clipper.Offset
        var type: Clipper.JoinType = JoinType.Square

        switch joinStyle
        {
        case Round: type = Clipper.JoinType.Round
        case Bevel: type = Clipper.JoinType.Square
        case Miter: type = Clipper.JoinType.Miter
        }

        catch off.addPath(points, type, Clipper.EndType.ClosedPolygon)
        catch off.execute(self, cast(f64) value)
    }

    method clean()
    {
        if !points.count return

        using Clipper
        var   clipper: Transform

        clipper.strictSimple = true
        catch clipper.addPath(points, PolyType.Subject)
        catch clipper.execute(ClipType.Union, self, PolyFillType.NonZero, PolyFillType.NonZero)
    }

    method compute()
    {
        if !points.count return

        area = 0
        clockwise = false
        isConvex  = true

        n := points.count
        q := 0
        p := n - 1

        lastDot := 0'f32
        while q < n
        {
            // Convex
            v   := points.buffer[(q + 2) % points.count] - points.buffer[(q + 1) % points.count]
            v1  := points.buffer[(q + 1) % points.count] - points.buffer[(q + 0) % points.count]
            dot := v.dot(v1)
            if lastDot == 0
                lastDot = dot
            else if Math.sign(dot) != Math.sign(lastDot)
                isConvex = false

            // Aread
            area += points[p].x * points[q].y - points[q].x * points[p].y
            p = q
            q += 1
        }

        clockwise = area >= 0
    }
}