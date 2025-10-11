using Core, Core.Math

public impl Painter
{
    #[Swag.Overload]
    internal mtd fillPath(t: *Poly2Tri.Tessellate, p: *LinePath, bvMin, bvMax: Vector2, brush: Brush)
    {
        .setTexture0(bvMin, bvMax, brush)
        .resetTextureFont()

        // Triangulation
        if p.triangles.triIndices.count == 0
        {
            t.triangulate()
            p.triangles = t.getResult()
            t.clear()
        }

        // Paint
        foreach i in p.triangles.triIndices
        {
            let tri  = p.triangles.triangles[i]
            let prim = .pushTriangle(tri.points[0].v, tri.points[1].v, tri.points[2].v, brush.color)
            if .curState.paintParams & .Antialiased
            {
                var e = tri.points[0].border[0]
                if e do
                    .addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)
                e = tri.points[0].border[1]
                if e do
                    .addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)

                e = tri.points[1].border[0]
                if e do
                    .addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)
                e = tri.points[1].border[1]
                if e do
                    .addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)

                e = tri.points[2].border[0]
                if e do
                    .addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)
                e = tri.points[2].border[1]
                if e do
                    .addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)
            }
        }
    }

    // Fill a list of paths.
    #[Swag.Overload]
    mtd fillPath(pathList: *LinePathList, color: Color)
    {
        if !pathList.paths.count do
            return

        let brush = Brush.createSolid(color)
        .fillPath(pathList, brush)
    }

    #[Swag.Overload]
    mtd fillPath(path: *LinePath, brush: Brush)
    {
        path.flatten(.curState.paintQuality)
        if Math.isZeroEpsilon(path.polygon.totalLen) do
            return

        let polygon = &path.polygon

        // Convex shape : faster
        if polygon.isConvex
        {
            .setTexture0(path.bvMin, path.bvMax, brush)
            .resetTextureFont()

            let count = path.polygon.points.count - 1

            // Convex shape : 1 triangle
            if count == 3
            {
                let v0   = polygon.getPoint(0)
                let v1   = polygon.getPoint(1)
                let v2   = polygon.getPoint(2)
                let prim = .pushTriangle(v0, v1, v2, brush.color)
                if .curState.paintParams & .Antialiased
                {
                    .addEdgeAA(prim, v0, v1)
                    .addEdgeAA(prim, v1, v2)
                    .addEdgeAA(prim, v2, v0)
                }
            }

            // Convex shape : 2 triangles
            elif count == 4
            {
                let v0    = polygon.getPoint(0)
                let v1    = polygon.getPoint(1)
                let v2    = polygon.getPoint(2)
                let v3    = polygon.getPoint(3)
                let prim0 = .pushTriangle(v0, v1, v2, brush.color)
                let prim1 = .pushTriangle(v0, v2, v3, brush.color)
                if .curState.paintParams & .Antialiased
                {
                    .addEdgeAA(prim0, v0, v1)
                    .addEdgeAA(prim0, v1, v2)
                    .addEdgeAA(prim0, v2, v3)
                    .addEdgeAA(prim0, v3, v0)
                    .addEdgeAA(prim1, v0, v1)
                    .addEdgeAA(prim1, v1, v2)
                    .addEdgeAA(prim1, v2, v3)
                    .addEdgeAA(prim1, v3, v0)
                }
            }

            // Convex shape: add a triangle from the center of the shape to each pair of
            // vertices
            else
            {
                let center = Vector2{path.bvMin.x + (path.bvMax.x - path.bvMin.x) * 0.5, path.bvMin.y + (path.bvMax.y - path.bvMin.y) * 0.5}
                for i in count
                {
                    let v0   = polygon.getPoint(i)
                    let v1   = polygon.getPoint((i + 1) % count)
                    let prim = .pushTriangle(center, v0, v1, brush.color)

                    if .curState.paintParams & .Antialiased
                    {
                        let v2 = polygon.getPoint((i + 2) % count)
                        let vp = polygon.getPoint((i + count - 1) % count)
                        .addEdgeAA(prim, v0, v1)
                        .addEdgeAA(prim, v1, v2)
                        .addEdgeAA(prim, vp, v0)
                    }
                }
            }
        }

        // Non convex shape: complex painting (triangulation)
        else
        {
            .sharedLinePathList.paths.add(path)
            .fillPath(&.sharedLinePathList, brush)
            .sharedLinePathList.paths.count = 0
        }
    }

    #[Swag.Overload]
    mtd fillPath(path: *LinePath, color: Color)
    {
        let brush = Brush.createSolid(color)
        .fillPath(path, brush)
    }

    // Fill a list of paths.
    // If the path is clockwise, it will be filled. If the path is anti-clockwise, it will be considered as a hole.
    #[Swag.Overload]
    mtd fillPath(pathList: *LinePathList, brush: Brush)
    {
        // Flatten all paths
        pathList.flatten(.curState.paintQuality)

        var flush: *LinePath

        // Everything in the cache ?
        var serial = 0
        var dirty  = false
        foreach p in pathList.paths
        {
            if (p.polygon.isClockwise or serial != p.serial) and flush
            {
                if !flush.triangles.triIndices.count
                {
                    dirty = true
                    break
                }

                serial = p.serial
                flush  = null
            }

            if p.polygon.points.count >= 3
            {
                flush = p
            }
        }

        // If everything has been computed, then just paint them (no need to generate polylines as 'triangulate' will not use them)
        if flush and !flush.triangles.triIndices.count do
            dirty = true

        serial = 0
        flush  = null

        foreach p in pathList.paths
        {
            if (p.polygon.isClockwise or serial != p.serial) and flush
            {
                serial = p.serial
                .fillPath(dirty ? &.tess : null, flush, pathList.bvMin, pathList.bvMax, brush)
                flush = null
            }

            if p.polygon.points.count >= 3
            {
                flush = p
                if dirty do
                    .tess.addPolyLine(p.polygon)
            }
        }

        if flush do
            .fillPath(dirty ? &.tess : null, flush, pathList.bvMin, pathList.bvMax, brush)
    }

    // Fill the polygon with the given color
    #[Swag.Overload]
    mtd fillPolygon(path: *LinePath, color: Color)
    {
        if color.a == 0 do
            return

        let brush = Brush.createSolid(color)
        .fillPolygon(path, brush)
    }

    // Fill the polygon with the given 'brush'
    #[Swag.Overload]
    mtd fillPolygon(path: *LinePath, brush: Brush)
    {
        if brush.color.a == 0 do
            return

        path.flatten(.curState.paintQuality)
        if path.polygon.isConvex
        {
            .fillPath(path, brush)
        }
        else
        {
            path.polygon.isClockwise = true
            let res = path.polygon.getClean()
            foreach p in res.paths do
                .fillPath(p, brush)
        }
    }

    // Fill the polygon with the given 'brush'
    #[Swag.Overload]
    mtd fillPolygon(points: [..] Math.Vector2, brush: Brush)
    {
        if @countof(points) < 3 do
            return

        var path: LinePath
        path.start(points[0].x, points[0].y)
        foreach pt in points where @index do
            path.lineTo(pt.x, pt.y)
        path.close()
        .fillPath(&path, brush)
    }
}
