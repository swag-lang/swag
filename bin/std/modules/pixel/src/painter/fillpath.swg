using Core, Math

public impl Painter
{
    // Fill the line path
    mtd fillPath(pathList: *LinePathList, color: Color)
    {
        if !pathList.paths.count
            return
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        fillPath(pathList, sharedSolidBrush)
    }

    // Fill the line path with the given 'brush'
    mtd fillPath(path: *LinePath, brush: Brush)
    {
        path.flatten(curState.paintQuality)
        if Math.isZeroEpsilon(path.polygon.totalLen)
            return

        // Setup texture
        setTexture0(path.bvMin, path.bvMax, brush)
        resetTextureFont()

        // Convex shape: just add a triangle from the center of the shape to each pair of
        // vertices
        polygon := &path.polygon
        if polygon.isConvex
        {
            count := path.polygon.points.count - 1
            center := Vector2{path.bvMin.x + (path.bvMax.x - path.bvMin.x) * 0.5, path.bvMin.y + (path.bvMax.y - path.bvMin.y) * 0.5}
            loop i: count
            {
                v0 := polygon.getPoint(i)
                v1 := polygon.getPoint((i + 1) % count)
                prim := pushTriangle(center, v0, v1, brush.color)

                if curState.paintParams & .Antialiased
                {
                    v2 := polygon.getPoint((i + 2) % count)
                    vp := polygon.getPoint((i + count - 1) % count)
                    addEdgeAA(prim, v0, v1)
                    addEdgeAA(prim, v1, v2)
                    addEdgeAA(prim, vp, v0)
                }
            }
        }

        // Non convex shape: triangulate
        else
        {
            result := triangulate(path)

            pidx := result.buffer
            firstPrim := vertexBuffer.count
            loop result.count / 3
            {
                v0 := polygon.getPoint(cast(uint) pidx[0] + 1)
                v1 := polygon.getPoint(cast(uint) pidx[1] + 1)
                v2 := polygon.getPoint(cast(uint) pidx[2] + 1)
                prim := pushTriangle(v0, v1, v2, brush.color)
                pidx += 3
            }

            // Generate all AA edges : for each vertex of the triangle, add the previous and
            // next edge (even if not in the triangle)
            if curState.paintParams & .Antialiased
            {
                cpt := cast(s32) polygon.points.count - 1
                psrc := result.buffer
                for i := 0; i < result.count; i += 3
                {
                    index  := psrc[0]
                    indexn := (index + 1) % cpt
                    indexp := index ? index - 1 : cpt - 1
                    v  := polygon.getPoint(cast(uint) index + 1)
                    vp := polygon.getPoint(cast(uint) indexp + 1)
                    vn := polygon.getPoint(cast(uint) indexn + 1)
                    addEdgeAA(firstPrim, v, vp)
                    addEdgeAA(firstPrim, v, vn)

                    index  = psrc[1]
                    indexn = (index + 1) % cpt
                    indexp = index ? index - 1 : cpt - 1
                    v  = polygon.getPoint(cast(uint) index + 1)
                    vp = polygon.getPoint(cast(uint) indexp + 1)
                    vn = polygon.getPoint(cast(uint) indexn + 1)
                    addEdgeAA(firstPrim, v, vp)
                    addEdgeAA(firstPrim, v, vn)

                    index  = psrc[2]
                    indexn = (index + 1) % cpt
                    indexp = index ? index - 1 : cpt - 1
                    v  = polygon.getPoint(cast(uint) index + 1)
                    vp = polygon.getPoint(cast(uint) indexp + 1)
                    vn = polygon.getPoint(cast(uint) indexn + 1)
                    addEdgeAA(firstPrim, v, vp)
                    addEdgeAA(firstPrim, v, vn)

                    psrc += 3
                    firstPrim += 3
                }
            }
        }
    }

    // Fill the line path
    mtd fillPath(path: *LinePath, color: Color)
    {
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        fillPath(path, sharedSolidBrush)
    }

    // Fill the list of paths
    // clockwise = fill, anti-clockwise = hole
    mtd fillPath(pathList: *LinePathList, brush: Brush)
    {
        // Flatten all paths
        pathList.flatten(curState.paintQuality)

        var t:      Poly2Tri.Tesselate
        var flush:  *LinePath

        // Everything in the cache ?
        serial := 0
        dirty := false
        visit p: pathList.paths
        {
            if (p.polygon.isClockwise or serial != p.serial) and flush
            {
                if !flush.triangles.count
                {
                    dirty = true
                    break
                }
                serial = p.serial
                flush = null
            }

            if p.polygon.points.count >= 3
            {
                flush = p
            }
        }

        if flush and !flush.triangles.count
            dirty = true

        serial = 0
        flush = null

        // If everything has been computed, then just paint them (no need to generate polylines as 'triangulate' will not use them)
        if !dirty
        {
            visit p: pathList.paths
            {
                if (p.polygon.isClockwise or serial != p.serial) and flush
                {
                    serial = p.serial
                    triangulate(&t, flush, pathList.bvMin, pathList.bvMax, brush)
                    flush = null
                }

                if p.polygon.points.count >= 3
                {
                    flush = p
                }
            }
        }

        // Full compute (triangulation)
        else
        {
            visit p: pathList.paths
            {
                if (p.polygon.isClockwise or serial != p.serial) and flush
                {
                    serial = p.serial
                    triangulate(&t, flush, pathList.bvMin, pathList.bvMax, brush)
                    flush = null
                }

                if p.polygon.points.count >= 3
                {
                    flush = p
                    t.startPolyLine()
                    loop i: 0..p.polygon.points.count-2
                    {
                        pt := p.polygon.getPoint(i)
                        t.addPoint(cast(f64) pt.x, cast(f64) pt.y)
                    }

                    t.endPolyLine()
                }
            }
        }

        if flush
            triangulate(&t, flush, pathList.bvMin, pathList.bvMax, brush)
    }

    // Fill the polygon with the given color
    mtd fillPolygon(path: *LinePath, color: Color)
    {
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        fillPolygon(path, sharedSolidBrush)
    }

    // Fill the polygon with the given 'brush'
    mtd fillPolygon(path: *LinePath, brush: Brush)
    {
        path.flatten(curState.paintQuality)
        if path.polygon.isConvex
        {
            fillPath(path, brush)
        }
        else
        {
            path.polygon.isClockwise = true
            res := path.polygon.getClean()
            visit p: res.paths
                fillPath(p, brush)
        }
    }
}