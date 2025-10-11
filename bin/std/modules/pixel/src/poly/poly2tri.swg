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
        .points.clear()
        .edges.clear()
        .triangles.clear()
        .triIndices.clear()
    }

    mtd release()
    {
        foreach t in .points do
            Memory.delete(t)
        foreach t in .edges do
            Memory.delete(t)
        foreach t in .triangles do
            Memory.delete(t)
        .points.clear()
        .edges.clear()
        .triangles.clear()
        .triIndices.clear()
    }
}

impl Edge
{
    mtd set(p1, p2: *Point)
    {
        .p = p1
        .q = p2
        if p1.y > p2.y
        {
            .q = p1
            .p = p2
        }
        elif p1.y == p2.y
        {
            if p1.x > p2.x
            {
                .q = p1
                .p = p2
            }
            elif p1.x == p2.x
            {
                Debug.assert(false, "Edge::Edge: p1 == p2")
            }
        }

        .q.edges.add(me)
        if .p.border[0] do
            .p.border[1] = me
        else do
            .p.border[0] = me
        if .q.border[0] do
            .q.border[1] = me
        else do
            .q.border[0] = me
    }
}

impl Triangle
{
    #[Swag.Overload]
    mtd markConstrainedEdge(index: s32)
    {
        .constrainedEdge[index] = true
    }

    #[Swag.Overload]
    mtd markConstrainedEdge(edge: *Edge)
    {
        .markConstrainedEdge(edge.p, edge.q)
    }

    #[Swag.Overload]
    mtd markConstrainedEdge(p, q: *Point)
    {
        if (q == .points[0] and p == .points[1]) or (q == .points[1] and p == .points[0]) do
            .constrainedEdge[2] = true
        elif (q == .points[0] and p == .points[2]) or (q == .points[2] and p == .points[0]) do
            .constrainedEdge[1] = true
        elif (q == .points[1] and p == .points[2]) or (q == .points[2] and p == .points[1]) do
            .constrainedEdge[0] = true
    }

    mtd oppositePoint(t: *Triangle, p: *Point)->*Point
    {
        let cw = t.pointCW(p)
        return .pointCW(cw)
    }

    mtd edgeIndex(p1, p2: *Point)->s32
    {
        switch p1
        {
        case .points[0]:
            if .points[1] == p2 do
                return 2
            if .points[2] == p2 do
                return 1

        case .points[1]:
            if .points[2] == p2 do
                return 0
            if .points[0] == p2 do
                return 2

        case .points[2]:
            if .points[0] == p2 do
                return 1
            if .points[1] == p2 do
                return 0
        }

        return -1
    }

    mtd index(p: *Point)->s32
    {
        switch p
        {
        case .points[0]:
            return 0
        case .points[1]:
            return 1
        case .points[2]:
            return 2
        }

        Debug.assert(false)
        return -1
    }

    mtd neighborAcross(point: *Point)->*Triangle
    {
        switch point
        {
        case .points[0]:
            return .neighbors[0]
        case .points[1]:
            return .neighbors[1]
        default:
            return .neighbors[2]
        }
    }

    mtd neighborCW(point: *Point)->*Triangle
    {
        switch point
        {
        case .points[0]:
            return .neighbors[1]
        case .points[1]:
            return .neighbors[2]
        default:
            return .neighbors[0]
        }
    }

    mtd neighborCCW(p: *Point)->*Triangle
    {
        switch p
        {
        case .points[0]:
            return .neighbors[2]
        case .points[1]:
            return .neighbors[0]
        default:
            return .neighbors[1]
        }
    }

    mtd getConstrainedEdgeCCW(p: *Point)->bool
    {
        switch p
        {
        case .points[0]:
            return .constrainedEdge[2]
        case .points[1]:
            return .constrainedEdge[0]
        default:
            return .constrainedEdge[1]
        }
    }

    mtd getConstrainedEdgeCW(p: *Point)->bool
    {
        switch p
        {
        case .points[0]:
            return .constrainedEdge[1]
        case .points[1]:
            return .constrainedEdge[2]
        default:
            return .constrainedEdge[0]
        }
    }

