/*
    :LICENCE
    This code is based on the Clipper library (http://www.angusj.com)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Clipper
using Core

const DefaultArcTolerance = 0.25'f64 // Default tolerance for arc approximation in polygon offsetting

// Used internally for high precision geometric calculations during offsetting operations.
struct DoublePoint { x, y: f64 }

// Polygon offset processor for creating parallel curves and outlines.
// Handles the creation of offset polygons with different join types and miter limits.
struct Offset
{
    miterLimit:       f64 = 2                       // Maximum ratio of miter length to offset distance before square join is used
    arcTolerance:     f64 = DefaultArcTolerance     // Maximum allowed distance between true arc and approximating line segments
    destPolys:        ClipperPaths                  // Output collection of offset polygons
    srcPoly:          ClipperPath                   // Current source polygon being processed
    destPoly:         ClipperPath                   // Current destination polygon being built
    normals:          Array'DoublePoint             // Unit normal vectors for each edge of the source polygon
    deltaVal:         f64                           // Current offset distance (positive for outward, negative for inward)
    sinaVal:          f64                           // Sine of angle between adjacent edge normals
    sinVal:           f64                           // Sine value for arc step calculations
    cosVal:           f64                           // Cosine value for arc step calculations
    miterLim:         f64                           // Computed miter limit threshold for join type switching
    stepsPerRad:      f64                           // Number of steps per radian for arc approximation
    lowest:           IntPoint = {-1, 0}            // Index of polygon and vertex with lowest Y coordinate (leftmost if tied)
    polyNodes:        PolyNode                      // Tree structure holding all input polygons with their join types
}

impl Offset
{
    // Clean up resources when the offset processor is destroyed.
    // Ensures all dynamically allocated polygon nodes are properly freed.
    mtd opDrop()
    {
        me.clear()
    }

    // Clear all stored polygons and reset the processor state.
    // Deallocates all polygon nodes and resets the lowest point tracking.
    mtd clear()
    {
        foreach node in me.polyNodes.childs:
            Memory.delete(node)
        me.polyNodes.childs.clear()
        me.lowest.x = -1
    }

    // Add a polygon path to be processed with the specified join type.
    // Removes duplicate points and tracks the bottommost point for orientation determination.
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
                let kC = newNode.contour[k]
                if path[i].y > kC.y or (path[i].y == kC.y and path[i].x < kC.x):
                    k = j
            }
        }

        if j < 2
        {
            Memory.delete(newNode)
            return
        }

        me.polyNodes.addChild(newNode)

        if me.lowest.x < 0:
            me.lowest = IntPoint{me.polyNodes.childs.count - 1, k}
        else
        {
            let ip = me.polyNodes.childs[cast(s32) me.lowest.x].contour[cast(s32) me.lowest.y]
            if newNode.contour[k].y > ip.y or (newNode.contour[k].y == ip.y and newNode.contour[k].x < ip.x):
                me.lowest = IntPoint{me.polyNodes.childs.count - 1, k}
        }
    }

    // Execute the offset operation and return the resulting polygons.
    // Processes all added paths with the specified 'delta' offset distance.
    mtd execute(solution: *ClipperPaths, delta: f64, reverseNeg: bool = false)
    {
        solution.clear()
        me.fixOrientations()
        me.doOffset(delta)

        var clpr: Transform
        foreach &p in me.destPolys:
            clpr.addPath(p, PolyType.Subject)

        if delta > 0
        {
            clpr.execute(ClipType.Union, solution, PolyFillType.Positive, PolyFillType.Positive)
        }
        else
        {
            let r      = clpr.getBounds()
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

    // Calculate the signed area of a polygon.
    // Returns positive area for counter-clockwise polygons, negative for clockwise.
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
            let pI = poly[i]
            a += (cast(f64) pJ.x + pI.x) * (cast(f64) pJ.y - pI.y)
            j  = i
            pJ = pI
        }

        return -a * 0.5
    }

    // Determine the orientation of a polygon based on its signed area.
    // Returns true for counter-clockwise (positive area), false for clockwise.
    #[Swag.Inline]
    func orientation(poly: ClipperPath)->bool
    {
        return area(poly) >= 0
    }

    // Ensure all polygons have consistent orientation for proper offsetting.
    // Reverses polygon vertex order if the bottommost polygon has incorrect orientation.
    mtd fixOrientations()
    {
        if me.lowest.x >= 0 and !orientation(me.polyNodes.childs[me.lowest.x].contour)
        {
            foreach node in me.polyNodes.childs:
                Slice.reverse(node.contour.toSlice())
        }
    }

    // Calculate the unit normal vector perpendicular to the line segment from 'pt1' to 'pt2'.
    // The normal points to the left of the line direction (counter-clockwise 90° rotation).
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

    // Round a floating-point value to the nearest integer for coordinate conversion.
    // Used to convert double precision calculations back to integer coordinates.
    #[Swag.Inline]
    func cround(val: f64) => cast(CInt) Math.round(val)

    // Perform the actual offset operation on all added polygons.
    // Calculates offset parameters and processes each polygon with appropriate join handling.
    mtd doOffset(delta: f64)
    {
        using Math

        me.destPolys.clear()
        me.deltaVal = delta

        if Math.isZeroEpsilon(delta)
        {
            me.destPolys.reserve(me.polyNodes.childs.count)
            foreach node in me.polyNodes.childs:
                me.destPolys.add(node.contour)
            return
        }

        if me.miterLimit > 2:
            me.miterLim = 2 / (me.miterLimit * me.miterLimit)
        else:
            me.miterLim = 0.5

        var arcT: f64 = undefined
        if me.arcTolerance <= 0.0:
            arcT = DefaultArcTolerance
        elif me.arcTolerance > abs(delta) * DefaultArcTolerance:
            arcT = abs(delta) * DefaultArcTolerance
        else:
            arcT = me.arcTolerance

        const Pi    = ConstF64.Pi
        const TwoPi = ConstF64.TwoPi
        var steps   = Pi / acos(1 - arcT / abs(delta))
        if steps > abs(delta) * Pi:
            steps = abs(delta) * Pi
        me.sinVal      = sin(TwoPi / steps)
        me.cosVal      = cos(TwoPi / steps)
        me.stepsPerRad = steps / TwoPi
        if delta < 0.0:
            me.sinVal = -me.sinVal

        me.destPolys.reserve(me.polyNodes.childs.count * 2)
        foreach node, i in me.polyNodes.childs
        {
            me.srcPoly = node.contour

            let len = cast(s32) me.srcPoly.count
            if len == 0 or (delta <= 0 and len < 3):
                continue

            me.destPoly.clear()
            if len == 1
            {
                if node.jointype == .Round
                {
                    var x = 1.0'f64
                    var y = 0.0'f64
                    for var j = 1; j <= steps; j += 1
                    {
                        let r1 = cround(me.srcPoly[0].x + x * delta)
                        let r2 = cround(me.srcPoly[0].y + y * delta)
                        me.destPoly.add(IntPoint{r1, r2})
                        let x2 = x
                        x = x * me.cosVal - me.sinVal * y
                        y = x2 * me.sinVal + y * me.cosVal
                    }
                }
                else
                {
                    var x = -1.0'f64
                    var y = -1.0'f64
                    for j in 4
                    {
                        let r1 = cround(me.srcPoly[0].x + x * delta)
                        let r2 = cround(me.srcPoly[0].y + y * delta)
                        me.destPoly.add(IntPoint{r1, r2})
                        if x < 0:
                            x = 1
                        elif y < 0:
                            y = 1
                        else:
                            x = -1
                    }
                }

                me.destPolys.add(me.destPoly)
                continue
            }

            // Build normals
            me.normals.clear()
            me.normals.reserve(cast(u64) len)
            for j in len - 1:
                me.normals.add(getUnitNormal(me.srcPoly[j], me.srcPoly[j + 1]))
            me.normals.add(getUnitNormal(me.srcPoly[len - 1], me.srcPoly[0]))

            var k = len - 1
            for j in len:
                me.offsetPoint(j, &k, node.jointype)
            me.destPolys.add(me.destPoly)
        }
    }

    // Process a single vertex offset with appropriate join handling.
    // Determines the offset position based on adjacent edge normals and join type.
    mtd offsetPoint(j: s32, ptrk: *s32, jointype: JoinType)
    {
        let k = dref ptrk

        let kN = me.normals[k]
        let jN = me.normals[j]
        let jP = me.srcPoly[j]

        me.sinaVal = (kN.x * jN.y - jN.x * kN.y)
        if Math.abs(me.sinaVal * me.deltaVal) < 1.0
        {
            let cosA = (kN.x * jN.x + jN.y * kN.y)
            if cosA > 0
            {
                let r1 = cround(jP.x + kN.x * me.deltaVal)
                let r2 = cround(jP.y + kN.y * me.deltaVal)
                me.destPoly.add(IntPoint{r1, r2})
                return
            }
        }
        elif me.sinaVal > 1.0:
            me.sinaVal = 1.0
        elif me.sinaVal < -1.0:
            me.sinaVal = -1.0

        if me.sinaVal * me.deltaVal < 0
        {
            var r1 = cround(jP.x + kN.x * me.deltaVal)
            var r2 = cround(jP.y + kN.y * me.deltaVal)
            me.destPoly.add(IntPoint{r1, r2})
            me.destPoly.add(jP)
            r1 = cround(jP.x + jN.x * me.deltaVal)
            r2 = cround(jP.y + jN.y * me.deltaVal)
            me.destPoly.add(IntPoint{r1, r2})
        }
        else
        {
            switch jointype
            {
            case Miter:
                let r = 1 + (jN.x * kN.x + jN.y * kN.y)
                if r >= me.miterLim:
                    me.doMiter(j, k, r)
                else:
                    me.doSquare(j, k)
            case Square:
                me.doSquare(j, k)
            case Round:
                me.doRound(j, k)
            }
        }

        dref ptrk = j
    }

    // Create a square join between two offset edges.
    // Extends the offset lines and connects them with a perpendicular segment.
    mtd doSquare(j, k: s32)
    {
        let kN = me.normals[k]
        let jN = me.normals[j]
        let jP = me.srcPoly[j]

        let dx = Math.tan(Math.atan2(me.sinaVal, kN.x * jN.x + kN.y * jN.y) / 4)
        var r1 = cround(jP.x + me.deltaVal * (kN.x - kN.y * dx))
        var r2 = cround(jP.y + me.deltaVal * (kN.y + kN.x * dx))
        me.destPoly.add(IntPoint{r1, r2})

        r1 = cround(jP.x + me.deltaVal * (jN.x + jN.y * dx))
        r2 = cround(jP.y + me.deltaVal * (jN.y - jN.x * dx))
        me.destPoly.add(IntPoint{r1, r2})
    }

    // Create a miter join by extending offset edges until they intersect.
    // Used when the miter length is within acceptable limits.
    mtd doMiter(j, k: s32, r: f64)
    {
        let kN = me.normals[k]
        let jN = me.normals[j]
        let jP = me.srcPoly[j]

        let q  = me.deltaVal / r
        let r1 = cround(jP.x + (kN.x + jN.x) * q)
        let r2 = cround(jP.y + (kN.y + jN.y) * q)
        me.destPoly.add(IntPoint{r1, r2})
    }

    // Create a rounded join by approximating a circular arc.
    // Connects offset edges with line segments that approximate the curve.
    mtd doRound(j, k: s32)
    {
        let kN = me.normals[k]
        let jN = me.normals[j]
        let jP = me.srcPoly[j]

        let a     = Math.atan2(me.sinaVal, kN.x * jN.x + kN.y * jN.y)
        let steps = Math.max(cast(s32) cround(me.stepsPerRad * Math.abs(a)), 1)

        var x = kN.x
        var y = kN.y
        for i in steps
        {
            let r1 = cround(jP.x + x * me.deltaVal)
            let r2 = cround(jP.y + y * me.deltaVal)
            me.destPoly.add(IntPoint{r1, r2})
            let x2 = x
            x = x * me.cosVal - me.sinVal * y
            y = x2 * me.sinVal + y * me.cosVal
        }

        let r1 = cround(jP.x + jN.x * me.deltaVal)
        let r2 = cround(jP.y + jN.y * me.deltaVal)
        me.destPoly.add(IntPoint{r1, r2})
    }
}
