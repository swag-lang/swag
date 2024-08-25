/*
    :LICENCE
    This code is based on the Clipper library (http://www.angusj.com)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Clipper
using Core

const DefaultArcTolerance = 0.25'f64

struct DoublePoint
{
    x, y: f64
}

struct Offset
{
    miterLimit:       f64 = 2
    arcTolerance:     f64 = DefaultArcTolerance
    destPolys:        ClipperPaths
    srcPoly:          ClipperPath
    destPoly:         ClipperPath
    normals:          Array'DoublePoint
    deltaVal:         f64
    sinaVal:          f64
    sinVal:           f64
    cosVal:           f64
    miterLim:         f64
    stepsPerRad:      f64
    lowest:           IntPoint = {-1, 0}
    polyNodes:        PolyNode
}

impl Offset
{
    mtd opDrop()
    {
        clear()
    }

    mtd clear()
    {
        visit node in polyNodes.childs:
            Memory.delete(node)
        polyNodes.childs.clear()
        lowest.x = -1
    }

    mtd addPath(path: ClipperPath, joinType: JoinType)
    {
        var highI = path.count - 1
        if highI < 0:
            return
        while highI > 0 and path.buffer[0] == path.buffer[highI]:
            highI -= 1

        let newNode = Memory.new'PolyNode()
        newNode.jointype = joinType
        newNode.contour.reserve(highI + 1)
        newNode.contour.add(path.buffer[0])

        var j, k: s32
        for i in 1 to highI
        {
            if newNode.contour.buffer[j] != path.buffer[i]
            {
                j += 1
                newNode.contour.add(path.buffer[i])
                var kC = newNode.contour[k]
                if path[i].y > kC.y or (path[i].y == kC.y and path[i].x < kC.x):
                    k = j
            }
        }

        if j < 2
        {
            Memory.delete(newNode)
            return
        }

        polyNodes.addChild(newNode)

        if lowest.x < 0:
            lowest = IntPoint{polyNodes.childs.count - 1, k}
        else
        {
            var ip = polyNodes.childs[cast(s32) lowest.x].contour[cast(s32) lowest.y]
            if newNode.contour[k].y > ip.y or (newNode.contour[k].y == ip.y and newNode.contour[k].x < ip.x):
                lowest = IntPoint{polyNodes.childs.count - 1, k}
        }
    }

    mtd execute(solution: *ClipperPaths, delta: f64, reverseNeg: bool = false)
    {
        solution.clear()
        fixOrientations()
        doOffset(delta)

        var clpr: Transform
        visit &p in destPolys:
            clpr.addPath(p, PolyType.Subject)

        if delta > 0
        {
            clpr.execute(ClipType.Union, solution, PolyFillType.Positive, PolyFillType.Positive)
        }
        else
        {
            var r      = clpr.getBounds()
            var outer: ClipperPath
            outer.add(IntPoint{r.left - 10, r.bottom + 10})
            outer.add(IntPoint{r.right + 10, r.bottom + 10})
            outer.add(IntPoint{r.right + 10, r.top - 10})
            outer.add(IntPoint{r.left - 10, r.top - 10})

            clpr.addPath(&outer, PolyType.Subject)
            clpr.reverseOutput = reverseNeg
            clpr.execute(ClipType.Union, solution, PolyFillType.Negative, PolyFillType.Negative)
            if solution.count > 0:
                solution.removeAt(0)
        }
    }

    func area(poly: ClipperPath)->f64
    {
        let size = cast(s32) poly.count
        if size < 3:
            return 0

        var a  = 0'f64
        var j  = size - 1
        var pJ = poly[j]
        for i in size
        {
            var pI = poly[i]
            a += (cast(f64) pJ.x + pI.x) * (cast(f64) pJ.y - pI.y)
            j  = i
            pJ = pI
        }

        return -a * 0.5
    }

    #[Swag.Inline]
    func orientation(poly: ClipperPath)->bool
    {
        return area(poly) >= 0
    }

    mtd fixOrientations()
    {
        if lowest.x >= 0 and !orientation(polyNodes.childs[lowest.x].contour)
        {
            visit node in polyNodes.childs:
                Slice.reverse(node.contour.toSlice())
        }
    }

    func getUnitNormal(pt1, pt2: const &IntPoint)->DoublePoint
    {
        var result: retval
        if pt2.x == pt1.x and pt2.y == pt1.y:
            return result

        var dx = cast(f64) (pt2.x - pt1.x)
        var dy = cast(f64) (pt2.y - pt1.y)
        let f  = 1.0 / Math.sqrt(dx * dx + dy * dy)
        dx *= f
        dy *= f
        result.x = dy
        result.y = -dx
        return result
    }

    #[Swag.Inline]
    func cround(val: f64) => cast(CInt) Math.round(val)

    mtd doOffset(delta: f64)
    {
        using Math

        destPolys.clear()
        deltaVal = delta

        if Math.isZeroEpsilon(delta)
        {
            destPolys.reserve(polyNodes.childs.count)
            visit node in polyNodes.childs:
                destPolys.add(node.contour)
            return
        }

        if miterLimit > 2:
            miterLim = 2 / (miterLimit * miterLimit)
        else:
            miterLim = 0.5

        var arcT: f64 = undefined
        if arcTolerance <= 0.0:
            arcT = DefaultArcTolerance
        elif arcTolerance > abs(delta) * DefaultArcTolerance:
            arcT = abs(delta) * DefaultArcTolerance
        else:
            arcT = arcTolerance

        const Pi    = ConstF64.Pi
        const TwoPi = ConstF64.TwoPi
        var steps   = Pi / acos(1 - arcT / abs(delta))
        if steps > abs(delta) * Pi:
            steps = abs(delta) * Pi
        sinVal      = sin(TwoPi / steps)
        cosVal      = cos(TwoPi / steps)
        stepsPerRad = steps / TwoPi
        if delta < 0.0:
            sinVal = -sinVal

        destPolys.reserve(polyNodes.childs.count * 2)
        visit node, i in polyNodes.childs
        {
            srcPoly = node.contour

            let len = cast(s32) srcPoly.count
            if len == 0 or (delta <= 0 and len < 3):
                continue

            destPoly.clear()
            if len == 1
            {
                if node.jointype == .Round
                {
                    var x = 1.0'f64
                    var y = 0.0'f64
                    for var j = 1; j <= steps; j += 1
                    {
                        let r1 = cround(srcPoly[0].x + x * delta)
                        let r2 = cround(srcPoly[0].y + y * delta)
                        destPoly.add(IntPoint{r1, r2})
                        let x2 = x
                        x = x * cosVal - sinVal * y
                        y = x2 * sinVal + y * cosVal
                    }
                }
                else
                {
                    var x = -1.0'f64
                    var y = -1.0'f64
                    for j in 4
                    {
                        let r1 = cround(srcPoly[0].x + x * delta)
                        let r2 = cround(srcPoly[0].y + y * delta)
                        destPoly.add(IntPoint{r1, r2})
                        if x < 0:
                            x = 1
                        elif y < 0:
                            y = 1
                        else:
                            x = -1
                    }
                }

                destPolys.add(destPoly)
                continue
            }

            // Build normals
            normals.clear()
            normals.reserve(cast(u64) len)
            for j in len - 1:
                normals.add(getUnitNormal(srcPoly[j], srcPoly[j + 1]))
            normals.add(getUnitNormal(srcPoly[len - 1], srcPoly[0]))

            var k = len - 1
            for j in len:
                offsetPoint(j, &k, node.jointype)
            destPolys.add(destPoly)
        }
    }

    mtd offsetPoint(j: s32, ptrk: *s32, jointype: JoinType)
    {
        let k = dref ptrk

        var kN = normals[k]
        var jN = normals[j]
        var jP = srcPoly[j]

        sinaVal = (kN.x * jN.y - jN.x * kN.y)
        if Math.abs(sinaVal * deltaVal) < 1.0
        {
            let cosA = (kN.x * jN.x + jN.y * kN.y)
            if cosA > 0
            {
                let r1 = cround(jP.x + kN.x * deltaVal)
                let r2 = cround(jP.y + kN.y * deltaVal)
                destPoly.add(IntPoint{r1, r2})
                return
            }
        }
        elif sinaVal > 1.0:
            sinaVal = 1.0
        elif sinaVal < -1.0:
            sinaVal = -1.0

        if sinaVal * deltaVal < 0
        {
            var r1 = cround(jP.x + kN.x * deltaVal)
            var r2 = cround(jP.y + kN.y * deltaVal)
            destPoly.add(IntPoint{r1, r2})
            destPoly.add(jP)
            r1 = cround(jP.x + jN.x * deltaVal)
            r2 = cround(jP.y + jN.y * deltaVal)
            destPoly.add(IntPoint{r1, r2})
        }
        else
        {
            switch jointype
            {
            case Miter:
                let r = 1 + (jN.x * kN.x + jN.y * kN.y)
                if r >= miterLim:
                    doMiter(j, k, r)
                else:
                    doSquare(j, k)
            case Square:
                doSquare(j, k)
            case Round:
                doRound(j, k)
            }
        }

        dref ptrk = j
    }

    mtd doSquare(j, k: s32)
    {
        let kN = normals[k]
        let jN = normals[j]
        let jP = srcPoly[j]

        let dx = Math.tan(Math.atan2(sinaVal, kN.x * jN.x + kN.y * jN.y) / 4)
        var r1 = cround(jP.x + deltaVal * (kN.x - kN.y * dx))
        var r2 = cround(jP.y + deltaVal * (kN.y + kN.x * dx))
        destPoly.add(IntPoint{r1, r2})

        r1 = cround(jP.x + deltaVal * (jN.x + jN.y * dx))
        r2 = cround(jP.y + deltaVal * (jN.y - jN.x * dx))
        destPoly.add(IntPoint{r1, r2})
    }

    mtd doMiter(j, k: s32, r: f64)
    {
        let kN = normals[k]
        let jN = normals[j]
        let jP = srcPoly[j]

        let q  = deltaVal / r
        let r1 = cround(jP.x + (kN.x + jN.x) * q)
        let r2 = cround(jP.y + (kN.y + jN.y) * q)
        destPoly.add(IntPoint{r1, r2})
    }

    mtd doRound(j, k: s32)
    {
        var kN = normals[k]
        var jN = normals[j]
        var jP = srcPoly[j]

        let a     = Math.atan2(sinaVal, kN.x * jN.x + kN.y * jN.y)
        let steps = Math.max(cast(s32) cround(stepsPerRad * Math.abs(a)), 1)

        var x = kN.x
        var y = kN.y
        for i in steps
        {
            let r1 = cround(jP.x + x * deltaVal)
            let r2 = cround(jP.y + y * deltaVal)
            destPoly.add(IntPoint{r1, r2})
            let x2 = x
            x = x * cosVal - sinVal * y
            y = x2 * sinVal + y * cosVal
        }

        let r1 = cround(jP.x + jN.x * deltaVal)
        let r2 = cround(jP.y + jN.y * deltaVal)
        destPoly.add(IntPoint{r1, r2})
    }
}