    mtd pointCW(point: *Point)->*Point
    {
        switch point
        {
        case .points[0]:
            return .points[2]
        case .points[1]:
            return .points[0]
        default:
            return .points[1]
        }
    }

    mtd pointCCW(point: *Point)->*Point
    {
        switch point
        {
        case .points[0]:
            return .points[1]
        case .points[1]:
            return .points[2]
        default:
            return .points[0]
        }
    }

    mtd getDelaunayEdgeCCW(p: *Point)->bool
    {
        switch p
        {
        case .points[0]:
            return .delaunayEdge[2]
        case .points[1]:
            return .delaunayEdge[0]
        default:
            return .delaunayEdge[1]
        }
    }

    mtd getDelaunayEdgeCW(p: *Point)->bool
    {
        switch p
        {
        case .points[0]:
            return .delaunayEdge[1]
        case .points[1]:
            return .delaunayEdge[2]
        default:
            return .delaunayEdge[0]
        }
    }

    mtd setDelaunayEdgeCCW(p: *Point, e: bool)
    {
        switch p
        {
        case .points[0]:
            .delaunayEdge[2] = e
        case .points[1]:
            .delaunayEdge[0] = e
        default:
            .delaunayEdge[1] = e
        }
    }

    mtd setDelaunayEdgeCW(p: *Point, e: bool)
    {
        switch p
        {
        case .points[0]:
            .delaunayEdge[1] = e
        case .points[1]:
            .delaunayEdge[2] = e
        default:
            .delaunayEdge[0] = e
        }
    }

    mtd setConstrainedEdgeCCW(p: *Point, ce: bool)
    {
        switch p
        {
        case .points[0]:
            .constrainedEdge[2] = ce
        case .points[1]:
            .constrainedEdge[0] = ce
        default:
            .constrainedEdge[1] = ce
        }
    }

    mtd setConstrainedEdgeCW(p: *Point, ce: bool)
    {
        switch p
        {
        case .points[0]:
            .constrainedEdge[1] = ce
        case .points[1]:
            .constrainedEdge[2] = ce
        default:
            .constrainedEdge[0] = ce
        }
    }

    mtd clearNeighbors()
    {
        .neighbors[0] = null
        .neighbors[1] = null
        .neighbors[2] = null
    }

    mtd clearDelaunayEdges()
    {
        .delaunayEdge[0] = false
        .delaunayEdge[1] = false
        .delaunayEdge[2] = false
    }

    #[Swag.Overload]
    mtd contains(p: *Point)->bool
    {
        return p == .points[0] or p == .points[1] or p == .points[2]
    }

    #[Swag.Overload]
    mtd contains(p1, p2: *Point)->bool
    {
        return .contains(p1) and .contains(p2)
    }

    #[Swag.Overload]
    mtd markNeighbor(p1, p2: *Point, t: *Triangle)
    {
        if (p1 == .points[2] and p2 == .points[1]) or (p1 == .points[1] and p2 == .points[2]) do
            .neighbors[0] = t
        elif (p1 == .points[0] and p2 == .points[2]) or (p1 == .points[2] and p2 == .points[0]) do
            .neighbors[1] = t
        elif (p1 == .points[0] and p2 == .points[1]) or (p1 == .points[1] and p2 == .points[0]) do
            .neighbors[2] = t
        else do
            Debug.assert(false)
    }

    #[Swag.Overload]
    mtd markNeighbor(t: *Triangle)
    {
        if t.contains(.points[1], .points[2])
        {
            .neighbors[0] = t
            t.markNeighbor(.points[1], .points[2], me)
        }
        elif t.contains(.points[0], .points[2])
        {
            .neighbors[1] = t
            t.markNeighbor(.points[0], .points[2], me)
        }
        elif t.contains(.points[0], .points[1])
        {
            .neighbors[2] = t
            t.markNeighbor(.points[0], .points[1], me)
        }
    }

