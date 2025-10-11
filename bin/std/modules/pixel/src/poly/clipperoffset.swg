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
        .clear()
    }

    // Clear all stored polygons and reset the processor state.
    // Deallocates all polygon nodes and resets the lowest point tracking.
    mtd clear()
    {
        foreach node in .polyNodes.childs do
            Memory.delete(node)
        .polyNodes.childs.clear()
        .lowest.x = -1
    }

    // Add a polygon path to be processed with the specified join type.
    // Removes duplicate points and tracks the bottommost point for orientation determination.
    mtd addPath(path: ClipperPath, joinType: JoinType)
    {
        var highI = path.count - 1
        if highI < 0 do
            return
        while highI > 0 and path.buffer[0] == path.buffer[highI] do
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
                if path[i].y > kC.y or (path[i].y == kC.y and path[i].x < kC.x) do
                    k = j
            }
        }

        if j < 2
        {
            Memory.delete(newNode)
            return
        }

        .polyNodes.addChild(newNode)

        if .lowest.x < 0 do
            .lowest = IntPoint{.polyNodes.childs.count - 1, k}
        else
        {
            let ip = .polyNodes.childs[cast(s32) .lowest.x].contour[cast(s32) .lowest.y]
            if newNode.contour[k].y > ip.y or (newNode.contour[k].y == ip.y and newNode.contour[k].x < ip.x) do
                .lowest = IntPoint{.polyNodes.childs.count - 1, k}
        }
    }

    // Execute the offset operation and return the resulting polygons.
    // Processes all added paths with the specified 'delta' offset distance.
    mtd execute(solution: *ClipperPaths, delta: f64, reverseNeg: bool = false)
    {
        solution.clear()
        .fixOrientations()
        .doOffset(delta)

        var clpr: Transform
        foreach &p in .destPolys do
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
            if solution.count > 0 do
                solution.removeAt(0)
        }
    }

    // Calculate the signed area of a polygon.
    // Returns positive area for counter-clockwise polygons, negative for clockwise.
    func area(poly: ClipperPath)->f64
    {
        let size = cast(s32) poly.count
        if size < 3 do
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
        if .lowest.x >= 0 and !orientation(.polyNodes.childs[.lowest.x].contour)
        {
            foreach node in .polyNodes.childs do
                Slice.reverse(node.contour.toSlice())
        }
    }

    // Calculate the unit normal vector perpendicular to the line segment from 'pt1' to 'pt2'.
    // The normal points to the left of the line direction (counter-clockwise 90° rotation).
    func getUnitNormal(pt1, pt2: const &IntPoint)->DoublePoint
    {
        var result: retval
        if pt2.x == pt1.x and pt2.y == pt1.y do
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

        .destPolys.clear()
        .deltaVal = delta

        if Math.isZeroEpsilon(delta)
        {
            .destPolys.reserve(.polyNodes.childs.count)
            foreach node in .polyNodes.childs do
                .destPolys.add(node.contour)
            return
        }

        if .miterLimit > 2 do
            .miterLim = 2 / (.miterLimit * .miterLimit)
        else do
            .miterLim = 0.5

        var arcT: f64 = undefined
        if .arcTolerance <= 0.0 do
            arcT = DefaultArcTolerance
        elif .arcTolerance > abs(delta) * DefaultArcTolerance do
            arcT = abs(delta) * DefaultArcTolerance
        else do
            arcT = .arcTolerance

        const Pi    = ConstF64.Pi
        const TwoPi = ConstF64.TwoPi
        var steps   = Pi / acos(1 - arcT / abs(delta))
        if steps > abs(delta) * Pi do
            steps = abs(delta) * Pi
        .sinVal      = sin(TwoPi / steps)
        .cosVal      = cos(TwoPi / steps)
        .stepsPerRad = steps / TwoPi
        if delta < 0.0 do
            .sinVal = -.sinVal

        .destPolys.reserve(.polyNodes.childs.count * 2)
        foreach node, i in .polyNodes.childs
        {
            .srcPoly = node.contour

            let len = cast(s32) .srcPoly.count
            if len == 0 or (delta <= 0 and len < 3) do
                continue

            .destPoly.clear()
            if len == 1
            {
                if node.jointype == .Round
                {
                    var x = 1.0'f64
                    var y = 0.0'f64
                    for var j = 1; j <= steps; j += 1
                    {
                        let r1 = cround(.srcPoly[0].x + x * delta)
                        let r2 = cround(.srcPoly[0].y + y * delta)
                        .destPoly.add(IntPoint{r1, r2})
                        let x2 = x
                        x = x * .cosVal - .sinVal * y
                        y = x2 * .sinVal + y * .cosVal
                    }
                }
                else
                {
                    var x = -1.0'f64
                    var y = -1.0'f64
                    for j in 4
                    {
                        let r1 = cround(.srcPoly[0].x + x * delta)
                        let r2 = cround(.srcPoly[0].y + y * delta)
                        .destPoly.add(IntPoint{r1, r2})
                        if x < 0 do
                            x = 1
                        elif y < 0 do
                            y = 1
                        else do
                            x = -1
                    }
                }

                .destPolys.add(.destPoly)
                continue
            }

            // Build normals
            .normals.clear()
            .normals.reserve(cast(u64) len)
            for j in len - 1 do
                .normals.add(getUnitNormal(.srcPoly[j], .srcPoly[j + 1]))
            .normals.add(getUnitNormal(.srcPoly[len - 1], .srcPoly[0]))

            var k = len - 1
            for j in len do
                .offsetPoint(j, &k, node.jointype)
            .destPolys.add(.destPoly)
        }
    }

    // Process a single vertex offset with appropriate join handling.
    // Determines the offset position based on adjacent edge normals and join type.
    mtd offsetPoint(j: s32, ptrk: *s32, jointype: JoinType)
    {
        let k = dref ptrk

        let kN = .normals[k]
        let jN = .normals[j]
        let jP = .srcPoly[j]

        .sinaVal = (kN.x * jN.y - jN.x * kN.y)
        if Math.abs(.sinaVal * .deltaVal) < 1.0
        {
            let cosA = (kN.x * jN.x + jN.y * kN.y)
            if cosA > 0
            {
                let r1 = cround(jP.x + kN.x * .deltaVal)
                let r2 = cround(jP.y + kN.y * .deltaVal)
                .destPoly.add(IntPoint{r1, r2})
                return
            }
        }
        elif .sinaVal > 1.0 do
            .sinaVal = 1.0
        elif .sinaVal < -1.0 do
            .sinaVal = -1.0

        if .sinaVal * .deltaVal < 0
        {
            var r1 = cround(jP.x + kN.x * .deltaVal)
            var r2 = cround(jP.y + kN.y * .deltaVal)
            .destPoly.add(IntPoint{r1, r2})
            .destPoly.add(jP)
            r1 = cround(jP.x + jN.x * .deltaVal)
            r2 = cround(jP.y + jN.y * .deltaVal)
            .destPoly.add(IntPoint{r1, r2})
        }
        else
        {
            switch jointype
            {
            case Miter:
                let r = 1 + (jN.x * kN.x + jN.y * kN.y)
                if r >= .miterLim do
                    .doMiter(j, k, r)
                else do
                    .doSquare(j, k)
            case Square:
                .doSquare(j, k)
            case Round:
                .doRound(j, k)
            }
        }

        dref ptrk = j
    }

    // Create a square join between two offset edges.
    // Extends the offset lines and connects them with a perpendicular segment.
    mtd doSquare(j, k: s32)
    {
        let kN = .normals[k]
        let jN = .normals[j]
        let jP = .srcPoly[j]

        let dx = Math.tan(Math.atan2(.sinaVal, kN.x * jN.x + kN.y * jN.y) / 4)
        var r1 = cround(jP.x + .deltaVal * (kN.x - kN.y * dx))
        var r2 = cround(jP.y + .deltaVal * (kN.y + kN.x * dx))
        .destPoly.add(IntPoint{r1, r2})

        r1 = cround(jP.x + .deltaVal * (jN.x + jN.y * dx))
        r2 = cround(jP.y + .deltaVal * (jN.y - jN.x * dx))
        .destPoly.add(IntPoint{r1, r2})
    }

    // Create a miter join by extending offset edges until they intersect.
    // Used when the miter length is within acceptable limits.
    mtd doMiter(j, k: s32, r: f64)
    {
        let kN = .normals[k]
        let jN = .normals[j]
        let jP = .srcPoly[j]

        let q  = .deltaVal / r
        let r1 = cround(jP.x + (kN.x + jN.x) * q)
        let r2 = cround(jP.y + (kN.y + jN.y) * q)
        .destPoly.add(IntPoint{r1, r2})
    }

    // Create a rounded join by approximating a circular arc.
    // Connects offset edges with line segments that approximate the curve.
    mtd doRound(j, k: s32)
    {
        let kN = .normals[k]
        let jN = .normals[j]
        let jP = .srcPoly[j]

        let a     = Math.atan2(.sinaVal, kN.x * jN.x + kN.y * jN.y)
        let steps = Math.max(cast(s32) cround(.stepsPerRad * Math.abs(a)), 1)

        var x = kN.x
        var y = kN.y
        for i in steps
        {
            let r1 = cround(jP.x + x * .deltaVal)
            let r2 = cround(jP.y + y * .deltaVal)
            .destPoly.add(IntPoint{r1, r2})
            let x2 = x
            x = x * .cosVal - .sinVal * y
            y = x2 * .sinVal + y * .cosVal
        }

        let r1 = cround(jP.x + jN.x * .deltaVal)
        let r2 = cround(jP.y + jN.y * .deltaVal)
        .destPoly.add(IntPoint{r1, r2})
    }
}
