// The original code has been modified for swag
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
    MiterLimit:     f64 = 2
    ArcTolerance:   f64 = 0.25
    m_destPolys:    ClipperPaths
    m_srcPoly:      ClipperPath
    m_destPoly:     ClipperPath
    m_normals:      Array'DoublePoint
    m_delta:        f64
    m_sinA:         f64
    m_sin:          f64
    m_cos:          f64
    m_miterLim:     f64
    m_StepsPerRad:  f64
    m_lowest:       IntPoint = @{-1, 0}
    m_polyNodes:    PolyNode
}

impl Offset
{
    method opDrop()
    {
        clear()
    }

    method clear()
    {
        loop i: m_polyNodes.Childs
            Memory.delete(m_polyNodes.Childs[i])
        m_polyNodes.Childs.clear()
        m_lowest.x = -1
    }

    // Register a new series of point (line path) of a given type
    public method addPath(pg: Array'Math.Vector2, joinType: JoinType, endType: EndType) throw
    {
        var pt: ClipperPath
        visit p: pg
            pt.add(IntPoint{cast(s32) (p.x * ScaleCoords), cast(s32) (p.y * ScaleCoords)})
        try addPath(pt, joinType, endType)
    }

    public method execute(solution: *Polygon, delta: f64) throw
    {
        var res: ClipperPaths
        try execute(&res, delta * ScaleCoords)

        solution.clear()
        if res.count
        {
            visit *pt: dref res[0]
            {
                vec := Math.Vector2{cast(f32) (pt.x / ScaleCoords), cast(f32) (pt.y / ScaleCoords)}
                solution.add(vec)
            }

            v0 := solution.getPoint(0)
            solution.add(v0)
        }
    }

    method addPath(path: ClipperPath, joinType: JoinType, endType: EndType) throw
    {
        highI := cast(s32) path.count - 1
        if highI < 0 return

        newNode := Memory.new'PolyNode()
        newNode.jointype = joinType
        newNode.endtype = endType

        if (endType == .ClosedLine or endType == .ClosedPolygon)
            while highI > 0 and path.buffer[0] == path.buffer[highI]
                highI -= 1

        newNode.Contour.reserve(cast(uint) highI + 1)
        newNode.Contour.add(path.buffer[0])

        var j, k: s32
        loop i: 1 .. highI
        {
            if newNode.Contour.buffer[j] != path.buffer[i]
            {
                j += 1
                newNode.Contour.add(path.buffer[i])
                if path[i].y > newNode.Contour[k].y or
                   (path[i].y == newNode.Contour[k].y and path[i].x < newNode.Contour[k].x)
                        k = j
            }
        }

        if endType == .ClosedPolygon and j < 2
        {
            Memory.delete(newNode)
            return
        }

        m_polyNodes.addChild(newNode)

        if (endType != .ClosedPolygon) return

        if m_lowest.x < 0
            m_lowest = IntPoint{m_polyNodes.Childs.count - 1, k}
        else
        {
            ip := m_polyNodes.Childs[cast(s32) m_lowest.x].Contour[cast(s32) m_lowest.y]
            if  newNode.Contour[k].y > ip.y or
                (newNode.Contour[k].y == ip.y and newNode.Contour[k].x < ip.x)
            m_lowest = IntPoint{m_polyNodes.Childs.count - 1, k}
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
        if m_lowest.x >= 0 and !orientation(m_polyNodes.Childs[cast(s32) m_lowest.x].Contour)
        {
            loop i: m_polyNodes.Childs.count
            {
                node := m_polyNodes.Childs[i]
                if node.endtype == .ClosedPolygon or (node.endtype == .ClosedLine and orientation(node.Contour))
                    Slice.reverse(node.Contour.toSlice())
            }
        }
        else
        {
            loop i: m_polyNodes.Childs.count
            {
                node := m_polyNodes.Childs[i]
                if node.endtype == .ClosedLine and !orientation(node.Contour)
                    Slice.reverse(node.Contour.toSlice())
            }
        }
    }

    method execute(solution: *ClipperPaths, delta: f64) throw
    {
        solution.clear()
        fixOrientations()
        doOffset(delta)

        //now clean up 'corners' ...
        var clpr: Transform
        visit *p: m_destPolys
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
            clpr.reverseOutput = true
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
        m_destPolys.clear()
        m_delta = delta

        if Math.isZeroEpsilon(delta)
        {
            m_destPolys.reserve(m_polyNodes.Childs.count)
            visit node: m_polyNodes.Childs
            {
                if node.endtype == .ClosedPolygon
                    m_destPolys.add(node.Contour)
            }

            return
        }

        if MiterLimit > 2
            m_miterLim = 2 / (MiterLimit * MiterLimit)
        else
            m_miterLim = 0.5

        const def_arc_tolerance = 0.25'f64
        var y: f64
        if ArcTolerance <= 0.0
            y = def_arc_tolerance
        else if ArcTolerance > Math.abs(delta) * def_arc_tolerance
            y = Math.abs(delta) * def_arc_tolerance
        else
            y = ArcTolerance

        steps := Math.ConstF64.Pi / Math.acos(1 - y / Math.abs(delta))
        if steps > Math.abs(delta) * Math.ConstF64.Pi
            steps = Math.abs(delta) * Math.ConstF64.Pi
        m_sin = Math.sin(Math.ConstF64.TwoPi / steps)
        m_cos = Math.cos(Math.ConstF64.TwoPi / steps)
        m_StepsPerRad = steps / Math.ConstF64.TwoPi
        if (delta < 0.0) m_sin = -m_sin

        m_destPolys.reserve(m_polyNodes.Childs.count * 2)
        visit node, i: m_polyNodes.Childs
        {
            m_srcPoly = node.Contour

            len := cast(s32) m_srcPoly.count
            if len == 0 or (delta <= 0 and (len < 3 or node.endtype != .ClosedPolygon))
                continue

            m_destPoly.clear()
            if len == 1
            {
                if node.jointype == .Round
                {
                    var X: f64 = 1.0
                    var Y: f64 = 0.0
                    for j := 1; j <= steps; j += 1
                    {
                        m_destPoly.add(IntPoint{Transform.round(m_srcPoly[0].x + X * delta), Transform.round(m_srcPoly[0].y + Y * delta)});
                        X2 := X
                        X = X * m_cos - m_sin * Y
                        Y = X2 * m_sin + Y * m_cos
                    }
                }
                else
                {
                    var X: f64 = -1.0
                    var Y: f64 = -1.0
                    loop j: 4
                    {
                        m_destPoly.add(IntPoint{Transform.round(m_srcPoly[0].x + X * delta), Transform.round(m_srcPoly[0].y + Y * delta)})
                        if X < 0 X = 1
                        else if Y < 0 Y = 1
                        else X = -1
                    }
                }

                m_destPolys.add(m_destPoly)
                continue
            }

            //build m_normals ...
            m_normals.clear()
            m_normals.reserve(cast(uint) len)
            loop j: len - 1
                m_normals.add(getUnitNormal(m_srcPoly[j], m_srcPoly[j + 1]))

            if (node.endtype == .ClosedLine or node.endtype == .ClosedPolygon)
                m_normals.add(getUnitNormal(m_srcPoly[len - 1], m_srcPoly[0]))
            else
            {
                t := dref m_normals[len - 2]
                m_normals.add(t)
            }

            if (node.endtype == .ClosedPolygon)
            {
                k := len - 1
                loop j: len
                    offsetPoint(j, &k, node.jointype)
                m_destPolys.add(m_destPoly)
            }
            else if (node.endtype == .ClosedLine)
            {
                k := len - 1;
                loop j: len
                    offsetPoint(j, &k, node.jointype)
                m_destPolys.add(m_destPoly)

                m_destPoly.clear()
                n := dref m_normals[len - 1]
                for j := len - 1; j > 0; j -= 1
                    m_normals[j] = DoublePoint{-m_normals[j - 1].x, -m_normals[j - 1].y};
                m_normals[0] = DoublePoint{-n.x, -n.y}
                k = 0
                for j := len - 1; j >= 0; j -= 1
                    offsetPoint(j, &k, node.jointype)
                m_destPolys.add(m_destPoly)
            }
            else
            {
                k := 0
                for j := 1; j < len - 1; j += 1
                    offsetPoint(j, &k, node.jointype)

                if node.endtype == .OpenButt
                {
                    j := len - 1

                    r1 := cast(CInt) Transform.round(m_srcPoly[j].x + m_normals[j].x * delta)
                    r2 := cast(CInt) Transform.round(m_srcPoly[j].y + m_normals[j].y * delta)
                    pt1 := IntPoint{r1, r2}
                    m_destPoly.add(pt1)

                    r1 = cast(CInt) Transform.round(m_srcPoly[j].x - m_normals[j].x * delta)
                    r2 = cast(CInt) Transform.round(m_srcPoly[j].y - m_normals[j].y * delta)
                    pt1 = IntPoint{r1, r2}
                    m_destPoly.add(pt1)
                }
                else
                {
                    j := len - 1
                    k = len - 2
                    m_sinA = 0
                    m_normals[j] = DoublePoint{-m_normals[j].x, -m_normals[j].y}
                    if (node.endtype == .OpenSquare)
                        doSquare(j, k)
                    else
                        doRound(j, k)
                }

                //re-build m_normals ...
                for j := len - 1; j > 0; j -= 1
                    m_normals[j] = DoublePoint{-m_normals[j - 1].x, -m_normals[j - 1].y}
                m_normals[0] = DoublePoint{-m_normals[1].x, -m_normals[1].y}

                k = len - 1
                for j := k - 1; j > 0; j -= 1
                    offsetPoint(j, &k, node.jointype)

                if (node.endtype == .OpenButt)
                {
                    r1 := cast(CInt) Transform.round(m_srcPoly[0].x - m_normals[0].x * delta)
                    r2 := cast(CInt) Transform.round(m_srcPoly[0].y - m_normals[0].y * delta)
                    pt1 := IntPoint{r1, r2}
                    m_destPoly.add(pt1)

                    r1 = cast(CInt) Transform.round(m_srcPoly[0].x + m_normals[0].x * delta)
                    r2 = cast(CInt) Transform.round(m_srcPoly[0].y + m_normals[0].y * delta)
                    pt1 = IntPoint{r1, r2}
                    m_destPoly.add(pt1)

                }
                else
                {
                    k = 1
                    m_sinA = 0
                    if (node.endtype == .OpenSquare)
                        doSquare(0, 1)
                        else
                        doRound(0, 1)
                }

                m_destPolys.add(m_destPoly)
            }
        }
    }

    method offsetPoint(j: s32, ptrk: *s32, jointype: JoinType)
    {
        k := dref ptrk
        m_sinA = (m_normals[k].x * m_normals[j].y - m_normals[j].x * m_normals[k].y)
        if Math.abs(m_sinA * m_delta) < 1.0
        {
            cosA := (m_normals[k].x * m_normals[j].x + m_normals[j].y * m_normals[k].y)
            if cosA > 0
            {
                r1 := Transform.round(m_srcPoly[j].x + m_normals[k].x * m_delta)
                r2 := Transform.round(m_srcPoly[j].y + m_normals[k].y * m_delta)
                m_destPoly.add(IntPoint{r1, r2})
                return
            }
        }
        else if m_sinA > 1.0
            m_sinA = 1.0
        else if m_sinA < -1.0
            m_sinA = -1.0

        if m_sinA * m_delta < 0
        {
            r1 := Transform.round(m_srcPoly[j].x + m_normals[k].x * m_delta)
            r2 := Transform.round(m_srcPoly[j].y + m_normals[k].y * m_delta)
            m_destPoly.add(IntPoint{r1, r2})
            m_destPoly.add(dref m_srcPoly[j])
            r1 = Transform.round(m_srcPoly[j].x + m_normals[j].x * m_delta)
            r2 = Transform.round(m_srcPoly[j].y + m_normals[j].y * m_delta)
            m_destPoly.add(IntPoint{r1, r2})
        }
        else
        {
            switch jointype
            {
            case Miter:
                r := 1 + (m_normals[j].x * m_normals[k].x + m_normals[j].y * m_normals[k].y)
                if r >= m_miterLim
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
        dx := Math.tan(Math.atan2(m_sinA, m_normals[k].x * m_normals[j].x + m_normals[k].y * m_normals[j].y) / 4)
        r1 := Transform.round(m_srcPoly[j].x + m_delta * (m_normals[k].x - m_normals[k].y * dx))
        r2 := Transform.round(m_srcPoly[j].y + m_delta * (m_normals[k].y + m_normals[k].x * dx))
        m_destPoly.add(IntPoint{r1, r2})

        r1 = Transform.round(m_srcPoly[j].x + m_delta * (m_normals[j].x + m_normals[j].y * dx))
        r2 = Transform.round(m_srcPoly[j].y + m_delta * (m_normals[j].y - m_normals[j].x * dx))
        m_destPoly.add(IntPoint{r1, r2})
    }

    method doMiter(j, k: s32, r: f64)
    {
        q := m_delta / r
        r1 := Transform.round(m_srcPoly[j].x + (m_normals[k].x + m_normals[j].x) * q)
        r2 := Transform.round(m_srcPoly[j].y + (m_normals[k].y + m_normals[j].y) * q)
        m_destPoly.add(IntPoint{r1, r2})
    }

    method doRound(j, k: s32)
    {
        a := Math.atan2(m_sinA, m_normals[k].x * m_normals[j].x + m_normals[k].y * m_normals[j].y)
        steps := Math.max(cast(s32) Transform.round(m_StepsPerRad * Math.abs(a)), 1)

        x := m_normals[k].x
        y := m_normals[k].y
        var X2: f64
        loop i: steps
        {
            r1 := Transform.round(m_srcPoly[j].x + x * m_delta)
            r2 := Transform.round(m_srcPoly[j].y + y * m_delta)
            m_destPoly.add(IntPoint{r1, r2})
            X2 = x
            x = x * m_cos - m_sin * y
            y = X2 * m_sin + y * m_cos
        }

        r1 := Transform.round(m_srcPoly[j].x + m_normals[j].x * m_delta)
        r2 := Transform.round(m_srcPoly[j].y + m_normals[j].y * m_delta)
        m_destPoly.add(IntPoint{r1, r2})
    }
}