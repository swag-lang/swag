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

    // Returns the minimal segment between two segments (and the minimal distance)
    //
    // ---
    // David Eberly, Geometric Tools, Redmond WA 98052
    // Copyright (c) 1998-2022
    // Distributed under the Boost Software License, Version 1.0.
    // https://www.boost.org/LICENSE_1_0.txt
    // https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
    // https://www.geometrictools.com/GTE/Mathematics/DistSegmentSegment.h
    // ---
    func nearestPointsSegSeg(p0, p1, q0, q1: Math.Vector2)->{a: Math.Vector2, b: Math.Vector2, dist: f32}
    {
        p1p0 := p1 - p0
        q1q0 := q1 - q0
        p0q0 := p0 - q0
        a := p1p0.dot(p1p0)
        b := p1p0.dot(q1q0)
        c := q1q0.dot(q1q0)
        d := p1p0.dot(p0q0)
        e := q1q0.dot(p0q0)
        det := a * c - b * b

        var s, t: f32 = undefined
        if det > 0
        {
            bte := b * e
            ctd := c * d
            if bte <= ctd
            {
                s = 0
                if e <= 0
                {
                    t = 0
                    nd := -d
                    if nd >= a
                        s = 1
                    elif nd > 0
                        s = nd / a
                }
                elif e < c
                {
                    t = e / c
                }
                else
                {
                    t = 1
                    bmd := b - d
                    if bmd >= a
                        s = 1
                    elif bmd > 0
                        s = bmd / a
                }
            }
            else
            {
                s = bte - ctd
                if s >= det
                {
                    s = 1
                    bpe := b + e
                    if bpe <= 0
                    {
                        t = 0
                        nd := -d
                        if nd <= 0
                            s = 0
                        elif nd < a
                            s = nd / a
                    }
                    elif bpe < c
                    {
                        t = bpe / c
                    }
                    else
                    {
                        t = 1
                        bmd := b - d
                        if bmd <= 0
                            s = 0
                        elif bmd < a
                            s = bmd / a
                    }
                }
                else
                {
                    ate := a * e
                    btd := b * d
                    if ate <= btd
                    {
                        t = 0
                        nd := -d
                        if nd <= 0
                            s =0
                        elif nd >= a
                            s = 1
                        else
                            s = nd / a
                    }
                    else
                    {
                        t = ate - btd;
                        if t >= det
                        {
                            t = 1
                            bmd := b - d
                            if bmd <= 0
                                s = 0
                            elif bmd >= a
                                s = 1
                            else
                                s = bmd / a
                        }
                        else
                        {
                            s /= det
                            t /= det
                        }
                    }
                }
            }
        }
        else
        {
            if e <= 0
            {
                t = 0
                nd := -d
                if nd <= 0
                    s = 0
                elif nd >= a
                    s = 1
                else
                    s = nd / a
            }
            elif e >= c
            {
                t = 1
                bmd := b - d
                if bmd <= 0
                    s = 0
                elif bmd >= a
                    s = 1
                else
                    s = bmd / a
            }
            else
            {
                s = 0
                t = e / c
            }
        }

        var result: retval
        result.a = p0 + s * p1p0
        result.b = q0 + t * q1q0
        diff := result.a - result.b
        result.dist = diff.length()
        return result
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
        if !yes
            return result

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
        if !yes
            return result

        if d0 < -epsilon or d0 > 1 + epsilon or d1 < -epsilon or d1 > 1 + epsilon
            return result

        result.yes = true
        result.pos.x = start0.x + d0 * (end0.x - start0.x)
        result.pos.y = start0.y + d0 * (end0.y - start0.y)
        return result
    }

    // Returns true if the given point is inside the polygon
    // PNPoly by W. Randolph Franklin
    func polyContains(poly: Array'Math.Vector2, pt: Math.Vector2)->bool
    {
        i := 0
        j := cast(s32) poly.count - 1
        c := false
        verti := poly.buffer + i
        vertj := poly.buffer + j
        loop poly.count
        {
            if ((verti.y > pt.y) != (vertj.y > pt.y)) and
               (pt.x < (vertj.x - verti.x) * (pt.y - verti.y) / (vertj.y - verti.y) + verti.x)
            {
                c = !c
            }

            vertj = verti
            verti += 1
        }

        return c
    }
}