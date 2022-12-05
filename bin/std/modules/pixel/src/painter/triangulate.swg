using Core, Math

impl Painter
{
    func triangulateSnip(border: const [..] Vector2, u, v, w, n: s32, pv: ^s32)->bool
    {
        a := border[pv[u]]
        b := border[pv[v]]
        c := border[pv[w]]

        // Cross product to detect if the triangle is in the wrong direction
        val := ((b.x - a.x) * (c.y - a.y)) - ((b.y - a.y) * (c.x - a.x))
        if val < Math.ConstF32.Epsilon
            return false

        // Detect if a point of the graph is inside that triangle
        for p := 0; p < n; p += 1
        {
            if (p == u) or (p == v) or (p == w)
                continue
            pt := border[pv[p]]
            if pt.isInTriangle(a, b, c)
                return false
        }

        return true
    }

    mtd triangulate(path: *LinePath)->Array's32
    {
        var result: retval

        slice := @mkslice(path.polygon.points.buffer + 1, path.polygon.points.count - 1)
        n := cast(s32) @countof(slice)

        // We want a counter-clockwise polygon in V
        triangulateIdx.resize(cast(uint) n)
        idxPtr := triangulateIdx.buffer
        if path.polygon.isClockwise
        {
            loop i: n
                idxPtr[i] = cast(s32) i
        }
        else
        {
            loop i: n
                idxPtr[i] = cast(s32) ((n - 1) - i)
        }

        // Remove nv-2 Vertices, creating 1 triangle every time
        nv := n
        count := 2 * nv
        m := 0
        v := nv - 1
        while nv > 2
        {
            // If we loop, it is probably a non-simple polygon
            if count <= 0
            {
                result.clear()
                return result
            }

            count -= 1

            // Three consecutive vertices in current polygon, <u,v,w>
            u := v
            if nv <= u u = 0
            v = u + 1
            if nv <= v v = 0
            w := v + 1
            if nv <= w w = 0

            if triangulateSnip(slice, u, v, w, nv, idxPtr)
            {
                // Output Triangle
                a := idxPtr[u]
                b := idxPtr[v]
                c := idxPtr[w]
                result.add(a)
                result.add(b)
                result.add(c)

                // Remove v from remaining polygon
                m += 1
                s := v
                t := v + 1
                while t < nv
                {
                    triangulateIdx.buffer[s] = triangulateIdx.buffer[t]
                    s += 1
                    t += 1
                }

                nv -= 1

                // Reset error detection counter
                count = 2 * nv
            }
        }

        return result
    }

    mtd triangulate(t: *Poly2Tri.Tesselate, p: *LinePath, bvMin, bvMax: Vector2, brush: Brush)
    {
        setTexture0(bvMin, bvMax, brush)
        resetTextureFont()

        if p.triangles.count == 0
        {
            t.triangulate()

            // "Eat" points, triangles and edges
            p.points =,move t.points
            p.points.add(t.head); t.head = null
            p.points.add(t.tail); t.tail = null
            p.triangles =,move t.triangles
            p.edgeList  =,move t.edgeList

            // t.triangles only contains 'interior' triangles, so we need to delete all other
            // allocated triangles to avoid a leak
            visit tri: t.map
            {
                if !tri.interior
                    Memory.delete(tri)
            }

            t.map.clear()
        }

        t.clear()

        visit tri, ii: p.triangles
        {
            Debug.assert(tri.interior)
            v0 := Vector2{cast(f32) tri.points[0].x, cast(f32) tri.points[0].y}
            v1 := Vector2{cast(f32) tri.points[1].x, cast(f32) tri.points[1].y}
            v2 := Vector2{cast(f32) tri.points[2].x, cast(f32) tri.points[2].y}
            prim := pushTriangle(v0, v1, v2, brush.color)

            if curState.paintParams & .Antialiased
            {
                e := tri.points[0].border[0]
                if e addEdgeAA(prim, acast e.p.x, acast e.p.y, acast e.q.x, acast e.q.y)
                e = tri.points[0].border[1]
                if e addEdgeAA(prim, acast e.p.x, acast e.p.y, acast e.q.x, acast e.q.y)

                e = tri.points[1].border[0]
                if e addEdgeAA(prim, acast e.p.x, acast e.p.y, acast e.q.x, acast e.q.y)
                e = tri.points[1].border[1]
                if e addEdgeAA(prim, acast e.p.x, acast e.p.y, acast e.q.x, acast e.q.y)

                e = tri.points[2].border[0]
                if e addEdgeAA(prim, acast e.p.x, acast e.p.y, acast e.q.x, acast e.q.y)
                e = tri.points[2].border[1]
                if e addEdgeAA(prim, acast e.p.x, acast e.p.y, acast e.q.x, acast e.q.y)
            }
        }
    }
}
