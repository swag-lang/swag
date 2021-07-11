// The original code from the Clipper library has been modified for swag
// http://www.angusj.com

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

    method addPath(path: ClipperPath, joinType: JoinType, endType: EndType) throw
    {
        highI := cast(s32) path.count - 1
        if highI < 0 return

        newNode := Memory.new'PolyNode()
        newNode.jointype = joinType
        newNode.endtype = endType

        if endType == .ClosedLine or endType == .ClosedPolygon
        {
            while highI > 0 and path.buffer[0] == path.buffer[highI]
                highI -= 1
        }

        newNode.contour.reserve(cast(uint) highI + 1)
        newNode.contour.add(path.buffer[0])

        var j, k: s32
        loop i: 1 .. highI
        {
            if newNode.contour.buffer[j] != path.buffer[i]
            {
                j += 1
                newNode.contour.add(path.buffer[i])
                if path[i].y > newNode.contour[k].y or
                   (path[i].y == newNode.contour[k].y and path[i].x < newNode.contour[k].x)
                        k = j
            }
        }

        if endType == .ClosedPolygon and j < 2
        {
            Memory.delete(newNode)
            return
        }

        polyNodes.addChild(newNode)

        if endType != .ClosedPolygon
            return

        if lowest.x < 0
            lowest = IntPoint{polyNodes.childs.count - 1, k}
        else
        {
            ip := polyNodes.childs[cast(s32) lowest.x].contour[cast(s32) lowest.y]
            if  newNode.contour[k].y > ip.y or
                (newNode.contour[k].y == ip.y and newNode.contour[k].x < ip.x)
            lowest = IntPoint{polyNodes.childs.count - 1, k}
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

    func orientation(poly: ClipperPath)->bool
    {
        return area(poly) >= 0
    }

    method fixOrientations()
    {
        if lowest.x >= 0 and !orientation(polyNodes.childs[cast(s32) lowest.x].contour)
        {
            loop i: polyNodes.childs.count
            {
                node := polyNodes.childs[i]
                if node.endtype == .ClosedPolygon or (node.endtype == .ClosedLine and orientation(node.contour))
                    Slice.reverse(node.contour.toSlice())
            }
        }
        else
        {
            loop i: polyNodes.childs.count
            {
                node := polyNodes.childs[i]
                if node.endtype == .ClosedLine and !orientation(node.contour)
                    Slice.reverse(node.contour.toSlice())
            }
        }
    }

    method execute(solution: *ClipperPaths, delta: f64, reverseNeg: bool = false) throw
    {
        solution.clear()
        fixOrientations()
        doOffset(delta)

        var clpr: Transform
        visit *p: destPolys
            try clpr.addPath(p, PolyType.Subject)

        if delta > 0
        {
            try clpr.execute(ClipType.Union, solution, PolyFillType.Positive, PolyFillType.Positive)
        }
        else
        {
            r := clpr.getBounds()
            var outer: ClipperPath
            outer.add(IntPoint{r.left - 10, r.bottom + 10})
            outer.add(IntPoint{r.right + 10, r.bottom + 10})
            outer.add(IntPoint{r.right + 10, r.top - 10})
            outer.add(IntPoint{r.left - 10, r.top - 10})

            try clpr.addPath(&outer, PolyType.Subject)
            clpr.reverseOutput = reverseNeg
            try clpr.execute(ClipType.Union, solution, PolyFillType.Negative, PolyFillType.Negative)
            if solution.count > 0
                solution.removeAt(0)
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

    method doOffset(delta: f64)
    {
        destPolys.clear()
        deltaVal = delta

        if Math.isZeroEpsilon(delta)
        {
            destPolys.reserve(polyNodes.childs.count)
            visit node: polyNodes.childs
            {
                if node.endtype == .ClosedPolygon
                    destPolys.add(node.contour)
            }

            return
        }

        if miterLimit > 2
            miterLim = 2 / (miterLimit * miterLimit)
        else
            miterLim = 0.5

        const DefaultArcTolerance = 0.25'f64
        var y: f64
        if arcTolerance <= 0.0
            y = DefaultArcTolerance
        else if arcTolerance > Math.abs(delta) * DefaultArcTolerance
            y = Math.abs(delta) * DefaultArcTolerance
        else
            y = arcTolerance

        steps := Math.ConstF64.Pi / Math.acos(1 - y / Math.abs(delta))
        if steps > Math.abs(delta) * Math.ConstF64.Pi
            steps = Math.abs(delta) * Math.ConstF64.Pi
        sinVal = Math.sin(Math.ConstF64.TwoPi / steps)
        cosVal = Math.cos(Math.ConstF64.TwoPi / steps)
        stepsPerRad = steps / Math.ConstF64.TwoPi
        if (delta < 0.0) sinVal = -sinVal

        destPolys.reserve(polyNodes.childs.count * 2)
        visit node, i: polyNodes.childs
        {
            srcPoly = node.contour

            len := cast(s32) srcPoly.count
            if len == 0 or (delta <= 0 and (len < 3 or node.endtype != .ClosedPolygon))
                continue

            destPoly.clear()
            if len == 1
            {
                if node.jointype == .Round
                {
                    var X: f64 = 1.0
                    var Y: f64 = 0.0
                    for j := 1; j <= steps; j += 1
                    {
                        destPoly.add(IntPoint{Transform.round(srcPoly[0].x + X * delta), Transform.round(srcPoly[0].y + Y * delta)});
                        X2 := X
                        X = X * cosVal - sinVal * Y
                        Y = X2 * sinVal + Y * cosVal
                    }
                }
                else
                {
                    var X: f64 = -1.0
                    var Y: f64 = -1.0
                    loop j: 4
                    {
                        destPoly.add(IntPoint{Transform.round(srcPoly[0].x + X * delta), Transform.round(srcPoly[0].y + Y * delta)})
                        if X < 0 X = 1
                        else if Y < 0 Y = 1
                        else X = -1
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

            if (node.endtype == .ClosedLine or node.endtype == .ClosedPolygon)
                normals.add(getUnitNormal(srcPoly[len - 1], srcPoly[0]))
            else
            {
                t := dref normals[len - 2]
                normals.add(t)
            }

            if (node.endtype == .ClosedPolygon)
            {
                k := len - 1
                loop j: len
                    offsetPoint(j, &k, node.jointype)
                destPolys.add(destPoly)
            }
            else if (node.endtype == .ClosedLine)
            {
                k := len - 1;
                loop j: len
                    offsetPoint(j, &k, node.jointype)
                destPolys.add(destPoly)

                destPoly.clear()
                n := dref normals[len - 1]
                for j := len - 1; j > 0; j -= 1
                    normals[j] = DoublePoint{-normals[j - 1].x, -normals[j - 1].y};
                normals[0] = DoublePoint{-n.x, -n.y}
                k = 0
                for j := len - 1; j >= 0; j -= 1
                    offsetPoint(j, &k, node.jointype)
                destPolys.add(destPoly)
            }
            else
            {
                k := 0
                for j := 1; j < len - 1; j += 1
                    offsetPoint(j, &k, node.jointype)

                if node.endtype == .OpenButt
                {
                    j := len - 1

                    r1 := cast(CInt) Transform.round(srcPoly[j].x + normals[j].x * delta)
                    r2 := cast(CInt) Transform.round(srcPoly[j].y + normals[j].y * delta)
                    pt1 := IntPoint{r1, r2}
                    destPoly.add(pt1)

                    r1 = cast(CInt) Transform.round(srcPoly[j].x - normals[j].x * delta)
                    r2 = cast(CInt) Transform.round(srcPoly[j].y - normals[j].y * delta)
                    pt1 = IntPoint{r1, r2}
                    destPoly.add(pt1)
                }
                else
                {
                    j := len - 1
                    k = len - 2
                    sinaVal = 0
                    normals[j] = DoublePoint{-normals[j].x, -normals[j].y}
                    if (node.endtype == .OpenSquare)
                        doSquare(j, k)
                    else
                        doRound(j, k)
                }

                //re-build normals ...
                for j := len - 1; j > 0; j -= 1
                    normals[j] = DoublePoint{-normals[j - 1].x, -normals[j - 1].y}
                normals[0] = DoublePoint{-normals[1].x, -normals[1].y}

                k = len - 1
                for j := k - 1; j > 0; j -= 1
                    offsetPoint(j, &k, node.jointype)

                if (node.endtype == .OpenButt)
                {
                    r1 := cast(CInt) Transform.round(srcPoly[0].x - normals[0].x * delta)
                    r2 := cast(CInt) Transform.round(srcPoly[0].y - normals[0].y * delta)
                    pt1 := IntPoint{r1, r2}
                    destPoly.add(pt1)

                    r1 = cast(CInt) Transform.round(srcPoly[0].x + normals[0].x * delta)
                    r2 = cast(CInt) Transform.round(srcPoly[0].y + normals[0].y * delta)
                    pt1 = IntPoint{r1, r2}
                    destPoly.add(pt1)

                }
                else
                {
                    k = 1
                    sinaVal = 0
                    if (node.endtype == .OpenSquare)
                        doSquare(0, 1)
                        else
                        doRound(0, 1)
                }

                destPolys.add(destPoly)
            }
        }
    }

    method offsetPoint(j: s32, ptrk: *s32, jointype: JoinType)
    {
        k := dref ptrk
        sinaVal = (normals[k].x * normals[j].y - normals[j].x * normals[k].y)
        if Math.abs(sinaVal * deltaVal) < 1.0
        {
            cosA := (normals[k].x * normals[j].x + normals[j].y * normals[k].y)
            if cosA > 0
            {
                r1 := Transform.round(srcPoly[j].x + normals[k].x * deltaVal)
                r2 := Transform.round(srcPoly[j].y + normals[k].y * deltaVal)
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
            r1 := Transform.round(srcPoly[j].x + normals[k].x * deltaVal)
            r2 := Transform.round(srcPoly[j].y + normals[k].y * deltaVal)
            destPoly.add(IntPoint{r1, r2})
            destPoly.add(dref srcPoly[j])
            r1 = Transform.round(srcPoly[j].x + normals[j].x * deltaVal)
            r2 = Transform.round(srcPoly[j].y + normals[j].y * deltaVal)
            destPoly.add(IntPoint{r1, r2})
        }
        else
        {
            switch jointype
            {
            case Miter:
                r := 1 + (normals[j].x * normals[k].x + normals[j].y * normals[k].y)
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
        dx := Math.tan(Math.atan2(sinaVal, normals[k].x * normals[j].x + normals[k].y * normals[j].y) / 4)
        r1 := Transform.round(srcPoly[j].x + deltaVal * (normals[k].x - normals[k].y * dx))
        r2 := Transform.round(srcPoly[j].y + deltaVal * (normals[k].y + normals[k].x * dx))
        destPoly.add(IntPoint{r1, r2})

        r1 = Transform.round(srcPoly[j].x + deltaVal * (normals[j].x + normals[j].y * dx))
        r2 = Transform.round(srcPoly[j].y + deltaVal * (normals[j].y - normals[j].x * dx))
        destPoly.add(IntPoint{r1, r2})
    }

    method doMiter(j, k: s32, r: f64)
    {
        q := deltaVal / r
        r1 := Transform.round(srcPoly[j].x + (normals[k].x + normals[j].x) * q)
        r2 := Transform.round(srcPoly[j].y + (normals[k].y + normals[j].y) * q)
        destPoly.add(IntPoint{r1, r2})
    }

    method doRound(j, k: s32)
    {
        a := Math.atan2(sinaVal, normals[k].x * normals[j].x + normals[k].y * normals[j].y)
        steps := Math.max(cast(s32) Transform.round(stepsPerRad * Math.abs(a)), 1)

        x := normals[k].x
        y := normals[k].y
        var X2: f64
        loop i: steps
        {
            r1 := Transform.round(srcPoly[j].x + x * deltaVal)
            r2 := Transform.round(srcPoly[j].y + y * deltaVal)
            destPoly.add(IntPoint{r1, r2})
            X2 = x
            x = x * cosVal - sinVal * y
            y = X2 * sinVal + y * cosVal
        }

        r1 := Transform.round(srcPoly[j].x + normals[j].x * deltaVal)
        r2 := Transform.round(srcPoly[j].y + normals[j].y * deltaVal)
        destPoly.add(IntPoint{r1, r2})
    }
}