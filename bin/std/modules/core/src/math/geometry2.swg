#global namespace Math
#global namespace Geometry
#global public

// Test if a point is inside an ellipse.
// Uses the standard ellipse equation: (x-cx)²/rx² + (y-cy)²/ry² ≤ 1.
func isInEllipse(self, center: Vector2, radiusX, radiusY: f32)->bool
{
    return (pow((self.x - center.x), 2) / pow(radiusX, 2)) + (pow((self.y - center.y), 2) / pow(radiusY, 2)) <= 1
}

// Test if a point is inside a triangle using cross product method.
// Uses barycentric coordinates by checking if all cross products have the same sign.
func isInTriangle(self, a, b, c: Vector2)->bool
{
    let ax  = c.x - b.x
    let ay  = c.y - b.y
    let bx  = a.x - c.x
    let by  = a.y - c.y
    let cx  = b.x - a.x
    let cy  = b.y - a.y
    let apx = self.x - a.x
    let apy = self.y - a.y
    let bpx = self.x - b.x
    let bpy = self.y - b.y
    let cpx = self.x - c.x
    let cpy = self.y - c.y

    let aCROSSbp = ax * bpy - ay * bpx
    let cCROSSap = cx * apy - cy * apx
    let bCROSScp = bx * cpy - by * cpx

    return ((aCROSSbp >= 0) and (bCROSScp >= 0) and (cCROSSap >= 0)) or ((aCROSSbp <= 0) and (bCROSScp <= 0) and (cCROSSap <= 0))
}

// Calculate the perpendicular distance from a point to an infinite line.
// Projects the point onto the line and returns the distance to the projection.
func distanceToLine(self, a, b: Vector2)->f32
{
    let toPt = self - a
    var seg  = b - a
    seg.normalizeSafe()
    let dot = toPt.dot(seg)
    seg *= dot
    let cross = toPt - seg
    return cross.length()
}

// Calculate the shortest distance from a point to a line segment.
// Handles endpoints by clamping the projection to the segment bounds.
func distanceToSegment(self, a, b: Vector2)->f32
{
    let ab = b - a
    let be = self - b
    let ae = self - a

    let abbe = ab.dot(be)
    if abbe > 0 do
        return be.length()

    let abae = ab.dot(ae)
    if abae < 0 do
        return ae.length()

    let x1  = ab.x
    let y1  = ab.y
    let x2  = ae.x
    let y2  = ae.y
    let mod = Math.sqrt(x1 * x1 + y1 * y1)
    return Math.abs(x1 * y2 - y1 * x2) / mod
}

// Find the closest points between two line segments and their distance.
// Implementation based on David Eberly's Geometric Tools library algorithm.
//
// ---
// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2022
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// https://www.geometrictools.com/Documentation/DistanceSegments3D.pdf
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
                if nd >= a do
                    s = 1
                elif nd > 0 do
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
                if bmd >= a do
                    s = 1
                elif bmd > 0 do
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
                    if nd <= 0 do
                        s = 0
                    elif nd < a do
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
                    if bmd <= 0 do
                        s = 0
                    elif bmd < a do
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
                    if nd <= 0 do
                        s = 0
                    elif nd >= a do
                        s = 1
                    else do
                        s = nd / a
                }
                else
                {
                    t = ate - btd
                    if t >= det
                    {
                        t = 1
                        let bmd = b - d
                        if bmd <= 0 do
                            s = 0
                        elif bmd >= a do
                            s = 1
                        else do
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
            if nd <= 0 do
                s = 0
            elif nd >= a do
                s = 1
            else do
                s = nd / a
        }
        elif e >= c
        {
            t = 1
            let bmd = b - d
            if bmd <= 0 do
                s = 0
            elif bmd >= a do
                s = 1
            else do
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
    let diff = result.a - result.b
    result.dist = diff.length()
    return result
}

// Calculate intersection coefficients for two infinite lines.
// Returns parametric coefficients 'd0' and 'd1' where intersection = start + d * (end - start).
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

// Find the intersection point of two infinite lines.
// Returns the intersection point if lines are not parallel, otherwise returns failure.
func lineLineIntersect(start0, end0, start1, end1: Vector2, epsilon: f32 = ConstF32.Epsilon)->{ yes: bool, pos: Vector2 }
{
    var result: retval

    let (yes, d0, ?) = getLineLineIntersectCoefs(start0, end0, start1, end1, epsilon)
    if !yes do
        return result

    result.yes   = true
    result.pos.x = start0.x + d0 * (end0.x - start0.x)
    result.pos.y = start0.y + d0 * (end0.y - start0.y)
    return result
}

// Find the intersection point of two line segments.
// Returns the intersection point only if it lies within both segment bounds.
func segSegIntersect(start0, end0, start1, end1: Vector2, epsilon: f32 = ConstF32.Epsilon)->{ yes: bool, pos: Vector2 }
{
    var result: retval

    let (yes, d0, d1) = getLineLineIntersectCoefs(start0, end0, start1, end1, epsilon)
    if !yes do
        return result

    if d0 < -epsilon or d0 > 1 + epsilon or d1 < -epsilon or d1 > 1 + epsilon do
        return result

    result.yes   = true
    result.pos.x = start0.x + d0 * (end0.x - start0.x)
    result.pos.y = start0.y + d0 * (end0.y - start0.y)
    return result
}

// Test if a point is inside a polygon using ray casting algorithm.
// Implementation of the PNPoly algorithm by W. Randolph Franklin.
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
