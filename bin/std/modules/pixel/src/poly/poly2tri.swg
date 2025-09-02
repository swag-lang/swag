/*
    :LICENCE
    This code is based on the Poly2Tri library (https://github.com/jhasse/poly2tri)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Poly2Tri
using Core

enum Orientation
{
    CW
    CCW
    Collinear
}

#[Swag.Opaque]
public struct Tessellate
{
    basin:          Basin
    edgeEvent:      EdgeEvent

    triangles:      Array'*Triangle
    edges:          Array'*Edge
    points:         Array'*Point
    triIndices:     Array'u64

    nodes:          Array'*Node
    freeNodes:      Array'*Node

    front:          AdvancingFront
    head:           *Point
    tail:           *Point
    afHead:         *Node
    afMiddle:       *Node
    afTail:         *Node
    polyStart:      u64
}

public struct Result
{
    triangles:      Array'*Triangle
    edges:          Array'*Edge
    points:         Array'*Point
    triIndices:     Array'u64
    tess:           *Tessellate
}

public struct Point
{
    using v:     Math.Vector2
    edges:       Array'*Edge
    border:      [2] *Edge
}

public struct Edge
{
    p:     *Point
    q:     *Point
}

public struct Triangle
{
    constrainedEdge:     [3] bool
    delaunayEdge:        [3] bool
    points:              [3] *Point
    neighbors:           [3] *Triangle
    idx:                 u64
    interior:            bool
}

struct EdgeEvent
{
    constrainedEdge:     *Edge
    right:               bool
}

struct Basin
{
    leftNode:        *Node
    bottomNode:      *Node
    rightNode:       *Node
    width:           f64
    leftHighest:     bool
}

struct Node
{
    point:        *Point
    triangle:     *Triangle
    next:         *Node
    prev:         *Node
    value:        f64
}

struct AdvancingFront
{
    head:           *Node
    tail:           *Node
    searchNode:     *Node
}

impl Result
{
    mtd clear()
    {
        me.points.clear()
        me.edges.clear()
        me.triangles.clear()
        me.triIndices.clear()
    }

    mtd release()
    {
        foreach t in me.points:
            Memory.delete(t)
        foreach t in me.edges:
            Memory.delete(t)
        foreach t in me.triangles:
            Memory.delete(t)
        me.points.clear()
        me.edges.clear()
        me.triangles.clear()
        me.triIndices.clear()
    }
}

impl Edge
{
    mtd set(p1, p2: *Point)
    {
        me.p = p1
        me.q = p2
        if p1.y > p2.y
        {
            me.q = p1
            me.p = p2
        }
        elif p1.y == p2.y
        {
            if p1.x > p2.x
            {
                me.q = p1
                me.p = p2
            }
            elif p1.x == p2.x
            {
                Debug.assert(false, "Edge::Edge: p1 == p2")
            }
        }

        me.q.edges.add(me)
        if me.p.border[0]:
            me.p.border[1] = me
        else:
            me.p.border[0] = me
        if me.q.border[0]:
            me.q.border[1] = me
        else:
            me.q.border[0] = me
    }
}

impl Triangle
{
    #[Swag.Overload]
    mtd markConstrainedEdge(index: s32)
    {
        me.constrainedEdge[index] = true
    }

    #[Swag.Overload]
    mtd markConstrainedEdge(edge: *Edge)
    {
        me.markConstrainedEdge(edge.p, edge.q)
    }

    #[Swag.Overload]
    mtd markConstrainedEdge(p, q: *Point)
    {
        if (q == me.points[0] and p == me.points[1]) or (q == me.points[1] and p == me.points[0]):
            me.constrainedEdge[2] = true
        elif (q == me.points[0] and p == me.points[2]) or (q == me.points[2] and p == me.points[0]):
            me.constrainedEdge[1] = true
        elif (q == me.points[1] and p == me.points[2]) or (q == me.points[2] and p == me.points[1]):
            me.constrainedEdge[0] = true
    }

    mtd oppositePoint(t: *Triangle, p: *Point)->*Point
    {
        let cw = t.pointCW(p)
        return me.pointCW(cw)
    }

    mtd edgeIndex(p1, p2: *Point)->s32
    {
        switch p1
        {
        case me.points[0]:
            if me.points[1] == p2:
                return 2
            if me.points[2] == p2:
                return 1

        case me.points[1]:
            if me.points[2] == p2:
                return 0
            if me.points[0] == p2:
                return 2

        case me.points[2]:
            if me.points[0] == p2:
                return 1
            if me.points[1] == p2:
                return 0
        }

        return -1
    }

    mtd index(p: *Point)->s32
    {
        switch p
        {
        case me.points[0]:
            return 0
        case me.points[1]:
            return 1
        case me.points[2]:
            return 2
        }

        Debug.assert(false)
        return -1
    }

    mtd neighborAcross(point: *Point)->*Triangle
    {
        switch point
        {
        case me.points[0]:
            return me.neighbors[0]
        case me.points[1]:
            return me.neighbors[1]
        default:
            return me.neighbors[2]
        }
    }

    mtd neighborCW(point: *Point)->*Triangle
    {
        switch point
        {
        case me.points[0]:
            return me.neighbors[1]
        case me.points[1]:
            return me.neighbors[2]
        default:
            return me.neighbors[0]
        }
    }

    mtd neighborCCW(p: *Point)->*Triangle
    {
        switch p
        {
        case me.points[0]:
            return me.neighbors[2]
        case me.points[1]:
            return me.neighbors[0]
        default:
            return me.neighbors[1]
        }
    }

    mtd getConstrainedEdgeCCW(p: *Point)->bool
    {
        switch p
        {
        case me.points[0]:
            return me.constrainedEdge[2]
        case me.points[1]:
            return me.constrainedEdge[0]
        default:
            return me.constrainedEdge[1]
        }
    }

    mtd getConstrainedEdgeCW(p: *Point)->bool
    {
        switch p
        {
        case me.points[0]:
            return me.constrainedEdge[1]
        case me.points[1]:
            return me.constrainedEdge[2]
        default:
            return me.constrainedEdge[0]
        }
    }

    mtd pointCW(point: *Point)->*Point
    {
        switch point
        {
        case me.points[0]:
            return me.points[2]
        case me.points[1]:
            return me.points[0]
        default:
            return me.points[1]
        }
    }

    mtd pointCCW(point: *Point)->*Point
    {
        switch point
        {
        case me.points[0]:
            return me.points[1]
        case me.points[1]:
            return me.points[2]
        default:
            return me.points[0]
        }
    }

    mtd getDelaunayEdgeCCW(p: *Point)->bool
    {
        switch p
        {
        case me.points[0]:
            return me.delaunayEdge[2]
        case me.points[1]:
            return me.delaunayEdge[0]
        default:
            return me.delaunayEdge[1]
        }
    }

    mtd getDelaunayEdgeCW(p: *Point)->bool
    {
        switch p
        {
        case me.points[0]:
            return me.delaunayEdge[1]
        case me.points[1]:
            return me.delaunayEdge[2]
        default:
            return me.delaunayEdge[0]
        }
    }

    mtd setDelaunayEdgeCCW(p: *Point, e: bool)
    {
        switch p
        {
        case me.points[0]:
            me.delaunayEdge[2] = e
        case me.points[1]:
            me.delaunayEdge[0] = e
        default:
            me.delaunayEdge[1] = e
        }
    }

    mtd setDelaunayEdgeCW(p: *Point, e: bool)
    {
        switch p
        {
        case me.points[0]:
            me.delaunayEdge[1] = e
        case me.points[1]:
            me.delaunayEdge[2] = e
        default:
            me.delaunayEdge[0] = e
        }
    }

    mtd setConstrainedEdgeCCW(p: *Point, ce: bool)
    {
        switch p
        {
        case me.points[0]:
            me.constrainedEdge[2] = ce
        case me.points[1]:
            me.constrainedEdge[0] = ce
        default:
            me.constrainedEdge[1] = ce
        }
    }

    mtd setConstrainedEdgeCW(p: *Point, ce: bool)
    {
        switch p
        {
        case me.points[0]:
            me.constrainedEdge[1] = ce
        case me.points[1]:
            me.constrainedEdge[2] = ce
        default:
            me.constrainedEdge[0] = ce
        }
    }

    mtd clearNeighbors()
    {
        me.neighbors[0] = null
        me.neighbors[1] = null
        me.neighbors[2] = null
    }

    mtd clearDelaunayEdges()
    {
        me.delaunayEdge[0] = false
        me.delaunayEdge[1] = false
        me.delaunayEdge[2] = false
    }

    #[Swag.Overload]
    mtd contains(p: *Point)->bool
    {
        return p == me.points[0] or p == me.points[1] or p == me.points[2]
    }

    #[Swag.Overload]
    mtd contains(p1, p2: *Point)->bool
    {
        return me.contains(p1) and me.contains(p2)
    }

    #[Swag.Overload]
    mtd markNeighbor(p1, p2: *Point, t: *Triangle)
    {
        if (p1 == me.points[2] and p2 == me.points[1]) or (p1 == me.points[1] and p2 == me.points[2]):
            me.neighbors[0] = t
        elif (p1 == me.points[0] and p2 == me.points[2]) or (p1 == me.points[2] and p2 == me.points[0]):
            me.neighbors[1] = t
        elif (p1 == me.points[0] and p2 == me.points[1]) or (p1 == me.points[1] and p2 == me.points[0]):
            me.neighbors[2] = t
        else:
            Debug.assert(false)
    }

    #[Swag.Overload]
    mtd markNeighbor(t: *Triangle)
    {
        if t.contains(me.points[1], me.points[2])
        {
            me.neighbors[0] = t
            t.markNeighbor(me.points[1], me.points[2], me)
        }
        elif t.contains(me.points[0], me.points[2])
        {
            me.neighbors[1] = t
            t.markNeighbor(me.points[0], me.points[2], me)
        }
        elif t.contains(me.points[0], me.points[1])
        {
            me.neighbors[2] = t
            t.markNeighbor(me.points[0], me.points[1], me)
        }
    }

    mtd legalize(opoint, npoint: *Point)
    {
        switch opoint
        {
        case me.points[0]:
            me.points[1] = me.points[0]
            me.points[0] = me.points[2]
            me.points[2] = npoint
        case me.points[1]:
            me.points[2] = me.points[1]
            me.points[1] = me.points[0]
            me.points[0] = npoint
        case me.points[2]:
            me.points[0] = me.points[2]
            me.points[2] = me.points[1]
            me.points[1] = npoint
        default:
            Debug.assert(false)
        }
    }
}

impl AdvancingFront
{
    mtd locatePoint(point: *Point)->#null *Node
    {
        let px   = point.x
        var node = me.searchNode
        let nx   = node.point.x

        if px == nx
        {
            if point != node.point
            {
                if point == node.prev.point:
                    node = node.prev
                elif point == node.next.point:
                    node = node.next
                else:
                    Debug.assert(false)
            }
        }
        elif px < nx
        {
            node = node.prev
            while node
            {
                if point == node.point:
                    break
                node = node.prev
            }
        }
        else
        {
            node = node.next
            while node
            {
                if point == node.point:
                    break
                node = node.next
            }
        }

        if node:
            me.searchNode = node
        return node
    }

    mtd locateNode(x: f32)->*Node
    {
        var node = me.searchNode
        if x < node.value
        {
            node = node.prev
            while node
            {
                if x >= node.value
                {
                    me.searchNode = node
                    return node
                }

                node = node.prev
            }
        }
        else
        {
            node = node.next
            while node
            {
                if x < node.value
                {
                    me.searchNode = node.prev
                    return node.prev
                }

                node = node.next
            }
        }

        unreachable
    }
}

impl Tessellate
{
    public mtd opDrop()
    {
        me.clear()
        me.clearCache()
    }

    #[Swag.Inline]
    {
        func newPoint()->*Point { return Memory.new'Point(); }
        func deletePoint(ptr: *Point) { Memory.delete(ptr); }
        func newEdge()->*Edge { return Memory.new'Edge(); }
        func deleteEdge(ptr: *Edge) { Memory.delete(ptr); }
        func newTriangle()->*Triangle { return Memory.new'Triangle(); }
        func deleteTriangle(ptr: *Triangle) { Memory.delete(ptr); }

        mtd newNode()->*Node
        {
            if me.freeNodes.count
            {
                let p = me.freeNodes.popBack()
                @init(p, 1)
                return p
            }

            return Memory.new'Node()
        }

        mtd deleteNode(ptr: *Node)
        {
            me.freeNodes.add(ptr)
        }
    }

    // Clear content
    public mtd clear()
    {
        @init(&me.basin, 1)
        @init(&me.edgeEvent, 1)
        me.polyStart = 0

        deletePoint(me.head)
        deletePoint(me.tail)
        me.deleteNode(me.afHead)
        me.deleteNode(me.afMiddle)
        me.deleteNode(me.afTail)
        me.head, me.tail, me.afHead, me.afMiddle, me.afTail = null

        foreach p in me.points:
            deletePoint(p)
        foreach p in me.edges:
            deleteEdge(p)
        foreach p in me.triangles:
            deleteTriangle(p)
        foreach p in me.nodes:
            me.deleteNode(p)

        me.points.clear()
        me.edges.clear()
        me.triangles.clear()
        me.triIndices.clear()
        me.nodes.clear()
    }

    // Clear cache
    mtd clearCache()
    {
        foreach p in me.freeNodes:
            Memory.delete(p)
        me.freeNodes.clear()
    }

    // Get the triangulation result
    public mtd getResult()->Result
    {
        var result: retval
        result.points     = #move me.points
        result.edges      = #move me.edges
        result.triangles  = #move me.triangles
        result.triIndices = #move me.triIndices
        return result
    }

    // To call before adding a polyline.
    // A polyline is a polygon border.
    // See [[addPoint]] and [[endPolyLine]]
    public mtd startPolyLine()
    {
        me.polyStart = me.points.count
    }

    // Register a new point
    public mtd addPoint(pt: Math.Vector2)
    {
        let p = newPoint()
        p.x = pt.x
        p.y = pt.y
        me.points.add(p)
    }

    // To call to register the polyline.
    // The polyline registration should have been done before with [[startPolyline]]
    public mtd endPolyLine()
    {
        mtd initEdges(polyline: [..] *Point)
        {
            foreach pt, i in polyline
            {
                let nexti = (i + 1) % @countof(polyline)
                let edge  = newEdge()
                edge.set(polyline[i], polyline[nexti])
                me.edges.add(edge)
            }
        }

        Debug.assert(me.polyStart != me.points.count, "no points !")
        initEdges(me, me.points[me.polyStart..me.points.count - 1])
    }

    // Register a polygon as a polyline
    public mtd addPolyLine(poly: Polygon)
    {
        me.points.grow(me.points.count + poly.points.count)

        me.startPolyLine()
        for i in 0 until poly.points.count - 1
        {
            let p  = newPoint()
            let pt = poly.getPoint(i)
            p.x = pt.x
            p.y = pt.y
            me.points.add(p)
        }
        me.endPolyLine()
    }

    // Triangulate
    public mtd triangulate()
    {
        if me.points.isEmpty():
            return

        me.initTriangulation()
        me.createAdvancingFront()
        me.sweepPoints()
        me.finalizationPolygon()

        me.points.add(me.head)
        me.points.add(me.tail)
        me.head = null
        me.tail = null
    }

    mtd sweepPoints()
    {
        for i in 1 until me.points.count
        {
            let point = me.points[i]
            let node  = me.pointEvent(point)
            foreach e in point.edges:
                me.addEdgeEvent2(e, node)
        }
    }

    mtd addEdgeEvent(ep, eq: *Point, t: *Triangle, point: *Point)
    {
        var triangle = t
        if isEdgeSideOfTriangle(triangle, ep, eq):
            return

        let p1 = triangle.pointCCW(point)
        let o1 = orient2d(eq, p1, ep)
        if o1 == .Collinear
        {
            if triangle.contains(eq, p1)
            {
                triangle.markConstrainedEdge(eq, p1)
                me.edgeEvent.constrainedEdge.q = p1
                triangle                       = triangle.neighborAcross(point)
                me.addEdgeEvent(ep, p1, triangle, p1)
            }
            else
            {
                Debug.assert(false, "EdgeEvent - collinear points not supported")
            }

            return
        }

        let p2 = triangle.pointCW(point)
        let o2 = orient2d(eq, p2, ep)
        if o2 == .Collinear
        {
            if triangle.contains(eq, p2)
            {
                triangle.markConstrainedEdge(eq, p2)
                me.edgeEvent.constrainedEdge.q = p2
                triangle                       = triangle.neighborAcross(point)
                me.addEdgeEvent(ep, p2, triangle, p2)
            }
            else
            {
                Debug.assert(false, "EdgeEvent - collinear points not supported")
            }

            return
        }

        if o1 == o2
        {
            if o1 == .CW:
                triangle = triangle.neighborCCW(point)
            else:
                triangle = triangle.neighborCW(point)
            me.addEdgeEvent(ep, eq, triangle, point)
        }
        else
        {
            me.flipEdgeEvent(ep, eq, triangle, point)
        }
    }

    mtd flipEdgeEvent(ep, eq: *Point, tri: *Triangle, p: *Point)
    {
        var t  = tri
        let ot = t.neighborAcross(p)
        let op = ot.oppositePoint(t, p)

        if inScanArea(p, t.pointCCW(p), t.pointCW(p), op)
        {
            rotateTrianglePair(t, p, ot, op)
            me.mapTriangleToNodes(t)
            me.mapTriangleToNodes(ot)

            if p == eq and op == ep
            {
                if eq == me.edgeEvent.constrainedEdge.q and ep == me.edgeEvent.constrainedEdge.p
                {
                    t.markConstrainedEdge(ep, eq)
                    ot.markConstrainedEdge(ep, eq)
                    discard me.legalize(t)
                    discard me.legalize(ot)
                }
            }
            else
            {
                let o = orient2d(eq, op, ep)
                t = me.nextFlipTriangle(o, t, ot, p, op)
                me.flipEdgeEvent(ep, eq, t, p)
            }
        }
        else
        {
            let newP = nextFlipPoint(ep, eq, ot, op)
            me.flipScanEdgeEvent(ep, eq, t, ot, newP)
            me.addEdgeEvent(ep, eq, t, p)
        }
    }

    mtd nextFlipTriangle(o: Orientation, t, ot: *Triangle, p, op: *Point)->*Triangle
    {
        if o == .CCW
        {
            let edgeIndex = ot.edgeIndex(p, op)
            ot.delaunayEdge[edgeIndex] = true
            discard me.legalize(ot)
            ot.clearDelaunayEdges()
            return t
        }

        let edgeIndex = t.edgeIndex(p, op)
        t.delaunayEdge[edgeIndex] = true
        discard me.legalize(t)
        t.clearDelaunayEdges()
        return ot
    }

    func nextFlipPoint(ep, eq: *Point, ot: *Triangle, op: *Point)->*Point
    {
        let o2d = orient2d(eq, op, ep)
        if o2d == .CW:
            return ot.pointCCW(op)
        Debug.assert(o2d == .CCW, "[Unsupported] Opposing point on constrained edge")
        return ot.pointCW(op)
    }

    mtd flipScanEdgeEvent(ep, eq: *Point, flipTriangle: *Triangle, t: *Triangle, p: *Point)
    {
        let ot = t.neighborAcross(p)
        let op = ot.oppositePoint(t, p)
        let p1 = flipTriangle.pointCCW(eq)
        let p2 = flipTriangle.pointCW(eq)
        if inScanArea(eq, p1, p2, op):
            me.flipEdgeEvent(eq, op, ot, op)
        else
        {
            let newP = nextFlipPoint(ep, eq, ot, op)
            me.flipScanEdgeEvent(ep, eq, flipTriangle, ot, newP)
        }
    }

    mtd addEdgeEvent2(edge: *Edge, node: *Node)
    {
        me.edgeEvent.constrainedEdge = edge
        me.edgeEvent.right           = edge.p.x > edge.q.x

        if isEdgeSideOfTriangle(node.triangle, edge.p, edge.q):
            return

        me.fillEdgeEvent(edge, node)
        me.addEdgeEvent(edge.p, edge.q, node.triangle, edge.q)
    }

    mtd fillEdgeEvent(edge: *Edge, node: *Node)
    {
        if me.edgeEvent.right:
            me.fillRightAboveEdgeEvent(edge, node)
        else:
            me.fillLeftAboveEdgeEvent(edge, node)
    }

    mtd fillRightAboveEdgeEvent(edge: *Edge, n: *Node)
    {
        var node = n
        while node.next.point.x < edge.p.x
        {
            if orient2d(edge.q, node.next.point, edge.p) == .CCW:
                me.fillRightBelowEdgeEvent(edge, node)
            else:
                node = node.next
        }
    }

    mtd fillRightBelowEdgeEvent(edge: *Edge, node: *Node)
    {
        if node.point.x < edge.p.x
        {
            if orient2d(node.point, node.next.point, node.next.next.point) == .CCW:
                me.fillRightConcaveEdgeEvent(edge, node)
            else
            {
                me.fillRightConvexEdgeEvent(edge, node)
                me.fillRightBelowEdgeEvent(edge, node)
            }
        }
    }

    mtd fillRightConcaveEdgeEvent(edge: *Edge, node: *Node)
    {
        me.fill(node.next)
        if node.next.point != edge.p
        {
            if orient2d(edge.q, node.next.point, edge.p) == .CCW
            {
                if orient2d(node.point, node.next.point, node.next.next.point) == .CCW:
                    me.fillRightConcaveEdgeEvent(edge, node)
            }
        }
    }

    mtd fillRightConvexEdgeEvent(edge: *Edge, node: *Node)
    {
        if orient2d(node.next.point, node.next.next.point, node.next.next.next.point) == .CCW
        {
            me.fillRightConcaveEdgeEvent(edge, node.next)
        }
        else
        {
            if orient2d(edge.q, node.next.next.point, edge.p) == .CCW:
                me.fillRightConvexEdgeEvent(edge, node.next)
        }
    }

    mtd fillLeftAboveEdgeEvent(edge: *Edge, n: *Node)
    {
        var node = n
        while node.prev.point.x > edge.p.x
        {
            if orient2d(edge.q, node.prev.point, edge.p) == .CW:
                me.fillLeftBelowEdgeEvent(edge, node)
            else:
                node = node.prev
        }
    }

    mtd fillLeftBelowEdgeEvent(edge: *Edge, node: *Node)
    {
        if node.point.x > edge.p.x
        {
            if orient2d(node.point, node.prev.point, node.prev.prev.point) == .CW:
                me.fillLeftConcaveEdgeEvent(edge, node)
            else
            {
                me.fillLeftConvexEdgeEvent(edge, node)
                me.fillLeftBelowEdgeEvent(edge, node)
            }
        }
    }

    mtd fillLeftConvexEdgeEvent(edge: *Edge, node: *Node)
    {
        if orient2d(node.prev.point, node.prev.prev.point, node.prev.prev.prev.point) == .CW:
            me.fillLeftConcaveEdgeEvent(edge, node.prev)
        else
        {
            if orient2d(edge.q, node.prev.prev.point, edge.p) == .CW:
                me.fillLeftConvexEdgeEvent(edge, node.prev)
        }
    }

    mtd fillLeftConcaveEdgeEvent(edge: *Edge, node: *Node)
    {
        me.fill(node.prev)
        if node.prev.point != edge.p
        {
            if orient2d(edge.q, node.prev.point, edge.p) == .CW
            {
                if orient2d(node.point, node.prev.point, node.prev.prev.point) == .CW:
                    me.fillLeftConcaveEdgeEvent(edge, node)
            }
        }
    }

    func isEdgeSideOfTriangle(triangle: *Triangle, ep, eq: *Point)->bool
    {
        let index = triangle.edgeIndex(ep, eq)
        if index != -1
        {
            triangle.markConstrainedEdge(index)
            let t = triangle.neighbors[index]
            if t:
                t.markConstrainedEdge(ep, eq)
            return true
        }

        return false
    }

    mtd pointEvent(point: *Point)->*Node
    {
        let node = me.locateNode(point)
        Debug.assert(node.next and node.next.point, "null node")
        let newNode = me.newFrontTriangle(point, node)
        if point.x <= node.point.x + Math.ConstF64.Epsilon:
            me.fill(node)
        me.fillAdvancingFront(newNode)
        return newNode
    }

    mtd fillAdvancingFront(n: *Node)
    {
        var node = n.next
        while node and node.next
        {
            if largeHoleDontFill(node):
                break
            me.fill(node)
            node = node.next
        }

        node = n.prev
        while node and node.prev
        {
            if largeHoleDontFill(node):
                break
            me.fill(node)
            node = node.prev
        }

        if n.next and n.next.next
        {
            let angle = basinAngle(n)
            if angle < Math.ConstF64.ThreePiBy4:
                me.fillBasin(n)
        }
    }

    mtd fillBasin(node: *Node)
    {
        if orient2d(node.point, node.next.point, node.next.next.point) == .CCW:
            me.basin.leftNode = node.next.next
        else:
            me.basin.leftNode = node.next

        me.basin.bottomNode = me.basin.leftNode
        while me.basin.bottomNode.next and me.basin.bottomNode.point.y >= me.basin.bottomNode.next.point.y:
            me.basin.bottomNode = me.basin.bottomNode.next
        if me.basin.bottomNode == me.basin.leftNode:
            return

        me.basin.rightNode = me.basin.bottomNode
        while me.basin.rightNode.next and me.basin.rightNode.point.y < me.basin.rightNode.next.point.y:
            me.basin.rightNode = me.basin.rightNode.next
        if me.basin.rightNode == me.basin.bottomNode:
            return

        me.basin.width       = me.basin.rightNode.point.x - me.basin.leftNode.point.x
        me.basin.leftHighest = me.basin.leftNode.point.y > me.basin.rightNode.point.y

        me.fillBasinReq(me.basin.bottomNode)
    }

    mtd fillBasinReq(n: *Node)
    {
        var node = n
        if me.isShallow(node):
            return

        me.fill(node)

        if node.prev == me.basin.leftNode and node.next == me.basin.rightNode:
            return

        if node.prev == me.basin.leftNode
        {
            let o = orient2d(node.point, node.next.point, node.next.next.point)
            if o == .CW:
                return
            node = node.next
        }
        elif node.next == me.basin.rightNode
        {
            let o = orient2d(node.point, node.prev.point, node.prev.prev.point)
            if o == .CCW:
                return
            node = node.prev
        }
        else
        {
            if node.prev.point.y < node.next.point.y:
                node = node.prev
            else:
                node = node.next
        }

        me.fillBasinReq(node)
    }

    mtd isShallow(node: *Node)->bool
    {
        var height: f64
        if me.basin.leftHighest:
            height = me.basin.leftNode.point.y - node.point.y
        else:
            height = me.basin.rightNode.point.y - node.point.y
        return me.basin.width > height
    }

    func basinAngle(node: *Node)->f64
    {
        let ax = node.point.x - node.next.next.point.x
        let ay = node.point.y - node.next.next.point.y
        return Math.atan2(ay, ax)
    }

    func largeHoleDontFill(node: *Node)->bool
    {
        let nextNode = node.next
        let prevNode = node.prev
        if !angleExceeds90Degrees(node.point, nextNode.point, prevNode.point):
            return false
        if angleIsNegative(node.point, nextNode.point, prevNode.point):
            return true

        let next2Node = nextNode.next
        if next2Node and !angleExceedsPlus90DegreesOrIsNegative(node.point, next2Node.point, prevNode.point):
            return false

        let prev2Node = prevNode.prev
        if prev2Node and !angleExceedsPlus90DegreesOrIsNegative(node.point, nextNode.point, prev2Node.point):
            return false

        return true
    }

    func angleIsNegative(origin, pa, pb: *Point)->bool
    {
        let angle = angle(origin, pa, pb)
        return angle < 0
    }

    func angleExceeds90Degrees(origin, pa, pb: *Point)->bool
    {
        let angle = angle(origin, pa, pb)
        return (angle > Math.ConstF64.PiBy2) or (angle < -Math.ConstF64.PiBy2)
    }

    func angleExceedsPlus90DegreesOrIsNegative(origin, pa, pb: *Point)->bool
    {
        let angle = angle(origin, pa, pb)
        return (angle > Math.ConstF64.PiBy2) or (angle < 0)
    }

    func angle(origin, pa, pb: *Point)->f64
    {
        let px = origin.x
        let py = origin.y
        let ax = pa.x - px
        let ay = pa.y - py
        let bx = pb.x - px
        let by = pb.y - py
        let x  = ax * by - ay * bx
        let y  = ax * bx + ay * by
        return Math.atan2(x, y)
    }

    mtd fill(node: *Node)
    {
        let triangle = newTriangle()
        triangle.points[0] = node.prev.point
        triangle.points[1] = node.point
        triangle.points[2] = node.next.point

        triangle.markNeighbor(node.prev.triangle)
        triangle.markNeighbor(node.triangle)

        triangle.idx = me.triangles.count
        me.triangles.add(triangle)

        node.prev.next = node.next
        node.next.prev = node.prev

        if !me.legalize(triangle):
            me.mapTriangleToNodes(triangle)
    }

    mtd newFrontTriangle(point: *Point, node: *Node)->*Node
    {
        let triangle = newTriangle()
        triangle.points[0] = point
        triangle.points[1] = node.point
        triangle.points[2] = node.next.point

        triangle.markNeighbor(node.triangle)

        triangle.idx = me.triangles.count
        me.triangles.add(triangle)

        let newNode = me.newNode()
        newNode.point = point
        newNode.value = point.x
        me.nodes.add(newNode)

        newNode.next   = node.next
        newNode.prev   = node
        node.next.prev = newNode
        node.next      = newNode

        if !me.legalize(triangle):
            me.mapTriangleToNodes(triangle)

        return newNode
    }

    func orient2d(pa, pb, pc: *Point)->Orientation
    {
        let detleft  = (pa.x - pc.x) * (pb.y - pc.y)
        let detright = (pa.y - pc.y) * (pb.x - pc.x)
        let val      = detleft - detright
        if Math.isZeroEpsilon(val):
            return Orientation.Collinear
        if val > 0:
            return Orientation.CCW
        return Orientation.CW
    }

    func inScanArea(pa, pb, pc, pd: *Point)->bool
    {
        let oadb = (pa.x - pb.x) * (pd.y - pb.y) - (pd.x - pb.x) * (pa.y - pb.y)
        if oadb >= -Math.ConstF64.Epsilon:
            return false

        let oadc = (pa.x - pc.x) * (pd.y - pc.y) - (pd.x - pc.x) * (pa.y - pc.y)
        if oadc <= Math.ConstF64.Epsilon:
            return false

        return true
    }

    func incircle(pa, pb, pc, pd: *Point)->bool
    {
        let adx = pa.x - pd.x
        let ady = pa.y - pd.y
        let bdx = pb.x - pd.x
        let bdy = pb.y - pd.y

        let adxbdy = adx * bdy
        let bdxady = bdx * ady
        let oabd   = adxbdy - bdxady

        if oabd <= 0:
            return false

        let cdx = pc.x - pd.x
        let cdy = pc.y - pd.y

        let cdxady = cdx * ady
        let adxcdy = adx * cdy
        let ocad   = cdxady - adxcdy

        if ocad <= 0:
            return false

        let bdxcdy = bdx * cdy
        let cdxbdy = cdx * bdy

        let alift = adx * adx + ady * ady
        let blift = bdx * bdx + bdy * bdy
        let clift = cdx * cdx + cdy * cdy

        let det = alift * (bdxcdy - cdxbdy) + blift * ocad + clift * oabd
        return det > 0
    }

    mtd legalize(t: *Triangle)->bool
    {
        for i in 3
        {
            if t.delaunayEdge[i]:
                continue

            let ot = t.neighbors[i]
            if ot
            {
                let p  = t.points[i]
                let op = ot.oppositePoint(t, p)
                let oi = ot.index(op)

                if (ot.constrainedEdge[oi] or ot.delaunayEdge[oi])
                {
                    t.constrainedEdge[i] = ot.constrainedEdge[oi]
                    continue
                }

                let inside = incircle(p, t.pointCCW(p), t.pointCW(p), op)
                if inside
                {
                    t.delaunayEdge[i]   = true
                    ot.delaunayEdge[oi] = true

                    rotateTrianglePair(t, p, ot, op)

                    var notLegalized = !me.legalize(t)
                    if notLegalized:
                        me.mapTriangleToNodes(t)

                    notLegalized = !me.legalize(ot)
                    if notLegalized:
                        me.mapTriangleToNodes(ot)

                    t.delaunayEdge[i]   = false
                    ot.delaunayEdge[oi] = false
                    return true
                }
            }
        }

        return false
    }

    func rotateTrianglePair(t: *Triangle, p: *Point, ot: *Triangle, op: *Point)
    {
        let n1 = t.neighborCCW(p)
        let n2 = t.neighborCW(p)
        let n3 = ot.neighborCCW(op)
        let n4 = ot.neighborCW(op)

        let ce1 = t.getConstrainedEdgeCCW(p)
        let ce2 = t.getConstrainedEdgeCW(p)
        let ce3 = ot.getConstrainedEdgeCCW(op)
        let ce4 = ot.getConstrainedEdgeCW(op)

        let de1 = t.getDelaunayEdgeCCW(p)
        let de2 = t.getDelaunayEdgeCW(p)
        let de3 = ot.getDelaunayEdgeCCW(op)
        let de4 = ot.getDelaunayEdgeCW(op)

        t.legalize(p, op)
        ot.legalize(op, p)

        ot.setDelaunayEdgeCCW(p, de1)
        t.setDelaunayEdgeCW(p, de2)
        t.setDelaunayEdgeCCW(op, de3)
        ot.setDelaunayEdgeCW(op, de4)

        ot.setConstrainedEdgeCCW(p, ce1)
        t.setConstrainedEdgeCW(p, ce2)
        t.setConstrainedEdgeCCW(op, ce3)
        ot.setConstrainedEdgeCW(op, ce4)

        t.clearNeighbors()
        ot.clearNeighbors()
        if n1:
            ot.markNeighbor(n1)
        if n2:
            t.markNeighbor(n2)
        if n3:
            t.markNeighbor(n3)
        if n4:
            ot.markNeighbor(n4)
        t.markNeighbor(ot)
    }

    mtd mapTriangleToNodes(t: *Triangle)
    {
        for i in 3
        {
            if !t.neighbors[i]
            {
                let n = me.front.locatePoint(t.pointCW(t.points[i]))
                if n:
                    n.triangle = t
            }
        }
    }

    mtd locateNode(point: *Point)->*Node
    {
        return me.front.locateNode(point.x)
    }

    mtd finalizationPolygon()
    {
        var t = me.front.head.next.triangle
        let p = me.front.head.next.point
        while t and !t.getConstrainedEdgeCW(p):
            t = t.neighborCCW(p)
        if t:
            me.meshClean(t)
    }

    mtd meshClean(triangle: *Triangle)
    {
        var stack: Array'*Triangle
        stack.add(triangle)

        me.triIndices.clear()

        while stack.count
        {
            let t = stack.popBack()
            if !t or t.interior:
                continue

            t.interior = true
            me.triIndices.add(t.idx)

            if !t.constrainedEdge[0]:
                stack.add(t.neighbors[0])
            if !t.constrainedEdge[1]:
                stack.add(t.neighbors[1])
            if !t.constrainedEdge[2]:
                stack.add(t.neighbors[2])
        }

        // Remove neighbors which are not part of the mesh
        foreach i in me.triIndices
        {
            let tri = me.triangles[i]
            for e in 3
            {
                let neighbor = tri.neighbors[e]
                if neighbor and !neighbor.interior:
                    tri.neighbors[e] = null
            }
        }
    }

    mtd initTriangulation()
    {
        var xmin, xmax = me.points[0].x
        var ymin, ymax = me.points[0].y
        foreach p in me.points
        {
            xmax = Math.max(xmax, p.x)
            xmin = Math.min(xmin, p.x)
            ymax = Math.max(ymax, p.y)
            ymin = Math.min(ymin, p.y)
        }

        const KAlpha = 0.3'f32
        let dx       = KAlpha * (xmax - xmin)
        let dy       = KAlpha * (ymax - ymin)
        me.head   = newPoint()
        me.head.x = xmin - dx
        me.head.y = ymin - dy
        me.tail   = newPoint()
        me.tail.x = xmax + dx
        me.tail.y = ymin - dy

        func cmp(a, b: *Point)->s32
        {
            if a.y < b.y:
                return -1
            if a.y > b.y:
                return 1
            if a.x < b.x:
                return -1
            if a.x > b.x:
                return 1
            return 0
        }

        Slice.sort(me.points.toSlice(), &cmp)
    }

    mtd createAdvancingFront()
    {
        let triangle = newTriangle()
        triangle.points[0] = me.points[0]
        triangle.points[1] = me.head
        triangle.points[2] = me.tail

        triangle.idx = me.triangles.count
        me.triangles.add(triangle)

        me.afHead   = me.newNode()
        me.afMiddle = me.newNode()
        me.afTail   = me.newNode()

        me.front.head       = me.afHead
        me.front.tail       = me.afTail
        me.front.searchNode = me.afHead

        me.afHead.point    = triangle.points[1]
        me.afHead.triangle = triangle
        me.afHead.next     = me.afMiddle
        me.afHead.value    = me.afHead.point.x

        me.afMiddle.point    = triangle.points[0]
        me.afMiddle.triangle = triangle
        me.afMiddle.next     = me.afTail
        me.afMiddle.prev     = me.afHead
        me.afMiddle.value    = me.afMiddle.point.x

        me.afTail.point    = triangle.points[2]
        me.afTail.triangle = triangle
        me.afTail.prev     = me.afMiddle
        me.afTail.value    = me.afTail.point.x
    }
}
