#global namespace Math
#global public

impl Vector2
{
    // Returns true if the point 'pt' is inside the triangle defined with 'a, b, c'
    func isInTriangle(using const self, a, b, c: Vector2)->bool
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

        return (aCROSSbp >= 0) and (bCROSScp >= 0) and (cCROSSap >= 0)
    }

    // Returns the distance of the point to the given segment
    func distanceToLine(using const self, a, b: Vector2)->f32
    {
        toPt := dref self - a
        seg  := b - a
        seg.normalizeSafe()
        dot  := toPt.dot(seg)
        seg  *= dot
        cross := toPt - seg
        return cross.length()
    }

    private func getLineLineIntersectCoefs(v1, v2, v3, v4: Vector2, epsilon: f32)->{yes: bool, d0: f32, d1: f32}
    {
        var result: retval

        ua := (v4.x - v3.x) * (v1.y - v3.y) - (v4.y - v3.y) * (v1.x - v3.x)
        ub := (v2.x - v1.x) * (v1.y - v3.y) - (v2.y - v1.y) * (v1.x - v3.x)
        d  := (v4.y - v3.y) * (v2.x - v1.x) - (v4.x - v3.x) * (v2.y - v1.y)

        // Parallel lines
        if Math.abs(d) < epsilon
        {
            result.d0, result.d1 = 0
            if Math.abs(ua) < epsilon and Math.abs(ub) < epsilon
                result.yes = true
            return result
        }

        result.yes = true
        result.d0 = ua / d
        result.d1 = ub / d
        return result
    }

    func lineLineIntersect(v1, v2, v3, v4: Vector2, epsilon: f32 = ConstF32.Epsilon)->{yes: bool, pos: Vector2}
    {
        var result: retval

        (yes, d0, d1) := getLineLineIntersectCoefs(v1, v2, v3, v4, epsilon)
        if !yes return result

        result.yes = true
        result.pos.x = v1.x + d0 * (v2.x - v1.x)
        result.pos.y = v1.y + d0 * (v2.y - v1.y)
        return result
    }
}