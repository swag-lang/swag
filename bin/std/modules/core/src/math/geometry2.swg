#global namespace Math.Geometry
#global public

// Returns true if the point 'pt' is inside an ellipse
func isInEllipse(me, center: Vector2, radiusX, radiusY: f32)->bool
{
    return (pow((me.x - center.x), 2) / pow(radiusX, 2)) + (pow((me.y - center.y), 2) / pow(radiusY, 2)) <= 1
}

// Returns true if the point 'pt' is inside the triangle defined with 'a, b, c'
func isInTriangle(me, a, b, c: Vector2)->bool
{
    let ax  = c.x - b.x
    let ay  = c.y - b.y
    let bx  = a.x - c.x
    let by  = a.y - c.y
    let cx  = b.x - a.x
    let cy  = b.y - a.y
    let apx = me.x - a.x
    let apy = me.y - a.y
    let bpx = me.x - b.x
    let bpy = me.y - b.y
    let cpx = me.x - c.x
    let cpy = me.y - c.y

    let aCROSSbp = ax * bpy - ay * bpx
    let cCROSSap = cx * apy - cy * apx
    let bCROSScp = bx * cpy - by * cpx

    return ((aCROSSbp >= 0) and (bCROSScp >= 0) and (cCROSSap >= 0)) or ((aCROSSbp <= 0) and (bCROSScp <= 0) and (cCROSSap <= 0))
}

// Returns the distance of the point to the given line
func distanceToLine(me, a, b: Vector2)->f32
{
    let toPt = me - a
    var seg  = b - a
    seg.normalizeSafe()
    let dot = toPt.dot(seg)
    seg *= dot
    let cross = toPt - seg
    return cross.length()
}

// Returns the distance of the point to the given segment
func distanceToSegment(me, a, b: Vector2)->f32
{
    let ab = b - a
    let be = me - b
    let ae = me - a

    let abbe = ab.dot(be)
    if abbe > 0:
        return be.length()

    let abae = ab.dot(ae)
    if abae < 0:
        return ae.length()

    let x1  = ab.x
    let y1  = ab.y
    let x2  = ae.x
    let y2  = ae.y
    let mod = Math.sqrt(x1 * x1 + y1 * y1)
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
func nearestPointsSegSeg(p0, p1, q0, q1: Math.Vector2)->{ a: Math.Vector2, b: Math.Vector2, dist: f32 }
{
    let p1p0 = p1 - p0
    let q1q0 = q1 - q0
    let p0q0 = p0 - q0
    let a    = p1p0.dot(p1p0)
    let b    = p1p0.dot(q1q0)
    let c    = q1q0.dot(q1q0)
    let d    = p1p0.dot(p0q0)
    let e    = q1q0.dot(p0q0)
    let det  = a * c - b * b

    var s, t: f32 = undefined
    if det > 0
    {
        let bte = b * e
        let ctd = c * d
        if bte <= ctd
        {
            s = 0
            if e <= 0
            {
                t = 0
                let nd = -d
                if nd >= a:
                    s = 1
                elif nd > 0:
                    s = nd / a
            }
            elif e < c
            {
                t = e / c
            }
            else
            {
                t = 1
                let bmd = b - d
                if bmd >= a:
                    s = 1
                elif bmd > 0:
                    s = bmd / a
            }
        }
        else
        {
            s = bte - ctd
            if s >= det
            {
                s = 1
                let bpe = b + e
                if bpe <= 0
                {
                    t = 0
                    let nd = -d
                    if nd <= 0:
                        s = 0
                    elif nd < a:
                        s = nd / a
                }
                elif bpe < c
                {
                    t = bpe / c
                }
                else
                {
                    t = 1
                    let bmd = b - d
                    if bmd <= 0:
                        s = 0
                    elif bmd < a:
                        s = bmd / a
                }
            }
            else
            {
                let ate = a * e
                let btd = b * d
                if ate <= btd
                {
                    t = 0
                    let nd = -d
                    if nd <= 0:
                        s = 0
                    elif nd >= a:
                        s = 1
                    else:
                        s = nd / a
                }
                else
                {
                    t = ate - btd
                    if t >= det
                    {
                        t = 1
                        let bmd = b - d
                        if bmd <= 0:
                            s = 0
                        elif bmd >= a:
                            s = 1
                        else:
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
            let nd = -d
            if nd <= 0:
                s = 0
            elif nd >= a:
                s = 1
            else:
                s = nd / a
        }
        elif e >= c
        {
            t = 1
            let bmd = b - d
            if bmd <= 0:
                s = 0
            elif bmd >= a:
                s = 1
            else:
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
    var diff = result.a - result.b
    result.dist = diff.length()
    return result
}

internal func getLineLineIntersectCoefs(start0, end0, start1, end1: Vector2, epsilon: f32)->{ yes: bool, d0: f32, d1: f32 }
{
    var result: retval

    let ua = (end1.x - start1.x) * (start0.y - start1.y) - (end1.y - start1.y) * (start0.x - start1.x)
    let ub = (end0.x - start0.x) * (start0.y - start1.y) - (end0.y - start0.y) * (start0.x - start1.x)
    let d  = (end1.y - start1.y) * (end0.x - start0.x) - (end1.x - start1.x) * (end0.y - start0.y)

    // Parallel lines
    if Math.abs(d) < epsilon
    {
        result.yes = Math.abs(ua) < epsilon and Math.abs(ub) < epsilon
        return result
    }

    result.yes = true
    result.d0  = ua / d
    result.d1  = ub / d
    return result
}

// Compute the intersection point of two lines
func lineLineIntersect(start0, end0, start1, end1: Vector2, epsilon: f32 = ConstF32.Epsilon)->{ yes: bool, pos: Vector2 }
{
    var result: retval

    let (yes, d0, ?) = getLineLineIntersectCoefs(start0, end0, start1, end1, epsilon)
    if !yes:
        return result

    result.yes   = true
    result.pos.x = start0.x + d0 * (end0.x - start0.x)
    result.pos.y = start0.y + d0 * (end0.y - start0.y)
    return result
}

// Compute the intersection point of two segments
func segSegIntersect(start0, end0, start1, end1: Vector2, epsilon: f32 = ConstF32.Epsilon)->{ yes: bool, pos: Vector2 }
{
    var result: retval

    let (yes, d0, d1) = getLineLineIntersectCoefs(start0, end0, start1, end1, epsilon)
    if !yes:
        return result

    if d0 < -epsilon or d0 > 1 + epsilon or d1 < -epsilon or d1 > 1 + epsilon:
        return result

    result.yes   = true
    result.pos.x = start0.x + d0 * (end0.x - start0.x)
    result.pos.y = start0.y + d0 * (end0.y - start0.y)
    return result
}

// Returns true if the given point is inside the polygon
// PNPoly by W. Randolph Franklin
func polyContains(poly: Array'Math.Vector2, pt: Math.Vector2)->bool
{
    var c     = false
    var verti = poly.buffer
    var vertj = poly.buffer + cast(s32) poly.count - 1
    for poly.count
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
