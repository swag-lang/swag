// The original code from the Clipper library (http://www.angusj.com) has been modified for swag
// Only the ClosedPolygon end type is supported (everything else has been removed)

/*******************************************************************************
*                                                                              *
* Author    :  Angus Johnson                                                   *
* Version   :  6.4.2                                                           *
* Date      :  27 February 2017                                                *
* Website   :  http://www.angusj.com                                           *
* Copyright :  Angus Johnson 2010-2017                                         *
*                                                                              *
* License:                                                                     *
* Use, modification & distribution is subject to Boost Software License Ver 1. *
* http://www.boost.org/LICENSE_1_0.txt                                         *
*                                                                              *
* Attributions:                                                                *
* The code in this library is an extension of Bala Vatti's clipping algorithm: *
* "A generic solution to polygon clipping"                                     *
* Communications of the ACM, Vol 35, Issue 7 (July 1992) pp 56-63.             *
* http://portal.acm.org/citation.cfm?id=129906                                 *
*                                                                              *
* Computer graphics and geometric modeling: implementation and algorithms      *
* By Max K. Agoston                                                            *
* Springer; 1 edition (January 4, 2005)                                        *
* http://books.google.com/books?q=vatti+clipping+agoston                       *
*                                                                              *
* See also:                                                                    *
* "Polygon Offsetting by Computing Winding Numbers"                            *
* Paper no. DETC2005-85513 pp. 565-575                                         *
* ASME 2005 International Design Engineering Technical Conferences             *
* and Computers and Information in Engineering Conference (IDETC/CIE2005)      *
* September 24-28, 2005 , Long Beach, California, USA                          *
* http://www.me.berkeley.edu/~mcmains/pubs/DAC05OffsetPolygon.pdf              *
*                                                                              *
*******************************************************************************/
#global namespace Clipper
using Core

struct DoublePoint
{
    x, y: f64
}

#[Swag.opaque]
public struct Offset
{
    miterLimit:     f64 = 2
    arcTolerance:   f64 = 0.25
    destPolys:      ClipperPaths
    srcPoly:        ClipperPath
    destPoly:       ClipperPath
    normals:        Array'DoublePoint
    deltaVal:       f64
    sinaVal:        f64
    sinVal:         f64
    cosVal:         f64
    miterLim:       f64
    stepsPerRad:    f64
    lowest:         IntPoint = @{-1, 0}
    polyNodes:      PolyNode
}

impl Offset
{
    method opDrop()
    {
        clear()
    }

    method clear()
    {
        loop i: polyNodes.childs
            Memory.delete(polyNodes.childs[i])
        polyNodes.childs.clear()
        lowest.x = -1
    }

    method addPath(path: ClipperPath, joinType: JoinType)
    {
        highI := path.count - 1
        if highI < 0 return
        while highI > 0 and path.buffer[0] == path.buffer[highI]
            highI -= 1

        newNode := Memory.new'PolyNode()
        newNode.jointype = joinType
        newNode.contour.reserve(highI + 1)
        newNode.contour.add(path.buffer[0])

        var j, k: s32
        loop i: 1 .. highI
        {
            if newNode.contour.buffer[j] != path.buffer[i]
            {
                j += 1
                newNode.contour.add(path.buffer[i])
                kC := newNode.contour[k]
                if path[i].y > kC.y or (path[i].y == kC.y and path[i].x < kC.x)
                    k = j
            }
        }

        if j < 2
        {
            Memory.delete(newNode)
            return
        }

        polyNodes.addChild(newNode)

        if lowest.x < 0
            lowest = IntPoint{polyNodes.childs.count - 1, k}
        else
        {
            ip := polyNodes.childs[cast(s32) lowest.x].contour[cast(s32) lowest.y]
            if newNode.contour[k].y > ip.y or (newNode.contour[k].y == ip.y and newNode.contour[k].x < ip.x)
                lowest = IntPoint{polyNodes.childs.count - 1, k}
        }
    }

