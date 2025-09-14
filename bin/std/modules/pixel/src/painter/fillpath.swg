using Core, Core.Math

public impl Painter
{
    #[Swag.Overload]
    internal mtd fillPath(t: *Poly2Tri.Tessellate, p: *LinePath, bvMin, bvMax: Vector2, brush: Brush)
    {
        me.setTexture0(bvMin, bvMax, brush)
        me.resetTextureFont()

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
            let prim = me.pushTriangle(tri.points[0].v, tri.points[1].v, tri.points[2].v, brush.color)
            if me.curState.paintParams & .Antialiased
            {
                var e = tri.points[0].border[0]
                if e:
                    me.addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)
                e = tri.points[0].border[1]
                if e:
                    me.addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)

                e = tri.points[1].border[0]
                if e:
                    me.addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)
                e = tri.points[1].border[1]
                if e:
                    me.addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)

                e = tri.points[2].border[0]
                if e:
                    me.addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)
                e = tri.points[2].border[1]
                if e:
                    me.addEdgeAA(prim, e.p.x, e.p.y, e.q.x, e.q.y)
            }
        }
    }

    // Fill a list of paths.
    #[Swag.Overload]
    mtd fillPath(pathList: *LinePathList, color: Color)
    {
        if !pathList.paths.count:
            return
        me.sharedSolidBrush.type  = .SolidColor
        me.sharedSolidBrush.color = color
        me.fillPath(pathList, me.sharedSolidBrush)
    }

    #[Swag.Overload]
    mtd fillPath(path: *LinePath, brush: Brush)
    {
        path.flatten(me.curState.paintQuality)
        if Math.isZeroEpsilon(path.polygon.totalLen):
            return

        let polygon = &path.polygon

        // Convex shape : faster
        if polygon.isConvex
        {
            me.setTexture0(path.bvMin, path.bvMax, brush)
            me.resetTextureFont()

            let count = path.polygon.points.count - 1

            // Convex shape : 1 triangle
            if count == 3
            {
                let v0   = polygon.getPoint(0)
                let v1   = polygon.getPoint(1)
                let v2   = polygon.getPoint(2)
                let prim = me.pushTriangle(v0, v1, v2, brush.color)
                if me.curState.paintParams & .Antialiased
                {
                    me.addEdgeAA(prim, v0, v1)
                    me.addEdgeAA(prim, v1, v2)
                    me.addEdgeAA(prim, v2, v0)
                }
            }

            // Convex shape : 2 triangles
            elif count == 4
            {
                let v0    = polygon.getPoint(0)
                let v1    = polygon.getPoint(1)
                let v2    = polygon.getPoint(2)
                let v3    = polygon.getPoint(3)
                let prim0 = me.pushTriangle(v0, v1, v2, brush.color)
                let prim1 = me.pushTriangle(v0, v2, v3, brush.color)
                if me.curState.paintParams & .Antialiased
                {
                    me.addEdgeAA(prim0, v0, v1)
                    me.addEdgeAA(prim0, v1, v2)
                    me.addEdgeAA(prim0, v2, v3)
                    me.addEdgeAA(prim0, v3, v0)
                    me.addEdgeAA(prim1, v0, v1)
                    me.addEdgeAA(prim1, v1, v2)
                    me.addEdgeAA(prim1, v2, v3)
                    me.addEdgeAA(prim1, v3, v0)
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
                    let prim = me.pushTriangle(center, v0, v1, brush.color)

                    if me.curState.paintParams & .Antialiased
                    {
                        let v2 = polygon.getPoint((i + 2) % count)
                        let vp = polygon.getPoint((i + count - 1) % count)
                        me.addEdgeAA(prim, v0, v1)
                        me.addEdgeAA(prim, v1, v2)
                        me.addEdgeAA(prim, vp, v0)
                    }
                }
            }
        }

        // Non convex shape: complex painting (triangulation)
        else
        {
            me.sharedLinePathList.paths.add(path)
            me.fillPath(&me.sharedLinePathList, brush)
            me.sharedLinePathList.paths.count = 0
        }
    }

    #[Swag.Overload]
    mtd fillPath(path: *LinePath, color: Color)
    {
        me.sharedSolidBrush.type  = .SolidColor
        me.sharedSolidBrush.color = color
        me.fillPath(path, me.sharedSolidBrush)
    }

    // Fill a list of paths.
    // If the path is clockwise, it will be filled. If the path is anti-clockwise, it will be considered as a hole.
    #[Swag.Overload]
    mtd fillPath(pathList: *LinePathList, brush: Brush)
    {
        // Flatten all paths
        pathList.flatten(me.curState.paintQuality)

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
        if flush and !flush.triangles.triIndices.count:
            dirty = true

        serial = 0
        flush  = null

        foreach p in pathList.paths
        {
            if (p.polygon.isClockwise or serial != p.serial) and flush
            {
                serial = p.serial
                me.fillPath(dirty ? &me.tess : null, flush, pathList.bvMin, pathList.bvMax, brush)
                flush = null
            }

            if p.polygon.points.count >= 3
            {
                flush = p
                if dirty:
                    me.tess.addPolyLine(p.polygon)
            }
        }

        if flush:
            me.fillPath(dirty ? &me.tess : null, flush, pathList.bvMin, pathList.bvMax, brush)
    }

    // Fill the polygon with the given color
    #[Swag.Overload]
    mtd fillPolygon(path: *LinePath, color: Color)
    {
        if color.a == 0:
            return

        me.sharedSolidBrush.type  = .SolidColor
        me.sharedSolidBrush.color = color
        me.fillPolygon(path, me.sharedSolidBrush)
    }

    // Fill the polygon with the given 'brush'
    #[Swag.Overload]
    mtd fillPolygon(path: *LinePath, brush: Brush)
    {
        if brush.color.a == 0:
            return

        path.flatten(me.curState.paintQuality)
        if path.polygon.isConvex
        {
            me.fillPath(path, brush)
        }
        else
        {
            path.polygon.isClockwise = true
            let res = path.polygon.getClean()
            foreach p in res.paths:
                me.fillPath(p, brush)
        }
    }

    // Fill the polygon with the given 'brush'
    #[Swag.Overload]
    mtd fillPolygon(points: [..] Math.Vector2, brush: Brush)
    {
        if @countof(points) < 3:
            return

        var path: LinePath
        path.start(points[0].x, points[0].y)
        foreach pt in points where @index:
            path.lineTo(pt.x, pt.y)
        path.close()
        me.fillPath(&path, brush)
    }
}
