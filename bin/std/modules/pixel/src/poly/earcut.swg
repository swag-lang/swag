/*
    :LICENCE
    This code is based on the Earcut library (https://github.com/mapbox/earcut.hpp)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Earcut
using Core

public typealias Poly = Array'(Array'Math.Vector2)
public typealias Ring = Array'Math.Vector2

struct Node
{
    i:                u32
    x, y:             f64
    prev, next:       *Node     // previous and next vertice nodes in a polygon ring
    z:                s32       // z-order curve value
    prevZ, nextZ:     *Node     // previous and next nodes in z-order
    steiner:          bool      // indicates whether this is a steiner point
}

#[Swag.Opaque]
public struct Tesselate
{
    indices:        Array'u32
    vertices:       u32
    hashing:        bool
    minX, maxX:     f64
    minY, maxY:     f64
    inv_size:       f64
    nodes:          Array'*Node
}

impl Tesselate
{
    mtd doIt(points: Poly)
    {
        // reset
        indices.clear()
        vertices = 0

        if points.isEmpty() do
            return

        var x, y: f64
        var threshold: s64 = 80
        var len:       s64

        for var i = 0; threshold >= 0 and i < points.count; i += 1
        {
            threshold -= points[i].count
            len += points[i].count
        }

        //estimate size of nodes and indices
        nodes.reserve(len * 3 / 2)
        indices.reserve(len + points[0].count)

        var outerNode = linkedList(points[0], true)
        if !outerNode or outerNode.prev == outerNode.next do
            return

        if points.count > 1 do
            outerNode = eliminateHoles(points, outerNode)

        // if the shape is not too simple, we'll use z-order curve hash later; calculate polygon bbox
        hashing = threshold < 0
        if hashing
        {
            var p = outerNode.next
            minX, maxX = outerNode.x
            minY, maxY = outerNode.y
            loop
            {
                x    = p.x
                y    = p.y
                minX = Math.min(minX, x)
                minY = Math.min(minY, y)
                maxX = Math.max(maxX, x)
                maxY = Math.max(maxY, y)
                p    = p.next
                if p == outerNode do
                    break
            }

            // minX, minY and inv_size are later used to transform coords into integers for z-order calculation
            inv_size = Math.max(maxX - minX, maxY - minY)
            inv_size = inv_size != 0.0 ? (32767.0 / inv_size) : 0.0
        }

        earcutLinked(outerNode)
        nodes.clear()
    }

    // create a circular doubly linked list from polygon points in the specified winding order
    mtd linkedList(points: Ring, clockwise: bool)->*Node
    {
        var sum:  f64
        let len   = cast(s32) points.count
        var last: *Node

        // calculate original winding order of a polygon ring
        var i = 0
        var j = len > 0 ? len - 1 : 0
        while i < len
        {
            let p1   = points[i]
            let p2   = points[j]
            let p20: f64 = p2.x
            let p10: f64 = p1.x
            let p11: f64 = p1.y
            let p21: f64 = p2.y
            sum += (p20 - p10) * (p11 + p21)
            j = i
            i += 1
        }

        // link points into circular doubly-linked list in the specified winding order
        if (clockwise == (sum > 0))
        {
            for i = 0; i < len; i += 1 do
                last = insertNode(vertices + i, points[i], last)
        }
        else
        {
            for i = len - 1; i >= 0; i -= 1 do
                last = insertNode(vertices + i, points[i], last)
        }

        if last and equals(last, last.next)
        {
            removeNode(last)
            last = last.next
        }

        vertices += len
        return last
    }

    // eliminate colinear or duplicate points
    mtd filterPoints(start: *Node, end_: *Node = null)->*Node
    {
        var end = end_
        if !end do
            end = start

        var p      = start
        var again: bool

        loop
        {
            again = false

            if !p.steiner and (equals(p, p.next) or area(p.prev, p, p.next) == 0)
            {
                removeNode(p)
                p, end = p.prev

                if p == p.next do
                    break
                again = true
            }
            else
            {
                p = p.next
            }

            if !again and p == end do
                break
        }

        return end
    }

    // main ear slicing loop which triangulates a polygon (given as a linked list)
    mtd earcutLinked(ear_: *Node, pass: s32 = 0)
    {
        var ear = ear_
        if !ear do
            return

        // interlink polygon nodes in z-order
        if !pass and hashing do
            indexCurve(ear)

        var stop  = ear
        var prev: *Node
        var next: *Node

        // iterate through ears, slicing them one by one
        while ear.prev != ear.next
        {
            prev = ear.prev
            next = ear.next

            if hashing ? isEarHashed(ear) : isEar(ear)
            {
                // cut off the triangle
                indices.add(prev.i)
                indices.add(ear.i)
                indices.add(next.i)

                removeNode(ear)

                // skipping the next vertice leads to less sliver triangles
                ear  = next.next
                stop = next.next

                continue
            }

            ear = next

            // if we looped through the whole remaining polygon and can't find any more ears
            if ear == stop
            {
                // try filtering points and slicing again
                if !pass do
                    earcutLinked(filterPoints(ear), 1)

                // if this didn't work, try curing all small self-intersections locally
                elif pass == 1
                {
                    ear = cureLocalIntersections(filterPoints(ear))
                    earcutLinked(ear, 2)
                }

                // as a last resort, try splitting the remaining polygon into two
                elif pass == 2 do
                    splitEarcut(ear)

                break
            }
        }
    }

    // check whether a polygon node forms a valid ear with adjacent nodes
    mtd isEar(ear: *Node)->bool
    {
        let a = ear.prev
        let b = ear
        let c = ear.next

        if area(a, b, c) >= 0 do
            return false // reflex, can't be an ear

        // now make sure we don't have other points inside the potential ear
        var p = ear.next.next

        while p != ear.prev
        {
            if (pointInTriangle(a.x, a.y, b.x, b.y, c.x, c.y, p.x, p.y) and
               area(p.prev, p, p.next) >= 0) do
                return false
            p = p.next
        }

        return true
    }

    mtd isEarHashed(ear: *Node)->bool
    {
        let a = ear.prev
        let b = ear
        let c = ear.next

        if area(a, b, c) >= 0 do
            return false // reflex, can't be an ear

        // triangle bbox; min & max are calculated like this for speed
        let minTX = Math.min(a.x, b.x, c.x)
        let minTY = Math.min(a.y, b.y, c.y)
        let maxTX = Math.max(a.x, b.x, c.x)
        let maxTY = Math.max(a.y, b.y, c.y)

        // z-order range for the current triangle bbox;
        let minZ = zOrder(minTX, minTY)
        let maxZ = zOrder(maxTX, maxTY)

        // first look for points inside the triangle in increasing z-order
        var p = ear.nextZ

        while p and p.z <= maxZ
        {
            if (p != ear.prev and p != ear.next and
               pointInTriangle(a.x, a.y, b.x, b.y, c.x, c.y, p.x, p.y) and
               area(p.prev, p, p.next) >= 0) do
                return false
            p = p.nextZ
        }

        // then look for points in decreasing z-order
        p = ear.prevZ

        while (p and p.z >= minZ)
        {
            if (p != ear.prev and p != ear.next and
               pointInTriangle(a.x, a.y, b.x, b.y, c.x, c.y, p.x, p.y) and
               area(p.prev, p, p.next) >= 0) do
                return false
            p = p.prevZ
        }

        return true
    }

    // go through all polygon nodes and cure small local self-intersections
    mtd cureLocalIntersections(start_: *Node)->*Node
    {
        var start = start_
        var p     = start
        loop
        {
            let a = p.prev
            let b = p.next.next

            // a self-intersection where edge (v[i-1],v[i]) intersects (v[i+1],v[i+2])
            if !equals(a, b) and intersects(a, p, p.next, b) and locallyInside(a, b) and locallyInside(b, a)
            {
                indices.add(a.i)
                indices.add(p.i)
                indices.add(b.i)

                // remove two nodes involved
                removeNode(p)
                removeNode(p.next)

                p, start = b
            }

            p = p.next
            if p == start do
                break
        }

        return filterPoints(p)
    }

    // try splitting polygon into two and triangulate them independently
    mtd splitEarcut(start: *Node)
    {
        // look for a valid diagonal that divides the polygon into two
        var a = start
        loop
        {
            var b = a.next.next
            while b != a.prev
            {
                if a.i != b.i and isValidDiagonal(a, b)
                {
                    // split the polygon in two by the diagonal
                    var c = splitPolygon(a, b)

                    // filter colinear points around the cuts
                    a = filterPoints(a, a.next)
                    c = filterPoints(c, c.next)

                    // run earcut on each half
                    earcutLinked(a)
                    earcutLinked(c)
                    return
                }

                b = b.next
            }

            a = a.next
            if a == start do
                break
        }
    }

    // link every hole into the outer loop, producing a single-ring polygon without holes
    mtd eliminateHoles(points: Poly, outerNode_: *Node)->*Node
    {
        let len = points.count

        var queue: Array'(*Node)
        for var i = 1; i < len; i += 1
        {
            let list = linkedList(points[i], false)
            if list
            {
                if list == list.next do
                    list.steiner = true
                queue.add(getLeftmost(list))
            }
        }

        Slice.sort(queue.toSlice(), func(a, b) => a.x <=> b.x)

        // process holes from left to right
        var outerNode = outerNode_
        for var i = 0; i < queue.count; i += 1 do
            outerNode = eliminateHole(queue[i], outerNode)
        return outerNode
    }

    // find a bridge between vertices that connects hole with an outer ring and and link it
    mtd eliminateHole(hole, outerNode: *Node)->*Node
    {
        let bridge = findHoleBridge(hole, outerNode)
        if !bridge
        {
            return outerNode
        }

        let bridgeReverse = splitPolygon(bridge, hole)

        // filter collinear points around the cuts
        discard filterPoints(bridgeReverse, bridgeReverse.next)

        // Check if input node was removed by the filtering
        return filterPoints(bridge, bridge.next)
    }

    // David Eberly's algorithm for finding a bridge between hole and outer polygon
    mtd findHoleBridge(hole, outerNode: *Node)->*Node
    {
        var p   = outerNode
        let hx: f64 = hole.x
        let hy: f64 = hole.y
        var qx: f64 = Swag.F64.NegInf
        var m:  *Node

        // find a segment intersected by a ray from the hole's leftmost Vertex to the left;
        // segment's endpoint with lesser x will be potential connection Vertex
        loop
        {
            if hy <= p.y and hy >= p.next.y and p.next.y != p.y
            {
                let x: f64 = p.x + (hy - p.y) * (p.next.x - p.x) / (p.next.y - p.y)
                if x <= hx and x > qx
                {
                    qx = x
                    m  = p.x < p.next.x ? p : p.next
                    // hole touches outer segment; pick leftmost endpoint
                    if x == hx do
                        return m
                }
            }

            p = p.next
            if p == outerNode do
                break
        }

        if !m do
            return null

        // look for points inside the triangle of hole Vertex, segment intersection and endpoint;
        // if there are no points found, we have a valid connection;
        // otherwise choose the Vertex of the minimum angle with the ray as connection Vertex

        let stop    = m
        var tanMin: f64 = Swag.F32.Inf
        var tanCur: f64

        p = m
        let mx: f64 = m.x
        let my: f64 = m.y

        loop
        {
            if (hx >= p.x and p.x >= mx and hx != p.x and pointInTriangle(hy < my ? hx : qx, hy, mx, my, hy < my ? qx : hx, hy, p.x, p.y))
            {
                tanCur = Math.abs(hy - p.y) / (hx - p.x) // tangential
                if (locallyInside(p, hole) and
                   (tanCur < tanMin or (tanCur == tanMin and (p.x > m.x or sectorContainsSector(m, p)))))
                {
                    m      = p
                    tanMin = tanCur
                }
            }

            p = p.next
            if p == stop do
                break
        }

        return m
    }

    // whether sector in vertex m contains sector in vertex p in the same coordinates
    mtd sectorContainsSector(m, p: *Node)->bool
    {
        return area(m.prev, m, p.prev) < 0 and area(p.next, m, m.next) < 0
    }

    // interlink polygon nodes in z-order
    mtd indexCurve(start: *Node)
    {
        var p = start

        loop
        {
            p.z     = p.z ? p.z : zOrder(p.x, p.y)
            p.prevZ = p.prev
            p.nextZ = p.next
            p       = p.next
            if p == start do
                break
        }

        p.prevZ.nextZ = null
        p.prevZ       = null

        discard sortLinked(p)
    }

    // Simon Tatham's linked list merge sort algorithm
    // http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
    func sortLinked(list_: *Node)->*Node
    {
        var list = list_
        var p, q, e, tail: *Node
        var numMerges, pSize, qSize: s32
        var inSize = 1

        loop
        {
            p         = list
            list      = null
            tail      = null
            numMerges = 0

            while p
            {
                numMerges += 1
                q     = p
                pSize = 0
                loop i: inSize
                {
                    pSize += 1
                    q = q.nextZ
                    if !q do
                        break
                }

                qSize = inSize

                while pSize > 0 or (qSize > 0 and q)
                {
                    if (pSize == 0)
                    {
                        e = q
                        q = q.nextZ
                        qSize -= 1
                    }
                    elif qSize == 0 or !q
                    {
                        e = p
                        p = p.nextZ
                        pSize -= 1
                    }
                    elif p.z <= q.z
                    {
                        e = p
                        p = p.nextZ
                        pSize -= 1
                    }
                    else
                    {
                        e = q
                        q = q.nextZ
                        qSize -= 1
                    }

                    if tail do
                        tail.nextZ = e
                    else do
                        list = e

                    e.prevZ = tail
                    tail    = e
                }

                p = q
            }

            tail.nextZ = null

            if numMerges <= 1 do
                return list
            inSize *= 2
        }
    }

    // z-order of a Vertex given coords and size of the data bounding box
    mtd zOrder(x_, y_: f64)->s32
    {
        // coords are transformed into non-negative 15-bit integer range
        var x = cast(s32) ((x_ - minX) * inv_size)
        var y = cast(s32) ((y_ - minY) * inv_size)

        x = (x | (x << 8)) & 0x00FF00FF
        x = (x | (x << 4)) & 0x0F0F0F0F
        x = (x | (x << 2)) & 0x33333333
        x = (x | (x << 1)) & 0x55555555

        y = (y | (y << 8)) & 0x00FF00FF
        y = (y | (y << 4)) & 0x0F0F0F0F
        y = (y | (y << 2)) & 0x33333333
        y = (y | (y << 1)) & 0x55555555

        return x | (y << 1)
    }

    // find the leftmost node of a polygon ring
    func getLeftmost(start: *Node)->*Node
    {
        var p        = start
        var leftmost = start
        loop
        {
            if p.x < leftmost.x or (p.x == leftmost.x and p.y < leftmost.y) do
                leftmost = p
            p = p.next
            if p == start do
                break
        }

        return leftmost
    }

    // check if a point lies within a convex triangle
    func pointInTriangle(ax, ay, bx, by, cx, cy, px, py: f64)->bool
    {
        return (cx - px) * (ay - py) >= (ax - px) * (cy - py) and
               (ax - px) * (by - py) >= (bx - px) * (ay - py) and
               (bx - px) * (cy - py) >= (cx - px) * (by - py)
    }

    // check if a diagonal between two polygon nodes is valid (lies in polygon interior)
    mtd isValidDiagonal(a, b: *Node)->bool
    {
        return a.next.i != b.i and a.prev.i != b.i and !intersectsPolygon(a, b) and
               ((locallyInside(a, b) and locallyInside(b, a) and middleInside(a, b) and
               (area(a.prev, a, b.prev) != 0.0 or area(a, b.prev, b) != 0.0) // locally visible
        ) or
               (equals(a, b) and area(a.prev, a, a.next) > 0 and area(b.prev, b, b.next) > 0)) // special zero-length case
         // dones't intersect other edges
    }

    // signed area of a triangle
    func area(p, q, r: *Node)->f64
    {
        return (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y)
    }

    // check if two points are equal
    func equals(p1, p2: *Node)->bool
    {
        return p1.x == p2.x and p1.y == p2.y
    }

    // check if two segments intersect
    mtd intersects(p1, q1, p2, q2: *Node)->bool
    {
        let o1 = Math.sign(area(p1, q1, p2))
        let o2 = Math.sign(area(p1, q1, q2))
        let o3 = Math.sign(area(p2, q2, p1))
        let o4 = Math.sign(area(p2, q2, q1))

        if o1 != o2 and o3 != o4 do
            return true // general case

        if o1 == 0 and onSegment(p1, p2, q1) do
            return true // p1, q1 and p2 are collinear and p2 lies on p1q1
        if o2 == 0 and onSegment(p1, q2, q1) do
            return true // p1, q1 and q2 are collinear and q2 lies on p1q1
        if o3 == 0 and onSegment(p2, p1, q2) do
            return true // p2, q2 and p1 are collinear and p1 lies on p2q2
        if o4 == 0 and onSegment(p2, q1, q2) do
            return true // p2, q2 and q1 are collinear and q1 lies on p2q2

        return false
    }

    // for collinear points p, q, r, check if point q lies on segment pr
    func onSegment(p, q, r: *Node)->bool
    {
        return q.x <= Math.max(p.x, r.x) and
               q.x >= Math.min(p.x, r.x) and
               q.y <= Math.max(p.y, r.y) and
               q.y >= Math.min(p.y, r.y)
    }

    /*mtd sign(val: f64)->s32
    {
        return (0.0 < val) - (val < 0.0)
    }*/

    // check if a polygon diagonal intersects any polygon segments
    mtd intersectsPolygon(a, b: *Node)->bool
    {
        var p = a
        loop
        {
            if p.i != a.i and p.next.i != a.i and p.i != b.i and p.next.i != b.i and intersects(p, p.next, a, b) do
                return true
            p = p.next
            if p == a do
                break
        }

        return false
    }

    // check if a polygon diagonal is locally inside the polygon
    mtd locallyInside(a, b: *Node)->bool
    {
        return area(a.prev, a, a.next) < 0 ? area(a, b, a.next) >= 0 and area(a, a.prev, b) >= 0 : area(a, b, a.prev) < 0 or area(a, a.next, b) < 0
    }

    // check if the middle Vertex of a polygon diagonal is inside the polygon
    func middleInside(a, b: *Node)->bool
    {
        var p      = a
        var inside = false

        let px: f64 = (a.x + b.x) / 2
        let py: f64 = (a.y + b.y) / 2
        loop
        {
            if ((p.y > py) != (p.next.y > py)) and p.next.y != p.y and (px < (p.next.x - p.x) * (py - p.y) / (p.next.y - p.y) + p.x) do
                inside = !inside
            p = p.next
            if p == a do
                break
        }

        return inside
    }

    // link two polygon vertices with a bridge; if the vertices belong to the same ring, it splits
    // polygon into two; if one belongs to the outer ring and another to a hole, it merges it into a
    // single ring
    mtd splitPolygon(a, b: *Node)->*Node
    {
        let a2 = addNode(a.i, a.x, a.y)
        let b2 = addNode(b.i, b.x, b.y)
        let an = a.next
        let bp = b.prev

        a.next = b
        b.prev = a

        a2.next = an
        an.prev = a2

        b2.next = a2
        a2.prev = b2

        bp.next = b2
        b2.prev = bp

        return b2
    }

    mtd addNode(i: u32, x, y: f64)->*Node
    {
        let node = Memory.new'Node()
        node.i = i
        node.x = x
        node.y = y
        nodes.add(node)
        return node
    }

    // create a node and util::optionally link it with previous one (in a circular doubly linked list)
    mtd insertNode(i: u32, pt: Math.Vector2, last: *Node)->*Node
    {
        let p = addNode(i, pt.x, pt.y)

        if !last
        {
            p.prev = p
            p.next = p
        }
        else
        {
            p.next         = last.next
            p.prev         = last
            last.next.prev = p
            last.next      = p
        }

        return p
    }

    func removeNode(p: *Node)
    {
        p.next.prev = p.prev
        p.prev.next = p.next

        if p.prevZ do
            p.prevZ.nextZ = p.nextZ
        if p.nextZ do
            p.nextZ.prevZ = p.prevZ
    }

    public func earcut(poly: Poly)->Array'u32
    {
        var earcut: Tesselate
        earcut.doIt(poly)
        return earcut.indices
    }
}