    method execute(solution: *ClipperPaths, delta: f64, reverseNeg: bool = false)
    {
        solution.clear()
        fixOrientations()
        doOffset(delta)

        var clpr: Transform
        visit *p: destPolys
            clpr.addPath(p, PolyType.Subject)

        if delta > 0
        {
            clpr.execute(ClipType.Union, solution, PolyFillType.Positive, PolyFillType.Positive)
        }
        else
        {
            r := clpr.getBounds()
            var outer: ClipperPath
            outer.add(IntPoint{r.left - 10, r.bottom + 10})
            outer.add(IntPoint{r.right + 10, r.bottom + 10})
            outer.add(IntPoint{r.right + 10, r.top - 10})
            outer.add(IntPoint{r.left - 10, r.top - 10})

            clpr.addPath(&outer, PolyType.Subject)
            clpr.reverseOutput = reverseNeg
            clpr.execute(ClipType.Union, solution, PolyFillType.Negative, PolyFillType.Negative)
            if solution.count > 0
                solution.removeAt(0)
        }
    }

    func area(poly: ClipperPath)->f64
    {
        size := cast(s32) poly.count
        if size < 3 return 0

        a := 0'f64
        j := size - 1
        loop i: size
        {
            a += (cast(f64) poly.buffer[j].x + poly.buffer[i].x) * (cast(f64) poly.buffer[j].y - poly.buffer[i].y)
            j = i
        }

        return -a * 0.5
    }

    #[Swag.inline]
    func orientation(poly: ClipperPath)->bool
    {
        return area(poly) >= 0
    }

    method fixOrientations()
    {
        if lowest.x >= 0 and !orientation(polyNodes.childs[lowest.x].contour)
        {
            loop i: polyNodes.childs.count
            {
                node := polyNodes.childs[i]
                Slice.reverse(node.contour.toSlice())
            }
        }
    }

    func getUnitNormal(pt1, pt2: *IntPoint)->DoublePoint
    {
        var result: retval
        if pt2.x == pt1.x and pt2.y == pt1.y
            return result

        dx := cast(f64) (pt2.x - pt1.x)
        dy := cast(f64) (pt2.y - pt1.y)
        f  := 1.0 / Math.sqrt(dx*dx + dy*dy)
        dx *= f
        dy *= f
        result.x = dy
        result.y = -dx
        return result
    }

    #[Swag.inline]
    func round(val: f64)->CInt
    {
        if val < 0 return cast(CInt) (val - 0.5)
        return cast(CInt) (val + 0.5)
    }

    method doOffset(delta: f64)
    {
        destPolys.clear()
        deltaVal = delta

        if Math.isZeroEpsilon(delta)
        {
            destPolys.reserve(polyNodes.childs.count)
            visit node: polyNodes.childs
                destPolys.add(node.contour)
            return
        }

        if miterLimit > 2
            miterLim = 2 / (miterLimit * miterLimit)
        else
            miterLim = 0.5

        const DefaultArcTolerance = 0.25'f64
        var y: f64 = ?
        if arcTolerance <= 0.0
            y = DefaultArcTolerance
        else if arcTolerance > Math.abs(delta) * DefaultArcTolerance
            y = Math.abs(delta) * DefaultArcTolerance
        else
            y = arcTolerance

        const Pi    = Math.ConstF64.Pi
        const TwoPi = Math.ConstF64.TwoPi
        steps := Pi / Math.acos(1 - y / Math.abs(delta))
        if steps > Math.abs(delta) * Pi
            steps = Math.abs(delta) * Pi
        sinVal = Math.sin(TwoPi / steps)
        cosVal = Math.cos(TwoPi / steps)
        stepsPerRad = steps / TwoPi
        if delta < 0.0 sinVal = -sinVal

        destPolys.reserve(polyNodes.childs.count * 2)
        visit node, i: polyNodes.childs
        {
            srcPoly = node.contour

            len := cast(s32) srcPoly.count
            if len == 0 or (delta <= 0 and len < 3)
                continue

            destPoly.clear()
            if len == 1
            {
                if node.jointype == .Round
                {
                    x := 1.0'f64
                    y := 0.0'f64
                    for j := 1; j <= steps; j += 1
                    {
                        r1 := round(srcPoly[0].x + x * delta)
                        r2 := round(srcPoly[0].y + y * delta)
                        destPoly.add(IntPoint{r1, r2})
                        x2 := x
                        x = x * cosVal - sinVal * y
                        y = x2 * sinVal + y * cosVal
                    }
                }
                else
                {
                    x := -1.0'f64
                    y := -1.0'f64
                    loop j: 4
                    {
                        r1 := round(srcPoly[0].x + x * delta)
                        r2 := round(srcPoly[0].y + y * delta)
                        destPoly.add(IntPoint{r1, r2})
                        if x < 0 x = 1
                        else if y < 0 y = 1
                        else x = -1
                    }
                }

                destPolys.add(destPoly)
                continue
            }

            // Build normals
            normals.clear()
            normals.reserve(cast(uint) len)
            loop j: len - 1
                normals.add(getUnitNormal(srcPoly[j], srcPoly[j + 1]))
            normals.add(getUnitNormal(srcPoly[len - 1], srcPoly[0]))

            k := len - 1
            loop j: len
                offsetPoint(j, &k, node.jointype)
            destPolys.add(destPoly)
        }
    }

