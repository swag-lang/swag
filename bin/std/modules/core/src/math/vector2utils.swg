#global namespace Math
#global public

impl Vector2
{
    // Returns true if the point 'pt' is inside an ellipse
    mtdc isInEllipse(center: Vector2, radiusX, radiusY: f32)->bool
    {
        return (pow((x - center.x), 2) / pow(radiusX, 2)) + (pow((y - center.y), 2) / pow(radiusY, 2)) <= 1;
    }

    // Returns true if the point 'pt' is inside the triangle defined with 'a, b, c'
    mtdc isInTriangle(a, b, c: Vector2)->bool
    {
        ax  := c.x - b.x
        ay  := c.y - b.y
        bx  := a.x - c.x
        by  := a.y - c.y
        cx  := b.x - a.x
        cy  := b.y - a.y
        apx := x - a.x
        apy := y - a.y
        bpx := x - b.x
        bpy := y - b.y
        cpx := x - c.x
        cpy := y - c.y

        aCROSSbp := ax * bpy - ay * bpx
        cCROSSap := cx * apy - cy * apx
        bCROSScp := bx * cpy - by * cpx

        return ((aCROSSbp >= 0) and (bCROSScp >= 0) and (cCROSSap >= 0)) or ((aCROSSbp <= 0) and (bCROSScp <= 0) and (cCROSSap <= 0))
    }

    // Returns the distance of the point to the given line
    mtdc distanceToLine(a, b: Vector2)->f32
    {
        toPt := dref self - a
        seg  := b - a
        seg.normalizeSafe()
        dot  := toPt.dot(seg)
        seg  *= dot
        cross := toPt - seg
        return cross.length()
    }

    // Returns the distance of the point to the given segment
    mtdc distanceToSegment(a, b: Vector2)->f32
    {
        ab := b - a
        be := dref self - b
        ae := dref self - a

        abbe := ab.dot(be)
        if abbe > 0
            return be.length()

        abae := ab.dot(ae)
        if abae < 0
            return ae.length()

        x1 := ab.x
        y1 := ab.y
        x2 := ae.x
        y2 := ae.y
        mod := Math.sqrt(x1 * x1 + y1 * y1)
        return Math.abs(x1 * y2 - y1 * x2) / mod
    }

    private func getLineLineIntersectCoefs(start0, end0, start1, end1: Vector2, epsilon: f32)->{yes: bool, d0: f32, d1: f32}
    {
        var result: retval

        ua := (end1.x - start1.x) * (start0.y - start1.y) - (end1.y - start1.y) * (start0.x - start1.x)
        ub := (end0.x - start0.x) * (start0.y - start1.y) - (end0.y - start0.y) * (start0.x - start1.x)
        d  := (end1.y - start1.y) * (end0.x - start0.x) - (end1.x - start1.x) * (end0.y - start0.y)

        // Parallel lines
        if Math.abs(d) < epsilon
        {
            result.yes = Math.abs(ua) < epsilon and Math.abs(ub) < epsilon
            return result
        }

        result.yes = true
        result.d0 = ua / d
        result.d1 = ub / d
        return result
    }

    // Compute the intersection point of two lines
    func lineLineIntersect(start0, end0, start1, end1: Vector2, epsilon: f32 = ConstF32.Epsilon)->{yes: bool, pos: Vector2}
    {
        var result: retval

        (yes, d0, d1) := getLineLineIntersectCoefs(start0, end0, start1, end1, epsilon)
        if !yes return result

        result.yes = true
        result.pos.x = start0.x + d0 * (end0.x - start0.x)
        result.pos.y = start0.y + d0 * (end0.y - start0.y)
        return result
    }

    // Compute the intersection point of two segments
    func segSegIntersect(start0, end0, start1, end1: Vector2, epsilon: f32 = ConstF32.Epsilon)->{yes: bool, pos: Vector2}
    {
        var result: retval

        (yes, d0, d1) := getLineLineIntersectCoefs(start0, end0, start1, end1, epsilon)
        if !yes return result

        if d0 < -epsilon or d0 > 1 + epsilon or d1 < -epsilon or d1 > 1 + epsilon
            return result

        result.yes = true
        result.pos.x = start0.x + d0 * (end0.x - start0.x)
        result.pos.y = start0.y + d0 * (end0.y - start0.y)
        return result
    }
}