    mtd legalize(opoint, npoint: *Point)
    {
        switch opoint
        {
        case .points[0]:
            .points[1] = .points[0]
            .points[0] = .points[2]
            .points[2] = npoint
        case .points[1]:
            .points[2] = .points[1]
            .points[1] = .points[0]
            .points[0] = npoint
        case .points[2]:
            .points[0] = .points[2]
            .points[2] = .points[1]
            .points[1] = npoint
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
        var node = .searchNode
        let nx   = node.point.x

        if px == nx
        {
            if point != node.point
            {
                if point == node.prev.point do
                    node = node.prev
                elif point == node.next.point do
                    node = node.next
                else do
                    Debug.assert(false)
            }
        }
        elif px < nx
        {
            node = node.prev
            while node
            {
                if point == node.point do
                    break
                node = node.prev
            }
        }
        else
        {
            node = node.next
            while node
            {
                if point == node.point do
                    break
                node = node.next
            }
        }

        if node do
            .searchNode = node
        return node
    }

    mtd locateNode(x: f32)->*Node
    {
        var node = .searchNode
        if x < node.value
        {
            node = node.prev
            while node
            {
                if x >= node.value
                {
                    .searchNode = node
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
                    .searchNode = node.prev
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
        .clear()
        .clearCache()
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
            if .freeNodes.count
            {
                let p = .freeNodes.popBack()
                @init(p, 1)
                return p
            }

            return Memory.new'Node()
        }

        mtd deleteNode(ptr: *Node)
        {
            .freeNodes.add(ptr)
        }
    }

    // Clear content
    public mtd clear()
    {
        @init(&.basin, 1)
        @init(&.edgeEvent, 1)
        .polyStart = 0

        deletePoint(.head)
        deletePoint(.tail)
        .deleteNode(.afHead)
        .deleteNode(.afMiddle)
        .deleteNode(.afTail)
        .head, .tail, .afHead, .afMiddle, .afTail = null

        foreach p in .points do
            deletePoint(p)
        foreach p in .edges do
            deleteEdge(p)
        foreach p in .triangles do
            deleteTriangle(p)
        foreach p in .nodes do
            .deleteNode(p)

        .points.clear()
        .edges.clear()
        .triangles.clear()
        .triIndices.clear()
        .nodes.clear()
    }

    // Clear cache
    mtd clearCache()
    {
        foreach p in .freeNodes do
            Memory.delete(p)
        .freeNodes.clear()
    }

    // Get the triangulation result
    public mtd getResult()->Result
    {
        var result: retval
        result.points     = #move .points
        result.edges      = #move .edges
        result.triangles  = #move .triangles
        result.triIndices = #move .triIndices
        return result
    }

    // To call before adding a polyline.
    // A polyline is a polygon border.
    // See [[addPoint]] and [[endPolyLine]]
    public mtd startPolyLine()
    {
        .polyStart = .points.count
    }

    // Register a new point
    public mtd addPoint(pt: Math.Vector2)
    {
        let p = newPoint()
        p.x = pt.x
        p.y = pt.y
        .points.add(p)
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
                .edges.add(edge)
            }
        }

        Debug.assert(.polyStart != .points.count, "no points !")
        initEdges(me, .points[.polyStart to .points.count - 1])
    }

    // Register a polygon as a polyline
    public mtd addPolyLine(poly: Polygon)
    {
        .points.grow(.points.count + poly.points.count)

        .startPolyLine()
        for i in 0 until poly.points.count - 1
        {
            let p  = newPoint()
            let pt = poly.getPoint(i)
            p.x = pt.x
            p.y = pt.y
            .points.add(p)
        }
        .endPolyLine()
    }

    // Triangulate
    public mtd triangulate()
    {
        if .points.isEmpty() do
            return

        .initTriangulation()
        .createAdvancingFront()
        .sweepPoints()
        .finalizationPolygon()

        .points.add(.head)
        .points.add(.tail)
        .head = null
        .tail = null
    }

    mtd sweepPoints()
    {
        for i in 1 until .points.count
        {
            let point = .points[i]
            let node  = .pointEvent(point)
            foreach e in point.edges do
                .addEdgeEvent2(e, node)
        }
    }

    mtd addEdgeEvent(ep, eq: *Point, t: *Triangle, point: *Point)
    {
        var triangle = t
        if isEdgeSideOfTriangle(triangle, ep, eq) do
            return

        let p1 = triangle.pointCCW(point)
        let o1 = orient2d(eq, p1, ep)
        if o1 == .Collinear
        {
            if triangle.contains(eq, p1)
            {
                triangle.markConstrainedEdge(eq, p1)
                .edgeEvent.constrainedEdge.q = p1
                triangle                     = triangle.neighborAcross(point)
                .addEdgeEvent(ep, p1, triangle, p1)
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
                .edgeEvent.constrainedEdge.q = p2
                triangle                     = triangle.neighborAcross(point)
                .addEdgeEvent(ep, p2, triangle, p2)
            }
            else
            {
                Debug.assert(false, "EdgeEvent - collinear points not supported")
            }

            return
        }

        if o1 == o2
        {
            if o1 == .CW do
                triangle = triangle.neighborCCW(point)
            else do
                triangle = triangle.neighborCW(point)
            .addEdgeEvent(ep, eq, triangle, point)
        }
        else
        {
            .flipEdgeEvent(ep, eq, triangle, point)
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
            .mapTriangleToNodes(t)
            .mapTriangleToNodes(ot)

            if p == eq and op == ep
            {
                if eq == .edgeEvent.constrainedEdge.q and ep == .edgeEvent.constrainedEdge.p
                {
                    t.markConstrainedEdge(ep, eq)
                    ot.markConstrainedEdge(ep, eq)
                    discard .legalize(t)
                    discard .legalize(ot)
                }
            }
            else
            {
                let o = orient2d(eq, op, ep)
                t = .nextFlipTriangle(o, t, ot, p, op)
                .flipEdgeEvent(ep, eq, t, p)
            }
        }
        else
        {
            let newP = nextFlipPoint(ep, eq, ot, op)
            .flipScanEdgeEvent(ep, eq, t, ot, newP)
            .addEdgeEvent(ep, eq, t, p)
        }
    }

    mtd nextFlipTriangle(o: Orientation, t, ot: *Triangle, p, op: *Point)->*Triangle
    {
        if o == .CCW
        {
            let edgeIndex = ot.edgeIndex(p, op)
            ot.delaunayEdge[edgeIndex] = true
            discard .legalize(ot)
            ot.clearDelaunayEdges()
            return t
        }

        let edgeIndex = t.edgeIndex(p, op)
        t.delaunayEdge[edgeIndex] = true
        discard .legalize(t)
        t.clearDelaunayEdges()
        return ot
    }

    func nextFlipPoint(ep, eq: *Point, ot: *Triangle, op: *Point)->*Point
    {
        let o2d = orient2d(eq, op, ep)
        if o2d == .CW do
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
        if inScanArea(eq, p1, p2, op) do
            .flipEdgeEvent(eq, op, ot, op)
        else
        {
            let newP = nextFlipPoint(ep, eq, ot, op)
            .flipScanEdgeEvent(ep, eq, flipTriangle, ot, newP)
        }
    }

    mtd addEdgeEvent2(edge: *Edge, node: *Node)
    {
        .edgeEvent.constrainedEdge = edge
        .edgeEvent.right           = edge.p.x > edge.q.x

        if isEdgeSideOfTriangle(node.triangle, edge.p, edge.q) do
            return

        .fillEdgeEvent(edge, node)
        .addEdgeEvent(edge.p, edge.q, node.triangle, edge.q)
    }

    mtd fillEdgeEvent(edge: *Edge, node: *Node)
    {
        if .edgeEvent.right do
            .fillRightAboveEdgeEvent(edge, node)
        else do
            .fillLeftAboveEdgeEvent(edge, node)
    }

    mtd fillRightAboveEdgeEvent(edge: *Edge, n: *Node)
    {
        var node = n
        while node.next.point.x < edge.p.x
        {
            if orient2d(edge.q, node.next.point, edge.p) == .CCW do
                .fillRightBelowEdgeEvent(edge, node)
            else do
                node = node.next
        }
    }

    mtd fillRightBelowEdgeEvent(edge: *Edge, node: *Node)
    {
        if node.point.x < edge.p.x
        {
            if orient2d(node.point, node.next.point, node.next.next.point) == .CCW do
                .fillRightConcaveEdgeEvent(edge, node)
            else
            {
                .fillRightConvexEdgeEvent(edge, node)
                .fillRightBelowEdgeEvent(edge, node)
            }
        }
    }

    mtd fillRightConcaveEdgeEvent(edge: *Edge, node: *Node)
    {
        .fill(node.next)
        if node.next.point != edge.p
        {
            if orient2d(edge.q, node.next.point, edge.p) == .CCW
            {
                if orient2d(node.point, node.next.point, node.next.next.point) == .CCW do
                    .fillRightConcaveEdgeEvent(edge, node)
            }
        }
    }

    mtd fillRightConvexEdgeEvent(edge: *Edge, node: *Node)
    {
        if orient2d(node.next.point, node.next.next.point, node.next.next.next.point) == .CCW
        {
            .fillRightConcaveEdgeEvent(edge, node.next)
        }
        else
        {
            if orient2d(edge.q, node.next.next.point, edge.p) == .CCW do
                .fillRightConvexEdgeEvent(edge, node.next)
        }
    }

    mtd fillLeftAboveEdgeEvent(edge: *Edge, n: *Node)
    {
        var node = n
        while node.prev.point.x > edge.p.x
        {
            if orient2d(edge.q, node.prev.point, edge.p) == .CW do
                .fillLeftBelowEdgeEvent(edge, node)
            else do
                node = node.prev
        }
    }

    mtd fillLeftBelowEdgeEvent(edge: *Edge, node: *Node)
    {
        if node.point.x > edge.p.x
        {
            if orient2d(node.point, node.prev.point, node.prev.prev.point) == .CW do
                .fillLeftConcaveEdgeEvent(edge, node)
            else
            {
                .fillLeftConvexEdgeEvent(edge, node)
                .fillLeftBelowEdgeEvent(edge, node)
            }
        }
    }

    mtd fillLeftConvexEdgeEvent(edge: *Edge, node: *Node)
    {
        if orient2d(node.prev.point, node.prev.prev.point, node.prev.prev.prev.point) == .CW do
            .fillLeftConcaveEdgeEvent(edge, node.prev)
        else
        {
            if orient2d(edge.q, node.prev.prev.point, edge.p) == .CW do
                .fillLeftConvexEdgeEvent(edge, node.prev)
        }
    }

    mtd fillLeftConcaveEdgeEvent(edge: *Edge, node: *Node)
    {
        .fill(node.prev)
        if node.prev.point != edge.p
        {
            if orient2d(edge.q, node.prev.point, edge.p) == .CW
            {
                if orient2d(node.point, node.prev.point, node.prev.prev.point) == .CW do
                    .fillLeftConcaveEdgeEvent(edge, node)
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
            if t do
                t.markConstrainedEdge(ep, eq)
            return true
        }

        return false
    }

    mtd pointEvent(point: *Point)->*Node
    {
        let node = .locateNode(point)
        Debug.assert(node.next and node.next.point, "null node")
        let newNode = .newFrontTriangle(point, node)
        if point.x <= node.point.x + Math.ConstF64.Epsilon do
            .fill(node)
        .fillAdvancingFront(newNode)
        return newNode
    }

    mtd fillAdvancingFront(n: *Node)
    {
        var node = n.next
        while node and node.next
        {
            if largeHoleDontFill(node) do
                break
            .fill(node)
            node = node.next
        }

        node = n.prev
        while node and node.prev
        {
            if largeHoleDontFill(node) do
                break
            .fill(node)
            node = node.prev
        }

        if n.next and n.next.next
        {
            let angle = basinAngle(n)
            if angle < Math.ConstF64.ThreePiBy4 do
                .fillBasin(n)
        }
    }

    mtd fillBasin(node: *Node)
    {
        if orient2d(node.point, node.next.point, node.next.next.point) == .CCW do
            .basin.leftNode = node.next.next
        else do
            .basin.leftNode = node.next

        .basin.bottomNode = .basin.leftNode
        while .basin.bottomNode.next and .basin.bottomNode.point.y >= .basin.bottomNode.next.point.y do
            .basin.bottomNode = .basin.bottomNode.next
        if .basin.bottomNode == .basin.leftNode do
            return

        .basin.rightNode = .basin.bottomNode
        while .basin.rightNode.next and .basin.rightNode.point.y < .basin.rightNode.next.point.y do
            .basin.rightNode = .basin.rightNode.next
        if .basin.rightNode == .basin.bottomNode do
            return

        .basin.width       = .basin.rightNode.point.x - .basin.leftNode.point.x
        .basin.leftHighest = .basin.leftNode.point.y > .basin.rightNode.point.y

        .fillBasinReq(.basin.bottomNode)
    }

    mtd fillBasinReq(n: *Node)
    {
        var node = n
        if .isShallow(node) do
            return

        .fill(node)

        if node.prev == .basin.leftNode and node.next == .basin.rightNode do
            return

        if node.prev == .basin.leftNode
        {
            let o = orient2d(node.point, node.next.point, node.next.next.point)
            if o == .CW do
                return
            node = node.next
        }
        elif node.next == .basin.rightNode
        {
            let o = orient2d(node.point, node.prev.point, node.prev.prev.point)
            if o == .CCW do
                return
            node = node.prev
        }
        else
        {
            if node.prev.point.y < node.next.point.y do
                node = node.prev
            else do
                node = node.next
        }

        .fillBasinReq(node)
    }

    mtd isShallow(node: *Node)->bool
    {
        var height: f64
        if .basin.leftHighest do
            height = .basin.leftNode.point.y - node.point.y
        else do
            height = .basin.rightNode.point.y - node.point.y
        return .basin.width > height
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
        if !angleExceeds90Degrees(node.point, nextNode.point, prevNode.point) do
            return false
        if angleIsNegative(node.point, nextNode.point, prevNode.point) do
            return true

        let next2Node = nextNode.next
        if next2Node and !angleExceedsPlus90DegreesOrIsNegative(node.point, next2Node.point, prevNode.point) do
            return false

        let prev2Node = prevNode.prev
        if prev2Node and !angleExceedsPlus90DegreesOrIsNegative(node.point, nextNode.point, prev2Node.point) do
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

        triangle.idx = .triangles.count
        .triangles.add(triangle)

        node.prev.next = node.next
        node.next.prev = node.prev

        if !.legalize(triangle) do
            .mapTriangleToNodes(triangle)
    }

    mtd newFrontTriangle(point: *Point, node: *Node)->*Node
    {
        let triangle = newTriangle()
        triangle.points[0] = point
        triangle.points[1] = node.point
        triangle.points[2] = node.next.point

        triangle.markNeighbor(node.triangle)

        triangle.idx = .triangles.count
        .triangles.add(triangle)

        let newNode = .newNode()
        newNode.point = point
        newNode.value = point.x
        .nodes.add(newNode)

        newNode.next   = node.next
        newNode.prev   = node
        node.next.prev = newNode
        node.next      = newNode

        if !.legalize(triangle) do
            .mapTriangleToNodes(triangle)

        return newNode
    }

    func orient2d(pa, pb, pc: *Point)->Orientation
    {
        let detleft  = (pa.x - pc.x) * (pb.y - pc.y)
        let detright = (pa.y - pc.y) * (pb.x - pc.x)
        let val      = detleft - detright
        if Math.isZeroEpsilon(val) do
            return Orientation.Collinear
        if val > 0 do
            return Orientation.CCW
        return Orientation.CW
    }

    func inScanArea(pa, pb, pc, pd: *Point)->bool
    {
        let oadb = (pa.x - pb.x) * (pd.y - pb.y) - (pd.x - pb.x) * (pa.y - pb.y)
        if oadb >= -Math.ConstF64.Epsilon do
            return false

        let oadc = (pa.x - pc.x) * (pd.y - pc.y) - (pd.x - pc.x) * (pa.y - pc.y)
        if oadc <= Math.ConstF64.Epsilon do
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

        if oabd <= 0 do
            return false

        let cdx = pc.x - pd.x
        let cdy = pc.y - pd.y

        let cdxady = cdx * ady
        let adxcdy = adx * cdy
        let ocad   = cdxady - adxcdy

        if ocad <= 0 do
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
            if t.delaunayEdge[i] do
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

                    var notLegalized = !.legalize(t)
                    if notLegalized do
                        .mapTriangleToNodes(t)

                    notLegalized = !.legalize(ot)
                    if notLegalized do
                        .mapTriangleToNodes(ot)

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
        if n1 do
            ot.markNeighbor(n1)
        if n2 do
            t.markNeighbor(n2)
        if n3 do
            t.markNeighbor(n3)
        if n4 do
            ot.markNeighbor(n4)
        t.markNeighbor(ot)
    }

    mtd mapTriangleToNodes(t: *Triangle)
    {
        for i in 3
        {
            if !t.neighbors[i]
            {
                let n = .front.locatePoint(t.pointCW(t.points[i]))
                if n do
                    n.triangle = t
            }
        }
    }

    mtd locateNode(point: *Point)->*Node
    {
        return .front.locateNode(point.x)
    }

    mtd finalizationPolygon()
    {
        var t = .front.head.next.triangle
        let p = .front.head.next.point
        while t and !t.getConstrainedEdgeCW(p) do
            t = t.neighborCCW(p)
        if t do
            .meshClean(t)
    }

    mtd meshClean(triangle: *Triangle)
    {
        var stack: Array'*Triangle
        stack.add(triangle)

        .triIndices.clear()

        while stack.count
        {
            let t = stack.popBack()
            if !t or t.interior do
                continue

            t.interior = true
            .triIndices.add(t.idx)

            if !t.constrainedEdge[0] do
                stack.add(t.neighbors[0])
            if !t.constrainedEdge[1] do
                stack.add(t.neighbors[1])
            if !t.constrainedEdge[2] do
                stack.add(t.neighbors[2])
        }

        // Remove neighbors which are not part of the mesh
        foreach i in .triIndices
        {
            let tri = .triangles[i]
            for e in 3
            {
                let neighbor = tri.neighbors[e]
                if neighbor and !neighbor.interior do
                    tri.neighbors[e] = null
            }
        }
    }

    mtd initTriangulation()
    {
        var xmin, xmax = .points[0].x
        var ymin, ymax = .points[0].y
        foreach p in .points
        {
            xmax = Math.max(xmax, p.x)
            xmin = Math.min(xmin, p.x)
            ymax = Math.max(ymax, p.y)
            ymin = Math.min(ymin, p.y)
        }

        const KAlpha = 0.3'f32
        let dx       = KAlpha * (xmax - xmin)
        let dy       = KAlpha * (ymax - ymin)
        .head   = newPoint()
        .head.x = xmin - dx
        .head.y = ymin - dy
        .tail   = newPoint()
        .tail.x = xmax + dx
        .tail.y = ymin - dy

        func cmp(a, b: *Point)->s32
        {
            if a.y < b.y do
                return -1
            if a.y > b.y do
                return 1
            if a.x < b.x do
                return -1
            if a.x > b.x do
                return 1
            return 0
        }

        Slice.sort(.points.toSlice(), &cmp)
    }

    mtd createAdvancingFront()
    {
        let triangle = newTriangle()
        triangle.points[0] = .points[0]
        triangle.points[1] = .head
        triangle.points[2] = .tail

        triangle.idx = .triangles.count
        .triangles.add(triangle)

        .afHead   = .newNode()
        .afMiddle = .newNode()
        .afTail   = .newNode()

        .front.head       = .afHead
        .front.tail       = .afTail
        .front.searchNode = .afHead

        .afHead.point    = triangle.points[1]
        .afHead.triangle = triangle
        .afHead.next     = .afMiddle
        .afHead.value    = .afHead.point.x

        .afMiddle.point    = triangle.points[0]
        .afMiddle.triangle = triangle
        .afMiddle.next     = .afTail
        .afMiddle.prev     = .afHead
        .afMiddle.value    = .afMiddle.point.x

        .afTail.point    = triangle.points[2]
        .afTail.triangle = triangle
        .afTail.prev     = .afMiddle
        .afTail.value    = .afTail.point.x
    }
}