    method offsetPoint(j: s32, ptrk: *s32, jointype: JoinType)
    {
        k := dref ptrk

        kN := normals[k]
        jN := normals[j]
        jP := srcPoly[j]

        sinaVal = (kN.x * jN.y - jN.x * kN.y)
        if Math.abs(sinaVal * deltaVal) < 1.0
        {
            cosA := (kN.x * jN.x + jN.y * kN.y)
            if cosA > 0
            {
                r1 := round(jP.x + kN.x * deltaVal)
                r2 := round(jP.y + kN.y * deltaVal)
                destPoly.add(IntPoint{r1, r2})
                return
            }
        }
        else if sinaVal > 1.0
            sinaVal = 1.0
        else if sinaVal < -1.0
            sinaVal = -1.0

        if sinaVal * deltaVal < 0
        {
            r1 := round(jP.x + kN.x * deltaVal)
            r2 := round(jP.y + kN.y * deltaVal)
            destPoly.add(IntPoint{r1, r2})
            destPoly.add(dref jP)
            r1 = round(jP.x + jN.x * deltaVal)
            r2 = round(jP.y + jN.y * deltaVal)
            destPoly.add(IntPoint{r1, r2})
        }
        else
        {
            switch jointype
            {
            case Miter:
                r := 1 + (jN.x * kN.x + jN.y * kN.y)
                if r >= miterLim
                    doMiter(j, k, r)
                else
                    doSquare(j, k)
            case Square:
                doSquare(j, k)
            case Round:
                doRound(j, k)
            }
        }

        dref ptrk = j
    }

    method doSquare(j, k: s32)
    {
        kN := normals[k]
        jN := normals[j]
        jP := srcPoly[j]

        dx := Math.tan(Math.atan2(sinaVal, kN.x * jN.x + kN.y * jN.y) / 4)
        r1 := round(jP.x + deltaVal * (kN.x - kN.y * dx))
        r2 := round(jP.y + deltaVal * (kN.y + kN.x * dx))
        destPoly.add(IntPoint{r1, r2})

        r1 = round(jP.x + deltaVal * (jN.x + jN.y * dx))
        r2 = round(jP.y + deltaVal * (jN.y - jN.x * dx))
        destPoly.add(IntPoint{r1, r2})
    }

    method doMiter(j, k: s32, r: f64)
    {
        kN := normals[k]
        jN := normals[j]
        jP := srcPoly[j]

        q := deltaVal / r
        r1 := round(jP.x + (kN.x + jN.x) * q)
        r2 := round(jP.y + (kN.y + jN.y) * q)
        destPoly.add(IntPoint{r1, r2})
    }

    method doRound(j, k: s32)
    {
        kN := normals[k]
        jN := normals[j]
        jP := srcPoly[j]

        a := Math.atan2(sinaVal, kN.x * jN.x + kN.y * jN.y)
        steps := Math.max(cast(s32) round(stepsPerRad * Math.abs(a)), 1)

        x := kN.x
        y := kN.y
        loop i: steps
        {
            r1 := round(jP.x + x * deltaVal)
            r2 := round(jP.y + y * deltaVal)
            destPoly.add(IntPoint{r1, r2})
            x2 := x
            x = x * cosVal - sinVal * y
            y = x2 * sinVal + y * cosVal
        }

        r1 := round(jP.x + jN.x * deltaVal)
        r2 := round(jP.y + jN.y * deltaVal)
        destPoly.add(IntPoint{r1, r2})
    }
}